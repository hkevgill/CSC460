#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int IdlePID;

MUTEX bluetooth_mutex;
MUTEX ls_mutex;

int x, y = 375;
uint16_t photocellReading;

uint8_t LASER = 0;
uint8_t SERVO = 1;
uint8_t LS = 2;
uint8_t SCREEN = 3;

int servoState = 375;

uint8_t laser = 0;
uint8_t previousLaser = 0;

#define MAX 10

int lSQueue[MAX];
int lSFront = 0;
int lSRear = 0;

// ------------------------------ isFull ------------------------------ //
int buffer_isFull(int *front, int *rear) {
  return *rear == (*front - 1) & MAX;
}

int buffer_isEmpty(int *front, int *rear) {
  return *rear == *front;
}

// ------------------------------ Enqueue ------------------------------ //
void buffer_enqueue(int val, int *queue, int *front, int *rear){
  if(buffer_isFull(front, rear)) {
    return;
  }

  queue[*rear] = val;
  *rear = (*rear + 1) % MAX;
}

// ------------------------------ Dequeue ------------------------------ //
int buffer_dequeue(int *queue, int *front, int *rear) {
  if(buffer_isEmpty(front, rear)) {
    return -1;
  }

  int result = queue[*front];

  *front = (*front + 1) % MAX;

  return result;
}

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void Idle() {
    for(;;) {
    }
}

void InitADC() {
    ADMUX |= (1<<REFS0);
    ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); //ENABLE ADC, PRESCALER 128
}

void JoystickTask() {
    for(;;) {

        // Read x
        ADMUX = (ADMUX & 0xE0); // Channel 8

        ADCSRB |= (1<<MUX5);

        ADCSRA |= (1<<ADSC); // Start conversion

        while((ADCSRA)&(1<<ADSC));    // wait until conversion is complete

        x = ADC;

        x = (0.458*x) + 140;

        Mutex_Lock(bluetooth_mutex);

        if ((x - servoState > 0 && x >= 380) || (x < 370 && x - servoState < 0)) {

            uint8_t x1 = x>>8;
            uint8_t x2 = x;

            Bluetooth_Send_Byte(SERVO);
            Bluetooth_Send_Byte(x1);
            Bluetooth_Send_Byte(x2);

            servoState = x;
        }
        else if ((x <= 380) && (x >= 370)) {
            x = 375;
            servoState = 375;

            uint8_t x1 = x>>8;
            uint8_t x2 = x;

            Bluetooth_Send_Byte(SERVO);
            Bluetooth_Send_Byte(x1);
            Bluetooth_Send_Byte(x2);
        }

        Mutex_Unlock(bluetooth_mutex);

        Task_Sleep(20);

        PORTL &= ~_BV(PORTL6);
    }
}

void LaserTask() {
    DDRB |= (0<<DDB1);
    PORTB |= (1<<PORTB1);
    for(;;) {
        // Read laser
        laser = (PINB & _BV(PB1)) ? 0 : 1;

        if (laser != previousLaser) {
            Mutex_Lock(bluetooth_mutex);

            Bluetooth_Send_Byte(LASER);
            Bluetooth_Send_Byte(laser);

            Mutex_Unlock(bluetooth_mutex);

            if(laser == 1) {
                PORTL |= _BV(PORTL6);
            }
            else {
                PORTL &= ~_BV(PORTL6);
            }

            previousLaser = laser;
        }

        // Sleep 100 ms
        Task_Sleep(10);
    }
}

void bluetoothReceive() {

    for(;;) {
        uint8_t flag;
        uint16_t ls_data;
        uint8_t ls_data1;
        uint8_t ls_data2;
        if(( UCSR1A & (1<<RXC1))) {
            flag = Bluetooth_Receive_Byte();

            if (flag == LS){
                ls_data1 = Bluetooth_Receive_Byte();
                ls_data2 = Bluetooth_Receive_Byte();
                ls_data = (ls_data1<<8) | (ls_data2);

                Mutex_Lock(ls_mutex);
                buffer_enqueue(ls_data, lSQueue, &lSFront, &lSRear);
                Mutex_Unlock(ls_mutex);
            }
            // else if(flag == SERVO) {

            // }
        }

        Task_Sleep(15);
    }
}

void screenTask() {
    for(;;) {
        Mutex_Lock(ls_mutex);
        uint16_t lSState = buffer_dequeue(lSQueue, &lSFront, &lSRear);
        Mutex_Unlock(ls_mutex);

        Task_Sleep(15);
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {
    DDRL |= _BV(DDL6);

    bluetooth_mutex = Mutex_Init();
    ls_mutex = Mutex_Init();

    Bluetooth_UART_Init();

    InitADC();

    Task_Create(JoystickTask, 2, 1);
    Task_Create(screenTask, 2, 1);
    Task_Create(LaserTask, 2, 1);
    Task_Create(bluetoothReceive, 2, 1);
    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    Task_Terminate();
}
