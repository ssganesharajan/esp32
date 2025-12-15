#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// --- 1. Your Wi-Fi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- 2. Your Telegram Bot and Chat IDs ---
// Get this from @BotFather
#define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN_HERE" 
// Get this from the getUpdates API call
#define TELEGRAM_CHAT_ID "YOUR_CHAT_ID_HERE"      
// Example format for ID: "123456789" (Note: It's a string, even if numeric)

WebServer server(80);
// --- HTML for the Webpage ---
const char* htmlContent = R"raw(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Telegram Sender</title>
<style>
  body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
  .btn {
    background-color: #0088cc; 
    color: white; 
    padding: 15px 32px; 
    text-align: center; 
    text-decoration: none; 
    display: inline-block; 
    font-size: 16px; 
    margin: 4px 2px; 
    cursor: pointer;
    border: none;
    border-radius: 8px;
  }
</style>
</head>
<body>
  <h1>ESP32 Telegram Messenger</h1>
  <p>Click the button to send a message to your Telegram chat.</p>
  <form action="/send-alert" method="POST">
    <button class="btn" type="submit">Send Telegram Alert</button>
  </form>
</body>
</html>
)raw";
// --- Function to Send Telegram Message ---
void sendTelegramMessage(String message) {
  HTTPClient http;
  
  // Construct the full Telegram API URL
  String telegramApiUrl = "https://api.telegram.org/bot";
  telegramApiUrl += TELEGRAM_BOT_TOKEN;
  telegramApiUrl += "/sendMessage?chat_id=";
  telegramApiUrl += TELEGRAM_CHAT_ID;
  telegramApiUrl += "&text=";
  telegramApiUrl += message; 
  
  // Replace spaces with URL-encoded '+' for clean transmission
  telegramApiUrl.replace(" ", "+");

  Serial.print("Attempting to send message...");

  // Begin HTTPS GET request
  http.begin(telegramApiUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf(" [HTTP] Success! Code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      // You can read the response payload if needed
      // String payload = http.getString();
      // Serial.println("Response: " + payload);
    }
  } else {
    Serial.printf(" [HTTP] Failed. Error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end(); // Free the resources
}

// --- Handler for Alert Action (/send-alert) ---
void handleSendAlert() {
  Serial.println(">>> Received request to send alert.");
  // The message content
  sendTelegramMessage("Alert from ESP32! The webpage button was pressed at " + String(millis()));

  // Redirect back to the main page after sending the alert
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Message sent!"); 
}
// --- Handler for Root URL (/) ---
void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Server Routing: Handle requests to the main page and the alert endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/send-alert", HTTP_POST, handleSendAlert);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started. Access it via the IP address above.");
}

// --- Loop ---
void loop() {
  server.handleClient();
}