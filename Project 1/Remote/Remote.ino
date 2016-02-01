
#include "Arduino.h"
#include "Servo.h"
#include <stdlib.h>
#include <stdio.h>

// ------------------------------ GLOBALS ------------------------------ //
int LASER = 0;      
int SERVO = 1;
int SCREEN = 2;

Servo servo;        //  Servo motor

int laserPin = 30;  //  (digital pin)
int servoPin = 28;  //  (pwm pin)

char laserState;    //  0 = On, 1 = Off
char servoState;    //  0 = Full Back
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
 
// ------------------------------ SETUP ------------------------------ //
void setup() {
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

// ------------------------------ LASER TASK ------------------------------ //
void laserTask() {
  if (laserHead != NULL) {
    laserState = laserHead->data;

    // DEBUG
    Serial.print("Laser command: ");
    printList(laserHead);
    
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

    // DEBUG
    Serial.print("Servo command: ");
    printList(servoHead);
    
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
void bluetoothSend(int laserState, int servoState) {
  Serial1.print(SCREEN);
  Serial1.print(laserState);
  Serial1.print(servoState);
  
  Serial.print("Flag: ");
  Serial.print(SCREEN);
  Serial.print(" laserState: ");
  Serial.print(laserState);
  Serial.print(" servoState: ");
  Serial.println(servoState);
}

// ------------------------------ MAIN ------------------------------ //
int main() {
  init();
  setup();

  for(;;) {
    bluetoothReceive();
    delay(5);
    laserTask();
    delay(5);
    servoTask();
    delay(5);
    bluetoothSend(laserState, servoState);
    delay(25);
  }
}
