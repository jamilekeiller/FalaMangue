/*
  - FalaMangue!
  Projeto: Planta/instalação interativa que reage ao ambiente (luminosidade,
  umidade do solo, temperatura, umidade do ar e proximidade) mostrando
  "expressões" em uma matriz de LED 8x8 (MAX7219).
*/
#include <DHT.h>
#include <LedControl.h>

// DEFINIÇÕES DE PINOS
#define PORTA_LDR   A0   // Pino analógico do sensor de luminosidade (LDR)
#define PORTA_SOLO  A1   // Pino analógico do sensor de umidade do solo
#define PINO_DHT    2    // Pino digital de dados do sensor DHT11
#define DHT_TIPO    DHT11

#define PINO_TRIG   9    // Pino de trigger do sensor ultrassônico HC-SR04
#define PINO_ECHO   8    // Pino de echo do sensor ultrassônico HC-SR04

#define PINO_DIN    12   // Pino DIN da matriz de LED (MAX7219)
#define PINO_CLK    10   // Pino CLK da matriz de LED (MAX7219)
#define PINO_CS     11   // Pino CS/LOAD da matriz de LED (MAX7219)

// OBJETOS DE HARDWARE
DHT dht(PINO_DHT, DHT_TIPO);                 // Objeto do sensor DHT11
LedControl matriz(PINO_DIN, PINO_CLK, PINO_CS, 1); // 1 = apenas 1 módulo MAX7219

// VARIÁVEIS DE CONTROLE DE TEMPO
unsigned long anterior = 0;              // Guarda o millis() da última leitura ambiental
const unsigned long intervalo = 30000;   // Intervalo entre leituras ambientais (30s)

// VARIÁVEIS GLOBAIS DE LEITURA
int   leituraLuz;      // Valor bruto do LDR (0-1023)
int   leituraSolo;     // Valor bruto do sensor de solo (0-1023)
int   umidadeSolo;     // Umidade do solo convertida em % (0-100)
float temperatura;     // Temperatura em °C (DHT11)
float umidadeAr;       // Umidade relativa do ar em % (DHT11)
float distancia;       // Distância medida pelo sensor ultrassônico (cm)

// BITMAPS DAS EXPRESSÕES (8 linhas de 8 bits cada, exibidas na matriz de LED)
byte feliz[8]    = {B00111100,B01000010,B10100101,B10000001,B10100101,B10011001,B01000010,B00111100};
byte triste[8]   = {B00111100,B01000010,B10100101,B10000001,B10011001,B10100101,B01000010,B00111100};
byte dormindo[8] = {B00111100,B01000010,B10011001,B10000001,B10000001,B10111101,B01000010,B00111100};
byte gota[8]     = {B00011000,B00111100,B00111100,B01111110,B01111110,B00111100,B00011000,B00000000};
byte coracao[8]  = {B01100110,B11111111,B11111111,B11111111,B01111110,B00111100,B00011000,B00000000};
byte sol[8]      = {B10000001,B01000010,B00100100,B00011000,B00100100,B01000010,B10000001,B00011000};


// FUNÇÕES DE EXIBIÇÃO NA MATRIZ DE LED
/*
  mostrar()
  Envia um bitmap de 8 bytes para a matriz de LED, linha por linha.
  Parâmetro:
    d[] -> vetor de 8 bytes, cada byte representa uma linha da matriz (8 pixels).
  Retorno: nenhum (void).
*/
void mostrar(byte d[]) {
  for (int i = 0; i < 8; i++) {
    matriz.setRow(0, i, d[i]);
  }
}
/*
  animacaoFeliz()
  Alterna entre a expressão "feliz" e "coração" para simular uma animação
  de contentamento. Usada quando todas as condições ambientais estão boas.
*/
void animacaoFeliz() {
  mostrar(feliz);
  delay(600);
  mostrar(coracao);
  delay(400);
  mostrar(feliz);
}
/*
  animacaoSede()
  --------------
  Alterna entre a expressão "gota" e "triste" para indicar que o solo está
  com pouca umidade (a planta está "com sede").
*/
void animacaoSede() {
  mostrar(gota);
  delay(400);
  mostrar(triste);
  delay(400);
}
// FUNÇÕES DE LEITURA DE SENSORES
/*
  lerLuminosidade()
  Lê o valor bruto (0-1023) do sensor LDR e armazena em leituraLuz.
  Não há como detectar falha de hardware apenas com analogRead(), pois ele
  sempre devolve um número. A verificação de mau funcionamento deste sensor
  é feita pela função de diagnóstico testarLDR(), executada no setup().
*/
void lerLuminosidade() {
  leituraLuz = analogRead(PORTA_LDR);
}

