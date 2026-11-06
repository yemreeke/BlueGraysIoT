# BlueGrays IoT

Controle placas **ESP32** e **Raspberry Pi Pico W** pelo app [BlueGrays](https://bluegrays.com): interruptores, controles deslizantes, seletor de cores e valores de sensores ao vivo com gráficos, por um WebSocket criptografado.

[English](README.md) · [Türkçe](README.tr.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Français](README.fr.md) · [Italiano](README.it.md) · [日本語](README.ja.md) · **Português**

> **Status: prévia 0.1.0.** O serviço BlueGrays IoT está abrindo em etapas. A referência completa da API está no [README em inglês](README.md#api).

## O que ela oferece

- **Feita para iniciantes.** O app BlueGrays gera o sketch completo com seus pinos, seu Wi-Fi e a chave do seu dispositivo. Colar, enviar e pronto.
- **Segura por padrão.** Somente TLS (porta 443), com certificados raiz incluídos. Cada dispositivo tem sua própria chave, enviada apenas no cabeçalho da conexão e nunca exibida.
- **Limites no servidor.** Quantidade de dispositivos, variáveis, histórico de gráficos e taxa de envio são controlados pelo servidor; alterar o sketch não os contorna.
- **Resistente.** Reconexão automática ao Wi-Fi e ao servidor, ping a cada 25 s, valores medidos offline são enviados ao reconectar e o estado dos interruptores volta depois de uma queda de energia.

Compatível: ESP32 (núcleo esp32 3.x) e Raspberry Pi Pico W / Pico 2 W (arduino-pico). ESP8266 planejado.

## Instalação

**Arduino IDE:** Rascunho › Incluir biblioteca › Gerenciar bibliotecas, procure **BlueGraysIoT**, clique em Instalar e aceite as dependências.

**PlatformIO:** `lib_deps = yemreeke/BlueGraysIoT`

## Início rápido

1. No app BlueGrays: **Minha oficina › Meus dispositivos › Adicionar dispositivo** e escolha um modelo.
2. O app mostra o sketch com a chave do seu dispositivo. Copie.
3. Envie pelo Arduino IDE e abra o Monitor Serial em 115200.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_SUA_CHAVE_DE_DISPOSITIVO");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool ligado) { digitalWrite(LED_BUILTIN, ligado); });

  bg.begin("SEU_WIFI", "SUA_SENHA");
}

void loop() {
  bg.loop();                               // sem delay() aqui
  bg.send("sicaklik", lerTemperatura());   // enviado só quando muda
}
```

Os exemplos da pasta [examples](examples) correspondem aos modelos do app: LED remoto, lâmpada com relé, temperatura e umidade (DHT22), estação do cômodo, sensor de porta e contador de movimento.

## Solução de problemas

| Mensagem no Monitor Serial | O que fazer |
|---|---|
| `connecting to Wi-Fi` se repete | A placa só se conecta a redes de **2,4 GHz**. Confira o nome e a senha (maiúsculas e minúsculas). |
| `the device key looks wrong` | Copie o sketch de novo pelo app; a chave começa com `bgd_`. |
| `this key is in use on another board` | O mesmo sketch está rodando em duas placas. Crie um dispositivo separado no app para cada placa. |
| `server says: locked` | Conexões duplicadas demais. Renove a chave no app e envie o novo sketch. |
| O envio trava em `Connecting…` (ESP32) | Segure o botão **BOOT** até o envio começar. |

## Segurança

Relate problemas de segurança em particular para **info@bluegrays.com**. Não compartilhe um sketch com a sua chave; se isso acontecer, renove a chave no app. A senha do seu Wi-Fi fica na placa e nunca é enviada à BlueGrays.

[Licença MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
