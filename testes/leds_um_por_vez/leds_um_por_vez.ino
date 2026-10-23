const int PINOS_LED[] = {11, 12, 13};
const int QTD_LEDS =
  sizeof(PINOS_LED) / sizeof(PINOS_LED[0]);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < QTD_LEDS; i++) {
    pinMode(PINOS_LED[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < QTD_LEDS; i++) {
    digitalWrite(PINOS_LED[i], HIGH);
    Serial.println("LED LIGADO");
    delay(1000);
    digitalWrite(PINOS_LED[i], LOW);
    Serial.println("LED DESLIGADO");
    delay(1000);
  }
}
