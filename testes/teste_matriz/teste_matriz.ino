#include <LedControl.h>
const int PINO_DIN = 12;
const int PINO_CLK = 10;
const int PINO_CS  = 11;
LedControl matriz(PINO_DIN, PINO_CLK, PINO_CS, 1);
byte carinha[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};
void setup() {
  matriz.shutdown(0, false);   // Liga a matriz
  matriz.setIntensity(0, 8);   // Brilho (0 a 15)
  matriz.clearDisplay(0);      // Limpa a matriz
}
void loop() {
  mostrarCarinha();
  delay(2000);
  matriz.clearDisplay(0);
  delay(1000);
}
void mostrarCarinha() {
  for (int linha = 0; linha < 8; linha++) {
    matriz.setRow(0, linha, carinha[linha]);
  }

}
