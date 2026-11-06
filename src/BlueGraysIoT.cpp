/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT                                               ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Akış / Flow:
 *   Wi-Fi  →  saat eşitleme (TLS) / clock sync  →  WSS bağlantısı
 *   →  "hello"  →  sunucu "ok"  →  çevrimiçi / online
 *
 * Bütün yetki sunucuda: değişken listesi, hız sınırı ve cihaz sayısı
 * orada kontrol edilir. Bu kod yalnız "ben bu cihazım" der.
 * All permissions live on the server: the variable list, the rate limit
 * and the device count are checked there. This code only says
 * "I am this device".
 */

#include "BlueGraysIoT.h"
#include "BlueGraysIoTRootCA.h"
#include <time.h>
#include <math.h>
#include <string.h>

// Saat geçerli mi (2024 sonrası) / Is the clock valid (after 2024)
static bool bgTimeIsValid() { return time(nullptr) > 1704067200; }

BlueGraysIoT::BlueGraysIoT(const char *deviceKey) : _key(deviceKey), _rootCA(BG_ROOT_CA) {
  _authHeader[0] = '\0';
  _deviceName[0] = '\0';
}

// ── Başlatma / Start ─────────────────────────────────────────────

void BlueGraysIoT::begin(const char *ssid, const char *password) {
  _ssid = ssid;
  _pass = password;
  _manageWifi = true;
  begin();
}

void BlueGraysIoT::begin() {
  if (!_key || strncmp(_key, "bgd_", 4) != 0 || strlen(_key) < 20) {
    _log(F("the device key looks wrong. Copy the code again from the app."),
         F("cihaz anahtarı hatalı görünüyor. Kodu uygulamadan yeniden kopyala."));
  }

  // Anahtar yalnız bu başlıkta gider, hiçbir yere yazdırılmaz.
  // The key only travels in this header and is never printed.
#if defined(ARDUINO_ARCH_ESP32)
  const char *board = "esp32";
#else
  const char *board = "rp2040";
#endif
  snprintf(_authHeader, sizeof(_authHeader), "Authorization: Bearer %s\r\nX-BG-Lib: BlueGraysIoT/%s %s",
           _key ? _key : "", BLUEGRAYS_IOT_VERSION, board);

  if (_manageWifi) {
    WiFi.mode(WIFI_STA);
#if defined(ARDUINO_ARCH_ESP32)
    WiFi.setAutoReconnect(true);
#endif
    _log(F("connecting to Wi-Fi: "), F("Wi-Fi bağlanıyor: "), _ssid);
    WiFi.begin(_ssid, _pass);
    _lastWifiTryMs = millis();
  }
  _setState(BG_STATE_WIFI);
}

void BlueGraysIoT::setServer(const char *host, uint16_t port, const char *path) {
  _host = host;
  _port = port;
  _path = path;
}

// ── Ana döngü / Main loop ────────────────────────────────────────

void BlueGraysIoT::loop() {
  if (_state == BG_STATE_IDLE) return;

  if (WiFi.status() != WL_CONNECTED) {
    _handleWifi();
    return;
  }

  if (_state == BG_STATE_WIFI) {
    char ip[20];
    snprintf(ip, sizeof(ip), "%s", WiFi.localIP().toString().c_str());
    _log(F("Wi-Fi connected, IP "), F("Wi-Fi bağlandı, IP "), ip);
    _setState(BG_STATE_TIME);
  }

  if (_state == BG_STATE_TIME) {
    _handleTime();
    return;
  }

  _ws.loop();
  if (_state == BG_STATE_ONLINE) _flush();
}

void BlueGraysIoT::_handleWifi() {
  if (_state == BG_STATE_ONLINE || _state == BG_STATE_CONNECTING) {
    bool wasOnline = _state == BG_STATE_ONLINE;
    _log(F("Wi-Fi lost, waiting…"), F("Wi-Fi koptu, bekleniyor…"));
    _setState(BG_STATE_WIFI);
    if (wasOnline && _onDisconnect) _onDisconnect();
  }
  if (_manageWifi && millis() - _lastWifiTryMs > BG_WIFI_RETRY_MS) {
    _lastWifiTryMs = millis();
#if defined(ARDUINO_ARCH_ESP32)
    WiFi.reconnect();
#else
    WiFi.begin(_ssid, _pass);
#endif
  }
}

