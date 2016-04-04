#ifndef UART_H_
#define UART_H_

#include <stdint.h>

/**
  * Initializes UART 3, sets baud rate to 19.2k, enables transmitter and receiver, 8 bit data
  */
void Roomba_UART_Init(void);

/**
  * Sends 1 byte to the Roomba
  */
void Roomba_Send_Byte(uint8_t);

/**
  * Receives 1 byte from the Roomba
  */
unsigned char Roomba_Receive_Byte(void);

/**
  * Sends a string of characters to the Roomba
  */
void Roomba_Send_String(char*);


/**
  * Initializes UART 1, sets baud rate to 19.2k, enables transmitter and receiver, 8 bit data
  */
void Bluetooth_UART_Init(void);

/**
  * Sends 1 byte over bluetooth
  */
void Bluetooth_Send_Byte(uint8_t);

/**
  * Receives 1 byte from bluetooth
  */
unsigned char Bluetooth_Receive_Byte(void);

/**
  * Sends a string of characters over bluetooth
  */
void Bluetooth_Send_String(char*);

#endif /* UART_H_ */