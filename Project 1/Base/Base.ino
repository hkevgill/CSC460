#include "Arduino.h"
#include "LiquidCrystal.h"
#include "scheduler.h"

const int MAX = 10;

int screenQueue[MAX];
int screenFront = 0;
int screenRear = 0;

int LASER = 0;
int SERVO = 1;
int SCREEN = 2;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int servoState = 90;

int buzzer = 39;

int photocellPin = A7;    // the cell and 10K pulldown are connected to A7
int buzzerPin = 8;        // Digital pin

int photocellReading;     // the analog reading from the analog resistor divider
int joyX = A8;            // x
int joyZ = 52;            // Push-Down button

int laser = 1;
int previousLaser = 1;

int stopped = 0;

// Function prototypes
int isFull(int *front, int *rear);
int isEmpty(int *front, int *rear);
void enqueue(int val, int *queue, int *front, int *rear);
int dequeue(int *queue, int *front, int *rear);
void setup();
void bluetoothReceive();
void screenTask();
void laserTask();
void lightSensorTask();
void movementTask();
void readByte(int *inByte);


// ------------------------------ isFull ------------------------------ //
int isFull(int *front, int *rear) {
  return *rear == (*front - 1) & MAX;
}

int isEmpty(int *front, int *rear) {
  return *rear == *front;
}

// ------------------------------ Enqueue ------------------------------ //
void enqueue(int val, int *queue, int *front, int *rear){
  if(isFull(front, rear)) {
    return;
  }

  queue[*rear] = val;
  *rear = (*rear + 1) % MAX;
}

// ------------------------------ Dequeue ------------------------------ //
int dequeue(int *queue, int *front, int *rear) {
  if(isEmpty(front, rear)) {
    return -1;
  }

  int result = queue[*front];

  *front = (*front + 1) % MAX;

  return result;
}


// ------------------------------ PRINT QUEUE ------------------------------ //
// For debugging purposes
//void printQueue(int *queue, int *front, int *rear) {
// if (isEmpty(front, rear)){
//   return;
// }
// int curr = *front;
// while (curr != *rear) {
//   Serial.print(queue[curr % QSize]);
//   curr = (curr + 1) % QSize;
// }
//}

void setup() {

  // TTA
  Scheduler_Init();

  // Start tasks arguments
  // Offset in ms, period in ms, function callback
  Scheduler_StartTask(0, 150, bluetoothReceive);
  Scheduler_StartTask(5, 100, laserTask);
  Scheduler_StartTask(15, 100, movementTask);
  Scheduler_StartTask(10, 150, screenTask);
  Scheduler_StartTask(20, 100, lightSensorTask);
  
  // Serial
  Serial.begin(9600);
  Serial1.begin(9600);
  
  // LCD
  lcd.begin(16, 2);

  // Buzzer
  pinMode(buzzer, OUTPUT);
  
  // Joystick
  pinMode(joyX, INPUT);
  pinMode(joyZ, INPUT_PULLUP);
  
  // Buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
}

void readByte(int *inByte) {
  while(1) {
    if(Serial1.available() > 0) {
      *inByte = Serial1.read() - '0';
      break;
    }
  }
}

void bluetoothReceive() {
  if(Serial1.available() > 0) {
    int flag;
    int laserData;
    int digits;
    int servoData;
    int servoData1;
    int servoData2;
    int servoData3;
  
    readByte(&flag);
    readByte(&laserData);
    readByte(&digits);

    if (digits == 1) {
      readByte(&servoData);
    }
    else if(digits == 2) {
      readByte(&servoData1);
      readByte(&servoData2);

      servoData = servoData1*10 + servoData2;
    }
    else {
      readByte(&servoData1);
      readByte(&servoData2);
      readByte(&servoData3);

      servoData = servoData1*100 + servoData2*10 + servoData3;
    }
  
    if (flag == SCREEN) {
//      Serial.println(servoData);
      enqueue(laserData, screenQueue, &screenFront, &screenRear);
      enqueue(servoData, screenQueue, &screenFront, &screenRear);
    }
  }
}

void screenTask() {
  if(!isEmpty(&screenFront, &screenRear)) {
    lcd.clear();

    lcd.setCursor(0,1);

    int laserState = dequeue(screenQueue, &screenFront, &screenRear);
  
    if (laserState == 0) {
      lcd.print("ON");
    }
    else if (laserState == 1){
      lcd.print("OFF");
    }
  
    lcd.setCursor(0,0);
  
    int servoState = dequeue(screenQueue, &screenFront, &screenRear);
  
    lcd.print("Servo:");
    lcd.print(servoState);
  
    lcd.setCursor(4, 1);

    if (photocellReading > 650) {
      tone(buzzer, 50000, 2000);
      lcd.print("SHOT");
    }
    else {
      noTone(buzzer);
      lcd.print("NOT SHOT");
    }
  }
}

void laserTask() {
  laser = digitalRead(joyZ);

  if(laser != previousLaser) {
    // Send laser packet
    Serial1.print(LASER);
    Serial1.print(laser);

    previousLaser = laser;
  }

}

void lightSensorTask() {
  photocellReading = analogRead(photocellPin);
}

void movementTask() {
  int x;
  int digits;
  int newState;
  
  newState = map(analogRead(joyX), 0, 1023, 5, 175);

  if(newState < 10) {
    digits = 1;
  }
  else if (newState < 100) {
    digits = 2;
  }
  else {
    digits = 3;
  }

  if ((newState - servoState > 0 && newState > 92) || (newState < 88 && newState - servoState < 0)) {
    Serial1.print(SERVO);
    Serial1.print(digits);
    Serial1.print(newState);

    servoState = newState;
  }
  else if (newState <= 92 && newState >= 88) {

    Serial1.print(SERVO);
    Serial1.print(0);
    newState = 90;
    servoState = 90;
  }
}

void speedTask() {
  int x;
  int newState;
  
  x = map(analogRead(joyX), 0, 1023, 600, 2400);

  if(x < 800) {
    newState = 0;
  }
  else if(x < 1400) {
    newState = 1;
  }
  else if(x < 1600) {
    newState = 2;
  }
  else if(x < 2200) {
    newState = 3;
  }
  else if(x < 2430) {
    newState = 4;
  }

  if(newState != servoState) {
    Serial1.print(SERVO);
    Serial1.print(newState);
  }

  servoState = newState;
}

int main() {
 init();

 setup();
 
 for(;;) {
  Scheduler_Dispatch();
  
 }
}
