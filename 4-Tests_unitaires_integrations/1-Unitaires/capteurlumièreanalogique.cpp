// Définition de la classe pour le capteur analogique
class CapteurLumiereAnalogique {
  private:
    int pin; // Broche connectée au capteur

  public:
    // Constructeur pour initialiser la broche
    CapteurLumiereAnalogique(int broche) {
      pin = broche;
    }

    // Méthode pour lire la valeur analogique
    int lireValeur() {
      return analogRead(pin);
    }

    // Méthode pour afficher la valeur analogique dans le moniteur série
    void afficherValeur() {
      int valeur = lireValeur();
      Serial.print("Valeur luminosité = ");
      Serial.println(valeur);
    }
};

// Création d'une instance de la classe pour le capteur connecté à A0
CapteurLumiereAnalogique capteur(0);

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Lecture du capteur de lumière analogique");
}

void loop() {
  // Utilisation de la méthode de la classe pour lire et afficher la valeur
  capteur.afficherValeur();

  // Pause entre les lectures
  delay(1000);
}
