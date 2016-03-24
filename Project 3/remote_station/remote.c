#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int ReceivePID;
unsigned int IdlePID;

unsigned char rbyte;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
// void toggleLED() {
//     PORTL ^= _BV(PORTL6);
// }

void Idle() {
    for(;;) {
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {

    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    // LED
    // DDRL |= _BV(DDL6);

    Bluetooth_UART_Init();
    Roomba_UART_Init();
    Roomba_Init();

    for(;;) {
        Roomba_Drive(100,DRIVE_STRAIGHT);
        _delay_ms(5000);
        Roomba_Drive(-100,DRIVE_STRAIGHT);
        _delay_ms(5000);
    }

    Task_Terminate();
}