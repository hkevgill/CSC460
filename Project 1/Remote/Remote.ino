
#include "Arduino.h"
#include "Servo.h"
#include "scheduler.h"
//#include <stdlib.h>
//#include <stdio.h>

// ------------------------------ GLOBALS ------------------------------ //
int LASER = 0;      
int SERVO = 1;
int SCREEN = 2;

Servo servo;        //  Servo motor

int laserPin = 30;  //  (digital pin)
int servoPin = 28;  //  (pwm pin)

int laserState;     //  0 = On, 1 = Off
int servoState;     //  0 = Full Back
                    //  1 = Half Back
                    //  2 = Mid
                    //  3 = Half Forward
                    //  4 = Full Forward
int lastServoState;

// Queue globals
const int QSize = 10;

int servoQueue[QSize];
int servoFront;
int servoRear;

int laserQueue[QSize];
int laserFront;
int laserRear;

// ------------------------------ IS FULL ------------------------------ //
int isFull(int *front, int *rear) {
  return (*rear == (*front - 1) % QSize);
}

// ------------------------------ IS EMPTY ------------------------------ //
int isEmpty(int *front, int *rear) {
  return (*rear == *front);
}

// ------------------------------ ENQUEUE ------------------------------ //
void enqueue(int val, int *queue, int *front, int *rear) {
  if (isFull(front, rear)) {
    return;
  }
  queue[*rear] = val;
  *rear = (*rear + 1) % QSize;
}

// ------------------------------ DEQUEUE ------------------------------ //
int dequeue(int *queue, int *front, int *rear){
  if (isEmpty(front, rear)) {
    return -1;
  }
  int result = queue[*front];
  *front = (*front + 1) % QSize;
  return result;
}

// ------------------------------ PRINT QUEUE ------------------------------ //
// For debugging purposes
void printQueue(int *queue, int *front, int *rear) {
  if (isEmpty(front, rear)){
    return;
  }
  int curr = *front;
  while (curr != *rear) {
    Serial.print(queue[curr % QSize]);
    curr = (curr + 1) % QSize;
  }
}

// ------------------------------ FUNCTION PROTOTYPES ------------------------------ //
void laserTask();
void servoTask();
void readByte(int *inByte);
void bluetoothReceive();
void bluetoothSend();
void setup();

// ------------------------------ LASER TASK ------------------------------ //
void laserTask() {
  digitalWrite(41, HIGH);

  if (!isEmpty(&laserFront, &laserRear)) {
//    printQueue(laserQueue, &laserFront, &laserRear);
    laserState = dequeue(laserQueue, &laserFront, &laserRear);
    
    if(laserState == 0){
      // Fire the laser
      digitalWrite(laserPin, HIGH);
    } 
    else if(laserState == 1){
      // Turn off laser
      digitalWrite(laserPin, LOW);
    }
  }

  digitalWrite(41, LOW);
}

// ------------------------------ SERVO TASK ------------------------------ //
void servoTask() {
  digitalWrite(43, HIGH);

  if (!isEmpty(&servoFront, &servoRear)) {
//    printQueue(servoQueue, &servoFront, &servoRear);
    servoState = dequeue(servoQueue, &servoFront, &servoRear);
  }

  if (servoState > 92 && lastServoState <= 175) {
    if (servoState > 150) {
      lastServoState = lastServoState + 2;
    }
    lastServoState = lastServoState + 1;
    servo.write(lastServoState);
  }
  else if (servoState < 88 && lastServoState >= 5) {
    if (servoState < 30) {
      lastServoState = lastServoState - 2;
    }
    lastServoState = lastServoState - 1;
    servo.write(lastServoState);
  }

  digitalWrite(43, LOW);
}

// ------------------------------ READ BYTE ------------------------------ //
void readByte(int *inByte) {
 while(1) {
   if(Serial1.available() > 0) {
     *inByte = Serial1.read() - '0';
     break;
   }
 }
}

// ------------------------------ BLUETOOTH RECIEVE ------------------------------ //
void bluetoothReceive() {
  digitalWrite(45, HIGH);
//  digitalWrite(45, LOW);

  if(Serial1.available() > 0) {
     int flag;
     int digits;
     int data;
     int data1;
     int data2;
     int data3;

     readByte(&flag);
  
    if (flag == LASER) {
      readByte(&data);
      enqueue(data, laserQueue, &laserFront, &laserRear); 
    }
    else if (flag == SERVO) {
      readByte(&digits);

      if(digits == 1) {
        readByte(&data);
      }
      else if(digits == 2) {
        readByte(&data1);
        readByte(&data2);

        data = data1*10 + data2;
      }
      else if(digits == 3) {
        readByte(&data1);
        readByte(&data2);
        readByte(&data3);

        data = data1*100 + data2*10 + data3;
      }
      else if(digits == 0) {
        servoState = 90;
        digitalWrite(45, LOW);
        return;
      }
      enqueue(data, servoQueue, &servoFront, &servoRear);
    }
  }

//  digitalWrite(45, HIGH);
  digitalWrite(45, LOW);
}

// ------------------------------ BLUETOOTH SEND ------------------------------ //
void bluetoothSend() {
  digitalWrite(47, HIGH);
  
  Serial1.print(SCREEN);
  Serial1.print((int)laserState);

  if(lastServoState < 10) {
    Serial1.print(1);
  }
  else if(lastServoState < 100) {
    Serial1.print(2);
  }
  else {
    Serial1.print(3);
  }
  Serial1.print((int)lastServoState);

  digitalWrite(47, LOW);
}

// ------------------------------ SETUP ------------------------------ //
void setup() {
  // Queue
  servoFront = 0;
  servoRear = 0;
  laserFront = 0;
  laserRear = 0;
  
  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(0, 25, servoTask);
  Scheduler_StartTask(5, 50, bluetoothReceive);
  Scheduler_StartTask(20, 100, laserTask);
  Scheduler_StartTask(35, 100, bluetoothSend);
  
  // Serial
  Serial1.begin(9600);
  Serial.begin(9600);
  
  // Servo
  servo.attach(servoPin);
  servo.writeMicroseconds(1500);
  servoState = 90;
  lastServoState = 90;
  
  // Laser
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
  laserState = 1;

  // LOGIC ANALYSER
  pinMode(41, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(49, OUTPUT);
  digitalWrite(41, LOW);  // Laser
  digitalWrite(43, LOW);  // Servo
  digitalWrite(45, LOW);  // Bluetooth Receive
  digitalWrite(47, LOW);  // Bluetooth Send
  digitalWrite(49, LOW);  // Idle
}

// ------------------------------ MAIN ------------------------------ //
void idle(uint32_t idlePeriod){
  digitalWrite(49, HIGH);
  delay(idlePeriod);
  digitalWrite(49, LOW);
}

// ------------------------------ MAIN ------------------------------ //
int main() {
  init();
  setup();

  for(;;) {
    uint32_t idlePeriod = Scheduler_Dispatch();
    idle(idlePeriod);
  }
}
