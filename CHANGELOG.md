# Changelog

## 0.1.0 · 2026-11-06

First preview.

- ESP32 (esp32 core 3.x) and Raspberry Pi Pico W (arduino-pico) support.
- Secure WebSocket to `iot.bluegrays.com` with bundled root certificates (GTS R1, GTS R4, GlobalSign R1, ISRG X1, ISRG X2).
- `onSwitch`, `onNumber`, `onColor`, `onChoice` commands with acknowledgements.
- `send` for numbers, booleans, text and colors; change detection, deadband and server-driven rate limit.
- Automatic Wi-Fi and server reconnects with backoff, 60 s wait after a duplicate-key kick.
- State restore after reconnect, clock sync before TLS.
- English and Turkish status messages.
- Six examples matching the BlueGrays app templates.
