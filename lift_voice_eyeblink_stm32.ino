/*
 * ============================================================
 *        ACCESSIBLE LIFT CONTROL SYSTEM — STM32
 * ============================================================
 * Project      : Eye Blink & Voice Controlled Lift for
 *                Disabled Persons
 * Controller   : STM32
 * Framework    : Arduino
 * Author       : [Ramasamy.v,Naveen Kumar.J,Pothi Mathavan]
 * College      : [Ramco Institute Of Technology]
 * Date         : 2025
 * ============================================================
 * Description  :
 * This firmware controls a 3-floor lift system using:
 *   1. Eye Blink Gesture  — Blink 3/4/5 times for floor
 *   2. Offline Voice Recognition — Serial hex commands
 *   3. UART Hex Commands  — 0xA1, 0xA2, 0xA3
 * ============================================================
 */


/* ============================================================
 *  SECTION 1 — PIN DEFINITIONS
 * ============================================================ */

// Stepper Motor Pins
#define IN1 PA3
#define IN2 PA2
#define IN3 PA1
#define IN4 PA0

// Sensor Pins
#define SENSOR_PIN       PA4   // Analog floor detection sensor
#define BLINK_SENSOR_PIN PA5   // Digital eye blink sensor

// Output Pins
#define BUZZER_PIN       PA6   // Buzzer for audio floor feedback


/* ============================================================
 *  SECTION 2 — MOTOR CONFIGURATION
 * ============================================================ */

const int stepsPerRevolution = 2048;  // Steps for one full revolution
const int stepsPerFloor      = 5524;  // Steps needed to move one floor


/* ============================================================
 *  SECTION 3 — GLOBAL VARIABLES
 * ============================================================ */

int  currentFloor    = -1;    // Current floor (-1 = unknown/uninitialized)
int  blinkCount      = 0;     // Number of blinks detected in current window
bool blinkActive     = false; // TRUE when a blink is currently being detected
unsigned long blinkStartTime = 0; // Timestamp of first blink in current window


/* ============================================================
 *  SECTION 4 — SETUP
 * ============================================================ */

void setup() {

  // Motor pin modes
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensor pin modes
  pinMode(SENSOR_PIN,       INPUT);
  pinMode(BLINK_SENSOR_PIN, INPUT);

  // Output pin modes
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize serial communication
  Serial.begin(9600);   // Debug serial monitor
  Serial1.begin(9600);  // Voice recognition module UART

  Serial.println("============================================");
  Serial.println("  STM32 Lift System Initialized");
  Serial.println("  Eye Blink + Voice Control Ready");
  Serial.println("  Blink 3 = Ground | 4 = First | 5 = Second");
  Serial.println("============================================");

  // Auto-calibrate: move to ground floor on power ON
  moveToGroundFloor();
}


/* ============================================================
 *  SECTION 5 — MAIN LOOP
 * ============================================================ */

void loop() {

  // Check for incoming UART hex command (2 bytes: header + command)
  if (Serial1.available() >= 2) {
    byte header  = Serial1.read();
    byte command = Serial1.read();
    processCommand(header, command);
  }

  // Continuously check for eye blink input
  processBlinkInput();
}


/* ============================================================
 *  SECTION 6 — UART COMMAND PROCESSING
 * ============================================================
 * Protocol : Header (0x20) + Command byte
 * Commands :
 *   0x20 0xA1 → Ground Floor (0)
 *   0x20 0xA2 → First Floor  (1)
 *   0x20 0xA3 → Second Floor (2)
 * ============================================================ */

void processCommand(byte header, byte command) {

  // Validate packet header
  if (header != 0x20) {
    Serial.println("[ERROR] Invalid Header! Expected 0x20.");
    return;
  }

  // Process floor command
  switch (command) {
    case 0xA1:
      Serial.println("[CMD] Command: Ground Floor");
      moveToFloor(0);
      break;

    case 0xA2:
      Serial.println("[CMD] Command: First Floor");
      moveToFloor(1);
      break;

    case 0xA3:
      Serial.println("[CMD] Command: Second Floor");
      moveToFloor(2);
      break;

    default:
      Serial.println("[ERROR] Invalid Command! Use 0xA1, 0xA2, or 0xA3.");
      break;
  }
}


/* ============================================================
 *  SECTION 7 — EYE BLINK INPUT PROCESSING
 * ============================================================
 * Logic :
 *   - Count blinks within a 5-second window
 *   - After 5 seconds, evaluate blink count
 *   - 3 blinks = Ground Floor
 *   - 4 blinks = First Floor
 *   - 5 blinks = Second Floor
 *   - Others   = Buzzer error alert
 * ============================================================ */

