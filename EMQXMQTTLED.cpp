#include <WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT Broker Details
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "broker.emqx.io"; // Public EMQX Broker 

WiFiClient espClient;
PubSubClient client(espClient);
const int ledPin = 2; // GPIO 2

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

// Logic to handle incoming messages [cite: 5]
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }

  if (message == "1" || message == "ON") {
    digitalWrite(ledPin, HIGH);
  } else if (message == "2" || message == "OFF") {
    digitalWrite(ledPin, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe("esp32/led"); // Subscribing to topic 
    } else { delay(5000); }
  }
}

void loop() {
  if (!client.connected()) { reconnect(); }
  client.loop();
}