const int PINO_TRIG = 9;
const int PINO_ECHO = 10;

long duracao;
float distancia;
float medirDistancia() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);
  duracao = pulseIn(PINO_ECHO, HIGH);
  return duracao * 0.0343 / 2;
}
void mostrarDistancia(float distancia) {
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
}
void setup() {
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  Serial.begin(9600);
}
void loop() {
  distancia = medirDistancia();
  mostrarDistancia(distancia);
  delay(500);
}
