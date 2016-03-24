#ifndef ROOMBA_H_
#define ROOMBA_H_

#ifndef XPLAINED
//#error "Roomba.h can only be used with the Xplained board"
#endif

#include <stdint.h>

// Roomba serial opcodes (cleaning modes, lights and sound not included)
#define START           128
#define CONTROL_MODE    130
#define SAFE_MODE       131
#define FULL_MODE       132
#define POWER           133
#define DRIVE           137

// Sensors
#define GET_SENSOR_DATA 142
#define SENSOR_ALL_DATA 0
#define SENSOR_SUB_1    1
#define SENSOR_SUB_2    2
#define SENSOR_SUB_3    3


// Data special cases
#define DRIVE_STRAIGHT  32768
#define IN_PLACE_CW     -1
#define IN_PLACE_CCW     1

void Roomba_Init(void);

void Roomba_Drive(int16_t velocity, int16_t radius);

#endif /* ROOMBA_H_ */