#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

uint8_t LASER = 0;
uint8_t SERVO = 1;
uint8_t LIGHT = 2;
uint8_t SCREEN = 3;

uint8_t IdlePID;
uint8_t RoombaTestPID;
uint8_t BluetoothSendPID;
uint8_t BluetoothReceivePID;
uint8_t LaserTaskPID;
uint8_t ServoTaskPID;
uint8_t LightSensorTaskPID;

int laserState;
int servXState;
int servYState;

uint16_t photocellReading;

// An idle task that runs when there is nothing else to do
// Could be changed later to put CPU into low power state

typedef enum laser_states {
    OFF = 0,
    ON
} LASER_STATES;

typedef enum servo_states {
    FULL_BACK = 0,
    HALF_BACK,
    DEAD_ZONE,
    HALF_FORWARD,
    FULL_FORWARD
} SERVO_STATES;

// Queue globals
#define QSize 	10

int servoQueue[QSize];
int servoFront;
int servoRear;

int laserQueue[QSize];
int laserFront;
int laserRear;

// Mutexes
MUTEX laserMutex;
MUTEX servoMutex;

// ------------------------------ IS FULL ------------------------------ //
int buffer_isFull(int *front, int *rear) {
  return (*rear == (*front - 1) % QSize);
}

// ------------------------------ IS EMPTY ------------------------------ //
int buffer_isEmpty(int *front, int *rear) {
  return (*rear == *front);
}

// ------------------------------ ENQUEUE ------------------------------ //
void buffer_enqueue(int val, int *queue, int *front, int *rear) {
  if (buffer_isFull(front, rear)) {
    return;
  }
  queue[*rear] = val;
  *rear = (*rear + 1) % QSize;
}

// ------------------------------ DEQUEUE ------------------------------ //
int buffer_dequeue(int *queue, int *front, int *rear){
  if (buffer_isEmpty(front, rear)) {
    return -1;
  }
  int result = queue[*front];
  *front = (*front + 1) % QSize;
  return result;
}

// ------------------------------ TOGGLE PORTL6 ------------------------------ /
void enablePORTL6() {
	PORTL |= _BV(PORTL6);
}
void disablePORTL6() {
	PORTL &= ~_BV(PORTL6);
}
// ------------------------------ TOGGLE PORTL2 ------------------------------ /
void enablePORTL2() {
	PORTL |= _BV(PORTL2);
}
void disablePORTL2() {
	PORTL &= ~_BV(PORTL2);
}

// ------------------------------ ROOMBA TEST ------------------------------ //
void Roomba_Test() {
	for(;;) {
		Roomba_Drive(75,DRIVE_STRAIGHT);
		_delay_ms(5000);
		Roomba_Drive(-75,DRIVE_STRAIGHT);
		_delay_ms(5000);
	}
}


// ******************************************************************* //
// ****************************** TASKS ****************************** //
// ******************************************************************* //

void ADC_init() {
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
}

// ------------------------------ IDLE TASK ------------------------------ //
void Idle() {
	for(;;) {
		continue;
	}
}

// ------------------------------ LASER TASK ------------------------------ //
void Laser_Task() {
	for(;;) {
		Mutex_Lock(laserMutex);
		if(!buffer_isEmpty(&laserFront, &laserRear)) {
			laserState = buffer_dequeue(laserQueue, &laserFront, &laserRear);
			if (laserState == ON) {
				enablePORTL6();
			}
			else {
				disablePORTL6();
			}
		}
		Mutex_Unlock(laserMutex);
		Task_Sleep(10);
	}
}

// ------------------------------ SERVO TASK ------------------------------ //
void Servo_Task() {
	for(;;) {
		Mutex_Lock(servoMutex);
		if(!buffer_isEmpty(&servoFront, &servoRear)) {
			servXState = buffer_dequeue(servoQueue, &servoFront, &servoRear);
			if(!buffer_isEmpty(&servoFront, &servoRear)) {
				servYState = buffer_dequeue(servoQueue, &servoFront, &servoRear);

				// TEST CODE--------------
				// if (servYState > 100) {
				// 	enablePORTL6();
				// }
				// else {
				// 	disablePORTL6();
				// }
				// TEST CODE--------------
			}
		}
		Mutex_Unlock(servoMutex);
		Task_Sleep(10);
	}
}

