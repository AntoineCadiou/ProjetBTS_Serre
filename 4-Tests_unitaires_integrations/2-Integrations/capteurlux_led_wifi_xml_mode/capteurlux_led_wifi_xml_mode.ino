#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include <WiFi.h>
#include "config.h"
#include <ArduinoHttpClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Initialisation du client NTP pour obtenir l'heure
WiFiUDP ntpUDP;
// Définir le décalage horaire pour Paris (UTC+1 ou UTC+2 pendant l'heure d'été)
const long utcOffsetInSeconds = 3600; // UTC+1
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Classe XmlParser pour gérer l'extraction des données XML
class XmlParser {
  public:
    XmlParser() {}

    int extraireValeur(const String& xml, const String& balise) {
      int debut = xml.indexOf("<" + balise + ">") + balise.length() + 2;
      int fin = xml.indexOf("</" + balise + ">");
      if (debut == -1 || fin == -1 || debut >= fin) {
        return -1; // Valeur par défaut en cas d'erreur
      }
      return xml.substring(debut, fin).toInt();
    }

    String extraireTexte(const String& xml, const String& balise) {
      int debut = xml.indexOf("<" + balise + ">") + balise.length() + 2;
      int fin = xml.indexOf("</" + balise + ">");
      if (debut == -1 || fin == -1 || debut >= fin) {
        return ""; // Valeur par défaut en cas d'erreur
      }
      return xml.substring(debut, fin);
    }
};

// Classe Capteur pour gérer le capteur de lumière
class Capteur {
  private:
    Adafruit_TSL2591 tsl;
  public:
    Capteur() : tsl(Adafruit_TSL2591(2591)) {}

    bool initialiser() {
      while (!tsl.begin()) {
        Serial.println("Aucun capteur trouvé. Réessayer dans 2 secondes...");
        delay(2000);
      }
      Serial.println("Capteur TSL2591 trouvé");
      tsl.setGain(TSL2591_GAIN_MED);
      tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
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
          digitalWrite(brocheLED, LOW);
          Serial.println("LED éteinte (automatique)");
        } else {
          digitalWrite(brocheLED, HIGH);
          Serial.println("LED allumée (automatique)");
        }
      }

      delay(500);
    }

    void allumerLED() {
      digitalWrite(brocheLED, HIGH);
      Serial.println("LED allumée");
    }

    void eteindreLED() {
      digitalWrite(brocheLED, LOW);
      Serial.println("LED éteinte");
    }
};

class WiFiManager {
  public:
    WiFiManager() {}

