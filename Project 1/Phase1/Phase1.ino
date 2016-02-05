#include "Arduino.h"
#include "Servo.h"
#include "LiquidCrystal.h"

Servo servo;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int photocellPin = A7;    // the cell and 10K pulldown are connected to A7
int laserPin = 30;        // digital pin
int servoPin = 28;         // PWM pin
int buzzerPin = 8;        // Digital pin

int photocellReading;     // the analog reading from the analog resistor divider
int joyX = A8;            // x
int joyY = A9;            // y
int joyZ = 52;            // Push-Down button

void setup() {

 // LCD
 lcd.begin(16, 2);

 // Joystick
 pinMode(joyX, INPUT);
 pinMode(joyY, INPUT);
 pinMode(joyZ, INPUT_PULLUP);

 // Servo
 servo.attach(servoPin);

 // Laser
 pinMode(laserPin, OUTPUT);
 digitalWrite(laserPin, LOW);

 // Buzzer
 pinMode(buzzerPin, OUTPUT);
 digitalWrite(buzzerPin, HIGH);
}

void laserTask() {
  int z;
  z = digitalRead (joyZ);

   // FIRE THE LASER
   if(z == 0){
     digitalWrite(laserPin, HIGH);
   } else digitalWrite(laserPin, LOW);

   lcd.print (z, DEC);
}

void lightSensorTask() {
  photocellReading = analogRead(photocellPin);

  lcd.setCursor(0, 1);

  if (photocellReading > 350) {
    lcd.print("SHOT");
   }
   else {
    lcd.print("NOT SHOT");
   }
}

void movementTask(int *value1, int *value2, int *value3) {
  lcd.clear();
  int x, y;
  float xAvg;

  x = map(analogRead(joyX), 0, 1023, 800, 2200); // scale x and y to use with servo
  y = map(analogRead(joyY), 0, 1023, 800, 2200);

  *value3 = *value2;
  *value2 = *value1;
  *value1 = x;

  xAvg = ((*value1 * 0.6) + (*value2 * 0.3) + (*value3 * 0.1));

  if(x < 1550 && x > 1450) {
    // Dead zone
    servo.writeMicroseconds(1500);
   }
   else {
    servo.writeMicroseconds(xAvg);
   }

   lcd.print (x, DEC);
   lcd.print (",");
   lcd.print (y, DEC);
   lcd.print (",");
}

int main() {
 init();

 setup();

 int value1 = 1500;
 int value2 = 1500;
 int value3 = 1500;
 
 for(;;) {
   movementTask(&value1, &value2, &value3);
   laserTask();
   lightSensorTask();

   delay(100);
 }
}
