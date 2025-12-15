#include <WiFi.h> // Standard WiFi library for ESP32

// Network Credentials
const char* ssid1 = "YOUR_ID";
const char* password1 = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200); // Baud Rate

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid1);
  WiFi.begin(ssid1, password1);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connection successful
  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP()); // Get IP address of ESP32
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress()); // Get MAC address
}

void loop() {
  // Print the Received Signal Strength Indicator (RSSI)
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI()); // Get signal strength (in dBm)
  Serial.println(" dBm");

  delay(1000);
}