    void connecterWiFi() {
      Serial.println("Connexion au WiFi en cours...");
      WiFi.begin(ssid, password);

      int essais = 0;
      while (WiFi.status() != WL_CONNECTED && essais < 10) {
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

// Classe de base pour les modes
class Mode {
  public:
    virtual void executer() = 0;
};

// Classe pour le mode automatique
class ModeAutomatique : public Mode {
  private:
    CapteurLED& capteurLED;
    DataSender& dataSender;
    Capteur& capteur;
  public:
    ModeAutomatique(CapteurLED& capteurLED, DataSender& dataSender, Capteur& capteur)
      : capteurLED(capteurLED), dataSender(dataSender), capteur(capteur) {}

    void executer() override {
      capteurLED.mettreAJour();
      envoyerMesures();
    }

    void envoyerMesures() {
      String xmlData = "<mesures>";
      xmlData += "<capteur>" + String(capteur.lireLuminosite()) + "</capteur>";
      xmlData += "</mesures>";
      dataSender.envoyerDonnees(xmlData);
    }
};

// Classe pour le mode manuel
class ModeManuel : public Mode {
  private:
    CapteurLED& capteurLED;
    XmlParser xmlParser;

  public:
    ModeManuel(CapteurLED& capteurLED) : capteurLED(capteurLED), xmlParser() {}

    void executer() override {
      Serial.println("Mode manuel activé. Envoyez une commande XML pour allumer ou éteindre la LED.");
    }

    void traiterCommande(const String& commande) {
      String etatLED = xmlParser.extraireTexte(commande, "etat_led");
      if (etatLED == "on") {
        capteurLED.allumerLED();
        Serial.println("LED allumée manuellement");
      } else if (etatLED == "off") {
        capteurLED.eteindreLED();
        Serial.println("LED éteinte manuellement");
      }
    }
};

// Classe pour le mode programmé
class ModeProgramme : public Mode {
  private:
    CapteurLED& capteurLED;
    DataSender& dataSender;
    NTPClient& timeClient;
    XmlParser xmlParser;
    int heureDebut, minuteDebut, heureFin, minuteFin;

  public:
    ModeProgramme(CapteurLED& capteurLED, DataSender& dataSender, NTPClient& timeClient)
      : capteurLED(capteurLED), dataSender(dataSender), timeClient(timeClient), xmlParser(), heureDebut(0), minuteDebut(0), heureFin(0), minuteFin(0) {}

    void executer() override {
      timeClient.update();
      int heureActuelle = timeClient.getHours();
      int minuteActuelle = timeClient.getMinutes();

      Serial.print("Heure actuelle : ");
      Serial.println(formaterHeure(heureActuelle, minuteActuelle));

      Serial.print("Heure de début : ");
      Serial.println(formaterHeure(heureDebut, minuteDebut));

      Serial.print("Heure de fin : ");
      Serial.println(formaterHeure(heureFin, minuteFin));

      if (estDansPlage(heureActuelle, minuteActuelle)) {
        capteurLED.allumerLED();
        Serial.println("LED allumée avec le programme");
      } else {
        capteurLED.eteindreLED();
        Serial.println("LED éteinte avec le programme");
      }
    }

    void configurerProgramme(const String& xmlData) {
      heureDebut = xmlParser.extraireValeur(xmlData, "heure_debut");
      minuteDebut = xmlParser.extraireValeur(xmlData, "minute_debut");
      heureFin = xmlParser.extraireValeur(xmlData, "heure_fin");
      minuteFin = xmlParser.extraireValeur(xmlData, "minute_fin");
    }

    bool estDansPlage(int heure, int minute) {
      int debutEnMinutes = heureDebut * 60 + minuteDebut;
      int finEnMinutes = heureFin * 60 + minuteFin;
      int maintenantEnMinutes = heure * 60 + minute;

      if (finEnMinutes < debutEnMinutes) {
        // Cas où l'heure de fin est le lendemain
        finEnMinutes += 24 * 60; // Ajouter 24 heures en minutes
        if (maintenantEnMinutes < debutEnMinutes) {
          maintenantEnMinutes += 24 * 60; // Ajouter 24 heures en minutes
        }
      }

      return maintenantEnMinutes >= debutEnMinutes && maintenantEnMinutes < finEnMinutes;
    }

    String formaterHeure(int heure, int minute) {
      if (heure < 10) {
        if (minute < 10) {
          return "0" + String(heure) + ":0" + String(minute);
        }
        return "0" + String(heure) + ":" + String(minute);
      } else {
        if (minute < 10) {
          return String(heure) + ":0" + String(minute);
        }
        return String(heure) + ":" + String(minute);
      }
    }
};

WiFiManager wifi;
DataSender dataSender;
Capteur capteur1;
CapteurLED capteurLED1(13, 390.0, capteur1);
XmlParser xmlParser;

ModeAutomatique modeAutomatique(capteurLED1, dataSender, capteur1);
ModeManuel modeManuel(capteurLED1);
ModeProgramme modeProgramme(capteurLED1, dataSender, timeClient);

Mode* modeActuel = &modeAutomatique; // Initialiser avec le mode automatique

void setup() {
  Serial.begin(9600);
  Serial.println("Démarrage du programme...");
  wifi.connecterWiFi();
  if (capteur1.initialiser()) {
    capteurLED1.initialiser();
  }
  timeClient.begin();
}

void loop() {
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
  }

  // Vérifier les commandes série pour changer de mode
  if (Serial.available()) {
    String commande = Serial.readStringUntil('\n');
    commande.trim();

    String mode = xmlParser.extraireTexte(commande, "mode");
    if (mode == "auto") {
      modeActuel = &modeAutomatique;
      Serial.println("Mode changé pour : Automatique");
    } else if (mode == "manuel") {
      modeActuel = &modeManuel;
      Serial.println("Mode changé pour : Manuel");
      modeManuel.traiterCommande(commande); // Traiter la commande XML pour l'état de la LED
    } else if (mode == "prog") {
      modeActuel = &modeProgramme;
      Serial.println("Mode changé pour : Programmé");
      modeProgramme.configurerProgramme(commande);
    }
  }

  modeActuel->executer();
  delay(1000);
}