void BlueGraysIoT::_handleTime() {
  // Sertifikanın geçerlilik tarihini kontrol etmek için saat gerekir.
  // A real clock is needed to check certificate validity dates.
  if (!_timeStarted) {
    _timeStarted = true;
    if (!bgTimeIsValid()) {
#if defined(ARDUINO_ARCH_ESP32)
      configTime(0, 0, "pool.ntp.org", "time.google.com");
#else
      NTP.begin("pool.ntp.org", "time.google.com");
#endif
    }
  }
  if (bgTimeIsValid() || millis() - _stateSinceMs > BG_TIME_SYNC_TIMEOUT_MS) {
    if (!bgTimeIsValid()) {
      _log(F("clock sync failed, trying anyway"), F("saat eşitlenemedi, yine de deneniyor"));
    }
    _setState(BG_STATE_CONNECTING);
    _startSocket();
  }
}

void BlueGraysIoT::_startSocket() {
  // Yeniden bağlanmayı WebSocket kütüphanesi yapar; bu yüzden yalnız bir
  // kez başlatılır (Pico W'de her başlatma sertifika listesini yeniden kurar).
  // The WebSocket library reconnects by itself, so start it only once
  // (on Pico W every start rebuilds the certificate list).
  if (_wsStarted) return;
  _wsStarted = true;
  _log(F("connecting to the server…"), F("sunucuya bağlanıyor…"));
  _ws.setExtraHeaders(_authHeader);
  _ws.onEvent([this](WStype_t type, uint8_t *payload, size_t length) { _onWsEvent(type, payload, length); });
  _ws.setReconnectInterval(_retryDelayMs);
  _ws.enableHeartbeat(BG_PING_INTERVAL_MS, BG_PONG_TIMEOUT_MS, 2);
  _ws.beginSslWithCA(_host, _port, _path, _rootCA, "bluegrays-iot-1");
}

void BlueGraysIoT::_setRetry(uint32_t delayMs) {
  _retryDelayMs = delayMs;
  _ws.setReconnectInterval(delayMs);
}

void BlueGraysIoT::_setState(BlueGraysState s) {
  _state = s;
  _stateSinceMs = millis();
}

// ── WebSocket olayları / WebSocket events ────────────────────────

void BlueGraysIoT::_onWsEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      _log(F("secure connection open, checking the key…"), F("güvenli bağlantı açıldı, anahtar kontrol ediliyor…"));
      _sendHello();
      break;

    case WStype_TEXT:
      _handleMessage((const char *)payload, length);
      break;

    case WStype_DISCONNECTED: {
      bool wasOnline = _state == BG_STATE_ONLINE;
      if (_state == BG_STATE_ONLINE || _state == BG_STATE_CONNECTING) _setState(BG_STATE_CONNECTING);
      if (_kicked) {
        _log(F("this key is in use on another board, waiting 60 s"),
             F("bu anahtar başka bir kartta kullanılıyor, 60 sn bekleniyor"));
        _setRetry(BG_KICK_WAIT_MS);
      } else {
        if (wasOnline) _log(F("connection lost, retrying"), F("bağlantı koptu, yeniden deneniyor"));
        // Artan bekleme, sunucuyu yormamak için / Growing wait so the server is not flooded
        uint32_t next = wasOnline ? BG_RETRY_MIN_MS : _retryDelayMs * 2;
        if (next > BG_RETRY_MAX_MS) next = BG_RETRY_MAX_MS;
        _setRetry(next);
      }
      if (wasOnline && _onDisconnect) _onDisconnect();
      break;
    }

    case WStype_ERROR:
      _log(F("connection error"), F("bağlantı hatası"));
      break;

    default:
      break;
  }
}

