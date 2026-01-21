#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

// WiFi Credentials for Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ThingsBoard Config
const char* tb_host = "thingsboard.cloud";
const char* tb_token = "YOUR_ACCESS_TOKEN_HERE"; // Paste your token here

WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient);

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 Found!");

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  // Reconnect to ThingsBoard if needed
  if (!tb.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (!tb.connect(tb_host, tb_token)) {
      Serial.println("Failed to connect");
      return;
    }
    Serial.println("Connected!");
  }

  // Get sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // New Syntax for Sending Telemetry
  tb.sendTelemetryData("accel_x", a.acceleration.x);
  tb.sendTelemetryData("accel_y", a.acceleration.y);
  tb.sendTelemetryData("accel_z", a.acceleration.z);

  Serial.println("Data sent to ThingsBoard");

  tb.loop();
  delay(1000); 
}