/*
  lerUmidadeSolo()
  Lê o valor bruto do sensor de umidade do solo e converte para uma escala
  percentual (0-100%), usando map() e limitando o resultado com constrain().
  Assim como o LDR, não há detecção automática de falha durante a operação
  normal; isso é feito no diagnóstico inicial (testarSoloSensor()).
*/
void lerUmidadeSolo() {
  leituraSolo = analogRead(PORTA_SOLO);
  umidadeSolo = constrain(map(leituraSolo, 1023, 300, 0, 100), 0, 100);
}

/*
  lerTemperaturaUmidade()
  Lê temperatura e umidade do ar do sensor DHT11.
  Caso a leitura falhe (o DHT retorna NaN quando não consegue ler), um aviso
  é impresso no Serial e os valores antigos (temperatura/umidadeAr) são
  mantidos, evitando que um dado inválido "contamine" o restante do código.
*/
void lerTemperaturaUmidade() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("[AVISO] Falha na leitura do sensor DHT11 (temperatura/umidade do ar). Verifique conexao e fiacao.");
    // Mantém os últimos valores válidos (temperatura e umidadeAr não são sobrescritos)
  } else {
    temperatura = t;
    umidadeAr = h;
  }
}

/*
  lerSensoresReacao()
  Dispara um pulso ultrassônico no HC-SR04 e mede o tempo de retorno (echo)
  para calcular a distância em centímetros.
  Se o pulseIn() não detectar retorno dentro do timeout (30ms), ele retorna
  0. Nesse caso, a função registra um aviso e marca distancia = -1 para
  sinalizar leitura inválida (o restante do código já ignora valores <= 0).
*/
void lerSensoresReacao() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long dur = pulseIn(PINO_ECHO, HIGH, 30000);

  if (dur == 0) {
    Serial.println("[AVISO] Sensor ultrassonico (HC-SR04) sem retorno de eco. Verifique conexao ou objeto fora de alcance.");
    distancia = -1;
  } else {
    distancia = dur * 0.0343 / 2.0;
  }
}

/*
  mostrarLeituras()
  Imprime no Monitor Serial um resumo formatado de todas as leituras
  ambientais mais recentes (luminosidade, umidade do solo, temperatura e
  umidade do ar).
*/
void mostrarLeituras() {
  Serial.println("\n----- LEITURA -----");
  Serial.print("Luminosidade: "); Serial.println(leituraLuz);
  Serial.print("Umidade Solo: "); Serial.print(umidadeSolo); Serial.println("%");
  Serial.print("Temperatura: ");  Serial.print(temperatura); Serial.println(" C");
  Serial.print("Umidade Ar: ");   Serial.print(umidadeAr);   Serial.println("%");
}

// FUNÇÃO DE DECISÃO / REAÇÃO
/*
  reagirLocalmente()
  Decide qual expressão exibir na matriz de LED com base nas leituras atuais,
  seguindo uma ordem de prioridade:
    1) Alguém/algo próximo (distancia entre 0 e 20cm)  -> coração
    2) Solo seco (umidadeSolo < 30%)                    -> animação de sede
    3) Calor extremo (temperatura >= 35°C)               -> sol
    4) Pouca luz (leituraLuz < 300)                      -> dormindo
    5) Umidade do ar muito alta (> 90%)                  -> coração
    6) Caso nenhuma condição especial se aplique          -> animação feliz
*/
void reagirLocalmente() {
  if (distancia > 0 && distancia <= 20) { mostrar(coracao); return; }
  if (umidadeSolo < 30)                 { animacaoSede();  return; }
  if (temperatura >= 35)                { mostrar(sol);    return; }
  if (leituraLuz < 300)                 { mostrar(dormindo); return; }
  if (umidadeAr > 90)                   { mostrar(coracao); return; }
  animacaoFeliz();
}

/*
  lerSensoresAmbientais()
  Função "agendadora": só executa as leituras ambientais (luz, solo, DHT,
  distância), a exibição no Serial e a reação visual quando já se passou o
  "intervalo" (30 segundos) desde a última execução. Usa millis() para não
  travar o loop() com delay().
*/
void lerSensoresAmbientais() {
  if (millis() - anterior >= intervalo) {
    anterior = millis();
    lerLuminosidade();
    lerUmidadeSolo();
    lerTemperaturaUmidade();
    // lerSensoresReacao();
    // mostrarLeituras();
    // reagirLocalmente();
  }
}

