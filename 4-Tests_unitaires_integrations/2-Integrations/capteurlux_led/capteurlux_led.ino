#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

// Classe Capteur pour gérer le capteur de lumière
class Capteur {
  private:
  Adafruit_TSL2591 tsl;
  public:
  Capteur() : tsl(Adafruit_TSL2591(2591)) {}

  bool initialiser() {
    while (!tsl.begin()) {
      Serial.println("Aucun capteur trouvé. Réessayer dans 2 secondes...");
      delay(2000); // Attendre 2 secondes avant de réessayer
    }
    Serial.println("Capteur TSL2591 trouvé");
    // Configure le gain et le temps d'intégration
    tsl.setGain(TSL2591_GAIN_MED);      // Gain moyen
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);  // Temps d'intégration de 300 ms
    return true;
  }

  float lireLuminosite() {
    sensors_event_t evenement;
    tsl.getEvent(&evenement);
    if (evenement.light) {
      return evenement.light;
    } else {
      Serial.println("Erreur de lecture du capteur");
      return -1.0;
    }
  }
};

// Classe CapteurLED pour gérer le seuil et la LED
class CapteurLED {
  private:
  int brocheLED;
  float seuilLuminosite;
  Capteur& capteur;
  public:
  CapteurLED(int brocheLED, float seuilLuminosite, Capteur& capteur)
    : brocheLED(brocheLED), seuilLuminosite(seuilLuminosite), capteur(capteur) {}

  void initialiser() {
    pinMode(brocheLED, OUTPUT);
  }

  void mettreAJour() {
    float luminosite = capteur.lireLuminosite();
    if (luminosite >= 0) {
      Serial.print("Luminosité : ");
      Serial.print(luminosite);
      Serial.println(" lux");

      if (luminosite > seuilLuminosite) {
        digitalWrite(brocheLED, LOW); // Éteindre la LED
        Serial.println("LED éteinte");
      } else {
        digitalWrite(brocheLED, HIGH); // Allumer la LED
        Serial.println("LED allumée");
      }
    }

    delay(500);
  }
};

// Crée une instance de Capteur pour le premier capteur
Capteur capteur1;
CapteurLED capteurLED1(13, 390.0, capteur1); //numéro broche led associée, seuil, nom du capteur

void setup() {
  Serial.begin(9600);
  if (capteur1.initialiser()) {
    capteurLED1.initialiser();
  }
}

void loop() {
  capteurLED1.mettreAJour();
}
