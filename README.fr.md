# BlueGrays IoT

Pilote tes cartes **ESP32** et **Raspberry Pi Pico W** depuis l'application [BlueGrays](https://bluegrays.com) : interrupteurs, curseurs, sélecteur de couleur et valeurs de capteurs en direct avec graphiques, via un WebSocket chiffré.

[English](README.md) · [Türkçe](README.tr.md) · [Deutsch](README.de.md) · [Español](README.es.md) · **Français** · [Italiano](README.it.md) · [日本語](README.ja.md) · [Português](README.pt.md)

> **État : préversion 0.1.0.** Le service BlueGrays IoT ouvre par étapes. La référence complète de l'API se trouve dans le [README anglais](README.md#api).

## Ce qu'elle apporte

- **Pensée pour les débutants.** L'application BlueGrays génère le croquis complet avec tes broches, ton Wi-Fi et la clé de ton appareil. Coller, téléverser, c'est tout.
- **Sécurisée par défaut.** TLS uniquement (port 443), certificats racine inclus. Chaque appareil a sa propre clé, envoyée seulement dans l'en-tête de connexion et jamais affichée.
- **Limites côté serveur.** Nombre d'appareils, variables, historique des graphiques et fréquence d'envoi sont contrôlés par le serveur ; modifier le croquis ne les contourne pas.
- **Robuste.** Reconnexion automatique au Wi-Fi et au serveur, ping toutes les 25 s, les valeurs mesurées hors ligne sont envoyées à la reconnexion et l'état des interrupteurs revient après une coupure de courant.

Compatible : ESP32 (cœur esp32 3.x) et Raspberry Pi Pico W / Pico 2 W (arduino-pico). ESP8266 prévu.

## Installation

**Arduino IDE :** Croquis › Inclure une bibliothèque › Gérer les bibliothèques, cherche **BlueGraysIoT**, clique sur Installer et accepte les dépendances.

**PlatformIO :** `lib_deps = yemreeke/BlueGraysIoT`

## Démarrage rapide

1. Dans l'application BlueGrays : **Mon Atelier › Mes appareils › Ajouter un appareil**, choisis un modèle.
2. L'application affiche le croquis avec la clé de ton appareil. Copie-le.
3. Téléverse-le depuis Arduino IDE et ouvre le moniteur série à 115200.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_TA_CLE_APPAREIL");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool allume) { digitalWrite(LED_BUILTIN, allume); });

  bg.begin("TON_WIFI", "TON_MOT_DE_PASSE");
}

void loop() {
  bg.loop();                                 // pas de delay() ici
  bg.send("sicaklik", lireTemperature());    // envoyé seulement s'il change
}
```

Les exemples du dossier [examples](examples) correspondent aux modèles de l'application : LED à distance, lampe sur relais, température et humidité (DHT22), station de pièce, capteur de porte et compteur de mouvements.

## Dépannage

| Message du moniteur série | Que faire |
|---|---|
| `connecting to Wi-Fi` se répète | La carte se connecte uniquement aux réseaux **2,4 GHz**. Vérifie le nom et le mot de passe (majuscules et minuscules). |
| `the device key looks wrong` | Recopie le croquis depuis l'application ; la clé commence par `bgd_`. |
| `this key is in use on another board` | Le même croquis tourne sur deux cartes. Crée un appareil distinct dans l'application pour chaque carte. |
| `server says: locked` | Trop de connexions en double. Renouvelle la clé dans l'application et téléverse le nouveau croquis. |
| Le téléversement bloque sur `Connecting…` (ESP32) | Maintiens le bouton **BOOT** jusqu'au début du téléversement. |

## Sécurité

Signale les problèmes de sécurité en privé à **info@bluegrays.com**. Ne partage pas un croquis contenant ta clé ; si c'est fait, renouvelle la clé dans l'application. Ton mot de passe Wi-Fi reste sur la carte et n'est jamais envoyé à BlueGrays.

[Licence MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
