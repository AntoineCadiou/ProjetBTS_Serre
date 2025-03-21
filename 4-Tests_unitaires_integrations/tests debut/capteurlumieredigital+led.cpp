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

    // Méthode pour vérifier si la lumière est détectée
    bool estDetecte() {
      return lireEtat() == LOW; // LOW signifie lumière détectée
    }
};

// Définition de la classe pour la LED
class LED {
  private:
    int pinLED; // Broche connectée à la LED

  public:
    // Constructeur : initialise la broche
    LED(int pin) {
      pinLED = pin;
      pinMode(pinLED, OUTPUT); // Configurer la broche comme sortie
      eteindre(); // Éteindre la LED par défaut
    }

    // Méthode pour allumer la LED
    void allumer() {
      digitalWrite(pinLED, HIGH);
    }

    // Méthode pour éteindre la LED
    void eteindre() {
      digitalWrite(pinLED, LOW);
    }
};

// Instances des classes
CapteurLumiere capteur(2); // Capteur connecté à la broche D2
LED led(13);               // LED connectée à la broche D13

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Test du capteur de lumière avec LED");
}

void loop() {
  // Lecture de l'état du capteur
  bool lumiereDetectee = capteur.estDetecte();

  // Affichage de l'état dans le moniteur série
  if (lumiereDetectee) {
    Serial.println("Lumière détectée !");
    led.eteindre(); // Éteindre la LED
  } else {
    Serial.println("Pas de lumière détectée !");
    led.allumer(); // Allumer la LED
  }

  delay(500); // Pause de 500 ms entre chaque lecture
}
