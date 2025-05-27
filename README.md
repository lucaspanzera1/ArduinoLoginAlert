# ArduinoLoginAlert

**ArduinoLoginAlert** é uma aplicação Node.js que roda localmente e se comunica com um Arduino via USB. Ela monitora uma API externa hospedada na nuvem (no caso, a [TaskManagerAPI](https://taskmanagerapi-production-ad12.up.railway.app/api/)) e, ao detectar um login bem-sucedido, envia um comando para o Arduino executar efeitos visuais e motores como uma forma de "alerta físico".

---

## 🚀 Funcionalidades

* Monitora requisições de login da API em nuvem.
* Quando detecta login bem-sucedido (HTTP 200), envia comando ao Arduino.
* O Arduino acende LEDs e move um servo motor por 5 segundos como alerta.

---

## 🤺 Requisitos

### No servidor local (Node.js)

* Node.js v18+
* Arduino conectado via USB
* Dependências:

  * `node-fetch`
  * `serialport`
  * `dotenv`

### No Arduino

* Arduino Uno (ou compatível)
* LEDs conectados aos pinos 2 a 6
* Servo motor conectado ao pino 10

---

## 📝 Instalação

### 1. Clone este repositório

```bash
git clone https://github.com/lucaspanzera1/ArduinoLoginAlert.git
cd ArduinoLoginAlert
```

### 2. Instale as dependências

```bash
npm install
```

### 3. Configure as variáveis de ambiente


No Arquivo LoginAlert.js, altere 

```env
({ path: '/dev/ttyACM0', baudRate: 9600 });

const response = await fetch('https://taskmanagerapi-production-ad12.up.railway.app/api/arduino/comando');
```

Substitua `/dev/ttyACM0` com o caminho real da porta serial do seu Arduino. Para descobrir no macOS:

```bash
ls /dev/tty.*
```

Exemplo comum:

```bash
/dev/tty.usbmodem1101
```

---

## ⚖️ Como usar

1. **Ligue seu Arduino via USB**
2. **Suba o sketch (firmware) atualizado no Arduino** \[ver abaixo]
3. **Inicie o script Node localmente:**

```bash
node LogintAlert.js
```

O script:

* Verifica periodicamente se houve login na API remota
* Envia "on" para o Arduino se detectar um login

O Arduino então:

* Acende LEDs em sequência estilo Knight Rider
* Move o servo motor suavemente
* Após 5 segundos, desliga tudo

---

## 🛠️ Integração com a API remota

A [TaskManagerAPI](https://github.com/lucaspanzera1/TaskManagerAPI) é uma API REST hospedada na nuvem com autenticação usando Supabase.

A rota `/login` foi modificada para, após um login bem-sucedido, **gravar um "sinal" de login recente em um banco ou variável em memória**. O `LoginAlert.js` faz polling nessa rota para verificar se deve ativar o Arduino.

---

## 🔍 Exemplo de firmware Arduino

```cpp
#include <Servo.h>

const int ledPins[] = {2, 3, 4, 5, 6};
const int ledCount = sizeof(ledPins) / sizeof(ledPins[0]);
const int delayTime = 100;

const int servoPin = 10;
Servo myServo;

bool efeitoAtivo = false;
unsigned long inicioEfeito = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  myServo.attach(servoPin);
  myServo.write(90);
  Serial.println("Pronto para comandos.");
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando == "on") {
      efeitoAtivo = true;
      inicioEfeito = millis();
    }
  }

  if (efeitoAtivo) {
    knightRiderEffect();
    moveServo();
    if (millis() - inicioEfeito >= 5000) {
      pararEfeitos();
      efeitoAtivo = false;
    }
  }
}

void knightRiderEffect() {
  for (int i = 0; i < ledCount; i++) turnOnLed(i);
  for (int i = ledCount - 2; i > 0; i--) turnOnLed(i);
}

void turnOnLed(int index) {
  digitalWrite(ledPins[index], HIGH);
  delay(delayTime);
  digitalWrite(ledPins[index], LOW);
}

void moveServo() {
  static int angle = 0;
  static int step = 5;
  myServo.write(angle);
  angle += step;
  if (angle >= 180 || angle <= 0) step = -step;
  delay(20);
}

void pararEfeitos() {
  for (int i = 0; i < ledCount; i++) digitalWrite(ledPins[i], LOW);
  myServo.write(90);
}
```

---

## 🚫 Possíveis problemas

* **Arduino não responde:** verifique se a porta serial está correta e se o sketch foi enviado.
* **Permissão negada na serial (Linux/macOS):** execute com permissões ou adicione seu usuário ao grupo `dialout`.
* **API remota não retorna dados esperados:** verifique o endpoint GET `https://taskmanagerapi-production-ad12.up.railway.app/api/arduino/comando` ou semelhante.

---

## 🌟 Autor

Desenvolvido por [Lucas Panzera](https://github.com/lucaspanzera1) como experiência de integração físico-digital entre uma API de autenticação e hardware Arduino.

---

## 🎉 Sugestões futuras

* Integração com Telegram/WhatsApp
* Modo noturno (sem alertas visuais)
* Suporte a múltiplos tipos de eventos (signup, delete, etc.)
* Painel WebSocket em tempo real

---