void processBlinkInput() {

  int blinkSensorValue = digitalRead(BLINK_SENSOR_PIN);

  // Blink detected (sensor goes LOW)
  if (blinkSensorValue == LOW) {
    if (!blinkActive) {
      blinkActive    = true;
      blinkStartTime = millis();  // Record time of first blink
      blinkCount++;
      Serial.print("[BLINK] Blink detected. Count: ");
      Serial.println(blinkCount);
    }
  } else {
    // Blink released
    blinkActive = false;
  }

  // Evaluate after 5-second window
  if (blinkCount > 0 && millis() - blinkStartTime > 5000) {

    Serial.print("[BLINK] Final blink count: ");
    Serial.println(blinkCount);

    if (blinkCount == 3) {
      Serial.println("[BLINK] Selecting: Ground Floor");
      moveToFloor(0);

    } else if (blinkCount == 4) {
      Serial.println("[BLINK] Selecting: First Floor");
      moveToFloor(1);

    } else if (blinkCount == 5) {
      Serial.println("[BLINK] Selecting: Second Floor");
      moveToFloor(2);

    } else {
      // Invalid blink count — alert user with long buzzer
      Serial.println("[ERROR] Invalid blink count. Blink 3, 4, or 5 times.");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
      digitalWrite(BUZZER_PIN, LOW);
    }

    // Reset blink counter for next input
    blinkCount = 0;
  }
}


/* ============================================================
 *  SECTION 8 — FLOOR MOVEMENT CONTROL
 * ============================================================ */

void moveToFloor(int targetFloor) {

  // Already on target floor
  if (targetFloor == currentFloor) {
    Serial.println("[LIFT] Already on the desired floor.");
    playFloorSound(targetFloor);
    return;
  }

  // Calculate steps needed (positive = up, negative = down)
  int steps = (targetFloor - currentFloor) * stepsPerFloor;

  Serial.print("[LIFT] Moving to floor: ");
  Serial.println(targetFloor);

  playFloorSound(targetFloor);  // Play destination floor sound
  stepMotor(steps);             // Drive the motor
  currentFloor = targetFloor;   // Update current floor

  Serial.print("[LIFT] Reached floor: ");
  Serial.println(currentFloor);

  playFloorSound(currentFloor); // Confirm arrival sound
}


/* ============================================================
 *  SECTION 9 — STEPPER MOTOR DRIVER
 * ============================================================ */

// Drive stepper motor for given number of steps
void stepMotor(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    singleStep(direction);
    delay(2);  // Step delay — controls motor speed
  }
}

// Perform a single step in the given direction
void singleStep(int direction) {

  static int stepIndex = 0;

  // Wave drive step sequence (1-phase excitation)
  const int stepSequence[4][4] = {
    {1, 0, 0, 0},  // Step 0
    {0, 1, 0, 0},  // Step 1
    {0, 0, 1, 0},  // Step 2
    {0, 0, 0, 1}   // Step 3
  };

  // Advance step index with wraparound
  stepIndex += direction;
  if (stepIndex < 0) stepIndex = 3;
  if (stepIndex > 3) stepIndex = 0;

  // Apply step to motor pins
  digitalWrite(IN1, stepSequence[stepIndex][0]);
  digitalWrite(IN2, stepSequence[stepIndex][1]);
  digitalWrite(IN3, stepSequence[stepIndex][2]);
  digitalWrite(IN4, stepSequence[stepIndex][3]);
}


/* ============================================================
 *  SECTION 10 — GROUND FLOOR INITIALIZATION
 * ============================================================
 * On power ON, lift auto-moves to ground floor
 * using analog sensor feedback for calibration
 * ============================================================ */

void moveToGroundFloor() {
  Serial.println("[INIT] Initializing: Moving to ground floor...");

  // Keep stepping down until sensor detects ground floor
  while (!isAtGroundFloor()) {
    singleStep(-1);
    delay(2);
  }

  currentFloor = 0;
  Serial.println("[INIT] Ground floor reached. System Ready.");
}

// Returns TRUE when lift is at ground floor
bool isAtGroundFloor() {
  int sensorValue = analogRead(SENSOR_PIN);
  return sensorValue < 500;  // Threshold for ground floor detection
}


/* ============================================================
 *  SECTION 11 — BUZZER AUDIO FEEDBACK
 * ============================================================
 * Floor audio announcement via buzzer beeps:
 *   Ground Floor → 1 beep
 *   First Floor  → 2 beeps
 *   Second Floor → 3 beeps
 * ============================================================ */

void playFloorSound(int floor) {

  switch (floor) {

    case 0:  // Ground Floor — 1 beep
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      break;

    case 1:  // First Floor — 2 beeps
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      break;

    case 2:  // Second Floor — 3 beeps
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      break;

    default:
      Serial.println("[ERROR] Invalid floor for sound.");
      break;
  }
}

/* ============================================================
 *  END OF FIRMWARE
 * ============================================================ */