void BlueGraysIoT::_handleMessage(const char *json, size_t length) {
  JsonDocument doc;
  if (deserializeJson(doc, json, length)) return;
  const char *t = doc["t"] | "";

  if (strcmp(t, "ok") == 0) {
    uint32_t rate = doc["rate"] | (uint32_t)BG_DEFAULT_MIN_INTERVAL_MS;
    _minIntervalMs = rate < 100 ? 100 : rate;
    snprintf(_deviceName, sizeof(_deviceName), "%s", (const char *)(doc["name"] | ""));
    _kicked = false;
    _setRetry(BG_RETRY_MIN_MS);
    _setState(BG_STATE_ONLINE);
    if (_deviceName[0]) {
      char extra[40];
      snprintf(extra, sizeof(extra), " (%s)", _deviceName);
      _log(F("connected"), F("bağlandı"), extra);
    } else {
      _log(F("connected"), F("bağlandı"));
    }
    // Son durumu uygula: elektrik gidip gelince lamba eski haline döner.
    // Apply the last state: after a power cut the lamp returns to how it was.
    if (_restore) {
      JsonObjectConst vars = doc["vars"].as<JsonObjectConst>();
      for (JsonPairConst kv : vars) {
        Var *v = _find(kv.key().c_str());
        if (v && v->isCommand) _applyCommand(v, kv.value());
      }
    }
    // Çevrimdışıyken ölçülen son değerler gönderilsin / Send values measured while offline
    for (uint8_t i = 0; i < _varCount; i++) _vars[i].lastSentMs = 0;
    if (_onConnect) _onConnect();
    return;
  }

  if (strcmp(t, "cmd") == 0) {
    const char *k = doc["k"] | "";
    const char *id = doc["id"] | "";
    Var *v = _find(k);
    if (!v || !v->isCommand) {
      _log(F("command for an unknown variable: "), F("bilinmeyen değişkene komut: "), k);
      return;
    }
    _applyCommand(v, doc["v"]);
    _sendAck(id, v);
    return;
  }

  if (strcmp(t, "err") == 0) {
    const char *c = doc["c"] | "";
    const char *k = doc["k"] | "";
    char extra[64];
    snprintf(extra, sizeof(extra), "%s %s", c, k);
    _log(F("server says: "), F("sunucu diyor ki: "), extra);
    if (strcmp(c, "bad_key") == 0 || strcmp(c, "locked") == 0) {
      // Anahtar geçersiz ya da kilitli; sık deneme anlamsız.
      // Key is invalid or locked; retrying often is pointless.
      _log(F("renew the key in the app and upload the new code"),
           F("uygulamada anahtarı yenile ve yeni kodu yükle"));
      _setRetry(BG_RETRY_MAX_MS);
    }
    if (_onError) _onError(c, k);
    return;
  }

  if (strcmp(t, "kick") == 0) {
    _kicked = true;  // bağlantıyı sunucu kapatacak / the server will close the socket
    return;
  }
}

void BlueGraysIoT::_applyCommand(Var *v, JsonVariantConst value) {
  switch (v->type) {
    case VT_BOOL: {
      bool b = value.is<bool>() ? value.as<bool>() : (value.as<float>() != 0);
      v->lastNum = b ? 1 : 0;
      v->hasLast = true;
      if (v->onSwitch) v->onSwitch(b);
      break;
    }
    case VT_NUMBER: {
      float n = value.as<float>();
      v->lastNum = n;
      v->hasLast = true;
      if (v->onNumber) v->onNumber(n);
      break;
    }
    case VT_COLOR: {
      uint8_t r, g, b;
      const char *hex = value | "";
      if (_parseColor(hex, r, g, b)) {
        snprintf(v->lastText, sizeof(v->lastText), "%s", hex);
        v->hasLast = true;
        if (v->onColor) v->onColor(r, g, b);
      }
      break;
    }
    case VT_TEXT: {
      const char *s = value | "";
      snprintf(v->lastText, sizeof(v->lastText), "%s", s);
      v->hasLast = true;
      if (v->onChoice) v->onChoice(v->lastText);
      break;
    }
    default:
      break;
  }
}

