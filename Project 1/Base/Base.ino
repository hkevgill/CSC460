#include "Arduino.h"
#include "LiquidCrystal.h"
#include "scheduler.h"

int LASER = 0;
int SERVO = 1;
int SCREEN = 2;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int servoState = 2;

int photocellPin = A7;    // the cell and 10K pulldown are connected to A7
int buzzerPin = 8;        // Digital pin

int photocellReading;     // the analog reading from the analog resistor divider
int joyX = A8;            // x
int joyZ = 52;            // Push-Down button

int laser = 1;
int previousLaser = 1;


// ------------------------------ LIST ELEMENT ------------------------------ //
typedef struct ListElement{
 int data;
 struct ListElement *next;
}ListElem;

// GLOBAL LIST HEADS //
ListElem *screenHead = NULL;

// Function prototypes
void insertEnd(int val, ListElem **head);
void deleteElem(int val, ListElem **head);
void setup();
void bluetoothReceive();
void screenTask();
void laserTask();
void lightSensorTask();
void movementTask();
void readByte(int *inByte);


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

void setup() {

  // TTA
  Scheduler_Init();

  // Start tasks arguments
  // Offset in ms, period in ms, function callback
  Scheduler_StartTask(0, 150, bluetoothReceive);
  Scheduler_StartTask(5, 100, laserTask);
  Scheduler_StartTask(15, 10, movementTask);
  Scheduler_StartTask(10, 150, screenTask);
  Scheduler_StartTask(20, 100, lightSensorTask);
  
  // Serial
  Serial.begin(9600);
  Serial1.begin(9600);
  
  // LCD
  lcd.begin(16, 2);
  
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
      insertEnd(laserData, &screenHead);
      insertEnd(servoData, &screenHead);
    }
  }
}

void screenTask() {
  if(screenHead != NULL) {
    lcd.clear();

    lcd.setCursor(0,1);
  
    int laserState = screenHead->data;
  
    if (laserState == 0) {
      lcd.print("ON");
    }
    else {
      lcd.print("OFF");
    }
  
    deleteElem(laserState, &screenHead);
  
    lcd.setCursor(0,0);
  
    int servoState = screenHead->data;
  
    lcd.print("Servo:");
    lcd.print(servoState);
  
    deleteElem(servoState, &screenHead);
  
    lcd.setCursor(4, 1);

    if (photocellReading > 650) {
      lcd.print("SHOT");
    }
    else {
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

  if((newState - servoState > 0 && newState > 92) || (newState < 88 && newState - servoState < 0)) {
    Serial1.print(SERVO);
    Serial1.print(digits);
    Serial1.print(newState);

    servoState = newState;
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
