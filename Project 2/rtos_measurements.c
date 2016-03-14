#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"

// Measure aspects of the RTOS like cswitch time, dispatch time, lock mutex, etc.

MUTEX mut;
EVENT evt;

void Idle() {
    for(;;) {
    }
}

void Task_P1(int parameter)
{
    for(;;) {
    }
}

void Task_P2(int parameter)
{
    for(;;) {
    }
}

void a_main()
{
    mut = Mutex_Init();
    evt = Event_Init();

    disable_LED(PORTL0);
    disable_LED(PORTL1);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);
    disable_LED(PORTL7);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    enable_LED(PORTL1);
    Task_Create(Task_P1, 2, 0);
    disable_LED(PORTL1);
    Task_Create(Task_P2, 2, 0);
    Task_Create(Idle, 10, 0);

    Task_Terminate();

}