void BlueGraysIoT::_sendHello() {
  JsonDocument doc;
  doc["t"] = "hello";
  doc["fw"] = BLUEGRAYS_IOT_VERSION;
  JsonObject vars = doc["vars"].to<JsonObject>();
  for (uint8_t i = 0; i < _varCount; i++) {
    const char *type = "number";
    if (_vars[i].type == VT_BOOL) type = "bool";
    else if (_vars[i].type == VT_COLOR) type = "color";
    else if (_vars[i].type == VT_TEXT) type = "text";
    vars[_vars[i].key] = type;
  }
  char buf[512];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  _ws.sendTXT(buf, n);
}

void BlueGraysIoT::_sendAck(const char *id, Var *v) {
  JsonDocument doc;
  doc["t"] = "ack";
  doc["id"] = id;
  if (v->type == VT_BOOL) doc["v"] = v->lastNum != 0;
  else if (v->type == VT_NUMBER) doc["v"] = v->lastNum;
  else doc["v"] = (const char *)v->lastText;
  char buf[160];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  _ws.sendTXT(buf, n);
}

// ── Komut kayıtları / Command registration ───────────────────────

void BlueGraysIoT::onSwitch(const char *key, BlueGraysSwitchHandler handler) {
  Var *v = _findOrAdd(key, VT_BOOL, true);
  if (v) v->onSwitch = handler;
}

void BlueGraysIoT::onNumber(const char *key, BlueGraysNumberHandler handler) {
  Var *v = _findOrAdd(key, VT_NUMBER, true);
  if (v) v->onNumber = handler;
}

void BlueGraysIoT::onColor(const char *key, BlueGraysColorHandler handler) {
  Var *v = _findOrAdd(key, VT_COLOR, true);
  if (v) v->onColor = handler;
}

void BlueGraysIoT::onChoice(const char *key, BlueGraysChoiceHandler handler) {
  Var *v = _findOrAdd(key, VT_TEXT, true);
  if (v) v->onChoice = handler;
}

// ── Değer gönderme / Sending values ──────────────────────────────

bool BlueGraysIoT::send(const char *key, float value) {
  if (isnan(value) || isinf(value)) return false;  // sensör okunamadı / sensor read failed
  Var *v = _findOrAdd(key, VT_NUMBER, false);
  return v && _queue(v, value, nullptr);
}

bool BlueGraysIoT::send(const char *key, bool value) {
  Var *v = _findOrAdd(key, VT_BOOL, false);
  return v && _queue(v, value ? 1 : 0, nullptr);
}

bool BlueGraysIoT::send(const char *key, const char *value) {
  Var *v = _findOrAdd(key, VT_TEXT, false);
  return v && _queue(v, 0, value ? value : "");
}

bool BlueGraysIoT::sendColor(const char *key, uint8_t r, uint8_t g, uint8_t b) {
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02X%02X%02X", r, g, b);
  Var *v = _findOrAdd(key, VT_COLOR, false);
  return v && _queue(v, 0, hex);
}

void BlueGraysIoT::setDeadband(const char *key, float delta) {
  Var *v = _findOrAdd(key, VT_NUMBER, false);
  if (v) v->deadband = delta < 0 ? 0 : delta;
}

bool BlueGraysIoT::_queue(Var *v, float num, const char *text) {
  bool isText = v->type == VT_COLOR || v->type == VT_TEXT;
  // Son gönderilenle aynıysa bekleyeni de iptal et / Same as last sent: drop the pending one
  bool same;
  if (isText) {
    same = v->hasLast && strncmp(v->lastText, text, BG_MAX_TEXT_LEN) == 0;
  } else {
    float band = v->deadband > 0 ? v->deadband : 0.0001f;
    same = v->hasLast && fabsf(num - v->lastNum) < band;
  }
  if (same) {
    v->hasPending = false;
    return true;
  }
  if (isText) snprintf(v->pendingText, sizeof(v->pendingText), "%s", text);
  else v->pendingNum = num;
  v->hasPending = true;
  // Hız sınırı izin veriyorsa hemen gönder / Send now if the rate limit allows
  if (_state == BG_STATE_ONLINE && millis() - v->lastSentMs >= _minIntervalMs) _transmit(v);
  return true;
}

