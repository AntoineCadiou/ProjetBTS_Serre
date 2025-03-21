// Définition de la classe pour les capteurs analogiques
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

    // Méthode pour afficher la valeur lue
    void afficherValeur() {
      int valeur = lireValeur();
      Serial.print("Valeur sur A");
      Serial.print(pin);
      Serial.print(" : ");
      Serial.println(valeur);
    }
};

// Définition de la classe pour une zone
class Zone {
  private:
    CapteurLumiereAnalogique* capteurs; // Tableau de capteurs
    int nombreCapteurs;                // Nombre de capteurs dans la zone

  public:
    // Constructeur
    Zone(CapteurLumiereAnalogique* listeCapteurs, int nbCapteurs) {
      capteurs = listeCapteurs;
      nombreCapteurs = nbCapteurs;
    }

    // Méthode pour afficher les valeurs individuelles des capteurs
    void afficherValeursIndividuelles() {
      for (int i = 0; i < nombreCapteurs; i++) {
        capteurs[i].afficherValeur();
      }
    }

    // Méthode pour calculer la moyenne des valeurs des capteurs
    float calculerMoyenne() {
      int somme = 0;
      for (int i = 0; i < nombreCapteurs; i++) {
        somme += capteurs[i].lireValeur();
      }
      return somme / (float)nombreCapteurs;
    }

    // Méthode pour afficher la moyenne
    void afficherMoyenne(String nomZone) {
      float moyenne = calculerMoyenne();
      Serial.print("Moyenne ");
      Serial.print(nomZone);
      Serial.print(" : ");
      Serial.println(moyenne);
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
Zone zone1(capteursZone1, 3);

CapteurLumiereAnalogique capteursZone2[] = {capteur4, capteur5};
Zone zone2(capteursZone2, 2);

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Lecture des valeurs et moyennes des capteurs par zone");
}

void loop() {
  // Zone 1 : affichage des valeurs et de la moyenne
  Serial.println("Zone 1 :");
  //zone1.afficherValeursIndividuelles();
  zone1.afficherMoyenne("Zone 1");

  // Zone 2 : affichage des valeurs et de la moyenne
  Serial.println("Zone 2 :");
  //zone2.afficherValeursIndividuelles();
  zone2.afficherMoyenne("Zone 2");

  Serial.println("-----------------------");
  delay(1000); // Pause entre les lectures
}
