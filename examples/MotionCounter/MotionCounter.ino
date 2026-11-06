/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · MotionCounter                               ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  Hareket olunca gör, bugün kaç kez algılandığını say.        ║
 * ║  See motion and count how many times it happened today.      ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bağlantı / Wiring
 *   PIR HC-SR501 VCC → 5V (VIN / VBUS)   GND → GND
 *   PIR OUT → ESP32 GPIO 33  ·  Pico W GP13
 *
 * Uygulama şablonu / App template: "Hareket algılayıcı"
 * Uygulamadaki "sıfırla" düğmesi sayacı sıfırlar.
 * The "reset" button in the app clears the counter.
 */

#include <BlueGraysIoT.h>

const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#if defined(ARDUINO_ARCH_ESP32)
const int PIR_PIN = 33;
#else
const int PIR_PIN = 13;
#endif

BlueGraysIoT bg(DEVICE_KEY);

bool lastMotion = false;
long counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);
  // Düğme gibi çalışır: sıfırlar ve kendini kapatır.
  // Works like a button: resets the counter and switches itself off.
  bg.onSwitch("sifirla", [](bool on) {
    if (!on) return;
    counter = 0;
    bg.send("sifirla", false);
  });
  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();

  bool motion = digitalRead(PIR_PIN) == HIGH;
  if (motion && !lastMotion) counter++;  // yükselen kenar / rising edge
  lastMotion = motion;

  bg.send("hareket", motion);
  bg.send("sayac", counter);
}
