#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int IdlePID;

int x, y = 0;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void Idle() {
    for(;;) {
    }
}

void Send() {
    char *b = '1529165';
    for(;;) {
        PORTL ^= _BV(PORTL6);
        Bluetooth_Send_String(&b);

        _delay_ms(1000);
    }
}

void InitADC() {
    ADMUX |= (1<<REFS0);
    ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); //ENABLE ADC, PRESCALER 128
}

void ReadJoystick() {
    for(;;) {

        // Read x
        ADMUX = (ADMUX & 0xE0); // Channel 8

        ADCSRB |= (1<<MUX5);

        ADCSRA |= (1<<ADSC); // Start conversion

        while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

        x = (ADC);

        // Read y
        ADMUX = (ADMUX & 0xE0); // Channel 8
        ADMUX |= (1<<MUX0);

        ADCSRB |= (1<<MUX5);

        ADCSRA |= (1<<ADSC); // Start conversion

        while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

        y = ADC;

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
    Task_Create(ReadJoystick, 1, 1);
    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    Task_Terminate();
}
