#include <WiFi.h>
#include <PubSubClient.h>

// WiFi and ThingsBoard Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "demo.thingsboard.io"; // or your local IP
const char* access_token = "YOUR_ACCESS_TOKEN_HERE";

// Hardware Pins
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
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", access_token, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Measure Distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  // Prepare JSON payload
  String payload = "{";
  payload += "\"distance\":";
  payload += String(distance);
  payload += "}";

  // Send to ThingsBoard
  Serial.print("Sending distance: ");
  Serial.println(distance);
  client.publish("v1/devices/me/telemetry", payload.c_str());

  delay(5000); // Send every 5 seconds
}