#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "LED_Test.h"
#include "os.h"

unsigned int portL2_Mutex;
unsigned int portL6_Mutex;

unsigned int PingPID;
unsigned int PongPID;
unsigned int IdlePID;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void Idle() {
    for(;;) {
    }
}

// Ping task for testing
void Ping() {
    int  x;

    for(;;){
        toggle_LED(PORTL6);

        Task_Sleep(100);
    }
}

// Pong task for testing
void Pong() {
    int  x;
    for(;;) {
        toggle_LED(PORTL2);

        Task_Sleep(100);
    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {

    portL2_Mutex = Mutex_Init();
    portL6_Mutex = Mutex_Init();

    PongPID = Task_Create(Pong, 8, 1);
    PingPID = Task_Create(Ping, 8, 1);
    IdlePID = Task_Create(Idle, MINPRIORITY, 1);

    Task_Terminate();
}
