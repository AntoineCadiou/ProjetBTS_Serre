#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include "config.h"

// Classe pour un capteur de lumière analogique
class CapteurLumiereAnalogique {
  private:
    int pin;

  public:
    CapteurLumiereAnalogique(int broche) {
      pin = broche;
    }

    int lireValeur() {
      return analogRead(pin);
    }
};

// Classe pour une zone avec plusieurs capteurs et une LED
class Zone {
  private:
    CapteurLumiereAnalogique* capteurs;
    int nombreCapteurs;
    int pinLED;
    const int SEUIL = 990;

  public:
    Zone(CapteurLumiereAnalogique* listeCapteurs, int nbCapteurs, int brocheLED) {
      capteurs = listeCapteurs;
      nombreCapteurs = nbCapteurs;
      pinLED = brocheLED;
      pinMode(pinLED, OUTPUT);
      digitalWrite(pinLED, LOW);
    }

    float calculerMoyenne() {
      int somme = 0;
      for (int i = 0; i < nombreCapteurs; i++) {
        somme += capteurs[i].lireValeur();
      }
      return somme / (float)nombreCapteurs;
    }

    void verifierSeuilEtControleLED(String nomZone) {
      float moyenne = calculerMoyenne();
      Serial.print("Moyenne ");
      Serial.print(nomZone);
      Serial.print(" : ");
      Serial.println(moyenne);

      if (moyenne < SEUIL) {
        digitalWrite(pinLED, HIGH);
        Serial.print("LED ");
        Serial.print(nomZone);
        Serial.println(" ALLUMÉE !");
      } else {
        digitalWrite(pinLED, LOW);
        Serial.print("LED ");
        Serial.print(nomZone);
        Serial.println(" ÉTEINTE !");
      }
    }
};

// Classe pour la gestion des zones
class GestionZones {
  private:
    Zone* zones;
    int nombreZones;

  public:
    GestionZones(Zone* listeZones, int nbZones) {
      zones = listeZones;
      nombreZones = nbZones;
    }

    void verifierToutesLesZones() {
      for (int i = 0; i < nombreZones; i++) {
        Serial.print("Zone ");
        Serial.print(i + 1);
        Serial.println(" :");
        zones[i].verifierSeuilEtControleLED("Zone " + String(i + 1));
      }
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
        Serial.print("Adresse IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\nErreur de connexion au WiFi.");
      }
    }

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

// Instances des capteurs
CapteurLumiereAnalogique capteur1(0);
CapteurLumiereAnalogique capteur2(1);
CapteurLumiereAnalogique capteur3(2);
CapteurLumiereAnalogique capteur4(4);
CapteurLumiereAnalogique capteur5(5);

// Création des zones
CapteurLumiereAnalogique capteursZone1[] = {capteur1, capteur2, capteur3};
Zone zone1(capteursZone1, 3, 13);

CapteurLumiereAnalogique capteursZone2[] = {capteur4, capteur5};
Zone zone2(capteursZone2, 2, 12);

Zone listeZones[] = {zone1, zone2};
GestionZones gestionZones(listeZones, 2);

WiFiManager wifi;
DataSender dataSender;

void envoyerMesures() {
  String xmlData = "<mesures>";

  // Envoi des mesures avec la zone à laquelle appartient chaque capteur
  xmlData += "<capteur id='1' zone='1'>" + String(capteur1.lireValeur()) + "</capteur>";
  xmlData += "<capteur id='2' zone='1'>" + String(capteur2.lireValeur()) + "</capteur>";
  xmlData += "<capteur id='3' zone='1'>" + String(capteur3.lireValeur()) + "</capteur>";
  xmlData += "<capteur id='4' zone='2'>" + String(capteur4.lireValeur()) + "</capteur>";
  xmlData += "<capteur id='5' zone='2'>" + String(capteur5.lireValeur()) + "</capteur>";

  xmlData += "</mesures>";

  dataSender.envoyerDonnees(xmlData);  // Envoi des données XML au serveur
}

void setup() {
  Serial.begin(9600);
  wifi.connecterWiFi();
  Serial.println("Gestion des zones et contrôle des LEDs");
}

void loop() {
  gestionZones.verifierToutesLesZones();
  envoyerMesures();
  Serial.println("-----------------------");
  delay(5000);
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
    delay(5000);
  }
}
