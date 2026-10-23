# 🌱 FalaMangue

![Arduino](https://img.shields.io/badge/Arduino-00878F?style=flat&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)
![Status](https://img.shields.io/badge/status-prot%C3%B3tipo-EC4899)

> Protótipo educativo que transforma um pedaço do manguezal em um "pet": sensores medem luz, umidade do solo, temperatura, umidade do ar e a aproximação de pessoas, e uma matriz de LED 8x8 mostra como o mangue está se sentindo.

Projeto base do artigo **Mangue Interativo: prototipagem educacional orientada por um roadmap IoT & Maker**, aprovado no Congresso Senac PE 2026 (Espaço do Conhecimento).

🔗 Protótipo da interface no Figma: https://plain-detach-14896837.figma.site/

## 📋 Índice

- [Visão geral](#-visão-geral)
- [Expressões](#-expressões)
- [Arquitetura](#️-arquitetura)
- [Hardware necessário](#️-hardware-necessário)
- [Ligações](#-ligações)
- [Instalação](#-instalação)
- [Autodiagnóstico e Monitor Serial](#-autodiagnóstico-e-monitor-serial)
- [Estrutura do projeto](#-estrutura-do-projeto)
- [Testes](#-testes)
- [Problemas comuns](#️-problemas-comuns)
- [Roadmap](#️-roadmap)
- [Autora](#-autora)

## 👀 Visão geral

O FalaMangue aproxima estudantes do ecossistema do manguezal de um jeito lúdico. Em vez de números numa tela, o mangue "fala" por expressões:

- 🌡️ **Lê o ambiente**: luminosidade, umidade do solo, temperatura e umidade do ar, a cada 30 segundos
- 👋 **Percebe quem chega**: o sensor ultrassônico reage na hora quando alguém se aproxima
- 😊 **Mostra o que sente**: seis expressões na matriz de LED, escolhidas por prioridade
- 🩺 **Se testa sozinho**: ao ligar, roda um autodiagnóstico de todos os componentes e avisa no Monitor Serial o que está com problema

## 😊 Expressões

![Expressões da matriz de LED](docs/expressoes.png)

O mangue escolhe a expressão seguindo esta ordem de prioridade:

| Prioridade | Situação | Expressão |
|---|---|---|
| 1 | Alguém a até 20 cm | ❤️ Coração |
| 2 | Solo seco (umidade abaixo de 30%) | 💧 Gota alternando com triste ("com sede") |
| 3 | Calor extremo (35 °C ou mais) | ☀️ Sol |
| 4 | Pouca luz (leitura do LDR abaixo de 300) | 😴 Dormindo |
| 5 | Umidade do ar acima de 90% | ❤️ Coração |
| 6 | Tudo bem | 😊 Feliz alternando com coração |

## 🏗️ Arquitetura

```
 LDR ─────────┐
 Solo ────────┤   a cada 30 s
 DHT11 ───────┼──────────────────┐
              │                  ▼
 HC-SR04 ─────┴── contínuo ──► Arduino ──► decide a expressão ──► Matriz LED 8x8
                                  │
                                  └──► Monitor Serial (autodiagnóstico e avisos)
```

## 🛠️ Hardware necessário

| Componente | Modelo | Qtd. | Para quê |
|---|---|---|---|
| Microcontrolador | Arduino Uno ou compatível | 1 | Controle principal |
| Sensor de luminosidade | LDR com resistor (divisor de tensão) | 1 | Saber se é dia ou noite |
| Sensor de umidade do solo | Higrômetro resistivo | 1 | Saber se o mangue está "com sede" |
| Sensor de temperatura e umidade | DHT11 | 1 | Calor e umidade do ar |
| Sensor de distância | HC-SR04 (ultrassônico) | 1 | Perceber quem se aproxima |
| Display | Matriz de LED 8x8 com MAX7219 | 1 | Mostrar as expressões |
| Protoboard e jumpers | — | — | Ligações |

## 🔌 Ligações

| Componente | Pino no Arduino |
|---|---|
| LDR | A0 |
| Sensor de umidade do solo | A1 |
| DHT11 (dados) | D4 |
| HC-SR04 | TRIG D9, ECHO D8 |
| Matriz MAX7219 | DIN D7, CLK D5, CS D6 |

Todos os componentes usam 5V e GND do Arduino.

## 🚀 Instalação

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Em **Ferramentas → Gerenciar Bibliotecas**, instale:
   - **DHT sensor library** (Adafruit)
   - **Adafruit Unified Sensor**
   - **LedControl**
3. Abra `FalaMangue/FalaMangue.ino`.
4. Em **Ferramentas**, escolha a placa e a porta, e clique em **Carregar** (Ctrl+U).

## 🩺 Autodiagnóstico e Monitor Serial

Abra o **Monitor Serial** em **9600 bps**. Ao ligar, o FalaMangue testa cada componente e mostra um resumo:

```
        INICIANDO AUTODIAGNOSTICO DO SISTEMA
[OK] LDR (luminosidade) respondendo normalmente. Ultima leitura: 512
[OK] Sensor de umidade do solo respondendo normalmente. Ultima leitura bruta: 640
[OK] DHT11 respondendo normalmente. Temp: 27.00 C | Umidade: 70.00%
[OK] HC-SR04 respondendo normalmente. Distancia medida: 45.20 cm
[TESTE] Acendendo todos os LEDs da matriz por 1 segundo... Confira visualmente se todos acenderam.
-----------------------------------------------------
Resumo: 5/5 componentes OK.
```

Os valores acima são só um exemplo. Como a biblioteca LedControl não retorna erros, a matriz é conferida olhando: todos os LEDs acendem por 1 segundo.

## 📁 Estrutura do projeto

```
FalaMangue/
├── FalaMangue/
│   └── FalaMangue.ino                  # código principal
├── testes/
│   ├── leds_piscar_dois/               # primeiros passos: 2 LEDs piscando
│   ├── leds_um_por_vez/                # primeiros passos: 3 LEDs, um de cada vez
│   ├── leds_acender_em_sequencia/      # primeiros passos: 3 LEDs acendendo em sequência
│   ├── teste_ldr/                      # testa só o LDR
│   ├── teste_solo/                     # testa só o sensor de solo
│   ├── teste_dht11/                    # testa só o DHT11
│   ├── teste_ultrassonico/             # testa só o HC-SR04
│   ├── teste_matriz/                   # testa só a matriz de LED
│   ├── carinhas_rotacoes.h             # as 6 expressões em 4 rotações
│   └── FalaMangue_pinos_antigos/       # código completo com a ligação antiga
├── docs/
│   └── expressoes.png                  # prévia das expressões
└── README.md
```

## 🧪 Testes

### Primeiros passos com LEDs

Exercícios simples para começar a usar o Arduino, antes dos sensores:

| Sketch | Pinos | O que faz |
|---|---|---|
| `testes/leds_piscar_dois/` | D12 e D13 | Dois LEDs piscam juntos a cada segundo |
| `testes/leds_um_por_vez/` | D11, D12 e D13 | Acende um LED de cada vez e avisa no Monitor Serial |
| `testes/leds_acender_em_sequencia/` | D11, D12 e D13 | Os três LEDs acendem um após o outro e apagam juntos |

### Teste de cada componente

Antes de montar tudo junto, cada componente pode ser testado sozinho. Abra o sketch, carregue no Arduino e acompanhe pelo Monitor Serial em 9600 bps.

| Sketch | Componente | Pinos usados no teste | O que mostra |
|---|---|---|---|
| `testes/teste_ldr/` | LDR | A0, LED no D9 | A leitura de luz. O LED acende quando a leitura passa de 400 (ambiente escuro) |
| `testes/teste_solo/` | Sensor de umidade do solo | A0 | A leitura e "Solo seco" (acima de 500) ou "Solo umido" |
| `testes/teste_dht11/` | DHT11 | D12 | Temperatura e umidade do ar a cada 2 segundos |
| `testes/teste_ultrassonico/` | HC-SR04 | TRIG D9, ECHO D10 | A distância em cm a cada meio segundo |
| `testes/teste_matriz/` | Matriz MAX7219 | DIN D12, CLK D10, CS D11 | Uma carinha feliz piscando |

Os testes usam pinos diferentes do código principal. Ligue o componente nos pinos da tabela acima ou troque os números no começo do sketch.

### Outros arquivos de teste

- **`testes/carinhas_rotacoes.h`**: as 6 expressões em 4 rotações (0°, 90°, 180° e 270°). Serve para descobrir em qual posição a matriz física foi montada. Depois de achar a rotação certa, troque os bitmaps do `FalaMangue.ino` pelos dessa rotação.
- **`testes/FalaMangue_pinos_antigos/`**: o mesmo código com a ligação usada antes (DHT11 no D2 e matriz em DIN D12, CLK D10, CS D11). Útil para montagens que ainda usam essa ligação.

## 🛠️ Problemas comuns

| Mensagem no Monitor Serial | O que verificar |
|---|---|
| `[FALHA] LDR ... presas em 0 ou 1023` | Fio do LDR solto ou resistor do divisor faltando |
| `[FALHA] Sensor de umidade do solo ...` | Ligação do sensor de solo e se ele está enterrado na terra |
| `[FALHA] Sensor DHT11 nao respondeu` | Alimentação do DHT11 e o fio de dados. O DHT11 é lento: espere alguns segundos e reinicie |
| `[AVISO] Sensor ultrassonico ... sem retorno de eco` | Sem nada na frente do sensor isso é normal. Se aparecer com algo perto, confira TRIG e ECHO |
| Matriz não acende no teste | Ligações DIN, CLK e CS e a alimentação do módulo |
| Expressões aparecem de lado ou de cabeça para baixo | Use `testes/carinhas_rotacoes.h` para achar a rotação certa |

## 🗺️ Roadmap

- ✅ Leitura dos sensores e expressões na matriz de LED
- ✅ Autodiagnóstico ao ligar
- 🔄 Enviar as leituras para um app ou dashboard (`enviarDados`)
- 🔄 Receber comandos de fora, como mostrar uma expressão a distância (`receberDadosExternos` e `reagirRemotamente`)
- 📋 Integrar com o fórum onde os estudantes trocam observações

## 👩‍💻 Autora

**Jamile Keiller**: [@jamilekeiller](https://github.com/jamilekeiller) · [LinkedIn](https://www.linkedin.com/in/jamilekeiller)
