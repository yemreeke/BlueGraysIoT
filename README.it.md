# BlueGrays IoT

Controlla schede **ESP32** e **Raspberry Pi Pico W** dall'app [BlueGrays](https://bluegrays.com): interruttori, cursori, selettore di colore e valori dei sensori in tempo reale con grafici, tramite un WebSocket cifrato.

[English](README.md) · [Türkçe](README.tr.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Français](README.fr.md) · **Italiano** · [日本語](README.ja.md) · [Português](README.pt.md)

> **Stato: anteprima 0.1.0.** Il servizio BlueGrays IoT apre per fasi. Il riferimento completo dell'API è nel [README in inglese](README.md#api).

## Cosa offre

- **Pensata per chi inizia.** L'app BlueGrays genera lo sketch completo con i tuoi pin, il tuo Wi-Fi e la chiave del dispositivo. Incolla, carica, fatto.
- **Sicura per impostazione.** Solo TLS (porta 443), certificati radice inclusi. Ogni dispositivo ha la propria chiave, inviata solo nell'intestazione di connessione e mai stampata.
- **Limiti sul server.** Numero di dispositivi, variabili, cronologia dei grafici e frequenza di invio sono controllati dal server; modificare lo sketch non li aggira.
- **Robusta.** Riconnessione automatica a Wi-Fi e server, ping ogni 25 s, i valori misurati offline vengono inviati alla riconnessione e lo stato degli interruttori torna dopo un blackout.

Supportate: ESP32 (core esp32 3.x) e Raspberry Pi Pico W / Pico 2 W (arduino-pico). ESP8266 in programma.

## Installazione

**Arduino IDE:** Sketch › #include libreria › Gestione librerie, cerca **BlueGraysIoT**, premi Installa e accetta le dipendenze.

**PlatformIO:** `lib_deps = yemreeke/BlueGraysIoT`

## Avvio rapido

1. Nell'app BlueGrays: **La Mia Officina › I miei dispositivi › Aggiungi dispositivo**, scegli un modello.
2. L'app mostra lo sketch con la chiave del dispositivo. Copialo.
3. Caricalo da Arduino IDE e apri il Monitor seriale a 115200.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_LA_TUA_CHIAVE_DISPOSITIVO");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool acceso) { digitalWrite(LED_BUILTIN, acceso); });

  bg.begin("IL_TUO_WIFI", "LA_TUA_PASSWORD");
}

void loop() {
  bg.loop();                                   // niente delay() qui
  bg.send("sicaklik", leggiTemperatura());     // inviato solo se cambia
}
```

Gli esempi nella cartella [examples](examples) corrispondono ai modelli dell'app: LED remoto, lampada con relè, temperatura e umidità (DHT22), stazione per la stanza, sensore porta e contatore di movimento.

## Risoluzione dei problemi

| Messaggio nel Monitor seriale | Cosa fare |
|---|---|
| `connecting to Wi-Fi` si ripete | La scheda si collega solo a reti **2,4 GHz**. Controlla nome e password (maiuscole e minuscole). |
| `the device key looks wrong` | Copia di nuovo lo sketch dall'app; la chiave inizia con `bgd_`. |
| `this key is in use on another board` | Lo stesso sketch gira su due schede. Crea un dispositivo diverso nell'app per ogni scheda. |
| `server says: locked` | Troppe connessioni duplicate. Rinnova la chiave nell'app e carica il nuovo sketch. |
| Il caricamento si ferma su `Connecting…` (ESP32) | Tieni premuto il pulsante **BOOT** finché il caricamento non parte. |

## Sicurezza

Segnala i problemi di sicurezza in privato a **info@bluegrays.com**. Non condividere uno sketch con la tua chiave; se è successo, rinnova la chiave nell'app. La password Wi-Fi resta sulla scheda e non viene mai inviata a BlueGrays.

[Licenza MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
