#include "Arduino.h"
#include "LiquidCrystal.h"

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
    int servoData;
  
    readByte(&flag);
    readByte(&laserData);
    readByte(&servoData);

    Serial.print(flag);
    Serial.print(laserData);
    Serial.println(servoData);
  
    if (flag == SCREEN) {
      insertEnd(laserData, &screenHead);
      insertEnd(servoData, &screenHead);
    }
  }
}

void screenTask() {
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

  if (photocellReading > 350) {
    lcd.print("SHOT");
  }
  else {
    lcd.print("NOT SHOT");
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
    movementTask();
    delay(5);
    laserTask();
    delay(5);
    lightSensorTask();
    delay(5);
    bluetoothReceive();
    delay(5);
    screenTask();
    delay(5);
 }
}
