#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
//
// EXPECTED RUNNING ORDER: P1, P2
//

EVENT evt = 0;

void Idle() {
    for(;;) {
    }
}

void Task_P1()
{
    Event_Signal(evt);

    Task_Sleep(100);
    for(;;){
    }
}

void Task_P2()
{   
    Event_Wait(evt);

    for(;;){
    }
}

void a_main()
{
    Task_Create(Task_P1, 1, 0);
    Task_Create(Task_P2, 2, 0);
    Task_Create(Idle, MINPRIORITY, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Task_Terminate();

}
