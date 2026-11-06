/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · DoorSensor                                  ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  Kapı açık mı kapalı mı, telefondan gör.                     ║
 * ║  See whether a door is open or closed on your phone.         ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bağlantı / Wiring
 *   Manyetik reed sensörün bir ucu → GND
 *   Diğer ucu → ESP32 GPIO 27  ·  Pico W GP14 (iç pull-up açık)
 *   One reed switch lead → GND, the other → the pin above (internal pull-up)
 *
 * Uygulama şablonu / App template: "Kapı / pencere"
 */

#include <BlueGraysIoT.h>

const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#if defined(ARDUINO_ARCH_ESP32)
const int REED_PIN = 27;
#else
const int REED_PIN = 14;
#endif

BlueGraysIoT bg(DEVICE_KEY);

bool lastRaw = false;
bool stableOpen = false;
unsigned long changedAtMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(REED_PIN, INPUT_PULLUP);

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);
  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();

  // Mıknatıs uzaklaşınca pin HIGH olur: kapı açık.
  // The pin goes HIGH when the magnet moves away: door open.
  bool raw = digitalRead(REED_PIN) == HIGH;
  if (raw != lastRaw) {
    lastRaw = raw;
    changedAtMs = millis();
  }
  // 50 ms titreşim süzgeci / 50 ms debounce
  if (millis() - changedAtMs > 50 && raw != stableOpen) {
    stableOpen = raw;
  }
  bg.send("kapi", stableOpen ? "acik" : "kapali");
}
