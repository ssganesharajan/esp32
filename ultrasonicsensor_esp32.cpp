#include <NewPing.h>

// --- Configuration ---
// Define the digital pins connected to the sensor
#define TRIGGER_PIN 5  // ESP32 GPIO pin for the sensor's Trigger pin
#define ECHO_PIN    18 // ESP32 GPIO pin for the sensor's Echo pin
#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping. Max is 400cm (4 meters) for HC-SR04.

// NewPing object (uses a default speed of sound value)
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // Initialize the serial communication to display output
  Serial.begin(115200); 
  Serial.println("ESP32 Ultrasonic Sensor Test");
}

void loop() {
  // Wait 50ms between pings (20 pings/sec). 
  // This is mandatory to prevent trigger pulses from mixing.
  delay(50); 
  
  // Measures the distance in centimeters. 
  // The 'ping_cm()' function returns 0 if no echo is received within MAX_DISTANCE.
  unsigned int distance_cm = sonar.ping_cm(); 
  
  Serial.print("Distance: ");
  
  if (distance_cm == 0) {
    // If distance is 0, it means the object is out of range (further than MAX_DISTANCE)
    Serial.println("Out of Range ( > 400 cm)");
  } else {
    // Distance measured in cm
    Serial.print(distance_cm);
    Serial.println(" cm");
  }

  // Wait before the next measurement
  delay(500); 
}