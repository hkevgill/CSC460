#include <avr/io.h>
#include <avr/interrupt.h>
#include "LED_Test.h"
#include "os.h"

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state
void idle() {
    for(;;) {
    }
}

// Ping task for testing
void Ping() {
    int  x ;
    for(;;){
        // enable_LED(PORTL6);
        // disable_LED(PORTL2);

        toggle_LED(PORTL6);

        for( x=0; x < 32000; ++x );   /* do nothing */
        for( x=0; x < 32000; ++x );   /* do nothing */
        for( x=0; x < 32000; ++x );   /* do nothing */

        /* printf( "*" );  */
        Task_Sleep(100);
    }
}

// Pong task for testing
void Pong() {
    int  x;
    for(;;) {
        // enable_LED(PORTL2);
        // disable_LED(PORTL6);

        toggle_LED(PORTL2);

        for( x=0; x < 32000; ++x );   /* do nothing */
        for( x=0; x < 32000; ++x );   /* do nothing */
        for( x=0; x < 32000; ++x );   /* do nothing */

        /* printf( "." );  */
        Task_Sleep(100);

    }
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {
    Task_Create(Pong, 8, 1);
    Task_Create(Ping, 8, 1);
    Task_Create(idle, MINPRIORITY, 1);

    Task_Terminate();
}