# ESP32 Physical Webhook Button

A lightweight and production-ready ESP32 physical button project for triggering HTTP/HTTPS webhooks such as n8n, Home Assistant, Tasker APIs, ntfy notifications, and custom automation endpoints.

<br />
<img alt="ESP32 Physical Webhook Button" src="https://github.com/mskian/esp32-webhook-button/blob/main/chart.png" />
<br />

This project supports:

- Single physical button trigger
- Multi-action button trigger
- HTTPS webhook support
- n8n integration
- ntfy notifications
- Debounce handling

Perfect for:
- Home automation
- Smart desk buttons
- IoT webhook triggers
- Emergency buttons
- Task automation
- DIY smart home projects

---

# Features

## Single Button Version

- Single press action
- HTTPS GET webhook trigger
- ntfy success/failure notifications
- Stable debounce handling
- WiFi reconnect support
- Low CPU usage
- Optimized for continuous home usage

---
<br />

![ESP32](https://github.com/mskian/esp32-webhook-button/blob/main/photo_2026-05-07_09-17-36.jpg)
![Button](https://github.com/mskian/esp32-webhook-button/blob/main/photo_2026-05-07_09-17-42.jpg)
![ESP32 and Button](https://github.com/mskian/esp32-webhook-button/blob/main/photo_2026-05-07_09-17-46.jpg)

<br />
---

## Multi Action Button Version

Supports:

| Action | Trigger |
|---|---|
| Single Press | Webhook URL 1 |
| Double Press | Webhook URL 2 |
| Long Press | Webhook URL 3 |

Additional features:
- Multi-click detection
- Long press detection
- Production-ready state handling
- Non-blocking logic using `millis()`

---

# Hardware Required

- ESP32 Dev Board
- Push Button / Tactile Switch
- 2 Jumper Wires
- USB Cable

---

# Wiring

## Button Wiring

| ESP32 Pin | Button |
|---|---|
| GPIO 4 | Button Pin 1 |
| GND | Button Pin 2 |

Uses:
- Internal pull-up resistor
- No external resistor needed

---

# Flow Chart

```text
+-------------------+
| ESP32 Boot        |
+-------------------+
          |
          v
+-------------------+
| Connect WiFi      |
+-------------------+
          |
          v
+-------------------+
| Wait for Button   |
+-------------------+
          |
          v
+-------------------+
| Detect Press Type |
+-------------------+
   |       |       |
   |       |       |
   v       v       v
Single   Double   Long
Press    Press    Press
   |       |       |
   v       v       v
Trigger Trigger Trigger
URL 1   URL 2   URL 3
   |       |       |
   +-------+-------+
           |
           v
+-------------------+
| Send ntfy Status  |
+-------------------+
```

---

# Supported Platforms

This project works with:

- n8n
- Home Assistant
- ntfy
- Tasker APIs
- Node-RED
- Custom webhook APIs
- REST APIs
- Local network automation

---

# WiFi Configuration

Update:

```cpp
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

---

# Webhook Configuration

## Single Button

```cpp
const char* WEBHOOK_URL =
"https://your-webhook-url";
```

---

## Multi Button

```cpp
const char* WEBHOOK_SINGLE =
"https://your-single-url";

const char* WEBHOOK_DOUBLE =
"https://your-double-url";

const char* WEBHOOK_LONG =
"https://your-long-url";
```

---

# ntfy Notifications

Supports success and failure alerts.

Example:

```cpp
const char* NTFY_URL =
"https://your-ntfy-topic";
```

Notifications include:
- Success triggers
- HTTP failures
- WiFi disconnects
- Webhook errors

---

# Optimizations

This project is optimized for:
- Low CPU usage
- Stable WiFi reconnect
- Safe HTTP cleanup
- Low idle power usage
- Continuous home operation

Techniques used:
- `millis()` based timing
- Debounce protection
- Cooldown protection
- HTTPS timeout handling
- Non-blocking logic

---

# Recommended Usage

Best suited for:
- Home automation
- Personal productivity
- Smart room triggers
- Automation shortcuts
- IoT experiments
- Physical webhook triggers

---

# Example Use Cases

## Single Press
- Toggle lights
- Trigger n8n workflow
- Send Telegram notification

## Double Press
- Trigger alternate automation
- Start media mode
- Enable focus mode

## Long Press
- Emergency alert
- Shutdown workflow
- Panic notification
- Home security action

---

# Arduino IDE Setup

## Install ESP32 Board

Arduino IDE:
- File
- Preferences
- Additional Boards URL:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then:
- Boards Manager
- Install ESP32

---

# Libraries Used

Built-in ESP32 libraries:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
```

No extra libraries required.

---

# Upload Steps

1. Connect ESP32
2. Select ESP32 board
3. Select COM port
4. Upload code
5. Open Serial Monitor
6. Baud rate: `115200`

---

# Serial Monitor Example

```text
[WiFi] Connected
[Button] Single Press
[Single Press] HTTP Code: 200
[System] Single Press Success
```

---

## Youtube

Demo at: **<https://youtube.com/shorts/wLUjxjM3vpE?si=yuezYIr6ZCef1Q6i>**

---

# License

MIT License