// FUNÇÕES RESERVADAS PARA COMUNICAÇÃO FUTURA (ainda vazias no projeto original)

/*
  receberDadosExternos()
  Reservada para receber dados de uma fonte externa (ex.: rede, outro
  dispositivo, servidor). Ainda não implementada.
*/
void receberDadosExternos() {}

/*
  reagirRemotamente()
  Reservada para reagir a comandos/dados vindos de fora (ex.: acionar uma
  expressão específica remotamente). Ainda não implementada.
*/
void reagirRemotamente() {}

/*
  enviarDados()
  Reservada para enviar as leituras coletadas para um destino externo
  (ex.: servidor, aplicativo, dashboard). Ainda não implementada.
*/
void enviarDados() {}


/*
   /* FUNÇÕES DE TESTE / AUTODIAGNÓSTICO
   Cada função abaixo testa um componente específico e imprime no Serial se
   ele está "OK" ou em "FALHA", junto com uma explicação do problema quando
   aplicável. Essas funções são chamadas uma única vez, dentro de
   rodarAutoDiagnostico(), que é executada no início do setup(). */

/*
  testarLDR()
  Testa o sensor de luminosidade (LDR).
  Como analogRead() sempre retorna um valor (não existe "erro" de leitura),
  o teste verifica se as leituras estão persistentemente presas em um dos
  extremos (0 ou 1023) em 3 amostras seguidas, o que costuma indicar fiação
  solta, curto-circuito ou sensor desconectado.
  Retorno: true se o sensor parece OK, false se parece haver falha.
*/
bool testarLDR() {
  bool suspeito = true;
  int amostras[3];

  for (int i = 0; i < 3; i++) {
    amostras[i] = analogRead(PORTA_LDR);
    delay(50);
    if (amostras[i] != 0 && amostras[i] != 1023) {
      suspeito = false;
    }
  }

  if (suspeito) {
    Serial.println("[FALHA] LDR (luminosidade): leituras presas em 0 ou 1023. Verifique a fiacao do sensor em A0.");
    return false;
  }

  Serial.print("[OK] LDR (luminosidade) respondendo normalmente. Ultima leitura: ");
  Serial.println(amostras[2]);
  return true;
}

/*
  testarSoloSensor()
  Testa o sensor de umidade do solo com a mesma lógica do testarLDR():
  verifica se as leituras estão presas nos extremos (0 ou 1023).
  Retorno: true se o sensor parece OK, false se parece haver falha.
*/
bool testarSoloSensor() {
  bool suspeito = true;
  int amostras[3];

  for (int i = 0; i < 3; i++) {
    amostras[i] = analogRead(PORTA_SOLO);
    delay(50);
    if (amostras[i] != 0 && amostras[i] != 1023) {
      suspeito = false;
    }
  }

  if (suspeito) {
    Serial.println("[FALHA] Sensor de umidade do solo: leituras presas em 0 ou 1023. Verifique a fiacao do sensor em A1.");
    return false;
  }

  Serial.print("[OK] Sensor de umidade do solo respondendo normalmente. Ultima leitura bruta: ");
  Serial.println(amostras[2]);
  return true;
}

/*
  testarDHT()
  Testa o sensor DHT11 fazendo uma leitura de temperatura e umidade.
  Se qualquer uma das duas vier como NaN (Not a Number), o sensor não
  respondeu corretamente (fiação, alimentação ou o sensor precisa de mais
  tempo entre leituras, o DHT11 é lento).
  Retorno: true se OK, false se falhou.
*/
bool testarDHT() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("[FALHA] Sensor DHT11 nao respondeu. Verifique alimentacao e o pino de dados (pino 2).");
    return false;
  }

  Serial.print("[OK] DHT11 respondendo normalmente. Temp: ");
  Serial.print(t);
  Serial.print(" C | Umidade: ");
  Serial.print(h);
  Serial.println("%");
  return true;
}

