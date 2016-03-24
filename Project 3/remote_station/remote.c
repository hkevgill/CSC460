#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

unsigned int IdlePID;
unsigned int RoombaTestPID;
unsigned int BluetoothReceivePID;

unsigned char rbyte;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state

void toggleLED() {
	PORTL ^= _BV(PORTL6);
}

// ------------------------------ IDLE ------------------------------ //
void Idle() {
	for(;;) {
		continue;
	}
}

// ------------------------------ ROOMBA TEST ------------------------------ //
void Roomba_Test() {
	for(;;) {
		Roomba_Drive(75,DRIVE_STRAIGHT);
		_delay_ms(5000);
		Roomba_Drive(-75,DRIVE_STRAIGHT);
		_delay_ms(5000);
	}
}

// ------------------------------ BLUETOOTH RECIEVE ------------------------------ //
void Bluetooth_Receive() {
	for(;;){
		continue;
	}
}


// Application level main function
// Creates the required tasks and then terminates
void a_main() {

	IdlePID = Task_Create(Idle, MINPRIORITY, 1);
	RoombaTestPID = Task_Create(Roomba_Test, 9, 2);
	BluetoothReceivePID = Task_Create(Bluetooth_Receive, 1, 3);

	Bluetooth_UART_Init();
	Roomba_UART_Init();
	Roomba_Init();

	Task_Terminate();
}