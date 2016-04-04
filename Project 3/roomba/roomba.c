#define F_CPU 16000000

#include "roomba.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "../uart/uart.h" 

/**
  * Wakes Roomba from sleep and sets it to safe mode
  */
void Roomba_Init(){
    
	_delay_ms(2100);
   
    // Wake Roomba from sleep
    // Pin 25
    DDRA |= (1<<PA3);
    PORTA |= (1<<PA3);
    
    PORTA &= ~(1<<PA3);
    
    PORTA |= (1<<PA3);
    
    // set Roomba to passive mode
    Roomba_Send_Byte(START);

    // set Roomba to safe mode
    Roomba_Send_Byte(SAFE_MODE); 

    Roomba_Song(0); // Initialize song 0 
}

/**
  * Tells the roomba to drive with the given velocity and radius
  */
void Roomba_Drive(int16_t velocity, int16_t radius) {    
    Roomba_Send_Byte(DRIVE);
    Roomba_Send_Byte(velocity>>8);
    Roomba_Send_Byte(velocity);
    Roomba_Send_Byte(radius>>8);
    Roomba_Send_Byte(radius);   
}

/**
  * Plays the song corresponding to a song # from 0-4
  */
void Roomba_Play(uint8_t song) {
	Roomba_Send_Byte(PLAY);
	Roomba_Send_Byte(song);
}

/**
  * Requests a packet from the Roomba about a specific sensor
  */
void Roomba_Sensors(uint8_t packet_id) {
	Roomba_Send_Byte(SENSORS);
	Roomba_Send_Byte(packet_id);
}

/**
  * Requests multiple packets from the Roomba
  */
void Roomba_QueryList(uint8_t packet1, uint8_t packet2) {
    Roomba_Send_Byte(QUERYLIST);
    Roomba_Send_Byte(2);
    Roomba_Send_Byte(packet1);
    Roomba_Send_Byte(packet2);
}

/**
  * Loads a song onto the Roomba
  */
void Roomba_Song(uint8_t n) {
	Roomba_Send_Byte(SONG);
	Roomba_Send_Byte(n);  // Song 0
	Roomba_Send_Byte(6);  // 6 Notes
	
	Roomba_Send_Byte(72); // C
	Roomba_Send_Byte(16);
	Roomba_Send_Byte(69); // A
	Roomba_Send_Byte(16);
	Roomba_Send_Byte(67); // G
	Roomba_Send_Byte(16);
	Roomba_Send_Byte(64); // E
	Roomba_Send_Byte(16);
	Roomba_Send_Byte(62); // D
	Roomba_Send_Byte(16);
	Roomba_Send_Byte(60); // C
	Roomba_Send_Byte(16);
}