void BlueGraysIoT::_flush() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < _varCount; i++) {
    Var *v = &_vars[i];
    if (v->hasPending && now - v->lastSentMs >= _minIntervalMs) _transmit(v);
  }
}

bool BlueGraysIoT::_transmit(Var *v) {
  JsonDocument doc;
  doc["t"] = "set";
  doc["k"] = (const char *)v->key;
  if (v->type == VT_BOOL) doc["v"] = v->pendingNum != 0;
  else if (v->type == VT_NUMBER) doc["v"] = v->pendingNum;
  else doc["v"] = (const char *)v->pendingText;
  char buf[160];
  size_t n = serializeJson(doc, buf, sizeof(buf));
  if (!_ws.sendTXT(buf, n)) return false;
  if (v->type == VT_COLOR || v->type == VT_TEXT) snprintf(v->lastText, sizeof(v->lastText), "%s", v->pendingText);
  else v->lastNum = v->pendingNum;
  v->hasLast = true;
  v->hasPending = false;
  v->lastSentMs = millis();
  return true;
}

// ── Yardımcılar / Helpers ────────────────────────────────────────

BlueGraysIoT::Var *BlueGraysIoT::_find(const char *key) {
  for (uint8_t i = 0; i < _varCount; i++) {
    if (strncmp(_vars[i].key, key, BG_MAX_KEY_LEN) == 0) return &_vars[i];
  }
  return nullptr;
}

BlueGraysIoT::Var *BlueGraysIoT::_findOrAdd(const char *key, VarType type, bool isCommand) {
  if (!key || !key[0] || strlen(key) > BG_MAX_KEY_LEN) {
    _log(F("variable name is empty or too long: "), F("değişken adı boş ya da çok uzun: "), key ? key : "");
    return nullptr;
  }
  Var *v = _find(key);
  if (v) {
    if (v->type != type) {
      _log(F("variable already used with another type: "), F("değişken başka bir türle kullanılmış: "), key);
      return nullptr;
    }
    if (isCommand) v->isCommand = true;
    return v;
  }
  if (_varCount >= BG_MAX_VARS) {
    _log(F("too many variables, raise BG_MAX_VARS"), F("çok fazla değişken, BG_MAX_VARS'ı artır"));
    return nullptr;
  }
  v = &_vars[_varCount++];
  memset(v->key, 0, sizeof(v->key));
  strncpy(v->key, key, BG_MAX_KEY_LEN);
  v->type = type;
  v->isCommand = isCommand;
  v->lastNum = 0;
  v->pendingNum = 0;
  v->lastText[0] = '\0';
  v->pendingText[0] = '\0';
  v->hasLast = false;
  v->hasPending = false;
  v->deadband = 0;
  v->lastSentMs = 0;
  return v;
}

bool BlueGraysIoT::_parseColor(const char *hex, uint8_t &r, uint8_t &g, uint8_t &b) {
  if (!hex || hex[0] != '#' || strlen(hex) != 7) return false;
  char *end = nullptr;
  unsigned long n = strtoul(hex + 1, &end, 16);
  if (!end || *end != '\0') return false;
  r = (n >> 16) & 0xFF;
  g = (n >> 8) & 0xFF;
  b = n & 0xFF;
  return true;
}

void BlueGraysIoT::_log(const __FlashStringHelper *en, const __FlashStringHelper *tr, const char *extra) {
  if (!_debug) return;
  _debug->print(F("BlueGrays IoT: "));
  _debug->print(_lang == BG_LANG_TR ? tr : en);
  if (extra) _debug->print(extra);
  _debug->println();
}
