const int sensor = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int leitura = analogRead(sensor);

  Serial.print("Valor: ");
  Serial.print(leitura);

  if (leitura > 500) {
    Serial.println(" - Solo seco");
  } else {
    Serial.println(" - Solo umido");
  }

  delay(1000);
}
