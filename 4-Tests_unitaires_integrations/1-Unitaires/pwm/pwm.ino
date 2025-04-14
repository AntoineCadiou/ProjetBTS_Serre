int ledPin = 10;  // Broche où est branchée la LED

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);  // Allume la LED
  delay(5000);                 // Attend 5 secondes

  digitalWrite(ledPin, LOW);   // Éteint la LED
  delay(5000);                // Attend 10 secondes
}
