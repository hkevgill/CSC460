#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../rtos/os.h"
#include "../uart/uart.h"

uint8_t LASER = 0;
uint8_t SERVO = 1;
uint8_t PHOTO = 2;
uint8_t SCREEN = 3;
uint8_t ROOMBA = 4;

uint8_t IdlePID;
uint8_t RoombaTestPID;
uint8_t RoombaTaskPID;
uint8_t BluetoothSendPID;
uint8_t BluetoothReceivePID;
uint8_t LaserTaskPID;
uint8_t ServoTaskPID;
uint8_t LightSensorTaskPID;
uint8_t GetSensorDataTaskPID;

int laserState;
int servoState;
int lastServoState;
char roombaState;
int wallState;
int bumpState;

uint16_t photocellReading;
uint16_t photoThreshold;

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

int roombaQueue[QSize];
int roombaFront;
int roombaRear;

// Mutexes
MUTEX laserMutex;
MUTEX servoMutex;
MUTEX sensorMutex;

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
// ------------------------------ TOGGLE PORTL5 ------------------------------ /
void enablePORTL5() {
	PORTL |= _BV(PORTL5);
}
void disablePORTL5() {
	PORTL &= ~_BV(PORTL5);
}
// ------------------------------ TOGGLE PORTH3 ------------------------------ /
void enablePORTH3() {
	PORTL |= _BV(PORTH3);
}
void disablePORTH3() {
	PORTL &= ~_BV(PORTH3);
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

void Servo_Init() {
	// Setup ports and timers
    DDRA = 0xFF; // All output
    PORTA = 0;

    // Configure timer/counter1 as phase and frequency PWM mode
    TCNT4 = 0;
    TCCR4A = (1<<COM4A1) | (1<<COM4B1) | (1<<WGM41);  //NON Inverted PWM
    TCCR4B |= (1<<WGM43) | (1<<WGM42) | (1<<CS41) | (1<<CS40); //PRESCALER=64 MODE 14 (FAST PWM)
    ICR4 = 4999;

    OCR4A = 375; // 90 Degrees
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
			servoState = buffer_dequeue(servoQueue, &servoFront, &servoRear);
		}

		if (servoState > 380 && (lastServoState <= 610)) {
			if (servoState > 550) {
					lastServoState += 3;
			}
			lastServoState += 1;
			OCR4A = lastServoState;
		}
		else if (servoState < 370) {
			if (servoState < 1) {
				lastServoState -= 3;
			}
			lastServoState -= 1;
			OCR4A = lastServoState;
		}

		Mutex_Unlock(servoMutex);
		Task_Sleep(3);
	}
}

void Set_Photocell_Threshold() {
	uint16_t photo1;
	uint16_t photo2;
	uint16_t photo3;

	ADMUX = (ADMUX & 0xE0);

	ADMUX = (ADMUX | 0x07); // Channel 7

	ADCSRB |= (0<<MUX5);

	ADCSRA |= (1<<ADSC); // Start conversion

	while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

	photo1 = ADC;

	ADCSRA |= (1<<ADSC); // Start conversion

	while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

	photo2 = ADC;

	ADCSRA |= (1<<ADSC); // Start conversion

	while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

	photo3 = ADC;

	photoThreshold = (photo1 + photo2 + photo3) / 3;
}

// ------------------------------ LIGHT SENSOR TASK ------------------------------ //
void LightSensor_Task() {
	int i = 0;

	for(;;) {
		// Read photocell
		ADMUX = (ADMUX & 0xE0);

		ADMUX = (ADMUX | 0x07); // Channel 7

		ADCSRB |= (0<<MUX5);

		ADCSRA |= (1<<ADSC); // Start conversion

		while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE

		photocellReading = ADC;

		if (photocellReading >= (photoThreshold + 50)) {
			enablePORTL2();
			Roomba_Play(0);
			disablePORTL6();
			Roomba_Drive(0,0);
			OS_Abort();
		}

		if(i % 5 == 0) {
			Set_Photocell_Threshold();
			i = 0;
		}
		else {
			i++;
		}

		Task_Sleep(10);
	}
}

// ------------------------------ GET SENSOR DATA ------------------------------ //
void Get_Sensor_Data() {
	for(;;) {
		Roomba_QueryList(7, 13);

		// while(!(UCSR3A & (1<<RXC3)));
		Task_Sleep(2);
		bumpState = Roomba_Receive_Byte();
		// while(!(UCSR3A & (1<<RXC3)));
		Task_Sleep(2);
		wallState = Roomba_Receive_Byte();

		Task_Sleep(16);
	}
}

// ------------------------------ REVERSE ------------------------------ //
void Reverse() {
	buffer_dequeue(roombaQueue, &roombaFront, &roombaRear);

	if(roombaState == 'F') {
		Roomba_Drive(ROOMBA_SPEED,TURN_RADIUS); // Forward-Left
	}
	else if(roombaState == 'G') {
		Roomba_Drive(ROOMBA_SPEED,DRIVE_STRAIGHT); // Forward
	}
	else if(roombaState == 'H') {
		Roomba_Drive(ROOMBA_SPEED,-TURN_RADIUS); // Forward-Right
	}
	else if(roombaState == 'E') {
		Roomba_Drive(ROOMBA_TURN,IN_PLACE_CCW); // Left
	}
	else if(roombaState == 'D') {
		Roomba_Drive(ROOMBA_TURN,IN_PLACE_CW); // Right
	}
	else if(roombaState == 'A') {
		Roomba_Drive(-ROOMBA_SPEED,TURN_RADIUS); // Backward-left
	}
	else if(roombaState == 'B') {
		Roomba_Drive(-ROOMBA_SPEED,DRIVE_STRAIGHT); // Backward
	}
	else if(roombaState == 'C') {
		Roomba_Drive(-ROOMBA_SPEED,-TURN_RADIUS); // Backward-right
	}
	else if(roombaState == 'X') {
		Roomba_Drive(-ROOMBA_SPEED,DRIVE_STRAIGHT); // Backward
	}
}

