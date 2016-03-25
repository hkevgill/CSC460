#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int IdlePID;

uint16_t x, y = 0;

uint8_t LASER = 0;
uint8_t SERVO = 1;

uint8_t laser = 0;
uint8_t previousLaser = 0;

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

        while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

        x = ADC;
        x = 1.75*x + 600;

        // Read y
        ADMUX = (ADMUX & 0xE0); // Channel 8
        ADMUX |= (1<<MUX0);

        ADCSRB |= (1<<MUX5);

        ADCSRA |= (1<<ADSC); // Start conversion

        while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

        y = ADC;
        y = 1.75*y + 600;

        Bluetooth_Send_Byte(SERVO);
        Bluetooth_Send_Byte(x>>8);
        Bluetooth_Send_Byte(x);
        Bluetooth_Send_Byte(y>>8);
        Bluetooth_Send_Byte(y);

        Task_Sleep(10);
    }
}

void LaserTask() {
    DDRB |= (0<<DDB1);
    PORTB |= (1<<PORTB1);
    for(;;) {
        // Read laser
        laser = (PINB & _BV(PB1)) ? 0 : 1;

        if (laser != previousLaser) {
            Bluetooth_Send_Byte(LASER);
            Bluetooth_Send_Byte(laser);

            previousLaser = laser;
        }

        // Sleep 100 ms
        Task_Sleep(10);
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {
    DDRL |= _BV(DDL6);

    Bluetooth_UART_Init();

    InitADC();

    // Task_Create(Send, 5, 1);
    // Task_Create(JoystickTask, 1, 1);
    Task_Create(LaserTask, 1, 1);
    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    Task_Terminate();
}
