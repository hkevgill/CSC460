#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
//
// EXPECTED RUNNING ORDER: P1, ..., P15, P16
//
void Task_P16();

void Idle() {
    for(;;) {
    }
}

void Task_P1()
{
    Task_Terminate();
}

void Task_P2()
{
    Task_Terminate();
}

void Task_P3()
{
    Task_Terminate();
}

void Task_P4()
{
    Task_Terminate();
}

void Task_P5()
{
    Task_Terminate();
}

void Task_P6()
{
    Task_Terminate();
}

void Task_P7()
{
    Task_Terminate();
}

void Task_P8()
{
    Task_Terminate();
}

void Task_P9()
{
    Task_Terminate();
}

void Task_P10()
{
    Task_Terminate();
}

void Task_P11()
{
    Task_Terminate();
}

void Task_P12()
{
    Task_Terminate();
}

void Task_P13()
{
    Task_Terminate();
}

void Task_P14()
{
    Task_Terminate();
}

void Task_P15()
{
    Task_Create(Task_P16, 0, 0);
    Task_Terminate();
}

void Task_P16()
{
    for(;;);
}

void a_main()
{
    Task_Create(Task_P1, 1, 0);
    Task_Create(Task_P2, 1, 0);
    Task_Create(Task_P3, 1, 0);
    Task_Create(Task_P4, 1, 0);
    Task_Create(Task_P5, 1, 0);
    Task_Create(Task_P6, 1, 0);
    Task_Create(Task_P7, 1, 0);
    Task_Create(Task_P8, 1, 0);
    Task_Create(Task_P9, 1, 0);
    Task_Create(Task_P10, 1, 0);
    Task_Create(Task_P11, 1, 0);
    Task_Create(Task_P12, 1, 0);
    Task_Create(Task_P13, 1, 0);
    Task_Create(Task_P14, 1, 0);
    Task_Create(Task_P15, 1, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Task_Terminate();

}
