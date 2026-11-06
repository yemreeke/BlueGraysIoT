# BlueGrays IoT

Control **ESP32** and **Raspberry Pi Pico W** boards from the [BlueGrays](https://bluegrays.com) app: switches, sliders, colors and live sensor values with charts, over a secure WebSocket.

[Türkçe](#türkçe) · [Examples](#examples) · [API](#api) · [Protocol](extras/PROTOCOL.md) · [bluegrays.com](https://bluegrays.com)

> **Status: 0.1.0 preview.** The BlueGrays IoT service is opening in stages. The API below is stable enough to build on; breaking changes will bump the minor version until 1.0.

## Why

- **Made for beginners.** The BlueGrays app generates the full sketch with your pins, your Wi-Fi and your device key. Paste, upload, done.
- **Secure by default.** TLS only (port 443), with root certificates bundled for the CAs Cloudflare uses. Every device has its own key; it travels only in the connection header and is never printed.
- **Honest limits on the server.** Device count, variables, chart history and rate limits are enforced by the server, not by this code, so editing the sketch cannot bypass them.
- **Survives real life.** Automatic Wi-Fi and server reconnects with backoff, ping every 25 s, values measured while offline are sent on reconnect, and the last switch state is restored after a power cut.
- **No busy polling.** Values are sent only when they change (optional deadband) and never faster than the plan allows.

## Supported boards

| Board | Core | Status |
|---|---|---|
| ESP32 (Dev Module, WROOM, S3, C3) | [esp32 by Espressif](https://github.com/espressif/arduino-esp32) 3.x | supported |
| Raspberry Pi Pico W / Pico 2 W | [arduino-pico by Earle Philhower](https://github.com/earlephilhower/arduino-pico) | supported |
| ESP8266 | | planned |

## Install

**Arduino IDE:** Sketch › Include Library › Manage Libraries, search **BlueGraysIoT**, click Install and accept the dependencies.

**PlatformIO:**

```ini
lib_deps = yemreeke/BlueGraysIoT
```

Dependencies: [WebSockets](https://github.com/Links2004/arduinoWebSockets) (≥ 2.6), [ArduinoJson](https://arduinojson.org) (≥ 7.0), [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) (only for the sensor examples).

## Quick start

1. In the BlueGrays app: **Workshop › My devices › Add device**, pick a template.
2. The app shows the sketch with your device key. Copy it.
3. Upload it from Arduino IDE and open the Serial Monitor at 115200.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_PASTE_YOUR_DEVICE_KEY");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool on) { digitalWrite(LED_BUILTIN, on); });

  bg.begin("YOUR_WIFI_NAME", "YOUR_WIFI_PASSWORD");
}

void loop() {
  bg.loop();                               // no delay() here
  bg.send("sicaklik", readTemperature());  // sent only when it changes
}
```

Serial Monitor:

```
BlueGrays IoT: connecting to Wi-Fi: YOUR_WIFI_NAME
BlueGrays IoT: Wi-Fi connected, IP 192.168.1.42
BlueGrays IoT: connecting to the server…
BlueGrays IoT: secure connection open, checking the key…
BlueGrays IoT: connected (Workroom)
```

## Examples

| Example | Parts | App template |
|---|---|---|
| [RemoteLED](examples/RemoteLED) | none, on-board LED | Röle ile lamba |
| [RelayLamp](examples/RelayLamp) | relay module | Röle ile lamba |
| [TemperatureHumidity](examples/TemperatureHumidity) | DHT22 | Sıcaklık ve nem |
| [RoomStation](examples/RoomStation) | DHT22 + relay | Oda istasyonu |
| [DoorSensor](examples/DoorSensor) | reed switch | Kapı / pencere |
| [MotionCounter](examples/MotionCounter) | PIR HC-SR501 | Hareket algılayıcı |

## API

```cpp
BlueGraysIoT bg(deviceKey);

void begin(const char *ssid, const char *password);  // the library manages Wi-Fi
void begin();                                         // you connected Wi-Fi yourself
void loop();                                          // call every pass of loop()

// Commands from the app
void onSwitch(const char *key, std::function<void(bool)>);
void onNumber(const char *key, std::function<void(float)>);
void onColor (const char *key, std::function<void(uint8_t r, uint8_t g, uint8_t b)>);
void onChoice(const char *key, std::function<void(const char *)>);

// Values to the app (skipped when unchanged; NaN is ignored)
bool send(const char *key, float | int | long | bool | const char *value);
bool sendColor(const char *key, uint8_t r, uint8_t g, uint8_t b);
void setDeadband(const char *key, float delta);   // e.g. 0.1 °C

// Events and settings
void onConnect(std::function<void()>);
void onDisconnect(std::function<void()>);
void onError(std::function<void(const char *code, const char *key)>);
void setDebug(Stream &);                  // status lines on Serial
void setLanguage(BG_LANG_EN | BG_LANG_TR);
void restoreOnConnect(bool);              // default true
void setServer(host, port, path);         // testing only
void setRootCA(const char *pem);          // your own roots
bool connected();
BlueGraysState state();
```

Variable names are up to 24 characters and must match the template in the app. Unknown names are rejected by the server and reported through `onError` with the code `unknown_var`.

Compile-time settings live in [`BlueGraysIoTConfig.h`](src/BlueGraysIoTConfig.h); define them before the include to change them.

## Troubleshooting

| Serial message | What to do |
|---|---|
| `connecting to Wi-Fi` repeats | The board joins **2.4 GHz** networks only. Check the name and password, uppercase and lowercase. |
| `the device key looks wrong` | Copy the sketch again from the app; the key starts with `bgd_`. |
| `this key is in use on another board` | The same sketch runs on two boards. Each board needs its own device in the app. |
| `server says: locked` | Too many duplicate connections. Renew the key in the app and upload the new sketch. |
| `server says: unknown_var` | The variable name in the sketch does not exist in the app template. |
| Upload stuck at `Connecting…` (ESP32) | Hold the **BOOT** button until the upload starts. |

## Security

- Report security issues privately to **info@bluegrays.com**, not in public issues.
- Never share a sketch that contains your device key. If you did, renew the key in the app; the old key stops working immediately.
- Your Wi-Fi password stays in the sketch on your board. It is never sent to BlueGrays.

## License

[MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays)

---

## Türkçe

**BlueGrays IoT**, ESP32 ve Raspberry Pi Pico W kartlarını BlueGrays uygulamasından yönetmeni sağlar. Telefondan lambayı aç, sıcaklığı canlı izle, LED şeridin rengini değiştir.

**Nasıl kurulur**

1. Arduino IDE'de Taslak › Kütüphane Ekle › Kütüphaneleri Yönet'i aç, **BlueGraysIoT** yaz ve Kur'a bas. Bağımlılıkları sorarsa **Tümünü kur** de.
2. BlueGrays uygulamasında Atölyem › Cihazlarım › Yeni cihaz ekle'ye gir, şablonunu seç.
3. Uygulamanın hazırladığı kodu kopyala, Arduino IDE'ye yapıştır ve yükle.
4. Seri Port Ekranı'nı 115200 hızında aç. `BlueGrays IoT: bağlandı` yazısını gördüğünde telefonundan yönetebilirsin.

**Bilmen gerekenler**

- Kart yalnız **2,4 GHz** Wi-Fi ağlarına bağlanır.
- Her kartın kendi cihaz anahtarı var. Aynı kodu iki karta yüklersen ilk kart bağlantıyı kaybeder.
- Wi-Fi şifren kartta kalır, sunucumuza gönderilmez.
- Sınırlar (cihaz sayısı, grafik, hız) sunucuda kontrol edilir; kodu değiştirmek bunları aşmaz.
- Kodu paylaşma. Paylaştıysan uygulamada anahtarı yenile, eskisi hemen geçersiz olur.

Durum mesajlarını Türkçe görmek için `bg.setLanguage(BG_LANG_TR);` ekle. Uygulamanın ürettiği kodda bu satır zaten var.

Projeler, devre şemaları ve uygulama: [bluegrays.com](https://bluegrays.com)
