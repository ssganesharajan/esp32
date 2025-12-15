#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "YOUR_ID";
const char* password = "YOUR_PASSWORD";

// Create a WebServer object on port 80
WebServer server(80);

// Global variable to store the user's input
String userInput = "No input yet";

// Function to generate the HTML web page
String createHtmlPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 User Input</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    .container { margin-top: 50px; }
    input[type="text"] { padding: 10px; width: 80%; max-width: 300px; margin-bottom: 20px; }
    input[type="submit"] { padding: 10px 20px; background-color: #4CAF50; color: white; border: none; cursor: pointer; }
    .output { margin-top: 30px; padding: 20px; border: 1px solid #ccc; max-width: 400px; margin: 30px auto; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Web Input Demo</h1>
    
    <form action="/" method="GET">
      <label for="user_input">Enter Text:</label><br>
      <input type="text" id="user_input" name="user_input" required>
      <input type="submit" value="Submit">
    </form>
    
    <div class="output">
      <h2>Latest Input Received:</h2>
      <p style="font-size: 1.2em; color: blue;">%INPUT_PLACEHOLDER%</p>
    </div>
    
  </div>
</body>
</html>
)rawliteral";

  // Replace the placeholder with the current stored input value
  html.replace("%INPUT_PLACEHOLDER%", userInput);
  return html;
}

// Handler for the main page ("/")
void handleRoot() {
  // Check if a query parameter named 'user_input' exists
  if (server.hasArg("user_input")) {
    // Get the value and update the global variable
    userInput = server.arg("user_input");
    Serial.print("New input received: ");
    Serial.println(userInput);
  }

  // Send the page back to the client, showing the updated or initial content
  server.send(200, "text/html", createHtmlPage());
}

// Handler for unknown requests
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print the IP address to access the web server
  Serial.println("\nWiFi connected!");
  Serial.print("Access your web server at: http://");
  Serial.println(WiFi.localIP());
  
  // Set up server routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Must be called repeatedly to handle incoming client requests
  server.handleClient();
}