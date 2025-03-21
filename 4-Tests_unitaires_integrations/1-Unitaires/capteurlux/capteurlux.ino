#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

// Crée une instance du capteur TSL2591
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

void setup(void) {
  // Initialise la communication série
  Serial.begin(9600);

  // Initialise le capteur
  if (tsl.begin()) {
    Serial.println("Capteur TSL2591 trouvé");
  } else {
    Serial.println("Aucun capteur trouvé");
    while (1); // Arrête le programme si le capteur n'est pas trouvé
  }

  // Configure le gain et le temps d'intégration
  tsl.setGain(TSL2591_GAIN_MED);      // Gain moyen
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);  // Temps d'intégration de 300 ms
}

void loop(void) {
  // Obtient les données du capteur
  sensors_event_t event;
  tsl.getEvent(&event);

  // Affiche les valeurs de luminosité en lux
  if (event.light) {
    Serial.print("Luminosité : ");
    Serial.print(event.light);
    Serial.println(" lux");
  } else {
    Serial.println("Erreur de lecture du capteur");
  }

  // Attendre avant la prochaine lecture
  delay(500);
}
