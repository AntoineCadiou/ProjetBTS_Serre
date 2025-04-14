#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include "config.h"

class Lumiere {
private:
    int broches[2];

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
    Capteur capteurs[2];
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
        int luminosite = map(valeurMoyenne, 0, 1023, 255, 0);
        lumiere.definirLuminosite(luminosite);
    }

    String obtenirDonneesXML(int zoneID) const {
        String xml = "<data>";
        for (int i = 0; i < 2; ++i) {
            xml += "<capteur>";
            xml += "<id>" + String(i + 1) + "</id>";
            xml += "<zone>" + String(zoneID) + "</zone>";
            xml += "<valeur>" + String(capteurs[i].lireValeur()) + "</valeur>";
            xml += "</capteur>";
        }
        xml += "</data>";
        return xml;
    }
};

class BDDManager {
public:
    BDDManager() {}

    void envoyerDonnees(const String& xmlData) {
        WiFiClient wifi;
        String serverAddress = String(serverIP);
        HttpClient client(wifi, serverAddress, serverPort);

        Serial.println("Envoi des données au serveur...");
        client.beginRequest();
        client.post(apiEndpoint);
        client.sendHeader("Content-Type", "application/xml");
        client.sendHeader("Content-Length", xmlData.length());
        client.beginBody();
        client.print(xmlData);
        client.endRequest();

        int statusCode = client.responseStatusCode();
        String response = client.responseBody();

        Serial.print("Code de statut: ");
        Serial.println(statusCode);
        Serial.print("Réponse: ");
        Serial.println(response);

        if (statusCode == 200) {
            Serial.println("Données envoyées avec succès à la base de données.");
        } else {
            Serial.println("Échec de l'envoi des données à la base de données.");
        }
    }
};

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
BDDManager gestionnaireBDD;

// Création des objets pour chaque zone
Lumiere lumiere1(10, 11);
Capteur capteur1A0(A0);
Capteur capteur1A1(A1);
Zone zone1(capteur1A0, capteur1A1, lumiere1);

Lumiere lumiere2(5, 6);
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

    // Envoyer les données des capteurs à la base de données
    String xmlDataZone1 = zone1.obtenirDonneesXML(1);
    String xmlDataZone2 = zone2.obtenirDonneesXML(2);
    gestionnaireBDD.envoyerDonnees(xmlDataZone1);
    delay(1000); // Ajouter un délai pour éviter de surcharger le serveur
    gestionnaireBDD.envoyerDonnees(xmlDataZone2);

    // Afficher les valeurs de luminosité sur le moniteur série
    Serial.print("Luminosité Zone 1: ");
    Serial.println(map(capteur1A0.lireValeur(), 0, 1023, 255, 0));
    Serial.print("Luminosité Zone 2: ");
    Serial.println(map(capteur2A2.lireValeur(), 0, 1023, 255, 0));

    delay(5000); // Augmenter le délai pour éviter de surcharger le microcontrôleur
}
