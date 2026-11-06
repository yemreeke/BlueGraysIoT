# BlueGrays IoT

**ESP32** と **Raspberry Pi Pico W** を [BlueGrays](https://bluegrays.com) アプリから操作するための Arduino ライブラリです。スイッチ、スライダー、カラー選択、グラフ付きのリアルタイムなセンサー値を、暗号化された WebSocket でやり取りします。

[English](README.md) · [Türkçe](README.tr.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Français](README.fr.md) · [Italiano](README.it.md) · **日本語** · [Português](README.pt.md)

> **状態：プレビュー版 0.1.0。** BlueGrays IoT サービスは段階的に公開中です。API の詳細は[英語版 README](README.md#api) を参照してください。

## 特長

- **初心者向け。** BlueGrays アプリが、ピン・Wi-Fi・デバイスキーを含むスケッチをまるごと生成します。貼り付けて書き込むだけです。
- **最初から安全。** 通信は TLS のみ（ポート 443）、ルート証明書を同梱しています。デバイスごとに専用のキーがあり、接続ヘッダーでのみ送信され、シリアルに表示されることはありません。
- **制限はサーバー側。** デバイス数、変数、グラフ履歴、送信頻度はサーバーが管理します。スケッチを書き換えても回避できません。
- **実際の使用に強い。** Wi-Fi とサーバーへの自動再接続、25 秒ごとの ping、オフライン中に測った値の再送、停電後のスイッチ状態の復元に対応しています。

対応ボード：ESP32（esp32 コア 3.x）、Raspberry Pi Pico W / Pico 2 W（arduino-pico）。ESP8266 は対応予定です。

## インストール

**Arduino IDE：** スケッチ › ライブラリをインクルード › ライブラリを管理 で **BlueGraysIoT** を検索し、インストールを押して依存ライブラリも一緒にインストールします。

**PlatformIO：** `lib_deps = yemreeke/BlueGraysIoT`

## クイックスタート

1. BlueGrays アプリで **マイ工房 › マイデバイス › デバイスを追加** を開き、テンプレートを選びます。
2. アプリにデバイスキー入りのスケッチが表示されるので、コピーします。
3. Arduino IDE から書き込み、シリアルモニタを 115200 で開きます。

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_YOUR_DEVICE_KEY");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);

  bg.onSwitch("lamba", [](bool on) { digitalWrite(LED_BUILTIN, on); });

  bg.begin("WIFI_NAME", "WIFI_PASSWORD");
}

void loop() {
  bg.loop();                               // ここで delay() は使わない
  bg.send("sicaklik", readTemperature());  // 値が変わったときだけ送信
}
```

[examples](examples) フォルダのサンプルは、アプリのテンプレートに対応しています：リモート LED、リレーで点灯するランプ、温度・湿度（DHT22）、部屋ステーション、ドアセンサー、人感カウンター。

## トラブルシューティング

| シリアルモニタのメッセージ | 対処 |
|---|---|
| `connecting to Wi-Fi` が繰り返される | ボードは **2.4 GHz** の Wi-Fi にのみ接続できます。ネットワーク名とパスワード（大文字・小文字）を確認してください。 |
| `the device key looks wrong` | アプリからスケッチをもう一度コピーしてください。キーは `bgd_` で始まります。 |
| `this key is in use on another board` | 同じスケッチが 2 台のボードで動いています。ボードごとにアプリで別のデバイスを追加してください。 |
| `server says: locked` | 重複接続が多すぎます。アプリでキーを再発行し、新しいスケッチを書き込んでください。 |
| 書き込みが `Connecting…` で止まる（ESP32） | 書き込みが始まるまで **BOOT** ボタンを押し続けてください。 |

## セキュリティ

セキュリティ上の問題は公開の Issue ではなく **info@bluegrays.com** までご連絡ください。デバイスキーを含むスケッチは共有しないでください。共有してしまった場合は、アプリでキーを再発行すると古いキーはすぐに無効になります。Wi-Fi のパスワードはボード上に残り、BlueGrays に送信されることはありません。

[MIT ライセンス](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
