#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>


// ====================================================
// WIFI CONFIG
// ====================================================

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

// ====================================================
// WEBHOOK URLS
// ====================================================

// Single Press
const char* WEBHOOK_SINGLE = "https://your-single-url";

// Double Press
const char* WEBHOOK_DOUBLE = "https://your-double-url";

// Long Press
const char* WEBHOOK_LONG = "https://your-long-url";

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

const unsigned long DOUBLE_CLICK_DELAY = 400;

const unsigned long LONG_PRESS_TIME = 2000;

// ====================================================
// GLOBALS
// ====================================================

bool lastReading = HIGH;

bool stableState = HIGH;

bool buttonPressed = false;

unsigned long lastDebounceTime = 0;

unsigned long lastTriggerTime = 0;

unsigned long buttonPressStart = 0;

unsigned long lastClickTime = 0;

int clickCount = 0;

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

bool triggerWebhook(
  const char* webhookUrl,
  const String& actionName
) {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("[Webhook] WiFi disconnected");

    sendNtfy(
      "ESP32 Failed",
      actionName + ": WiFi disconnected",
      "high"
    );

    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, webhookUrl)) {
  Serial.println("[Webhook] begin failed");

    sendNtfy(
      "ESP32 Failed",
      actionName + ": begin failed",
      "high"
    );

    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "User-Agent",
    "ESP32-Webhook-Button/1.0"
  );

  // HTTPS GET request
  int code = http.GET();

  Serial.print("[");
  Serial.print(actionName);
  Serial.print("] HTTP Code: ");

  Serial.println(code);

  String response = http.getString();

  Serial.print("[");
  Serial.print(actionName);
  Serial.println("] Response:");

  Serial.println(response);

  bool success = false;

  if (code >= 200 && code < 300) {

    success = true;

    sendNtfy(
      "ESP32 Success",
      actionName + " triggered successfully"
    );

  } else {

    sendNtfy(
      "ESP32 Failed",
      actionName + " HTTP " + String(code),
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
  Serial.println("ESP32 Multi Action Button Ready");
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

  // Debounce timer reset
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  // Stable debounce check
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {

    if (reading != stableState) {

      stableState = reading;

      // ==================================
      // BUTTON PRESSED
      // ==================================

      if (stableState == LOW) {

        buttonPressStart = millis();

        buttonPressed = true;
      }

      // ==================================
      // BUTTON RELEASED
      // ==================================

      else {

        unsigned long pressDuration =
          millis() - buttonPressStart;

        buttonPressed = false;

        // Global cooldown protection
        if (
          (millis() - lastTriggerTime)
          < TRIGGER_COOLDOWN
        ) {

          Serial.println("[Button] Cooldown active");

          lastReading = reading;

          delay(5);

          return;
        }

        // ==================================
        // LONG PRESS
        // ==================================

        if (pressDuration >= LONG_PRESS_TIME) {

          lastTriggerTime = millis();

          Serial.println();
          Serial.println("[Button] Long Press");

          bool result = triggerWebhook(
            WEBHOOK_LONG,
            "Long Press"
          );

          if (result) {
            Serial.println("[System] Long Press Success");
          } else {
            Serial.println("[System] Long Press Failed");
          }
        }

        // ==================================
        // SHORT PRESS
        // ==================================

        else {

          clickCount++;

          lastClickTime = millis();
        }
      }
    }
  }

  // ==================================
  // SINGLE / DOUBLE CLICK DETECTION
  // ==================================

  if (
    clickCount > 0 &&
    (millis() - lastClickTime)
    > DOUBLE_CLICK_DELAY
  ) {

    // Double Press
    if (clickCount >= 2) {

      lastTriggerTime = millis();

      Serial.println();
      Serial.println("[Button] Double Press");

      bool result = triggerWebhook(
        WEBHOOK_DOUBLE,
        "Double Press"
      );

      if (result) {
        Serial.println("[System] Double Press Success");
      } else {
        Serial.println("[System] Double Press Failed");
      }
    }

    // Single Press
    else {

      lastTriggerTime = millis();
      Serial.println();
      Serial.println("[Button] Single Press");

      bool result = triggerWebhook(
        WEBHOOK_SINGLE,
        "Single Press"
      );

      if (result) {
        Serial.println("[System] Single Press Success");
      } else {
        Serial.println("[System] Single Press Failed");
      }
    }

    clickCount = 0;
  }

  lastReading = reading;

  delay(5);
}
