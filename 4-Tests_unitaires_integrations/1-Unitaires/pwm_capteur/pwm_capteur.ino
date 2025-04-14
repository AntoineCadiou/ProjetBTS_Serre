int capteurPin = A0;  // Broche du capteur analogique
int ledPin = 10;       // Broche PWM de la LED
int valeurCapteur = 0; // Valeur lue du capteur (0-1023)
int luminositeLED = 0; // Valeur PWM de la LED (0-255)

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // Initialise la communication série
}

void loop() {
  // Lire la valeur du capteur (0 à 1023)
  valeurCapteur = analogRead(capteurPin);
  
  // Convertir cette valeur en un signal PWM (0 à 255)
  luminositeLED = map(valeurCapteur, 0, 1023, 0, 255);

  // Appliquer le PWM à la LED
  analogWrite(ledPin, luminositeLED);

  // Afficher les valeurs dans le moniteur série
  Serial.print("Capteur: ");
  Serial.print(valeurCapteur);
  Serial.print(" | LED PWM: ");
  Serial.println(luminositeLED);

  delay(100);  // Petite pause pour éviter la surcharge du port série
}
