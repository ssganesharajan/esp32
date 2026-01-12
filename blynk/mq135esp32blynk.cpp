#define BLYNK_TEMPLATE_ID "Your_Template_ID"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitor"
#define BLYNK_AUTH_TOKEN "Your_Auth_Token"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi Credentials
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";

// Hardware Setup
#define MQ135_PIN 34          // GPIO 34 (Analog)
#define RL_VALUE 1            // Load resistance in KOhms (usually 1K on most modules)
#define RO_CLEAN_AIR_FACTOR 3.6 // RS/RO ratio in clean air

float Ro = 10.0; // Default Ro; will be calibrated in setup

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Calibrating MQ-135... Please wait.");
  // Basic Calibration: assumes you are in clean air (400ppm CO2)
  float sensor_volt;
  float RS_air; 
  float sensorValue = 0;

  for(int x = 0 ; x < 50 ; x++) {
    sensorValue = sensorValue + analogRead(MQ135_PIN);
    delay(100);
  }
  sensorValue = sensorValue/50.0;
  sensor_volt = sensorValue * (3.3 / 4095.0);
  RS_air = ((3.3 * RL_VALUE) / sensor_volt) - RL_VALUE;
  Ro = RS_air / RO_CLEAN_AIR_FACTOR;
  
  Serial.print("Calibration Done. Ro: ");
  Serial.println(Ro);
}

void loop() {
  Blynk.run();
  
  int rawValue = analogRead(MQ135_PIN);
  float v_out = rawValue * (3.3 / 4095.0);
  float Rs = ((3.3 * RL_VALUE) / v_out) - RL_VALUE;
  float ratio = Rs / Ro;

  // Specific Gas Formulas: PPM = a * (ratio)^b
  // Values 'a' and 'b' are derived from the MQ-135 Datasheet curves
  float CO2     = 110.47 * pow(ratio, -2.862);
  float NH3     = 102.20 * pow(ratio, -2.473);
  float Alcohol = 77.255 * pow(ratio, -3.18);
  float Benzene = 13.15 * pow(ratio, -2.474);
  float NOx     = 34.66 * pow(ratio, -3.369);

  // Send to Blynk (V1 to V5)
  Blynk.virtualWrite(V1, CO2);
  Blynk.virtualWrite(V2, NH3);
  Blynk.virtualWrite(V3, Alcohol);
  Blynk.virtualWrite(V4, Benzene);
  Blynk.virtualWrite(V5, NOx);

  // Print to Serial Monitor
  Serial.printf("CO2: %.2f | NH3: %.2f | Alcohol: %.2f | Benzene: %.2f\n", CO2, NH3, Alcohol, Benzene);
  
  delay(2000); // 2-second update interval
}

// Blynk cloud dashboard
// Developer zone
// New template (name, hardware, connection type)
// Datastream  ->  New datastream  ->  virtual pin  ->  pin, data type , units, min&max value
// Web dashboard  ->  edit  ->  gauge, chart  ->  edit and select data stream