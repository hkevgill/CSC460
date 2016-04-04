#ifndef ROOMBA_H_
#define ROOMBA_H_

#ifndef XPLAINED
//#error "Roomba.h can only be used with the Xplained board"
#endif

#include <stdint.h>


/**
  * Roomba serial opcodes
  */
#define START           128
#define CONTROL_MODE    130
#define SAFE_MODE       131
#define FULL_MODE       132
#define POWER           133
#define DRIVE           137
#define SONG            140
#define PLAY            141
#define SENSORS         142
#define QUERYLIST       149

/**
  * Sensors
  */
#define SENSOR_ALL_DATA 0
#define SENSOR_SUB_1    1
#define SENSOR_SUB_2    2
#define SENSOR_SUB_3    3

  /**
  * Data special cases
  */
#define ROOMBA_SPEED	250
#define ROOMBA_TURN		150
#define TURN_RADIUS		400
#define AUTOTURN_RADIUS	350
#define DRIVE_STRAIGHT  32768
#define IN_PLACE_CW     -1
#define IN_PLACE_CCW     1

/**
  * Wakes Roomba from sleep and sets it to safe mode
  */
void Roomba_Init(void);

/**
  * Tells the roomba to drive with the given velocity and radius
  */
void Roomba_Drive(int16_t velocity, int16_t radius);

/**
  * Plays the song corresponding to a song # from 0-4
  */
void Roomba_Play(uint8_t song);

/**
  * Requests a packet from the Roomba about a specific sensor
  */
void Roomba_Sensors(uint8_t packet_id);

/**
  * Requests multiple packets from the Roomba
  */
void Roomba_QueryList(uint8_t packet1, uint8_t packet2);

/**
  * Loads a song onto the Roomba
  */
void Roomba_Song(uint8_t n);

#endif /* ROOMBA_H_ */