# BlueGrays IoT

Steuere **ESP32** und **Raspberry Pi Pico W** mit der [BlueGrays](https://bluegrays.com)-App: Schalter, Schieberegler, Farbwähler und Live-Sensorwerte mit Diagrammen, über einen verschlüsselten WebSocket.

[English](README.md) · [Türkçe](README.tr.md) · **Deutsch** · [Español](README.es.md) · [Français](README.fr.md) · [Italiano](README.it.md) · [日本語](README.ja.md) · [Português](README.pt.md)

> **Status: Vorschau 0.1.0.** Der BlueGrays-IoT-Dienst startet schrittweise. Die vollständige API-Beschreibung steht im [englischen README](README.md#api).

## Was die Bibliothek kann

- **Für Einsteiger gemacht.** Die BlueGrays-App erzeugt den kompletten Sketch mit deinen Pins, deinem WLAN und deinem Geräteschlüssel. Einfügen, hochladen, fertig.
- **Standardmäßig sicher.** Nur TLS (Port 443), Root-Zertifikate sind enthalten. Jedes Gerät hat einen eigenen Schlüssel, der nur im Verbindungs-Header übertragen und nie ausgegeben wird.
- **Grenzen auf dem Server.** Geräteanzahl, Variablen, Diagrammverlauf und Senderate prüft der Server. Änderungen am Sketch umgehen sie nicht.
- **Robust im Alltag.** Automatische Wiederverbindung für WLAN und Server, Ping alle 25 s, offline gemessene Werte werden nachgereicht, Schalterzustände nach einem Stromausfall wiederhergestellt.

Unterstützt: ESP32 (esp32-Core 3.x) und Raspberry Pi Pico W / Pico 2 W (arduino-pico). ESP8266 ist geplant.

## Installation

**Arduino IDE:** Sketch › Bibliothek einbinden › Bibliotheken verwalten, nach **BlueGraysIoT** suchen, Installieren klicken und die Abhängigkeiten mitinstallieren.

**PlatformIO:** `lib_deps = yemreeke/BlueGraysIoT`

## Schnellstart

1. In der BlueGrays-App: **Werkstatt › Meine Geräte › Gerät hinzufügen**, eine Vorlage wählen.
2. Die App zeigt den Sketch mit deinem Geräteschlüssel. Kopieren.
3. In der Arduino IDE hochladen und den seriellen Monitor mit 115200 Baud öffnen.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_DEIN_GERAETESCHLUESSEL");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool an) { digitalWrite(LED_BUILTIN, an); });

  bg.begin("DEIN_WLAN", "DEIN_PASSWORT");
}

void loop() {
  bg.loop();                                // hier kein delay()
  bg.send("sicaklik", temperaturLesen());   // wird nur bei Änderung gesendet
}
```

Die Beispiele (Ordner [examples](examples)) passen zu den Vorlagen der App: Fern-LED, Relais-Lampe, Temperatur und Luftfeuchte (DHT22), Raumstation, Türsensor, Bewegungszähler.

## Fehlerbehebung

| Meldung im seriellen Monitor | Lösung |
|---|---|
| `connecting to Wi-Fi` wiederholt sich | Das Board verbindet sich nur mit **2,4-GHz**-Netzen. Name und Passwort prüfen (Groß- und Kleinschreibung). |
| `the device key looks wrong` | Sketch erneut aus der App kopieren; der Schlüssel beginnt mit `bgd_`. |
| `this key is in use on another board` | Derselbe Sketch läuft auf zwei Boards. Lege für jedes Board ein eigenes Gerät in der App an. |
| `server says: locked` | Zu viele doppelte Verbindungen. Schlüssel in der App erneuern und neuen Sketch hochladen. |
| Upload hängt bei `Connecting…` (ESP32) | Die **BOOT**-Taste gedrückt halten, bis der Upload startet. |

## Sicherheit

Sicherheitsprobleme bitte privat an **info@bluegrays.com** melden. Teile keinen Sketch mit deinem Geräteschlüssel; falls doch, erneuere den Schlüssel in der App. Dein WLAN-Passwort bleibt auf dem Board und wird nie an BlueGrays gesendet.

[MIT-Lizenz](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
