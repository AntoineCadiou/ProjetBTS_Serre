#include <WiFi.h>
#include "config.h"

class WiFiManager {
  public:
    // Constructeur
    WiFiManager() {}

    // Méthode pour se connecter au WiFi
    void connecterWiFi() {
      Serial.println("Connexion au WiFi en cours...");
      WiFi.begin(ssid, password);
      
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

WiFiManager wifi;

void setup() {
  Serial.begin(9600);
  Serial.println("Démarrage du programme...");
  wifi.connecterWiFi();
}

void loop() {
  if (!wifi.estConnecte()) {
    Serial.println("WiFi déconnecté ! Tentative de reconnexion...");
    wifi.connecterWiFi();
  }
  delay(5000);
}
