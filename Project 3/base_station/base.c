#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../roomba/roomba_sci.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int IdlePID;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void Idle() {
    for(;;) {
    }
}

void Send() {
    char *b = '1529876';
    for(;;) {
        PORTL ^= _BV(PORTL6);
        Bluetooth_Send_String(&b);

        _delay_ms(1000);
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {
    DDRL |= _BV(DDL6);

    Bluetooth_UART_Init();

    Task_Create(Send, 1, 1);
    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    Task_Terminate();
}
