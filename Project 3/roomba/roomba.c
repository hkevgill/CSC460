#include "Roomba.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "Circular_Buffer.h"

static volatile uint8_t overflow_counter = 0;
static volatile Circular_Buffer tx_buff;

void Roomba_Init(){
    // Wake Roomba from sleep
    DDRA |= (1<<PA3);
    PORTA |= (1<<PA3);
    //_delay_ms(2);
    
    PORTA &= ~(1<<PA3);
    //_delay_ms(100);
    
    PORTA |= (1<<PA3);
    
    // set Roomba to passive mode
    Roomba_Send_Byte(START);
    Roomba_UART_Transmit();     // transmit immediately
    //wait 22 ms
    //_delay_ms(22);
    
    // set Roomba to safe mode
    Roomba_Send_Byte(CONTROL_MODE);
    Roomba_UART_Transmit();     // transmit immediately
    //_delay_ms(22);

        
}

void Roomba_Send_Byte(uint8_t data_out){        
    Cir_Buf_Add(&tx_buff, data_out);
}

void Roomba_UART_Transmit(){
    while(tx_buff.size > 0){
        while(! (UCSR1A & (1<<UDRE1)));
        UDR1 = (Cir_Buf_Read(&tx_buff));    
    }
}

void Roomba_Drive(int16_t velocity, int16_t radius){    
    Roomba_Send_Byte(DRIVE);
    Roomba_Send_Byte(velocity>>8);
    Roomba_Send_Byte(velocity);
    Roomba_Send_Byte(radius>>8);
    Roomba_Send_Byte(radius);   
}