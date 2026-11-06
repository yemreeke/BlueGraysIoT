# BlueGrays IoT

Controla placas **ESP32** y **Raspberry Pi Pico W** desde la app [BlueGrays](https://bluegrays.com): interruptores, deslizadores, selector de color y valores de sensores en vivo con gráficas, a través de un WebSocket cifrado.

[English](README.md) · [Türkçe](README.tr.md) · [Deutsch](README.de.md) · **Español** · [Français](README.fr.md) · [Italiano](README.it.md) · [日本語](README.ja.md) · [Português](README.pt.md)

> **Estado: versión preliminar 0.1.0.** El servicio BlueGrays IoT se abre por etapas. La referencia completa de la API está en el [README en inglés](README.md#api).

## Qué ofrece

- **Pensada para principiantes.** La app BlueGrays genera el sketch completo con tus pines, tu Wi-Fi y la clave de tu dispositivo. Pegar, subir y listo.
- **Segura por defecto.** Solo TLS (puerto 443) con certificados raíz incluidos. Cada dispositivo tiene su propia clave, que solo viaja en la cabecera de conexión y nunca se imprime.
- **Límites en el servidor.** El número de dispositivos, las variables, el historial de gráficas y la frecuencia de envío los controla el servidor; modificar el sketch no los evita.
- **Resistente.** Reconexión automática de Wi-Fi y servidor, ping cada 25 s, los valores medidos sin conexión se envían al reconectar y el estado de los interruptores se recupera tras un corte de luz.

Compatible: ESP32 (núcleo esp32 3.x) y Raspberry Pi Pico W / Pico 2 W (arduino-pico). ESP8266, más adelante.

## Instalación

**Arduino IDE:** Programa › Incluir biblioteca › Administrar bibliotecas, busca **BlueGraysIoT**, pulsa Instalar y acepta las dependencias.

**PlatformIO:** `lib_deps = yemreeke/BlueGraysIoT`

## Inicio rápido

1. En la app BlueGrays: **Mi Taller › Mis dispositivos › Añadir dispositivo** y elige una plantilla.
2. La app muestra el sketch con la clave de tu dispositivo. Cópialo.
3. Súbelo desde Arduino IDE y abre el Monitor Serie a 115200.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_TU_CLAVE_DE_DISPOSITIVO");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool encendido) { digitalWrite(LED_BUILTIN, encendido); });

  bg.begin("TU_WIFI", "TU_CONTRASENA");
}

void loop() {
  bg.loop();                                // sin delay() aquí
  bg.send("sicaklik", leerTemperatura());   // solo se envía si cambia
}
```

Los ejemplos de la carpeta [examples](examples) coinciden con las plantillas de la app: LED remoto, lámpara con relé, temperatura y humedad (DHT22), estación de habitación, sensor de puerta y contador de movimiento.

## Solución de problemas

| Mensaje en el Monitor Serie | Qué hacer |
|---|---|
| `connecting to Wi-Fi` se repite | La placa solo se conecta a redes de **2,4 GHz**. Revisa nombre y contraseña (mayúsculas y minúsculas). |
| `the device key looks wrong` | Copia de nuevo el sketch desde la app; la clave empieza por `bgd_`. |
| `this key is in use on another board` | El mismo sketch funciona en dos placas. Crea un dispositivo distinto en la app para cada placa. |
| `server says: locked` | Demasiadas conexiones duplicadas. Renueva la clave en la app y sube el nuevo sketch. |
| La subida se queda en `Connecting…` (ESP32) | Mantén pulsado el botón **BOOT** hasta que empiece la subida. |

## Seguridad

Informa de problemas de seguridad en privado a **info@bluegrays.com**. No compartas un sketch que contenga tu clave; si lo hiciste, renueva la clave en la app. Tu contraseña Wi-Fi se queda en la placa y nunca se envía a BlueGrays.

[Licencia MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
