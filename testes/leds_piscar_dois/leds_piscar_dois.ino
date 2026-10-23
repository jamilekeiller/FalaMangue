int ledPin1 = 12;  // Pino digital para o primeiro LED
int ledPin2 = 13;  // Pino digital para o segundo LED

void setup() {
  pinMode(ledPin1, OUTPUT);  // Configura o pino 12 como saída
  pinMode(ledPin2, OUTPUT);  // Configura o pino 11 como saída
}

void loop() {
  digitalWrite(ledPin1, HIGH);  // Liga o LED 1
  digitalWrite(ledPin2, HIGH);  // Liga o LED 2
  delay(1000);                  // Aguardar 1 segundo
  digitalWrite(ledPin1, LOW);   // Desliga o LED 1
  digitalWrite(ledPin2, LOW);   // Desliga o LED 2
  delay(1000);                  // Aguardar 1 segundo
}
