/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · RemoteLED                                   ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ║  Kartın üstündeki LED'i telefondan aç-kapat.                 ║
 * ║  Turn the on-board LED on and off from your phone.           ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Ek parça gerekmez; ilk deneme için en kolayı.
 * No extra parts needed; the easiest first test.
 *
 * Uygulama / App: Atölyem › Cihazlarım › Yeni cihaz › "Röle ile lamba"
 */

#include <BlueGraysIoT.h>

// Uygulamanın verdiği anahtar / Key from the app
const char *DEVICE_KEY = "bgd_PASTE_YOUR_DEVICE_KEY";
// 2,4 GHz Wi-Fi ağın / Your 2.4 GHz Wi-Fi network
const char *WIFI_NAME = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // çoğu ESP32 geliştirme kartı / most ESP32 dev boards
#endif

BlueGraysIoT bg(DEVICE_KEY);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);

  // Uygulamadaki "lamba" anahtarı / The "lamba" switch in the app
  bg.onSwitch("lamba", [](bool on) {
    digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
  });

  bg.begin(WIFI_NAME, WIFI_PASSWORD);
}

void loop() {
  bg.loop();  // delay() kullanma / do not use delay()
}
