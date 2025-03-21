/// Code qui recupère les valeurs des capteurs, qui fais la moyenne de ces valeurs par zone
// et qui allume la led de la zone si la valeur est inférieure au seuil ///
#include <WiFi.h>
#include "config.h"

// Classe pour un capteur de lumière analogique
class CapteurLumiereAnalogique {
  private:
    int pin; // Broche connectée au capteur

  public:
    // Constructeur
    CapteurLumiereAnalogique(int broche) {
      pin = broche;
    }

    // Méthode pour lire la valeur analogique
    int lireValeur() {
      return analogRead(pin);
    }
};

// Classe pour une zone avec plusieurs capteurs et une LED
class Zone {
  private:
    CapteurLumiereAnalogique* capteurs; // Tableau de capteurs
    int nombreCapteurs;                // Nombre de capteurs dans la zone
    int pinLED;                        // Broche connectée à la LED
    const int SEUIL = 990;             // Constante seuil pour allumer la LED

  public:
    // Constructeur
    Zone(CapteurLumiereAnalogique* listeCapteurs, int nbCapteurs, int brocheLED) {
      capteurs = listeCapteurs;
      nombreCapteurs = nbCapteurs;
      pinLED = brocheLED;

      // Configuration de la broche LED comme sortie
      pinMode(pinLED, OUTPUT);
      digitalWrite(pinLED, LOW); // LED éteinte par défaut
    }

    // Méthode pour calculer la moyenne des valeurs des capteurs
    float calculerMoyenne() {
      int somme = 0;
      for (int i = 0; i < nombreCapteurs; i++) {
        somme += capteurs[i].lireValeur();
      }
      return somme / (float)nombreCapteurs;
    }

    // Méthode pour vérifier le seuil et contrôler la LED
    void verifierSeuilEtControleLED(String nomZone) {
      float moyenne = calculerMoyenne();

      // Affichage de la moyenne
      Serial.print("Moyenne ");
      Serial.print(nomZone);
      Serial.print(" : ");
      Serial.println(moyenne);

      // Contrôle de la LED
      if (moyenne < SEUIL) { // Allumer la LED si la moyenne dépasse le seuil
        digitalWrite(pinLED, HIGH); 
        Serial.print("LED ");
        Serial.print(nomZone);
        Serial.println(" ALLUMÉE !");
      } else { // Éteindre la LED sinon
        digitalWrite(pinLED, LOW); 
        Serial.print("LED ");
        Serial.print(nomZone);
        Serial.println(" ÉTEINTE !");
      }
    }
};

// Classe principale pour la gestion des zones
class GestionZones {
  private:
    Zone* zones;    // Tableau de zones
    int nombreZones; // Nombre de zones

  public:
    // Constructeur
    GestionZones(Zone* listeZones, int nbZones) {
      zones = listeZones;
      nombreZones = nbZones;
    }

    // Méthode pour vérifier toutes les zones
    void verifierToutesLesZones() {
      for (int i = 0; i < nombreZones; i++) {
        Serial.print("Zone ");
        Serial.print(i + 1);
        Serial.println(" :");
        zones[i].verifierSeuilEtControleLED("Zone " + String(i + 1));
      }
      Serial.println("-----------------------");
    }
};

class WiFiManager {
  public:
    // Constructeur
    WiFiManager() {}

    // Méthode pour se connecter au WiFi
    void connecterWiFi() {
      Serial.println("Connexion au WiFi en cours...");
      WiFi.begin(SSID, PASSWORD);
      
      int essais = 0;
      while (WiFi.status() != WL_CONNECTED && essais < 20) { // Limite le nombre d'essais
        delay(500);
        Serial.print(".");
        essais++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnecté au WiFi!");
        Serial.print("Adresse IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\nErreur de connexion au WiFi.");
      }
    }

    // Méthode pour vérifier la connexion
    bool estConnecte() {
      return WiFi.status() == WL_CONNECTED;
    }
};

// Instances des capteurs
CapteurLumiereAnalogique capteur1(0);
CapteurLumiereAnalogique capteur2(1);
CapteurLumiereAnalogique capteur3(2);
CapteurLumiereAnalogique capteur4(4);
CapteurLumiereAnalogique capteur5(5);

// Création des zones
CapteurLumiereAnalogique capteursZone1[] = {capteur1, capteur2, capteur3};
Zone zone1(capteursZone1, 3, 13); // Zone 1 avec LED sur la broche 13

CapteurLumiereAnalogique capteursZone2[] = {capteur4, capteur5};
Zone zone2(capteursZone2, 2, 12); // Zone 2 avec LED sur la broche 12

// Création de la gestion des zones
Zone listeZones[] = {zone1, zone2};
GestionZones gestionZones(listeZones, 2);

WiFiManager wifi;

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  wifi.connecterWiFi();
  Serial.println("Gestion des zones et contrôle des LEDs");
}

void loop() {

  // Vérification des seuils pour toutes les zones
  gestionZones.verifierToutesLesZones();
  delay(1000); // Pause entre les lectures
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
    delay(5000);
  }

}