/*
  testarUltrassonico()
  Testa o sensor ultrassônico HC-SR04 disparando um pulso e aguardando o
  eco. Se pulseIn() retornar 0 (timeout de 30ms sem eco), consideramos falha
  -- pode ser sensor desconectado, mal alimentado, ou nenhum obstáculo
  dentro do alcance máximo no momento do teste (por isso vale reposicionar
  algo na frente do sensor ao testar).
  Retorno: true se OK, false se falhou.
*/
bool testarUltrassonico() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long dur = pulseIn(PINO_ECHO, HIGH, 30000);

  if (dur == 0) {
    Serial.println("[FALHA] Sensor ultrassonico (HC-SR04) sem eco. Verifique fiacao (TRIG=9, ECHO=8) ou aproxime um objeto na frente do sensor.");
    return false;
  }

  float dist = dur * 0.0343 / 2.0;
  Serial.print("[OK] HC-SR04 respondendo normalmente. Distancia medida: ");
  Serial.print(dist);
  Serial.println(" cm");
  return true;
}

/*
  testarMatrizLed()
  Testa a matriz de LED 8x8 (MAX7219) acendendo todos os LEDs por um
  instante e depois apagando.
  IMPORTANTE: a biblioteca LedControl não retorna nenhum código de erro,
  ou seja, não é possível detectar falha por software aqui -- este teste
  serve apenas para CONFIRMAÇÃO VISUAL. Se a matriz não acender neste
  teste, o problema está na fiação (DIN=12, CLK=10, CS=11) ou na
  alimentação do módulo.
  Retorno: sempre true (o teste em si não falha, é apenas informativo).
*/
bool testarMatrizLed() {
  Serial.println("[TESTE] Acendendo todos os LEDs da matriz por 1 segundo... Confira visualmente se todos acenderam.");

  for (int linha = 0; linha < 8; linha++) {
    matriz.setRow(0, linha, B11111111);
  }
  delay(1000);
  matriz.clearDisplay(0);

  Serial.println("[INFO] Nao e possivel confirmar o funcionamento da matriz por software (a biblioteca LedControl nao retorna erros). Confirme visualmente.");
  return true;
}

/*
  rodarAutoDiagnostico()
  Executa o teste de cada componente do sistema, em sequência, e imprime um
  resumo final indicando quantos testes passaram e quais falharam.
  Chamada uma única vez, no início do setup().
*/
void rodarAutoDiagnostico() {
  Serial.println("        INICIANDO AUTODIAGNOSTICO DO SISTEMA         ");

  bool okLDR   = testarLDR();
  bool okSolo  = testarSoloSensor();
  bool okDHT   = testarDHT();
  bool okUltra = testarUltrassonico();
  bool okMatriz = testarMatrizLed();

  int totalOk = okLDR + okSolo + okDHT + okUltra + okMatriz;

  Serial.println("-----------------------------------------------------");
  Serial.print("Resumo: ");
  Serial.print(totalOk);
  Serial.println("/5 componentes OK.");

  if (totalOk < 5) {
    Serial.println("[ATENCAO] Um ou mais componentes apresentaram falha. Revise as mensagens [FALHA] acima antes de prosseguir.");
  } else {
    Serial.println("Todos os componentes testados responderam corretamente.");
  }
}

// SETUP E LOOP PRINCIPAL
/*
  setup()
  Executado uma única vez ao ligar/resetar o Arduino.
  1) Inicializa a comunicação Serial (9600 bps).
  2) Inicializa o sensor DHT11.
  3) Configura os pinos do sensor ultrassônico.
  4) Configura a matriz de LED (liga o driver, define brilho e limpa a tela).
  5) Roda o autodiagnóstico de todos os componentes.
*/
void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  matriz.shutdown(0, false);   // Tira o driver MAX7219 do modo de economia de energia
  matriz.setIntensity(0, 8);   // Define o brilho da matriz (0 a 15)
  matriz.clearDisplay(0);      // Limpa qualquer conteúdo residual na matriz

  rodarAutoDiagnostico();      // Testa todos os sensores/atuadores e avisa se algo falhar
}

/*
  loop()
  Executado repetidamente enquanto o Arduino estiver ligado.
  1) Atualiza os sensores ambientais a cada 30 segundos (lerSensoresAmbientais).
  2) Atualiza continuamente o sensor de distância (reação mais rápida a
     presença de algo próximo).
  3) Escolhe e exibe a expressão correspondente na matriz de LED.
  4) Chama as funções reservadas para comunicação externa futura.
*/
void loop() {
  // Atualiza os sensores ambientais a cada 30 segundos
  lerSensoresAmbientais();
  // Atualiza continuamente o sensor de distância
  lerSensoresReacao();
  // Escolhe a expressão da planta
  reagirLocalmente();
  // Reservado para comunicação futura
  receberDadosExternos();
  reagirRemotamente();
  enviarDados();
}