// ------------------------------ BUMP BACK ------------------------------ //
void Bump_Back() {
	buffer_dequeue(roombaQueue, &roombaFront, &roombaRear);
	Roomba_Drive(-ROOMBA_SPEED,DRIVE_STRAIGHT); // Backward
}

// ------------------------------ MANUAL DRIVE ------------------------------ //
void Manual_Drive() {
	roombaState = buffer_dequeue(roombaQueue, &roombaFront, &roombaRear);

	if(roombaState == 'A') {
		Roomba_Drive(ROOMBA_SPEED,TURN_RADIUS); // Forward-Left
	}
	else if(roombaState == 'B') {
		Roomba_Drive(ROOMBA_SPEED,DRIVE_STRAIGHT); // Forward
	}
	else if(roombaState == 'C') {
		Roomba_Drive(ROOMBA_SPEED,-TURN_RADIUS); // Forward-Right
	}
	else if(roombaState == 'D') {
		Roomba_Drive(ROOMBA_TURN,IN_PLACE_CCW); // Left
	}
	else if(roombaState == 'E') {
		Roomba_Drive(ROOMBA_TURN,IN_PLACE_CW); // Right
	}
	else if(roombaState == 'F') {
		Roomba_Drive(-ROOMBA_SPEED,TURN_RADIUS); // Backward-left
	}
	else if(roombaState == 'G') {
		Roomba_Drive(-ROOMBA_SPEED,DRIVE_STRAIGHT); // Backward
	}
	else if(roombaState == 'H') {
		Roomba_Drive(-ROOMBA_SPEED,-TURN_RADIUS); // Backward-right
	}
	else if(roombaState == 'X') {
		Roomba_Drive(0,0); // Stop
	}
}

// ------------------------------ ROOMBA TASK ------------------------------ //
void Roomba_Task() {
	for(;;) {
		wallState = 0;
		bumpState = 0;

		if(wallState) {
			Reverse();
		}
		else if(bumpState >= 1 && bumpState <= 3) {
			Bump_Back();
		}
		else {
			Manual_Drive();
		}

		Task_Sleep(20);
	}
}

// ------------------------------ BLUETOOTH SEND ------------------------------ //
void Bluetooth_Send() {
	for(;;) {
		// SEND LIGHT SENSOR DATA
		Bluetooth_Send_Byte(PHOTO);
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

	char roomba_data;

	for(;;){
		if(( UCSR1A & (1<<RXC1))) {
			flag = Bluetooth_Receive_Byte();

			if (flag == LASER){
				Mutex_Lock(laserMutex);

				laser_data = Bluetooth_Receive_Byte();
				buffer_enqueue(laser_data, laserQueue, &laserFront, &laserRear);

				Mutex_Unlock(laserMutex);
			}

			// else if (flag == SERVO){
			// 	Mutex_Lock(servoMutex);

			// 	servo_data1 = Bluetooth_Receive_Byte();
			// 	servo_data2 = Bluetooth_Receive_Byte();
			// 	servo_data = ( ((servo_data1)<<8) | (servo_data2) );

			// 	buffer_enqueue(servo_data, servoQueue, &servoFront, &servoRear);

			// 	Mutex_Unlock(servoMutex);
			// }

			else if (flag == ROOMBA) {
				roomba_data = Bluetooth_Receive_Byte();
				buffer_enqueue(roomba_data, roombaQueue, &roombaFront, &roombaRear);
			}

			else {
				continue;
			}
		}
		Task_Sleep(5);
	}
}

// Application level main function
// Creates the required tasks and then terminates
void a_main() {

	// Initialize Ports
	DDRL |= _BV(DDL6);
	DDRL |= _BV(DDL2);
	DDRL |= _BV(DDL5);
	DDRH |= _BV(DDH3);

	// Initialize Queues
	laserFront = 0;
	laserRear = 0;
	servoFront = 0;
	servoRear = 0;
	roombaFront = 0;
	roombaRear = 0;

	// Initialize Mutexes
	laserMutex = Mutex_Init();
	servoMutex = Mutex_Init();
	sensorMutex = Mutex_Init();

	// Initialize Bluetooth and Roomba UART
	Bluetooth_UART_Init();
	Roomba_UART_Init();
	ADC_init();
	// Servo_Init();
	Roomba_Init();

	// Evaluate light
	Set_Photocell_Threshold();

	// Initialize Values
	photocellReading = 0;
	servoState = 375;
	lastServoState = 375;
	wallState = 0;
	bumpState = 0;
	roombaState = NULL;

	// Create Tasks
	IdlePID 					= Task_Create(Idle, MINPRIORITY, 1);
	BluetoothReceivePID 		= Task_Create(Bluetooth_Receive, 1, 3);
	BluetoothSendPID 			= Task_Create(Bluetooth_Send, 2, 3);
	LaserTaskPID 				= Task_Create(Laser_Task, 2, 3);
	LightSensorTaskPID 			= Task_Create(LightSensor_Task, 2, 3);
	// ServoTaskPID 				= Task_Create(Servo_Task, 2, 3);
	RoombaTaskPID 				= Task_Create(Roomba_Task, 2, 2);
	GetSensorDataTaskPID 		= Task_Create(Get_Sensor_Data, 2, 2);

	Task_Terminate();
}