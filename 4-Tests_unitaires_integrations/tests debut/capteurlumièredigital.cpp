// Définition de la classe pour le capteur de lumière
class CapteurLumiere {
  private:
    int pinDO; // Broche connectée à la sortie numérique du capteur

  public:
    // Constructeur : initialise la broche
    CapteurLumiere(int pin) {
      pinDO = pin;
      pinMode(pinDO, INPUT); // Configurer la broche comme entrée
    }

    // Méthode pour lire l'état du capteur (0 ou 1)
    bool lireEtat() {
      return digitalRead(pinDO);
    }
};

CapteurLumiere capteur(2); // Instance de la classe pour le capteur connecté à la broche D2

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Test du capteur de lumière");
}

void loop() {
  // Lecture de l'état du capteur
  bool etat = capteur.lireEtat();

  // Affichage de l'état dans le moniteur série
  if (etat) {
    Serial.println("Pas de lumière détectée !");
  } else {
    Serial.println("Lumière détectée !");
  }

  delay(500); // Pause de 500 ms entre chaque lecture
}
