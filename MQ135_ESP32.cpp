#include <MQUnifiedsensor.h>

// --- Configuration ---
#define board       "ESP-32" 
#define Voltage_Resolution 3.3 
#define pin         4        
#define type        "MQ-135"  
#define ADC_Bit_Resolution 12  
#define RatioMQ135CleanAir 3.6 

// --- Calibration Value ---
// !! REPLACE THIS VALUE with your sensor's calibrated RZERO !!
#define RZERO       76.63 

// Create the sensor object
MQUnifiedsensor MQ135(board, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(115200);
  Serial.println("--- MQ-135 Air Quality Sensor Test ---");
  
  // Set the specific gas concentration curve
  MQ135.setA(102.2); 
  MQ135.setB(-2.473); 
  
  // **--- FIX FOR RZERO ERROR ---**
  // 1. Try setting RZERO as a public variable (Most common fix)
  //MQ135.RZERO = RZERO;
  // 2. If the line above fails, comment it out and try:
  MQ135.setR0(RZERO);
  
  Serial.print("RZERO used: ");
  Serial.print(RZERO);
  Serial.println(" ohms");
  Serial.println("Warming up... Please allow the sensor 3-5 minutes to stabilize.");
}

void loop() {
  // Update the sensor reading 
  MQ135.update(); 
  
  // Get the calculated concentration in PPM
  float ppm = MQ135.readSensor(); 

  Serial.print("Gas Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm (CO2 equiv.)");
  
  delay(2000);
}