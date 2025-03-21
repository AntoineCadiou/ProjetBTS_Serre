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
      Serial.print("Valeur lumineuse sur A");
      Serial.print(pin);
      Serial.print(" : ");
      Serial.println(valeur);
    }
};

// Instances des capteurs
CapteurLumiereAnalogique capteur1(0);
CapteurLumiereAnalogique capteur2(1);
CapteurLumiereAnalogique capteur3(2);
CapteurLumiereAnalogique capteur4(4);
CapteurLumiereAnalogique capteur5(5);

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Lecture de plusieurs capteurs analogiques");
}

void loop() {
  Serial.println("Zone 1 : ");
  // Lecture et affichage des valeurs de chaque capteur
  capteur1.afficherValeur();
  capteur2.afficherValeur();
  capteur3.afficherValeur();

  Serial.println("Zone 2 : ");
  // Lecture et affichage des valeurs de chaque capteur
  capteur4.afficherValeur();
  capteur5.afficherValeur();

  Serial.println("-----------------------");
  delay(1000); // Pause entre les lectures
}