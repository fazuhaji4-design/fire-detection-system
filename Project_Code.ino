#define BLYNK_TEMPLATE_ID "TMPL2Akobve0h"
#define BLYNK_TEMPLATE_NAME "Fire Alarm Security system IoT"
#define BLYNK_AUTH_TOKEN "UwPrmoLF4TWrLKyyEHJedgamUdkHzmia"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Nadiifo";
char pass[] = "615399562";

// Pin Definitions
const int flamePin = D1;        // Digital pin for flame sensor
const int smokePin = A0;        // Analog pin for MQ-2/MQ-135 smoke/gas sensor
const int redLed = D3;          // Red LED for alert
const int greenLed = D2;        // Green LED for safe
const int buzzerPin = D6;       // Buzzer for siren
const int waterPumpPin = D5;    // Relay control for water pump

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Pin modes
  pinMode(flamePin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);

  // Initial states
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, HIGH);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(waterPumpPin, LOW);

  Serial.println("🚨 Fire & Smoke/Gas Alarm System Initialized 🚨");
}

// Siren effect function
void playSiren(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    for (int freq = 2000; freq <= 4000; freq += 100) {
      tone(buzzerPin, freq);
      delay(5);
    }
    for (int freq = 4000; freq >= 2000; freq -= 100) {
      tone(buzzerPin, freq);
      delay(5);
    }
  }
  noTone(buzzerPin);
}

void loop() {
  Blynk.run();

  // Read sensors
  bool fireDetected = digitalRead(flamePin) == LOW;  // Flame sensor gives LOW when fire is detected
  int smokeValue = analogRead(smokePin);             // Analog value from MQ sensor
  bool smokeDetected = smokeValue > 400;             // Threshold can be adjusted

  // Debug output
  Serial.print("Flame: ");
  Serial.print(fireDetected ? "🔥 Detected" : "✅ None");
  Serial.print(" | Smoke Value: ");
  Serial.print(smokeValue);
  Serial.print(smokeDetected ? " 💨 Detected" : " ✅ Clean");
  Serial.println();

  // If either fire or smoke is detected
  if (fireDetected || smokeDetected) {
    Serial.println("🚨 ALERT: FIRE or SMOKE/GAS DETECTED!");

    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(waterPumpPin, HIGH);
    playSiren(1000);

    // Update Blynk virtual pins
    Blynk.virtualWrite(V0, fireDetected ? "🔥 Fire: YES" : "🔥 Fire: NO");
    Blynk.virtualWrite(V1, smokeDetected ? "💨 Smoke/Gas: YES" : "💨 Smoke/Gas: NO");
    Blynk.virtualWrite(V2, "💧 Water Pump: ON");

    // Push alerts
    if (fireDetected) {
      Blynk.logEvent("fire_alert", "🔥 Fire Detected!");
    }
    if (smokeDetected) {
      Blynk.logEvent("smoke_alert", "💨 Smoke/Gas Detected!");
    }

  } else {
    // Safe condition
    Serial.println("✅ All Clear: No Fire or Smoke");

    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(waterPumpPin, LOW);
    noTone(buzzerPin);

    // Update Blynk status
    Blynk.virtualWrite(V0, "🔥 Fire: NO");
    Blynk.virtualWrite(V1, "💨 Smoke/Gas: NO");
    Blynk.virtualWrite(V2, "💧 Water Pump: OFF");
  }

  delay(1000); // Wait 1 second before next reading
}
