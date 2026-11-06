/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · TemperatureHumidity                         ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  DHT22 ile sıcaklık ve nemi canlı izle, grafikte gör.        ║
 * ║  Watch temperature and humidity live with a DHT22.           ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bağlantı / Wiring
 *   DHT22 +   → 3V3      DHT22 −  → GND
 *   DHT22 OUT → ESP32 GPIO 4  ·  Pico W GP16
 *
 * Uygulama şablonu / App template: "Sıcaklık ve nem"
 */

#include <BlueGraysIoT.h>
#include <DHT.h>

const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#if defined(ARDUINO_ARCH_ESP32)
const int DHT_PIN = 4;
#else
const int DHT_PIN = 16;
#endif

BlueGraysIoT bg(DEVICE_KEY);
DHT dht(DHT_PIN, DHT22);

unsigned long lastReadMs = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);

  // Küçük titremeler gönderilmesin / Do not send tiny jitters
  bg.setDeadband("sicaklik", 0.1);
  bg.setDeadband("nem", 1.0);

  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();

  // DHT22 en fazla 2 saniyede bir okunur / DHT22 reads at most every 2 s
  if (millis() - lastReadMs >= 2000) {
    lastReadMs = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    // Okunamazsa (NaN) kütüphane göndermez / NaN readings are skipped
    bg.send("sicaklik", t);
    bg.send("nem", h);
  }
}
