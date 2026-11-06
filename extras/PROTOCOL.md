# BlueGrays IoT protocol, version 1

The device and the BlueGrays IoT gateway talk JSON text frames over a secure WebSocket. This document is the contract between this library and the server.

## Connection

- URL: `wss://iot.bluegrays.com/d`, subprotocol `bluegrays-iot-1`.
- Header: `Authorization: Bearer bgd_<32 random bytes, base62>`.
- Header: `X-BG-Lib: BlueGraysIoT/<version> <esp32|rp2040>`.
- TLS only. The server stores a SHA-256 hash of the key, never the key itself.
- Ping every 25 s; the server closes connections silent for 60 s.
- One active connection per key. A second connection with the same key closes the first with a `kick`.

## Device to server

| Message | Meaning |
|---|---|
| `{"t":"hello","fw":"0.1.0","vars":{"lamba":"bool","sicaklik":"number"}}` | First frame after the socket opens. `vars` lists what the sketch registered; informational, the server's template is the source of truth. |
| `{"t":"set","k":"sicaklik","v":23.4}` | A value changed. Types: `bool`, `number`, `color` (`"#RRGGBB"`), `text`. |
| `{"t":"ack","id":"c81","v":false}` | A command was applied; `v` is the resulting value. |

## Server to device

| Message | Meaning |
|---|---|
| `{"t":"ok","name":"Çalışma odası","rate":1000,"vars":{"lamba":true}}` | Key accepted. `rate` is the minimum time between two sends of one variable, in ms. `vars` holds the last known command values for state restore. |
| `{"t":"cmd","id":"c81","k":"lamba","v":false}` | Apply a command and answer with `ack`. |
| `{"t":"err","c":"unknown_var","k":"motor"}` | A frame was rejected. Codes: `unknown_var`, `bad_type`, `out_of_range`, `rate_limited`, `too_large`, `bad_key`, `locked`. |
| `{"t":"kick","r":"dup_key"}` | Another board connected with the same key; the server closes this socket next. The library waits 60 s before retrying. |

## Rules enforced by the server

- Variable names: `[a-z0-9_]{1,24}`, must exist in the device template.
- Frame size: 512 bytes maximum.
- Rate: per device token bucket; extra frames are merged, persistent abuse closes the socket.
- History: only variables marked for charts are stored, one point per minute, within the plan's retention.
- Three duplicate-key kicks within 10 minutes lock the device until the key is renewed in the app.
