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

  // LEDs
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Servo
  myServo.attach(servoPin);
  myServo.write(90); // posição inicial

  Serial.println("Pronto para comandos.");
}

void loop() {
  // Verifica comandos pela Serial
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "on") {
      efeitoAtivo = true;
      inicioEfeito = millis();
    }
  }

  if (efeitoAtivo) {
    // Roda efeitos
    knightRiderEffect();
    moveServo();

    // Verifica se passaram 5 segundos
    if (millis() - inicioEfeito >= 5000) {
      pararEfeitos();
      efeitoAtivo = false;
    }
  }
}

// ----------- Efeitos ------------

void knightRiderEffect() {
  for (int i = 0; i < ledCount; i++) {
    turnOnLed(i);
  }
  for (int i = ledCount - 2; i > 0; i--) {
    turnOnLed(i);
  }
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

  if (angle >= 180 || angle <= 0) {
    step = -step;
  }

  delay(20);
}

void pararEfeitos() {
  // Desliga todos os LEDs
  for (int i = 0; i < ledCount; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  // Volta servo à posição neutra
  myServo.write(90);
}
