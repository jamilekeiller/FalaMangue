// C++ code
//

#define LED_1 13
#define LED_2 12
#define LED_3 11


void setup()
{
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
}

void loop()
{
  digitalWrite(LED_1, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(LED_2, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(LED_3, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)

  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
}
