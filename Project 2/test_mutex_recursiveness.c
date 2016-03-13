#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
//
// EXPECTED RUNNING ORDER: P1, P2, P1, P2
//

MUTEX mut1;

void Idle() {
    for(;;) {
    }
}

void Task_P1()
{
    Mutex_Lock(mut1);
    Mutex_Lock(mut1);
    
    Task_Sleep(5);

    Mutex_Unlock(mut1);
    Mutex_Unlock(mut1);

    Task_Sleep(5);

    for(;;){
    }
}

void Task_P2()
{    
    Mutex_Lock(mut1);

    for(;;){
    }
}

void a_main()
{
    mut1 = Mutex_Init();

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
