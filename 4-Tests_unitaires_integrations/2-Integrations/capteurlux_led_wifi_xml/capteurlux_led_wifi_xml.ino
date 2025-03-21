#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include <WiFi.h>
#include "config.h"
#include <ArduinoHttpClient.h>

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

class WiFiManager {
  public:
    // Constructeur
    WiFiManager() {}

    // Méthode pour se connecter au WiFi
    void connecterWiFi() {
      Serial.println("Connexion au WiFi en cours...");
      WiFi.begin(ssid, password);
      
      int essais = 0;
      while (WiFi.status() != WL_CONNECTED && essais < 10) { // Limite le nombre d'essais
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
// Classe pour envoyer les données au serveur
class DataSender {
  private:
    WiFiClient wifiClient;
    HttpClient httpClient;

  public:
    DataSender() : httpClient(wifiClient, serverIP, 80) {}

    void envoyerDonnees(const String& xmlData) {
      if (WiFi.status() == WL_CONNECTED) {
        httpClient.beginRequest();
        httpClient.post(apiEndpoint);
        httpClient.sendHeader("Content-Type", "application/xml");
        httpClient.sendHeader("Content-Length", String(xmlData.length()));
        httpClient.beginBody();
        httpClient.print(xmlData);
        delay(100);
        httpClient.endRequest();

        int httpCode = httpClient.responseStatusCode();
        Serial.print("Code HTTP : ");
        Serial.println(httpCode);

        if (httpCode > 0) {
          String payload = httpClient.responseBody();
          Serial.print("Réponse serveur : ");
          Serial.println(payload);
        } else {
          Serial.print("Erreur lors de l'envoi: ");
          Serial.println(httpClient.responseStatusCode());
        }
      } else {
        Serial.println("WiFi non connecté, impossible d'envoyer les données.");
      }
    }
};

WiFiManager wifi;
DataSender dataSender;

// Crée une instance de Capteur pour le premier capteur
Capteur capteur1;
CapteurLED capteurLED1(13, 390.0, capteur1); //numéro broche led associée, seuil, nom du capteur

void envoyerMesures() {
  String xmlData = "<mesures>";
  // Envoi des mesures avec la zone à laquelle appartient chaque capteur
  xmlData += "<capteur>" + String(capteur1.lireLuminosite()) + "</capteur>";
  xmlData += "</mesures>";

  dataSender.envoyerDonnees(xmlData);  // Envoi des données XML au serveur
}

void setup() {
  Serial.begin(9600);
  Serial.println("Démarrage du programme...");
  wifi.connecterWiFi();
  if (capteur1.initialiser()) {
    capteurLED1.initialiser();
  }
}

void loop() {
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
  }
  capteurLED1.mettreAJour();
  envoyerMesures();
  delay(1000);
}
