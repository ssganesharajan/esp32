#include <WiFi.h>
#include <WebServer.h>

// --- 1. WiFi Settings ---
// **IMPORTANT:** Replace with your network credentials
const char* ssid = "YOUR_ID";
const char* password = "YOUR_PASSWORD";

// --- 2. Hardware Pin Configuration ---
// Using GPIO 2, which is often connected to the built-in LED on ESP32 boards.
const int ledPin = 2; 

// --- 3. Web Server Object ---
WebServer server(80);

// --- 4. HTML Content Generation (The Web Page) ---
String getHtmlPage() {
  // Read the current state of the LED pin
  int ledState = digitalRead(ledPin);
  String ledStatus = (ledState == HIGH) ? "ON" : "OFF";
  String buttonColor = (ledState == HIGH) ? "bg-green-500 hover:bg-green-600" : "bg-red-500 hover:bg-red-600";

  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>ESP32 Web Control</title>";
  
  // Tailwind CSS via CDN for modern, responsive styling
  html += "<script src=\"https://cdn.tailwindcss.com\"></script>";
  
  html += "<style>";
  html += "  @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;700&display=swap');";
  html += "  body { font-family: 'Inter', sans-serif; background-color: #f7f7f7; }";
  html += "</style>";
  
  html += "</head><body>";
  
  // Main Container and Layout
  html += "<div class='min-h-screen flex items-center justify-center p-4'>";
  html += "  <div class='bg-white shadow-xl rounded-xl p-8 w-full max-w-md'>";
  
  // Title Section
  html += "    <h1 class='text-3xl font-bold text-gray-800 mb-6 text-center'>ESP32 Web Control Panel</h1>";
  
  // Image Placeholder Section
  html += "    <div class='mb-6 p-4 border-4 border-dashed border-indigo-300 bg-indigo-50 rounded-lg text-center'>";
  html += "      <p class='text-gray-600 font-semibold mb-2'>Project Image Placeholder</p>";
  // Placeholder image using an external URL
  html += "      <img src='https://placehold.co/400x200/4F46E5/FFFFFF?text=Your+Project+Photo' alt='Placeholder Image' class='w-full rounded-md shadow-lg'>";
  html += "      <p class='text-xs text-gray-500 mt-2'>Your custom image or data visualization would go here.</p>";
  html += "    </div>";
  
  // LED Status Section
  html += "    <div class='mb-6 flex justify-between items-center p-3 rounded-lg " + buttonColor + " text-white font-bold'>";
  html += "      <span>LED Status:</span>";
  html += "      <span class='text-xl'>" + ledStatus + "</span>";
  html += "    </div>";
  
  // Control Buttons Section
  html += "    <div class='flex space-x-4'>";
  
  // LED ON Button
  html += "      <a href='/led/on' class='flex-1'>";
  html += "        <button class='w-full py-3 text-lg font-semibold text-white bg-green-600 rounded-lg shadow-md transition duration-150 ease-in-out transform hover:scale-105'>";
  html += "          LED ON";
  html += "        </button>";
  html += "      </a>";
  
  // LED OFF Button
  html += "      <a href='/led/off' class='flex-1'>";
  html += "        <button class='w-full py-3 text-lg font-semibold text-white bg-red-600 rounded-lg shadow-md transition duration-150 ease-in-out transform hover:scale-105'>";
  html += "          LED OFF";
  html += "        </button>";
  html += "      </a>";
  
  html += "    </div>"; // End of flex space-x-4
  
  html += "  </div>"; // End of shadow-xl container
  html += "</div>"; // End of min-h-screen container
  
  html += "</body></html>";
  return html;
}

// ==================================
// === 5. Web Server Handle Functions ===
// ==================================

// Handles the root path (/) and serves the main HTML page.
void handleRoot() {
  server.send(200, "text/html", getHtmlPage());
}

// Handles the /led/on path.
void handleLedOn() {
  digitalWrite(ledPin, HIGH); 
  Serial.println("LED turned ON");
  
  // Redirect back to the root page after the action to update the status
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "LED is ON");
}

// Handles the /led/off path.
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  Serial.println("LED turned OFF");
  
  // Redirect back to the root page after the action to update the status
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "LED is OFF");
}

// Handles 404 Not Found errors
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  server.send(404, "text/plain", message);
}

// ==================================
// === 6. WiFi Setup Function ===
// ==================================
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Start the connection
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ==================================
// === 7. Arduino Setup ===
// ==================================
void setup() {
  Serial.begin(115200);
  
  // Set the LED pin as an output and ensure it's off initially
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 
  
  // Connect to WiFi
  setup_wifi();

  // Define Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/led/on", HTTP_GET, handleLedOn);
  server.on("/led/off", HTTP_GET, handleLedOff);
  server.onNotFound(handleNotFound);

  // Start the Web Server
  server.begin();
  Serial.println("HTTP Web server started on port 80.");
}

// ==================================
// === 8. Arduino Loop ===
// ==================================
void loop() {
  // Handle incoming client requests frequently
  server.handleClient();
}