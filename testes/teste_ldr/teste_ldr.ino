const int PIN_LDR = A0;
const int PIN_LED = 9;
const int LIMIAR_ESCURO = 400;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
}
void loop() {
  const int leituraLuz = analogRead(PIN_LDR);
  Serial.println(leituraLuz);
  const bool ambienteEscuro =
    (leituraLuz > LIMIAR_ESCURO);

  if (ambienteEscuro) {
    digitalWrite(PIN_LED, HIGH);
  } else {
    digitalWrite(PIN_LED, LOW);
  }
}
