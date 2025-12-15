#include <DHT.h>

// Define the pin the DHT22 is connected to
#define DHTPIN 4     // Ensure this matches your wiring (Out/Data pin)
#define DHTTYPE DHT22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Set Serial Monitor baud rate to 115200
  Serial.begin(115200); 
  Serial.println("DHT22 Extended Metrics Test (Heat Index & Dew Point)");
  Serial.println("---------------------------------------------------");
  dht.begin();
}

void loop() {
  delay(2000); 

  // --- 1. Read Raw Values ---
  float h = dht.readHumidity();      // Relative Humidity (%)
  float t = dht.readTemperature();   // Temperature in Celsius (°C)

  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Failed to read from DHT sensor! Retrying...");
    return;
  }

  // --- 2. Calculate Derived Values ---

  // A. Calculate Heat Index (Apparent Temperature)
  // This function is built into the Adafruit library.
  // The 'false' argument means the result will be in Celsius.
  float hi = dht.computeHeatIndex(t, h, false); 

  // B. Calculate Dew Point
  // Using the Magnus-Tetens formula (a common approximation):
  // Es: Saturation Vapor Pressure, in hPa (hPa is equivalent to mbar)
  float Es = 6.112 * exp((17.62 * t) / (243.12 + t));
  // E: Actual Vapor Pressure
  float E = h / 100.0 * Es;
  // Dw: Dew Point Temperature in Celsius
  float Dw = 243.12 * log(E / 6.112) / (17.62 - log(E / 6.112));

  // --- 3. Output the Results ---
  
  Serial.println("--- Measurements ---");
  Serial.print("1. Temperature (T): ");
  Serial.print(t);
  Serial.println(" °C");

  Serial.print("2. Humidity (H):    ");
  Serial.print(h);
  Serial.println(" %");

  Serial.println("--- Calculated Metrics ---");
  Serial.print("3. Heat Index (Feels Like): ");
  Serial.print(hi);
  Serial.println(" °C");

  Serial.print("4. Dew Point:               ");
  Serial.print(Dw);
  Serial.println(" °C");

  Serial.println("---------------------------------------------------");
}
