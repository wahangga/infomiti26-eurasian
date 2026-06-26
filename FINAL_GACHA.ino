#include <Servo.h>

// PIN DEFINITIONS
const int ledPins[] = {2, 3, 4, 5}; // R1, Y1, R2, Y2
const int btnPins[] = {6, 7};       // Buttons for Yellow LEDs
const int ldrPins[] = {A0, A1};     // Sensors for Red LEDs
const int servoPin = 9;             // Servo Motor

// SERVO OBJECT
Servo myServo;
const int servoClose = 0;           // Starting/closed position
const int servoOpen  = 90;          // Open position (exactly 90°)

// SETTINGS
const int chaseSpeed = 150;
const int blinkSpeed = 200;
int lightThreshold = 150;           // Adjust for your room

// TIMING VARIABLES
unsigned long prevChaseMillis = 0;
unsigned long prevBlinkMillis = 0;
int chaseStep = 0;
bool blinkState = LOW;

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(btnPins[0], INPUT_PULLUP);
  pinMode(btnPins[1], INPUT_PULLUP);

  myServo.attach(servoPin);
  myServo.write(servoClose); // Start at closed position

  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. READ ALL SENSORS
  bool r1_trigger = (analogRead(ldrPins[0]) < lightThreshold);
  bool y1_trigger = (digitalRead(btnPins[0]) == LOW);
  bool r2_trigger = (analogRead(ldrPins[1]) < lightThreshold);
  bool y2_trigger = (digitalRead(btnPins[1]) == LOW);

  // 2. CHECK CONDITIONS

  // CONDITION 3: ALL FOUR ARE ACTIVATED
  if (r1_trigger && y1_trigger && r2_trigger && y2_trigger) {
    handleStep3();
  }

  // CONDITION 2: AT LEAST ONE (BUT NOT ALL) ACTIVATED
  else if (r1_trigger || y1_trigger || r2_trigger || y2_trigger) {
    myServo.write(servoClose);
    handleStep2(currentMillis, r1_trigger, y1_trigger, r2_trigger, y2_trigger);
  }

  // CONDITION 1: NOTHING ACTIVATED
  else {
    myServo.write(servoClose);
    handleStep1(currentMillis);
  }
}

// ==========================================
// LOGIC FOR STEP 1 (Sequential Chase)
// ==========================================
void handleStep1(unsigned long currentMillis) {
  if (currentMillis - prevChaseMillis >= chaseSpeed) {
    prevChaseMillis = currentMillis;
    for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LOW);
    digitalWrite(ledPins[chaseStep], HIGH);
    chaseStep++;
    if (chaseStep >= 4) chaseStep = 0;
  }
}

// ==========================================
// LOGIC FOR STEP 2 (Individual Blinking)
// ==========================================
void handleStep2(unsigned long currentMillis, bool r1, bool y1, bool r2, bool y2) {
  if (currentMillis - prevBlinkMillis >= blinkSpeed) {
    prevBlinkMillis = currentMillis;
    blinkState = !blinkState;
  }
  digitalWrite(ledPins[0], r1 ? blinkState : LOW);
  digitalWrite(ledPins[1], y1 ? blinkState : LOW);
  digitalWrite(ledPins[2], r2 ? blinkState : LOW);
  digitalWrite(ledPins[3], y2 ? blinkState : LOW);
}

// ==========================================
// LOGIC FOR STEP 3 (Puzzle Solved)
// ==========================================
void handleStep3() {
  // Light all LEDs solid
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], HIGH);

  // Open to 90°
  myServo.write(servoOpen);

  // Wait 5 seconds with door open
  delay(5000);

  // Close back to 0°
  myServo.write(servoClose);

  // Turn LEDs off after cycle completes
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LOW);
}