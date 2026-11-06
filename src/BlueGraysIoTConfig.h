/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  BlueGrays IoT · ayarlar / settings                          ║
 * ║  BlueGrays · bluegrays.com                                   ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Bu değerleri değiştirmek için #include <BlueGraysIoT.h> satırından
 * önce aynı adla #define yaz.
 * To change a value, #define it with the same name before
 * #include <BlueGraysIoT.h>.
 */

#ifndef BLUEGRAYS_IOT_CONFIG_H
#define BLUEGRAYS_IOT_CONFIG_H

// Sunucu / Server
#ifndef BG_HOST
#define BG_HOST "iot.bluegrays.com"
#endif
#ifndef BG_PORT
#define BG_PORT 443
#endif
#ifndef BG_DEVICE_PATH
#define BG_DEVICE_PATH "/d"
#endif

// En fazla değişken sayısı / Maximum number of variables
#ifndef BG_MAX_VARS
#define BG_MAX_VARS 16
#endif
// Değişken adı uzunluğu (sunucu da 24 ile sınırlar) / Key length (server limit is 24 too)
#ifndef BG_MAX_KEY_LEN
#define BG_MAX_KEY_LEN 24
#endif
// Metin değer uzunluğu / Text value length
#ifndef BG_MAX_TEXT_LEN
#define BG_MAX_TEXT_LEN 32
#endif

// Aynı değişken için iki gönderim arası en az süre. Sunucu bağlanınca
// planına göre bunu günceller (ücretsiz 1000 ms, premium 200 ms).
// Minimum time between two sends of the same variable. The server
// updates it on connect according to the plan (free 1000, premium 200).
#ifndef BG_DEFAULT_MIN_INTERVAL_MS
#define BG_DEFAULT_MIN_INTERVAL_MS 1000
#endif

// Yeniden bağlanma bekleme aralığı (artarak) / Reconnect backoff range
#ifndef BG_RETRY_MIN_MS
#define BG_RETRY_MIN_MS 2000
#endif
#ifndef BG_RETRY_MAX_MS
#define BG_RETRY_MAX_MS 60000
#endif
// Anahtar başka kartta kullanılınca bekleme / Wait after a duplicate-key kick
#ifndef BG_KICK_WAIT_MS
#define BG_KICK_WAIT_MS 60000
#endif

// Wi-Fi yeniden deneme aralığı / Wi-Fi retry interval
#ifndef BG_WIFI_RETRY_MS
#define BG_WIFI_RETRY_MS 10000
#endif

// Ping aralığı (Cloudflare boşta bağlantıyı 100 sn'de keser)
// Ping interval (Cloudflare closes idle connections after 100 s)
#ifndef BG_PING_INTERVAL_MS
#define BG_PING_INTERVAL_MS 25000
#endif
#ifndef BG_PONG_TIMEOUT_MS
#define BG_PONG_TIMEOUT_MS 8000
#endif

// TLS için saat eşitleme en fazla bu kadar bekler / Max wait for clock sync
#ifndef BG_TIME_SYNC_TIMEOUT_MS
#define BG_TIME_SYNC_TIMEOUT_MS 15000
#endif

#endif // BLUEGRAYS_IOT_CONFIG_H
