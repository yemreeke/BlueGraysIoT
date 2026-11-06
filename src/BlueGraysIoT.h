/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT                                               ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  ESP32 ve Raspberry Pi Pico W kartlarını BlueGrays           ║
 * ║  uygulamasından güvenli WebSocket ile yönetir.               ║
 * ║  Control ESP32 and Raspberry Pi Pico W boards from the       ║
 * ║  BlueGrays app over a secure WebSocket.                      ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * MIT License · https://github.com/yemreeke/BlueGraysIoT
 */

#ifndef BLUEGRAYS_IOT_H
#define BLUEGRAYS_IOT_H

#include <Arduino.h>
#include <functional>

#if !defined(ARDUINO_ARCH_ESP32) && !defined(ARDUINO_ARCH_RP2040)
#error "BlueGrays IoT yalnız ESP32 ve Raspberry Pi Pico W ile çalışır / BlueGrays IoT supports ESP32 and Raspberry Pi Pico W only."
#endif

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#include "BlueGraysIoTConfig.h"

#define BLUEGRAYS_IOT_VERSION "0.1.0"

// Durum mesajlarının dili / Language of the status messages
enum BlueGraysLanguage {
  BG_LANG_EN = 0,
  BG_LANG_TR = 1
};

// Bağlantı durumu / Connection state
enum BlueGraysState {
  BG_STATE_IDLE = 0,       // begin() çağrılmadı / begin() not called
  BG_STATE_WIFI,           // Wi-Fi bekleniyor / waiting for Wi-Fi
  BG_STATE_TIME,           // saat eşitleniyor (TLS için) / syncing clock for TLS
  BG_STATE_CONNECTING,     // sunucuya bağlanıyor / connecting to the server
  BG_STATE_ONLINE          // bağlı ve doğrulandı / connected and authenticated
};

// Sunucunun bildirdiği hata kodları / Error codes sent by the server
// "unknown_var", "bad_type", "out_of_range", "rate_limited", "dup_key", "locked", "bad_key"
typedef std::function<void(bool)> BlueGraysSwitchHandler;
typedef std::function<void(float)> BlueGraysNumberHandler;
typedef std::function<void(uint8_t r, uint8_t g, uint8_t b)> BlueGraysColorHandler;
typedef std::function<void(const char *)> BlueGraysChoiceHandler;
typedef std::function<void()> BlueGraysEventHandler;
typedef std::function<void(const char *code, const char *key)> BlueGraysErrorHandler;

class BlueGraysIoT {
public:
  // Cihaz anahtarı BlueGrays uygulamasında üretilir (bgd_ ile başlar).
  // The device key is generated in the BlueGrays app (starts with bgd_).
  explicit BlueGraysIoT(const char *deviceKey);

  // Wi-Fi'ı kütüphane yönetir / The library manages Wi-Fi
  void begin(const char *ssid, const char *password);
  // Wi-Fi'ı sen bağladıysan / If you connect Wi-Fi yourself
  void begin();

  // loop() içinde her turda çağır, delay() kullanma.
  // Call on every pass of loop(); avoid long delay() calls.
  void loop();

  // ── Uygulamadan gelen komutlar / Commands from the app ──────────
  void onSwitch(const char *key, BlueGraysSwitchHandler handler);
  void onNumber(const char *key, BlueGraysNumberHandler handler);
  void onColor(const char *key, BlueGraysColorHandler handler);
  void onChoice(const char *key, BlueGraysChoiceHandler handler);

  // ── Uygulamaya giden değerler / Values sent to the app ──────────
  // Değer değişmediyse gönderilmez; hız sınırını kütüphane uygular.
  // Unchanged values are not sent; the library applies the rate limit.
  bool send(const char *key, float value);
  bool send(const char *key, double value) { return send(key, (float)value); }
  bool send(const char *key, int value) { return send(key, (float)value); }
  bool send(const char *key, long value) { return send(key, (float)value); }
  bool send(const char *key, bool value);
  bool send(const char *key, const char *value);
  bool sendColor(const char *key, uint8_t r, uint8_t g, uint8_t b);

  // Sayılarda en küçük değişim (ör. 0.1 °C) / Minimum change for numbers
  void setDeadband(const char *key, float delta);

