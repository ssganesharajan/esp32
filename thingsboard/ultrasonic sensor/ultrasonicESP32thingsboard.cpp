#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "thingsboard.cloud"; // Use "demo.thingsboard.io" if on demo server
const char* token = "YOUR_ACCESS_TOKEN";       // Paste your Access Token here

// --- Hardware Pins ---
const int trigPin = 5;
const int echoPin = 18;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (Username is the Access Token, Password stays null)
    if (client.connect("ESP32Client", token, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // --- 1. Measure Distance ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- 2. Prepare JSON Payload ---
  StaticJsonDocument<200> doc;
  doc["distance"] = distance;
  char buffer[256];
  serializeJson(doc, buffer);

  // --- 3. Publish to ThingsBoard ---
  // Topic for telemetry is always v1/devices/me/telemetry
  client.publish("v1/devices/me/telemetry", buffer);

  delay(5000); // Wait 5 seconds before next reading
}