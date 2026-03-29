#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// ---------------- PIN DEFINITIONS ----------------
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 25

#define MODE_SWITCH 33
#define MODE_LED 2

// ---------------- VARIABLES ----------------
char currentCommand = 'S';
unsigned long lastCommandTime = 0;
unsigned long rcTimeout = 2000;

// ---------------- SETUP ----------------
void setup() {
  SerialBT.begin("ESP32_ROBOT");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(MODE_SWITCH, INPUT_PULLUP);
  pinMode(MODE_LED, OUTPUT);

  stopMotors();
}

// ---------------- MOTOR FUNCTIONS ----------------
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentCommand = 'S';
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ---------------- SAFE COMMAND APPLY ----------------
void applyCommand(char cmd) {
  if (cmd == currentCommand) return;

  stopMotors();
  delay(30);

  currentCommand = cmd;

  switch (cmd) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': left(); break;
    case 'R': right(); break;
    default: stopMotors(); break;
  }
}

// ---------------- RC MODE ----------------
void rcMode() {
  digitalWrite(MODE_LED, HIGH);

  if (!SerialBT.hasClient()) {
    stopMotors();
    return;
  }

  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    lastCommandTime = millis();

    if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' || cmd == 'S') {
      applyCommand(cmd);
    }
  }

  // Safety timeout
  if (millis() - lastCommandTime > rcTimeout) {
    stopMotors();
  }
}

// ---------------- STRAIGHT MODE ----------------
void straightMode() {
  digitalWrite(MODE_LED, LOW);
  applyCommand('F');
}

// ---------------- LOOP ----------------
void loop() {
  bool rcModeActive = (digitalRead(MODE_SWITCH) == LOW);

  if (rcModeActive) {
    rcMode();
  } else {
    straightMode();
  }
}
