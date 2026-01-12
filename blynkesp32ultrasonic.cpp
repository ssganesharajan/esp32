// 1. Paste your Blynk Auth Token and Template info here (from your Blynk Dashboard)
#define BLYNK_TEMPLATE_ID   "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Smart Sensor"
#define BLYNK_AUTH_TOKEN    "YourAuthTokenHere"

// 2. Include necessary libraries
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// 3. Your WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";

// 4. Define Sensor Pins
const int trigPin = 5;
const int echoPin = 18;

// Variables for calculation
long duration;
int distance;

BlynkTimer timer;

// This function sends the sensor data to Blynk
void sendSensorData() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Trigger the sensor by sending a 10us high pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin (returns the sound travel time in microseconds)
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance: Speed of sound is 0.034 cm/us
  distance = duration * 0.034 / 2;

  // Print to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send the distance value to Blynk Virtual Pin V1
  Blynk.virtualWrite(V1, distance);
}

void setup() {
  Serial.begin(115200);

  // Define pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}