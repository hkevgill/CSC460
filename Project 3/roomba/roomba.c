#define F_CPU 16000000

#include "roomba.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "../uart/uart.h" 

void Roomba_Init(){
    // Wake Roomba from sleep
    DDRA |= (1<<PA3);
    PORTA |= (1<<PA3);
    
    PORTA &= ~(1<<PA3);
    
    PORTA |= (1<<PA3);
    
    // set Roomba to passive mode
    Roomba_Send_Byte(START);

    // set Roomba to safe mode
    Roomba_Send_Byte(SAFE_MODE);  
}

void Roomba_Drive(int16_t velocity, int16_t radius){    
    Roomba_Send_Byte(DRIVE);
    Roomba_Send_Byte(velocity>>8);
    Roomba_Send_Byte(velocity);
    Roomba_Send_Byte(radius>>8);
    Roomba_Send_Byte(radius);   
}