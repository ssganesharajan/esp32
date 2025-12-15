#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

// --- WiFi Settings ---
const char* ssid = "YOUR_ID";
const char* password = "YOUR_PASSWORD";

// --- MQTT Broker Settings (HiveMQ Public Broker) ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32Client-12345"; // Must be unique

// --- MQTT Topics ---
const char* publish_topic = "esp32/hivemq/publish_data";
const char* subscribe_topic = "esp32/hivemq/subscribe_data";

// --- Objects ---
WiFiClient espClient;
PubSubClient client(espClient);
// Use WebServer on port 80
WebServer server(80);

// --- Global Variables ---
String lastReceivedMessage = "No message received yet.";

// Function prototypes
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect_mqtt();
void handleRoot();
void handlePublish();
void send_message(const char* topic, const char* message);

// =======================
// === 1. MQTT Callback ===
// =======================
// This function is called when a message is received on a subscribed topic.
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on [");
  Serial.print(topic);
  Serial.print("]: ");
  
  // Convert the payload to a String for the web page
  String incomingMessage;
  for (int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }
  Serial.println(incomingMessage);
  
  // Update the global variable for the web page
  lastReceivedMessage = incomingMessage;
}

// =========================
// === 2. WiFi Setup ===
// =========================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ========================
// === 3. MQTT Reconnect ===
// ========================
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
      // Once connected, subscribe to the topic
      client.subscribe(subscribe_topic);
      Serial.print("Subscribed to: ");
      Serial.println(subscribe_topic);
      
      // Publish an initial status message
      client.publish(publish_topic, "ESP32 connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ===========================
// === 4. Web Server Handlers ===
// ===========================

// HTML Content for the web page
String getHtmlPage() {
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>ESP32 MQTT Web Client</title>";
  html += "<style>body{font-family:Arial;} .container{max-width:400px; margin:0 auto; padding:20px; border:1px solid #ccc; border-radius:5px;} .msg{padding:10px; background:#e0ffe0; border:1px solid #c0c0c0; border-radius:4px;} h2{color:#333;}</style>";
  html += "<meta http-equiv=\"refresh\" content=\"5\">"; // Auto-refresh every 5 seconds
  html += "</head><body><div class=\"container\">";
  html += "<h2>ESP32 MQTT Status</h2>";
  html += "<p><strong>IP Address:</strong> " + String(WiFi.localIP()) + "</p>";
  html += "<p><strong>MQTT Status:</strong> " + (client.connected() ? String("CONNECTED") : String("DISCONNECTED")) + "</p>";
  
  html += "<h3>Latest Received Message (Topic: " + String(subscribe_topic) + ")</h3>";
  html += "<div class='msg'>" + lastReceivedMessage + "</div>";
  
  html += "<h3>Publish a Test Message (Topic: " + String(publish_topic) + ")</h3>";
  // The button's link triggers the handlePublish function
  html += "<p><a href='/publish'><button style='padding: 10px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;'>Send 'Hello from Web!'</button></a></p>";
  
  html += "<p style='font-size:small; color:#777;'>* Page auto-refreshes every 5 seconds.</p>";
  html += "</div></body></html>";
  return html;
}

// Handles the root URL (/)
void handleRoot() {
  server.send(200, "text/html", getHtmlPage());
}

// Handles the /publish URL to send a message
void handlePublish() {
  const char* message = "Hello from Web!";
  Serial.print("Publishing message from web: ");
  Serial.println(message);
  client.publish(publish_topic, message);
  
  // Redirect back to the root page after publishing
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

// =================
// === 5. Setup ===
// =================
void setup() {
  Serial.begin(115200);
  
  // 1. Connect to WiFi
  setup_wifi();
  
  // 2. Configure MQTT Client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // 3. Configure Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/publish", HTTP_GET, handlePublish);
  
  // 4. Start Web Server
  server.begin();
  Serial.println("HTTP Web server started.");
}

// =================
// === 6. Loop ===
// =================
void loop() {
  // 1. Maintain MQTT Connection
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); // Must be called frequently to maintain connection and process incoming messages
  
  // 2. Handle Web Server Requests
  server.handleClient();
  
  // Optional: Publish a periodic message (e.g., a dummy sensor value)
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 15000) { // Publish every 15 seconds
    lastMsg = now;
    String value = "Test Count: " + String(now / 1000); // Sample data
    Serial.print("Publishing periodic message: ");
    Serial.println(value);
    client.publish(publish_topic, value.c_str());
  }
}