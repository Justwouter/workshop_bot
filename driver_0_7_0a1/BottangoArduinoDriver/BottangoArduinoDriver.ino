// !!! DRIVER VERSION: 0.7.0a1 !!!
// !!! Api Version: 8 !!!

// Modified by Daniel Simu for the Acrobot Workshop
// https://github.com/hapiel/workshop_bot

#include "src/BottangoCore.h"
#include "src/BasicCommands.h"
#include <WiFi.h>
#include <esp_wifi.h>

// == Wouter's Stuff ==
const int feedbackPin1 = 32;
const int feedbackPin2 = 33;

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

void printServoRotations(){
  Serial.printf("[S1]: %f [S2]: %f",analogRead(feedbackPin1)/16.6, analogRead(feedbackPin2)/16.6);
  Serial.println();
  // Serial.println("[Servo1 Position] "+String();
  // Serial.println("[Servo2 Position] "+String();
}


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

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();
  readMacAddress();

  pinMode(34, INPUT);
  pinMode(36, INPUT);  // middle button
  pinMode(39, INPUT);
  pinMode(ledPin, OUTPUT);
  BottangoCore::bottangoSetup();
}

void loop() {
  printInfo();
  BottangoCore::bottangoLoop();
}


uint32_t printTimer = 0;
uint32_t theCoolerPrintTimer = 0;

void printInfo(){
  int battRaw = analogRead(35);
  float battVolt = battRaw * (3.3 / 4096) * 2;
  int battPercent = map(battVolt * 100, 250, 420, 0, 100);

  if (theCoolerPrintTimer + 250 < millis()) {
    theCoolerPrintTimer = millis();
    printServoRotations();
  }


  // print status every 3 seconds
  if (printTimer + 3000 < millis()) {
    printTimer = millis();
    Serial.printf("bat: %f v %d - %d",battVolt, battPercent, battRaw);
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
