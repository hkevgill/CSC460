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
unsigned int PungPID;

unsigned int i = 0;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void idle() {
    for(;;) {
    }
}

// Ping task for testing
void Pung() {

    for(;;){
        disable_LED(PORTL2);
        disable_LED(PORTL6);

        _delay_ms(200);

        Task_Resume(PongPID);

        Task_Next();
    }
}

// Ping task for testing
void Ping() {
    int  x;

    for(;;){
        toggle_LED(PORTL6);
        _delay_ms(200);
        Task_Suspend(PingPID);

        Task_Sleep(100);
    }
}

// Pong task for testing
void Pong() {
    int  x;
    for(;;) {
        toggle_LED(PORTL2);
        _delay_ms(200);
        Task_Suspend(PongPID);
        Task_Resume(PingPID);

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
    PungPID = Task_Create(Pung, 9, 1);
    unsigned int i = Task_Create(idle, MINPRIORITY, 1);

    Task_Terminate();
}