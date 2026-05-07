#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

*/

// ====================================================
// WIFI CONFIG
// ====================================================

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

// ====================================================
// WEBHOOK URL (GET METHOD)
// ====================================================

const char* WEBHOOK_URL =
"https://your-webhook-url";

// ====================================================
// NTFY
// ====================================================

const char* NTFY_URL =
"https://your-ntfy-topic";

// ====================================================
// BUTTON CONFIG
// ====================================================

#define BUTTON_PIN 4

const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long TRIGGER_COOLDOWN = 3000;

// ====================================================
// GLOBALS
// ====================================================

bool lastReading = HIGH;
bool stableState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long lastTriggerTime = 0;

// ====================================================
// WIFI CONNECT
// ====================================================

bool connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.println();
  Serial.println("[WiFi] Connecting...");

  WiFi.mode(WIFI_STA);

  WiFi.setSleep(true);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

    if (millis() - startTime > 20000) {

      Serial.println();
      Serial.println("[WiFi] Connection timeout");

      return false;
    }
  }

  Serial.println();
  Serial.println("[WiFi] Connected");

  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  return true;
}

// ====================================================
// NTFY SEND
// ====================================================

void sendNtfy(
  const String& title,
  const String& message,
  const String& priority = "default"
) {

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, NTFY_URL)) {

    Serial.println("[NTFY] begin failed");

    return;
  }

  http.setTimeout(8000);

  http.addHeader("Title", title);
  http.addHeader("Priority", priority);
  http.addHeader("Tags", "bell");

  int code = http.POST(message);

  Serial.print("[NTFY] HTTP Code: ");
  Serial.println(code);

  http.end();
}

// ====================================================
// WEBHOOK TRIGGER
// ====================================================

bool triggerWebhook() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("[Webhook] WiFi disconnected");

    sendNtfy(
      "ESP32 Failed",
      "WiFi disconnected",
      "high"
    );

    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, WEBHOOK_URL)) {

    Serial.println("[Webhook] begin failed");

    sendNtfy(
      "ESP32 Failed",
      "Webhook begin failed",
      "high"
    );

    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "User-Agent",
    "ESP32-Webhook-Button/1.0"
  );

  // =========================
  // GET REQUEST
  // =========================

  int code = http.GET();
  Serial.print("[Webhook] HTTP Code: ");
  Serial.println(code);

  String response = http.getString();

  Serial.println("[Webhook] Response:");
  Serial.println(response);

  bool success = false;

  if (code >= 200 && code < 300) {

    success = true;

    sendNtfy(
      "ESP32 Success",
      "n8n webhook triggered successfully"
    );

  } else {

    sendNtfy(
      "ESP32 Failed",
      "Webhook returned HTTP " + String(code),
      "high"
    );
  }

  http.end();

  return success;
}

// ====================================================
// SETUP
// ====================================================

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("==================================");
  Serial.println("ESP32 n8n Button Trigger Ready");
  Serial.println("==================================");

  connectWiFi();
}

// ====================================================
// LOOP
// ====================================================

void loop() {

  // Auto reconnect WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  bool reading = digitalRead(BUTTON_PIN);

  // debounce timer reset
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  // stable debounce check
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {

    if (reading != stableState) {

      stableState = reading;

      // button pressed
      if (stableState == LOW) {

        // cooldown protection
        if ((millis() - lastTriggerTime) > TRIGGER_COOLDOWN) {

          lastTriggerTime = millis();

          Serial.println();
          Serial.println("[Button] Press detected");

          bool result = triggerWebhook();

          if (result) {
            Serial.println("[System] Success");
          } else {
            Serial.println("[System] Failed");
          }

        } else {

          Serial.println("[Button] Cooldown active");
        }
      }
    }
  }

  lastReading = reading;

  delay(5);
}
