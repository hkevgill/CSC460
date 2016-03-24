#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../roomba/roomba_sci.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int ReceivePID;
unsigned int IdlePID;

unsigned char rbyte;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void toggleLED() {
    PORTL ^= _BV(PORTL6);
}

void Idle() {
    for(;;) {
    }
}

void Receive() {
    for(;;) {
        rbyte = Bluetooth_Receive_Byte();
        if(rbyte) {
            toggleLED();
        }
        _delay_ms(50);
        rbyte = 0;
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {

    // TEST
    rbyte = 0;
    DDRL |= _BV(DDL6);
    //

    Bluetooth_UART_Init();

    IdlePID = Task_Create(Idle, MINPRIORITY, 1);
    ReceivePID = Task_Create(Receive, 5, 2);

    Task_Terminate();
}