  // ── Olaylar / Events ─────────────────────────────────────────────
  void onConnect(BlueGraysEventHandler handler) { _onConnect = handler; }
  void onDisconnect(BlueGraysEventHandler handler) { _onDisconnect = handler; }
  void onError(BlueGraysErrorHandler handler) { _onError = handler; }

  // ── Ayarlar / Settings ───────────────────────────────────────────
  void setDebug(Stream &stream) { _debug = &stream; }
  void setLanguage(BlueGraysLanguage lang) { _lang = lang; }
  // Bağlanınca son durumu komut fonksiyonlarına uygula (varsayılan açık).
  // Apply the last known state to command handlers on connect (default on).
  void restoreOnConnect(bool enabled) { _restore = enabled; }
  // Test için farklı sunucu / A different server, for testing
  void setServer(const char *host, uint16_t port = 443, const char *path = BG_DEVICE_PATH);
  // Kendi kök sertifikaların (PEM) / Your own root certificates (PEM)
  void setRootCA(const char *pem) { _rootCA = pem; }

  bool connected() const { return _state == BG_STATE_ONLINE; }
  BlueGraysState state() const { return _state; }
  const char *version() const { return BLUEGRAYS_IOT_VERSION; }

private:
  enum VarType : uint8_t { VT_NONE = 0, VT_BOOL, VT_NUMBER, VT_COLOR, VT_TEXT };

  struct Var {
    char key[BG_MAX_KEY_LEN + 1];
    VarType type;
    bool isCommand;
    // Son gönderilen ve bekleyen değer / Last sent and pending value
    float lastNum;
    float pendingNum;
    char lastText[BG_MAX_TEXT_LEN + 1];
    char pendingText[BG_MAX_TEXT_LEN + 1];
    bool hasLast;
    bool hasPending;
    float deadband;
    uint32_t lastSentMs;
    BlueGraysSwitchHandler onSwitch;
    BlueGraysNumberHandler onNumber;
    BlueGraysColorHandler onColor;
    BlueGraysChoiceHandler onChoice;
  };

  const char *_key;
  const char *_ssid = nullptr;
  const char *_pass = nullptr;
  bool _manageWifi = false;
  const char *_host = BG_HOST;
  uint16_t _port = BG_PORT;
  const char *_path = BG_DEVICE_PATH;
  const char *_rootCA;

  WebSocketsClient _ws;
  BlueGraysState _state = BG_STATE_IDLE;
  uint32_t _stateSinceMs = 0;
  uint32_t _retryDelayMs = BG_RETRY_MIN_MS;
  uint32_t _minIntervalMs = BG_DEFAULT_MIN_INTERVAL_MS;
  uint32_t _lastWifiTryMs = 0;
  bool _wsStarted = false;
  bool _kicked = false;
  bool _restore = true;

  Var _vars[BG_MAX_VARS];
  uint8_t _varCount = 0;

  Stream *_debug = nullptr;
  BlueGraysLanguage _lang = BG_LANG_EN;
  char _authHeader[96];

  BlueGraysEventHandler _onConnect;
  BlueGraysEventHandler _onDisconnect;
  BlueGraysErrorHandler _onError;

  bool _timeStarted = false;
  char _deviceName[33];

  Var *_find(const char *key);
  Var *_findOrAdd(const char *key, VarType type, bool isCommand);
  void _setState(BlueGraysState s);
  void _handleWifi();
  void _handleTime();
  void _startSocket();
  void _setRetry(uint32_t delayMs);
  void _onWsEvent(WStype_t type, uint8_t *payload, size_t length);
  void _handleMessage(const char *json, size_t length);
  void _applyCommand(Var *v, JsonVariantConst value);
  void _sendHello();
  void _sendAck(const char *id, Var *v);
  bool _queue(Var *v, float num, const char *text);
  void _flush();
  bool _transmit(Var *v);
  void _log(const __FlashStringHelper *en, const __FlashStringHelper *tr, const char *extra = nullptr);
  static bool _parseColor(const char *hex, uint8_t &r, uint8_t &g, uint8_t &b);
};

#endif // BLUEGRAYS_IOT_H
