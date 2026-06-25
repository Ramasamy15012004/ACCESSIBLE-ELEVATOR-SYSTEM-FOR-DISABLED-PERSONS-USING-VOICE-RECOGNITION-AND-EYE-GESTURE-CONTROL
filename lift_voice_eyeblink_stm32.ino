
#define IN1 PA3
#define IN2 PA2
#define IN3 PA1
#define IN4 PA0


#define SENSOR_PIN PA4        
#define BLINK_SENSOR_PIN PA5  


#define BUZZER_PIN PA6


const int stepsPerRevolution = 2048; 
const int stepsPerFloor = 5524; 


int currentFloor = -1; 
unsigned long blinkStartTime = 0;
int blinkCount = 0;               
bool blinkActive = false;        

void setup() {
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensor pins setup
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BLINK_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600); 
  Serial1.begin(9600);
  Serial.println("STM32 Lift System with Eye Blink Control Initialized");
  Serial.println("Send Hex Commands or Blink 3, 4, or 5 Times to Select a Floor");

 
  moveToGroundFloor();
}

void loop() {
  
  if (Serial1.available() >= 2) { 
    byte header = Serial1.read(); 
    byte command = Serial1.read(); 

    processCommand(header, command);
  }

  
  processBlinkInput();
}


void processCommand(byte header, byte command) {
  // Validate header
  if (header != 0x20) {
    Serial.println("Invalid Header! Expected 0x20.");
    return;
  }

 
  switch (command) {
    case 0xA1:
      moveToFloor(0); 
      break;
    case 0xA2:
      moveToFloor(1);
      break;
    case 0xA3:
      moveToFloor(2); 
      break;
    default:
      Serial.println("Invalid Command! Use 0xA1, 0xA2, or 0xA3.");
      break;
  }
}


void processBlinkInput() {
  int blinkSensorValue = digitalRead(BLINK_SENSOR_PIN);

  if (blinkSensorValue == LOW) { 
    if (!blinkActive) {
      blinkActive = true;
      blinkStartTime = millis(); 
      blinkCount++;
      Serial.print("Blink detected. Current count: ");
      Serial.println(blinkCount);
    }
  } else { 
    blinkActive = false;
  }

  
  if (blinkCount > 0 && millis() - blinkStartTime > 5000) { 
    Serial.print("Final blink count: ");
    Serial.println(blinkCount);

    
    if (blinkCount == 3) {
      moveToFloor(0); 
    } else if (blinkCount == 4) {
      moveToFloor(1); 
    } else if (blinkCount == 5) {
      moveToFloor(2);
    } else {
      digitalWrite(PA6,HIGH);
      delay(2000);
      digitalWrite(PA6,LOW);
      Serial.println("Invalid blink count. Blink 3, 4, or 5 times to select a floor.");
    }

    
    blinkCount = 0;
  }
}


void moveToFloor(int targetFloor) {
  if (targetFloor == currentFloor) {
    Serial.println("Already on the desired floor.");
    playFloorSound(targetFloor);
    return;
  }

  int steps = (targetFloor - currentFloor) * stepsPerFloor;
   playFloorSound(targetFloor);
  Serial.print("Moving to floor: ");
  Serial.println(targetFloor);
  
  stepMotor(steps);
  currentFloor = targetFloor;
  Serial.print("Reached floor: ");
  Serial.println(currentFloor);
  playFloorSound(currentFloor);
}


void stepMotor(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    singleStep(direction);
    delay(2); 
  }
}


void singleStep(int direction) {
  static int stepIndex = 0;
  const int stepSequence[4][4] = {
    {1, 0, 0, 0}, 
    {0, 1, 0, 0}, 
    {0, 0, 1, 0}, 
    {0, 0, 0, 1}  
  };

  stepIndex += direction;
  if (stepIndex < 0) stepIndex = 3;
  if (stepIndex > 3) stepIndex = 0;

 
  digitalWrite(IN1, stepSequence[stepIndex][0]);
  digitalWrite(IN2, stepSequence[stepIndex][1]);
  digitalWrite(IN3, stepSequence[stepIndex][2]);
  digitalWrite(IN4, stepSequence[stepIndex][3]);
}


void moveToGroundFloor() {
  Serial.println("Initializing: Moving to ground floor...");

  
  while (!isAtGroundFloor()) {
    singleStep(-1);
    delay(2); 
  }

  currentFloor = 0; 
  Serial.println("Reached ground floor.");
}


bool isAtGroundFloor() {
  int sensorValue = analogRead(SENSOR_PIN);
  return sensorValue < 500; 
}


void playFloorSound(int floor) {
  switch (floor) {
    case 0: // Ground Floor
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
      break;
    case 1: // First Floor
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
      delay(500);
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
      break;
    case 2: 
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
      delay(500);
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
       delay(500);
      digitalWrite(PA6,HIGH);
      delay(500);
      digitalWrite(PA6,LOW);
      break;
    default:
      Serial.println("Invalid floor for sound.");
      break;
  }
}
