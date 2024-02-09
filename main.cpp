//#include <DFRobot_HX711_I2C.h>
//#define HX711_I2C_ADDR
#include <HX711.h>
#include <Wire.h>


// Definieer de pinnen waarop de HX711-module is aangesloten
const int HX711_DOUT_PIN = 5;
const int HX711_SCK_PIN = 6;

// Maak een HX711-object aan
HX711 scale(HX711_DOUT_PIN, HX711_SCK_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Weight Sensor Test");

  // Kalibreer de weegschaal. Je moet een bekend gewicht plaatsen en de aflezing noteren
  scale.set_scale();
  scale.tare(); // Reset de schaal naar 0
}

void loop() {
  // Lees de gewichtswaarde
  float weight = scale.get_units();

  // Geef het gewicht weer op de seriële monitor
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" kg");

  delay(1000); // Wacht 1 seconde voordat je opnieuw leest
}