// ------------------------------ LIGHT SENSOR TASK ------------------------------ //
void LightSensor_Task() {
	for(;;) {
		// Read photocell
		ADMUX = (ADMUX & 0xE0);

		ADMUX = (ADMUX | 0x07); // Channel 7

		ADCSRB |= (0<<MUX5);

		ADCSRA |= (1<<ADSC); // Start conversion

		while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

		photocellReading = ADC;

		if (photocellReading >= 500) {
			enablePORTL2();
		}
		if (photocellReading < 500) {
			disablePORTL2();
		}

		Task_Sleep(15);
	}
}

// ------------------------------ BLUETOOTH SEND ------------------------------ //
void Bluetooth_Send() {
	for(;;) {
		// SEND LIGHT SENSOR DATA
		Bluetooth_Send_Byte(LIGHT);
		Bluetooth_Send_Byte(photocellReading>>8);
		Bluetooth_Send_Byte(photocellReading);

		Task_Sleep(10);
	}
}

// ------------------------------ BLUETOOTH RECIEVE ------------------------------ //
void Bluetooth_Receive() {
	uint8_t flag;
	uint8_t laser_data;
	uint16_t servo_data;
	uint8_t servo_data1;
	uint8_t servo_data2;


	for(;;){
		if(( UCSR1A & (1<<RXC1))) {
			flag = Bluetooth_Receive_Byte();

			if (flag == LASER){
				Mutex_Lock(laserMutex);
				laser_data = Bluetooth_Receive_Byte();
				buffer_enqueue(laser_data, laserQueue, &laserFront, &laserRear);
				Mutex_Unlock(laserMutex);
			}

			if (flag == SERVO){
				Mutex_Lock(servoMutex);
				
				servo_data1 = Bluetooth_Receive_Byte();
				servo_data2 = Bluetooth_Receive_Byte();
				servo_data = (servo_data1<<8) | (servo_data2);
				buffer_enqueue(servo_data, servoQueue, &servoFront, &servoRear);

				servo_data1 = Bluetooth_Receive_Byte();
				servo_data2 = Bluetooth_Receive_Byte();
				servo_data = (servo_data1<<8) | (servo_data2);
				buffer_enqueue(servo_data, servoQueue, &servoFront, &servoRear);
				
				Mutex_Unlock(servoMutex);
			}
		}
		Task_Sleep(5);
	}
}


// Application level main function
// Creates the required tasks and then terminates
void a_main() {

	// Initialize LEDS
	DDRL |= _BV(DDL6);
	DDRL |= _BV(DDL2);

	// Initialize Queues
	laserFront = 0;
	laserRear = 0;
	servoFront = 0;
	servoRear = 0;

	// Initialize Mutexes
	laserMutex = Mutex_Init();
	servoMutex = Mutex_Init();

	// Initialize Bluetooth and Roomba UART
	Bluetooth_UART_Init();
	Roomba_UART_Init();
	// Roomba_Init();
	ADC_init();

	// Initialize Values
	photocellReading = 0;

	// Create Tasks
	IdlePID = Task_Create(Idle, MINPRIORITY, 1);
	// RoombaTestPID = Task_Create(Roomba_Test, 9, 2);

	BluetoothSendPID = Task_Create(Bluetooth_Send, 1, 3);
	BluetoothReceivePID = Task_Create(Bluetooth_Receive, 1, 3);
	LaserTaskPID = Task_Create(Laser_Task, 1, 3);
	// ServoTaskPID = Task_Create(Servo_Task, 1, 3);
	LightSensorTaskPID = Task_Create(LightSensor_Task, 1, 3);

	Task_Terminate();
}