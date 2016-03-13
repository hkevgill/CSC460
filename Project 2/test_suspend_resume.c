#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
//
// EXPECTED RUNNING ORDER: P1, P2, P2, P1
//

PID task1;
PID task2;

void Idle() {
    for(;;) {
    }
}

void Task_P1()
{
    Task_Sleep(2);

    for(;;){
    }
}

void Task_P2()
{   
    Task_Suspend(task1);
    Task_Sleep(3);
    Task_Resume(task1);

    for(;;){
    }
}

void a_main()
{
    task1 = Task_Create(Task_P1, 1, 0);
    task2 = Task_Create(Task_P2, 2, 0);
    Task_Create(Idle, MINPRIORITY, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Task_Terminate();

}
