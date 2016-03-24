#ifndef USB_UART_H_
#define UART_H_

#include <stdint.h>

void Roomba_UART_Init(void);
void Roomba_Send_Byte(uint8_t);
void Roomba_Receive_Byte(void);
void Roomba_Send_String(char*);

void Bluetooth_UART_Init(void);
void Bluetooth_Send_Byte(uint8_t);
void Bluetooth_Receive_Byte(void);
void Bluetooth_Send_String(char*);

#endif /* USB_UART_H_ */