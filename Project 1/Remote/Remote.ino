
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

// ------------------------------ LIST ELEMENT ------------------------------ //
typedef struct ListElement{
  int data;
  struct ListElement *next;
}ListElem;

// GLOBAL LIST HEADS //
ListElem *laserHead = NULL;
ListElem *servoHead = NULL;

// ------------------------------ INSERT END ------------------------------ //
void insertEnd(int val, ListElem **head){
  ListElem *newElem = (ListElem*)malloc(sizeof(ListElem));
  if (newElem == NULL) {
    exit(1);
  }
  newElem->data = val;
  newElem->next = NULL;
  
  ListElem *p;
  p = *head;
  
  if (*head == NULL) {
    *head = newElem;
  }
  else {
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = newElem;
  }
}

// ------------------------------ DELETE ELEM ------------------------------ //
void deleteElem(int val, ListElem **head){
  ListElem *curr = *head;
  ListElem *prev;
  while(curr != NULL){
    if(( (curr->data) == val)) {
      if(curr == *head) {
        *head = curr->next;
        free(curr);
        return;
      }
      else {
        prev->next = curr->next;
        free(curr);
        return;
      }
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }
}

// ------------------------------ PRINT LIST ------------------------------ //
// For debugging purposes
void printList(ListElem *head) {
  if (head == NULL){
    return;
  }
  ListElem *curr = head;
  for(curr=head; curr!=NULL; curr=curr->next) {
    Serial.print(curr->data);
    Serial.print(", ");
  }
  Serial.println();
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
  if (laserHead != NULL) {
    laserState = laserHead->data;
    
    if(laserState == 0){
      // Fire the laser
      digitalWrite(laserPin, HIGH);
    } 
    else {
      // Turn off laser
      digitalWrite(laserPin, LOW);
    }
    deleteElem(laserState, &laserHead);
  }
}

// ------------------------------ SERVO TASK ------------------------------ //
void servoTask() {
  if (servoHead != NULL) {
    servoState = servoHead->data;
    
    switch (servoState) {
      case 0:
        servo.writeMicroseconds(600);
        break;
      case 1:
        servo.writeMicroseconds(1050);
        break;
      case 2:
        servo.writeMicroseconds(1500);
        break;
      case 3:
        servo.writeMicroseconds(1950);
        break;
      case 4:
        servo.writeMicroseconds(2400);
        break;
    }
    deleteElem(servoState, &servoHead);
  }
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
  if(Serial1.available() > 0) {
     int flag;
     int data;

     readByte(&flag);
     readByte(&data);
  
    if (flag == LASER) {
      insertEnd(data, &laserHead); 
    }
    else if (flag == SERVO) {
       insertEnd(data, &servoHead);
    }
  }
}

// ------------------------------ BLUETOOTH SEND ------------------------------ //
void bluetoothSend() {
  Serial1.print(SCREEN);
  Serial1.print((int)laserState);
  Serial1.print((int)servoState);
}

// ------------------------------ SETUP ------------------------------ //
void setup() {
  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(0, 50, bluetoothReceive);
  Scheduler_StartTask(5, 100, laserTask);
  Scheduler_StartTask(10, 50, servoTask);
  Scheduler_StartTask(15, 150, bluetoothSend);
  
  // Serial
  Serial1.begin(9600);
  Serial.begin(9600);
  
  // Servo
  servo.attach(servoPin);
  servo.writeMicroseconds(1500);
  servoState = 2;
  
  // Laser
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
  laserState = 1;
}

// ------------------------------ MAIN ------------------------------ //
int main() {
  init();
  setup();

  for(;;) {
    Scheduler_Dispatch();
  }
}
