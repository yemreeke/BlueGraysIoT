/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · RelayLamp                                   ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  Röle ile lambayı telefondan aç-kapat.                       ║
 * ║  Switch a lamp through a relay from your phone.              ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bağlantı / Wiring
 *   Röle VCC → 5V (VIN / VBUS)    Röle GND → GND
 *   Röle IN  → ESP32 GPIO 26  ·  Pico W GP15
 *
 * 220 V ile çalışacaksan bunu bilen biriyle yap. Önce LED ile dene.
 * If you switch mains voltage, do it with someone experienced.
 *
 * Uygulama şablonu / App template: "Röle ile lamba"
 */

#include <BlueGraysIoT.h>

const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#if defined(ARDUINO_ARCH_ESP32)
const int RELAY_PIN = 26;
#else
const int RELAY_PIN = 15;
#endif
// Çoğu röle modülü LOW ile çeker / Most relay modules switch on LOW
const bool RELAY_ACTIVE_LOW = true;

BlueGraysIoT bg(DEVICE_KEY);

void setRelay(bool on) {
  digitalWrite(RELAY_PIN, (on != RELAY_ACTIVE_LOW) ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);

  // Elektrik gidip gelince son durum sunucudan gelir ve buraya uygulanır.
  // After a power cut the last state comes from the server and lands here.
  bg.onSwitch("lamba", setRelay);

  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();
}
