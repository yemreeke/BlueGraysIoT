# BlueGrays IoT

**ESP32** ve **Raspberry Pi Pico W** kartlarını [BlueGrays](https://bluegrays.com) uygulamasından yönet: anahtar, kaydırıcı, renk seçici ve grafikli canlı sensör değerleri, hepsi şifreli WebSocket üzerinden.

[English](README.md) · **Türkçe** · [Deutsch](README.de.md) · [Español](README.es.md) · [Français](README.fr.md) · [Italiano](README.it.md) · [日本語](README.ja.md) · [Português](README.pt.md)

> **Durum: 0.1.0 önizleme.** BlueGrays IoT hizmeti aşama aşama açılıyor. 1.0'a kadar uyumsuz değişiklikler ara sürüm numarasını artırır.

## Neden

- **Yeni başlayanlar için.** BlueGrays uygulaması pinlerini, Wi-Fi'ını ve cihaz anahtarını içeren kodun tamamını hazırlar. Yapıştır, yükle, bitti.
- **Varsayılan olarak güvenli.** Yalnız şifreli bağlantı (443), Cloudflare'in kullandığı kök sertifikalar kütüphanenin içinde. Her cihazın kendi anahtarı var; anahtar yalnız bağlantı başlığında gider, hiçbir yere yazdırılmaz.
- **Sınırlar sunucuda.** Cihaz sayısı, değişkenler, grafik geçmişi ve hız sınırı sunucuda kontrol edilir; kodu değiştirmek bunları aşmaz.
- **Gerçek hayata dayanıklı.** Wi-Fi ve sunucu bağlantısı koparsa kendisi artan aralıklarla yeniden dener, 25 saniyede bir ping atar, çevrimdışıyken ölçtüğü değerleri bağlanınca gönderir, elektrik gidip gelince anahtarların son durumunu geri yükler.
- **Boş yere istek yok.** Değer yalnız değişince gönderilir (istersen en küçük değişim payıyla) ve planın izin verdiğinden hızlı gönderilmez.

## Desteklenen kartlar

| Kart | Kart paketi | Durum |
|---|---|---|
| ESP32 (Dev Module, WROOM, S3, C3) | [esp32 by Espressif](https://github.com/espressif/arduino-esp32) 3.x | destekleniyor |
| Raspberry Pi Pico W / Pico 2 W | [arduino-pico by Earle Philhower](https://github.com/earlephilhower/arduino-pico) | destekleniyor |
| ESP8266 | | planlanıyor |

## Kurulum

**Arduino IDE:** Taslak › Kütüphane Ekle › Kütüphaneleri Yönet'i aç, **BlueGraysIoT** yaz, Kur'a bas. Bağımlılıkları sorarsa **Tümünü kur** de.

**PlatformIO:**

```ini
lib_deps = yemreeke/BlueGraysIoT
```

Bağımlılıklar: [WebSockets](https://github.com/Links2004/arduinoWebSockets) (≥ 2.6), [ArduinoJson](https://arduinojson.org) (≥ 7.0), [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) (yalnız sensör örnekleri için).

## Hızlı başlangıç

1. BlueGrays uygulamasında **Atölyem › Cihazlarım › Yeni cihaz ekle**'ye gir, şablonunu seç.
2. Uygulama cihaz anahtarınla birlikte kodu gösterir. Kopyala.
3. Arduino IDE'den yükle, Seri Port Ekranı'nı 115200 hızında aç.

```cpp
#include <BlueGraysIoT.h>

BlueGraysIoT bg("bgd_UYGULAMADAKI_ANAHTARIN");

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  bg.setDebug(Serial);
  bg.setLanguage(BG_LANG_TR);  // durum mesajları Türkçe

  bg.onSwitch("lamba", [](bool acik) { digitalWrite(LED_BUILTIN, acik); });

  bg.begin("WIFI_ADIN", "WIFI_SIFREN");
}

void loop() {
  bg.loop();                               // burada delay() kullanma
  bg.send("sicaklik", sicaklikOku());      // yalnız değişince gönderilir
}
```

Seri Port Ekranı:

```
BlueGrays IoT: Wi-Fi bağlanıyor: WIFI_ADIN
BlueGrays IoT: Wi-Fi bağlandı, IP 192.168.1.42
BlueGrays IoT: sunucuya bağlanıyor…
BlueGrays IoT: güvenli bağlantı açıldı, anahtar kontrol ediliyor…
BlueGrays IoT: bağlandı (Çalışma odası)
```

## Örnekler

| Örnek | Parçalar | Uygulama şablonu |
|---|---|---|
| [RemoteLED](examples/RemoteLED) | yok, karttaki LED | Röle ile lamba |
| [RelayLamp](examples/RelayLamp) | röle modülü | Röle ile lamba |
| [TemperatureHumidity](examples/TemperatureHumidity) | DHT22 | Sıcaklık ve nem |
| [RoomStation](examples/RoomStation) | DHT22 + röle | Oda istasyonu |
| [DoorSensor](examples/DoorSensor) | manyetik reed sensör | Kapı / pencere |
| [MotionCounter](examples/MotionCounter) | PIR HC-SR501 | Hareket algılayıcı |

## Fonksiyonlar

```cpp
BlueGraysIoT bg(cihazAnahtari);

void begin(const char *ssid, const char *sifre);  // Wi-Fi'ı kütüphane yönetir
void begin();                                      // Wi-Fi'ı sen bağladıysan
void loop();                                       // loop() içinde her turda çağır

// Uygulamadan gelen komutlar
void onSwitch(const char *ad, std::function<void(bool)>);
void onNumber(const char *ad, std::function<void(float)>);
void onColor (const char *ad, std::function<void(uint8_t r, uint8_t g, uint8_t b)>);
void onChoice(const char *ad, std::function<void(const char *)>);

// Uygulamaya giden değerler (değişmediyse gönderilmez, NaN atlanır)
bool send(const char *ad, float | int | long | bool | const char *deger);
bool sendColor(const char *ad, uint8_t r, uint8_t g, uint8_t b);
void setDeadband(const char *ad, float pay);   // ör. 0.1 °C

// Olaylar ve ayarlar
void onConnect(std::function<void()>);
void onDisconnect(std::function<void()>);
void onError(std::function<void(const char *kod, const char *ad)>);
void setDebug(Stream &);                  // durum satırları Serial'e
void setLanguage(BG_LANG_EN | BG_LANG_TR);
void restoreOnConnect(bool);              // varsayılan açık
bool connected();
```

Değişken adları en fazla 24 karakterdir ve uygulamadaki şablonla aynı olmalıdır. Bilinmeyen adları sunucu reddeder, `onError` fonksiyonuna `unknown_var` koduyla bildirilir.

Derleme ayarları [`BlueGraysIoTConfig.h`](src/BlueGraysIoTConfig.h) dosyasında; değiştirmek için `#include` satırından önce aynı adla `#define` yaz.

## Sorun giderme

| Seri ekrandaki mesaj | Ne yapmalı |
|---|---|
| `Wi-Fi bağlanıyor` tekrar ediyor | Kart yalnız **2,4 GHz** ağlara bağlanır. Ağ adını ve şifreyi, büyük-küçük harfe dikkat ederek kontrol et. |
| `cihaz anahtarı hatalı görünüyor` | Kodu uygulamadan yeniden kopyala; anahtar `bgd_` ile başlar. |
| `bu anahtar başka bir kartta kullanılıyor` | Aynı kod iki kartta çalışıyor. Her kart için uygulamada ayrı bir cihaz ekle. |
| `sunucu diyor ki: locked` | Çok fazla çakışan bağlantı oldu. Uygulamada anahtarı yenile, yeni kodu yükle. |
| `sunucu diyor ki: unknown_var` | Koddaki değişken adı uygulamadaki şablonda yok. |
| Yükleme `Connecting…` yazısında takılıyor (ESP32) | Yükleme başlayana kadar karttaki **BOOT** düğmesine basılı tut. |

## Güvenlik

- Güvenlik açığı bulursan herkese açık bir konu yerine **info@bluegrays.com** adresine yaz.
- Cihaz anahtarı olan kodu paylaşma. Paylaştıysan uygulamada anahtarı yenile; eski anahtar hemen çalışmaz olur.
- Wi-Fi şifren kartındaki kodda kalır, BlueGrays'e hiç gönderilmez.

## Lisans

[MIT](LICENSE) © 2026 Yunus Emre Eke (BlueGrays) · [bluegrays.com](https://bluegrays.com)
