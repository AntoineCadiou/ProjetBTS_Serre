#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include "config.h"

class Lumiere {
private:
    int broches[2]; // Utilisation d'un tableau pour stocker les broches

public:
    Lumiere(int broche1, int broche2) {
        broches[0] = broche1;
        broches[1] = broche2;
        pinMode(broche1, OUTPUT);
        pinMode(broche2, OUTPUT);
    }

    void definirLuminosite(int luminosite) {
        analogWrite(broches[0], luminosite);
        analogWrite(broches[1], luminosite);
    }
};

class Capteur {
private:
    int broche;

public:
    Capteur(int broche) : broche(broche) {
        pinMode(broche, INPUT);
    }

    int lireValeur() const {
        return analogRead(broche);
    }
};

class Zone {
private:
    Capteur capteurs[2]; // Utilisation d'un tableau pour stocker les capteurs
    Lumiere lumiere;

public:
    Zone(Capteur capteur1, Capteur capteur2, Lumiere lumiere)
        : capteurs{capteur1, capteur2}, lumiere(lumiere) {}

    void gererLuminosite() {
        int sommeValeurs = 0;
        for (int i = 0; i < 2; ++i) {
            sommeValeurs += capteurs[i].lireValeur();
        }
        int valeurMoyenne = sommeValeurs / 2;
        int luminosite = map(valeurMoyenne, 0, 1023, 255, 0); // Inverser la logique
        lumiere.definirLuminosite(luminosite);
    }
};

// Gestion du WiFi
class WiFiManager {
  public:
    WiFiManager() {}

    void connecterWiFi() {
      Serial.println("Connexion au WiFi en cours...");
      WiFi.begin(ssid, password);

      int essais = 0;
      while (WiFi.status() != WL_CONNECTED && essais < 20) {
        delay(500);
        Serial.print(".");
        essais++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnecté au WiFi!");
        Serial.print("Adresse IP de l'arduino : ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\nErreur de connexion au WiFi.");
      }
    }

    bool estConnecte() {
      return WiFi.status() == WL_CONNECTED;
    }
};

WiFiManager wifi;

// Création des objets pour chaque zone
Lumiere lumiere1(10, 11);
Capteur capteur1A0(A0);
Capteur capteur1A1(A1);
Zone zone1(capteur1A0, capteur1A1, lumiere1);

Lumiere lumiere2(5, 6); // Exemple pour une deuxième lampe sur d'autres broches
Capteur capteur2A2(A2);
Capteur capteur2A3(A3);
Zone zone2(capteur2A2, capteur2A3, lumiere2);

void setup() {
    Serial.begin(9600);
    wifi.connecterWiFi();
}

void loop() {
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
    delay(5000);
  }
    zone1.gererLuminosite();
    zone2.gererLuminosite();

    // Afficher les valeurs de luminosité sur le moniteur série
    Serial.print("Luminosité Zone 1: ");
    Serial.println(map(capteur1A0.lireValeur(), 0, 1023, 255, 0));
    Serial.print("Luminosité Zone 2: ");
    Serial.println(map(capteur2A2.lireValeur(), 0, 1023, 255, 0));

    delay(100); // Petit délai pour éviter de saturer le moniteur série
}
