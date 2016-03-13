#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
//
// EXPECTED RUNNING ORDER: 
//

MUTEX mut1;

void Idle() {
    for(;;) {
    }
}

void Task_P1()
{
    Mutex_Lock(mut1);
    
    Task_Sleep(2);

    for(;;){
    }
}

void Task_P2()
{    
    Mutex_Unlock(mut1);
    Mutex_Lock(mut1);
    for(;;){
    }
}

void a_main()
{
    mut1 = Mutex_Init();

    Task_Create(Task_P1, 1, 0);
    Task_Create(Task_P2, 1, 0);
    Task_Create(Idle, MINPRIORITY, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Task_Terminate();

}
