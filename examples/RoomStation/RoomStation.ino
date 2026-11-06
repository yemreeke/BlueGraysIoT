/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · RoomStation                                 ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  Lamba kontrolü ve sıcaklık-nem aynı kartta.                 ║
 * ║  Lamp control plus temperature and humidity on one board.    ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bağlantı / Wiring
 *   DHT22 +   → 3V3              DHT22 −  → GND
 *   DHT22 OUT → ESP32 GPIO 4   ·  Pico W GP16
 *   Röle VCC  → 5V (VIN / VBUS)  Röle GND → GND
 *   Röle IN   → ESP32 GPIO 26  ·  Pico W GP15
 *
 * Uygulama şablonu / App template: "Oda istasyonu"
 */

#include <BlueGraysIoT.h>
#include <DHT.h>

const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#if defined(ARDUINO_ARCH_ESP32)
const int DHT_PIN = 4;
const int RELAY_PIN = 26;
#else
const int DHT_PIN = 16;
const int RELAY_PIN = 15;
#endif
const bool RELAY_ACTIVE_LOW = true;

BlueGraysIoT bg(DEVICE_KEY);
DHT dht(DHT_PIN, DHT22);

unsigned long lastReadMs = 0;

void setRelay(bool on) {
  digitalWrite(RELAY_PIN, (on != RELAY_ACTIVE_LOW) ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);
  dht.begin();

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);
  bg.onSwitch("lamba", setRelay);
  bg.setDeadband("sicaklik", 0.1);
  bg.setDeadband("nem", 1.0);

  bg.onConnect([]() { Serial.println("Uygulamada çevrimiçi görünüyorum / Online in the app"); });

  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();

  if (millis() - lastReadMs >= 2000) {
    lastReadMs = millis();
    bg.send("sicaklik", dht.readTemperature());
    bg.send("nem", dht.readHumidity());
  }
}
