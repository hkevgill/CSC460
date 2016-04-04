#define F_CPU 16000000
#include <util/delay.h>
#include "uart.h"
#include <string.h>
#include <avr/io.h>

/**
  * Initializes UART 3, sets baud rate to 19.2k, enables transmitter and receiver, 8 bit data
  */
void Roomba_UART_Init(){   
    // Set baud rate to 19.2k
    UBRR3 = 0x33;
    
    // Enable receiver, transmitter
    UCSR3B = (1<<RXEN3) | (1<<TXEN3);

    // 8-bit data
    UCSR3C = ((1<<UCSZ31)|(1<<UCSZ30));

    // disable 2x speed
    UCSR3A &= ~(1<<U2X3);
}

/**
  * Sends 1 byte to the Roomba
  */
void Roomba_Send_Byte(uint8_t data_out){      
    // Wait for empty transmit buffer
    while(!( UCSR3A & (1<<UDRE3)));
    // Put data into buffer
    UDR3 = data_out;
}

/**
  * Receives 1 byte from the Roomba
  */
unsigned char Roomba_Receive_Byte(){      
    // Wait for data to be received
    while(!( UCSR3A & (1<<RXC3)));
    // Get and return data from buffer
    return UDR3;
}

/**
  * Sends a string of characters to the Roomba
  */
void Roomba_Send_String(char *string_out){
    for(; *string_out; string_out++){
        Roomba_Send_Byte(*string_out);
    }
}

/**
  * Initializes UART 1, sets baud rate to 19.2k, enables transmitter and receiver, 8 bit data
  */
void Bluetooth_UART_Init(){   
    // Set baud rate to 19.2k
    UBRR1 = 103;
    
    // Enable receiver, transmitter
    UCSR1B = (1<<RXEN1) | (1<<TXEN1);

    // 8-bit data
    UCSR1C = ((1<<UCSZ11)|(1<<UCSZ10));

    // disable 2x speed
    UCSR1A &= ~(1<<U2X1);
}

/**
  * Sends 1 byte over bluetooth
  */
void Bluetooth_Send_Byte(uint8_t data_out){      
    // Wait for empty transmit buffer
    while(!( UCSR1A & (1<<UDRE1)));
    // Put data into buffer
    UDR1 = data_out;
}

/**
  * Receives 1 byte from bluetooth
  */
unsigned char Bluetooth_Receive_Byte(){      
    // Wait for data to be received
    while(!( UCSR1A & (1<<RXC1)));
    // Get and return data from buffer
    return UDR1;
}

/**
  * Sends a string of characters over bluetooth
  */
void Bluetooth_Send_String(char *string_out){
    for(; *string_out; string_out++){
        _delay_ms(10);
        Bluetooth_Send_Byte(*string_out);
    }
}
