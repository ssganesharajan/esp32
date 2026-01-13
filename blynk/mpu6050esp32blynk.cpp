#define BLYNK_TEMPLATE_ID "Your_Template_ID"
#define BLYNK_TEMPLATE_NAME "Your_Device_Name"
#define BLYNK_AUTH_TOKEN "Your_Auth_Token"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";

Adafruit_MPU6050 mpu;
BlynkTimer timer;

void sendSensorData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // --- Send Accelerometer Data (m/s^2) ---
  Blynk.virtualWrite(V1, a.acceleration.x);
  Blynk.virtualWrite(V2, a.acceleration.y);
  Blynk.virtualWrite(V3, a.acceleration.z);

  // --- Send Gyroscope Data (rad/s) ---
  Blynk.virtualWrite(V4, g.gyro.x);
  Blynk.virtualWrite(V5, g.gyro.y);
  Blynk.virtualWrite(V6, g.gyro.z);

  // Optional: Send Temperature to V7
  Blynk.virtualWrite(V7, temp.temperature);
}

void setup() {
  Serial.begin(115200);
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  Blynk.begin(auth, ssid, pass);
  
  // Send data every 500ms for smoother updates
  timer.setInterval(500L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}