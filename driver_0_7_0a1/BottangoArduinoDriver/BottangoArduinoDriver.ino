// !!! DRIVER VERSION: 0.7.0a1 !!!
// !!! Api Version: 8 !!!

// Modified by Daniel Simu for the Acrobot Workshop
// https://github.com/hapiel/workshop_bot

#include "src/BottangoCore.h"
#include "src/BasicCommands.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>

// == Wouter's Stuff ==
// Definitions
const int middleButtonPin = 36;  // should align with onboard buttons
// Replace with your WiFi credentials
const char *ssid = "CKC DigitalArtLab";
const char *password = "digitalartlab";

// Replace with your target URL
const char *serverUrl = "http://10.0.1.74:59224/ControlInput/";

bool buttonState = false;
bool wifiMode = false;

struct MotorData {
  String bottangoIdentifier;
  int feedbackPin;
  bool inverted = false;
};

MotorData motors[2] = {
  { "legLeft", 32, true },
  { "legRight", 33, false }
};

// Wifi Stuff
void readMacAddress() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void disconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    Serial.println("Disconnected from WiFi");
  } else {
    Serial.println("WiFi is not connected");
  }
}

void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  readMacAddress();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  sendDataToBottango(motors[0]);  // Send intial data as test
}

bool sendDataToBottango(MotorData data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String valueString = String(normalizeServoValue(data));  // Normalize the servo value to 4 decimal places
    String jsonData = "{\"identifier\":\"" + String(data.bottangoIdentifier) + "\", \"value\":" + valueString + "}";
    Serial.println("HTTP PUT request with data: " + jsonData);
    int httpResponseCode = http.PUT(jsonData);  // PUT request

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Response:");
      Serial.println(response);
      return true;
    } else {
      Serial.print("Error in PUT request: ");
      Serial.println(http.errorToString(httpResponseCode));
      return true;
    }

    http.end();  // Free resources
    return true;
  } else {
    Serial.println("WiFi not connected");
    return false;
  }
  // delay(500);
}

void sendAllToBottango() {
  for (int i = 0; i < sizeof(motors) / sizeof(motors[0]); i++) {
    if (!sendDataToBottango(motors[i])) {
      Serial.println("Failed to send data for " + String(motors[i].bottangoIdentifier));
    }
  }
}

void wifiLoop() {
  buttonState = digitalRead(middleButtonPin);
  if (buttonState == LOW && wifiMode == false) {
    BottangoCore::effectorPool.clearAllCurves();
    BottangoCore::effectorPool.deregisterAll();

    BottangoCore::stop();

    setupWiFi();
    Serial.println("engage wifi");
    wifiMode = true;
    Serial.println(wifiMode);
    delay(500);
  } else if (buttonState == LOW && wifiMode == true) {
    esp_system_abort("Hello");
    Serial.println("stop wifi");
    wifiMode = false;
    Serial.println(wifiMode);
    delay(500);
  }

  if (wifiMode == true) {
    sendAllToBottango();
  }
  if (wifiMode == false) {
    BottangoCore::bottangoLoop();
  }
}

// Servo Stuff

float normalizeServoValue(MotorData data) {
  float value = analogRead(data.feedbackPin);
  // normalize to 0-1 range by deviding trough the max petentiometer value
  value = value > 0 ? value / 4095 : 0;
  if (data.inverted) {
    value = 1 - value;
  }
  return value;
}

void clearScrean() {
  for (int i = 0; i < 100; i++) {
    Serial.print("\n");
  }
}

void printRawServo() {
  // clearScrean();
  for (int i = 0; i < sizeof(motors) / sizeof(motors[0]); i++) {
    Serial.print(String(analogRead(motors[i].feedbackPin)) + " ");
  }
  Serial.println("");
}

// == Daniel's Stuff ==
const int ledPin = 21;
int ledState = LOW;  // Variable to store the state of the LED
unsigned long previousMillis = 0;

void blinkWithoutDelay(unsigned long onTime, unsigned long offTime) {
  // Get the current time
  unsigned long currentMillis = millis();

  // Check if it's time to change the state of the LED
  if ((ledState == HIGH) && (currentMillis - previousMillis >= onTime)) {
    ledState = LOW;                  // Turn LED off
    previousMillis = currentMillis;  // Save the time
    digitalWrite(ledPin, ledState);  // Update the LED
  } else if ((ledState == LOW) && (currentMillis - previousMillis >= offTime)) {
    ledState = HIGH;                 // Turn LED on
    previousMillis = currentMillis;  // Save the time
    digitalWrite(ledPin, ledState);  // Update the LED
  }
}

uint32_t printTimer = 0;

void printInfo() {
  int battRaw = analogRead(35);
  float battVolt = battRaw * (3.3 / 4096) * 2;
  int battPercent = map(battVolt * 100, 250, 420, 0, 100);

  // print status every 3 seconds
  if (printTimer + 3000 < millis()) {
    printTimer = millis();
    Serial.printf("bat: %f v %d - %d \n", battVolt, battPercent, battRaw);
    readMacAddress();
  }

  // check battery status with button
  if (!digitalRead(36)) {
    if (battPercent > 75) {
      digitalWrite(ledPin, HIGH);
    } else if (battPercent > 50) {
      blinkWithoutDelay(300, 50);
    } else if (battPercent > 25) {
      blinkWithoutDelay(200, 200);
    } else {
      blinkWithoutDelay(100, 400);
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  // setupWiFi();

  pinMode(34, INPUT);
  pinMode(36, INPUT);  // middle button
  pinMode(39, INPUT);
  pinMode(ledPin, OUTPUT);
  BottangoCore::bottangoSetup();
}

void loop() {
  wifiLoop();
  printInfo();
  // printRawServo();
  // BottangoCore::bottangoLoop();
}