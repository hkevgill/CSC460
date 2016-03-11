#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LED_Test.h"
#include "os.h"
#include "queue.h"

//Comment out the following line to remove debugging code from compiled version.
#define DEBUG

extern void a_main();

/*===========
  * RTOS Internal
  *===========
  */

/**
  * This internal kernel function is the context switching mechanism.
  * It is done in a "funny" way in that it consists two halves: the top half
  * is called "Exit_Kernel()", and the bottom half is called "Enter_Kernel()".
  * When kernel calls this function, it starts the top half (i.e., exit). Right in
  * the middle, "Cp" is activated; as a result, Cp is running and the kernel is
  * suspended in the middle of this function. When Cp makes a system call,
  * it enters the kernel via the Enter_Kernel() software interrupt into
  * the middle of this function, where the kernel was suspended.
  * After executing the bottom half, the context of Cp is saved and the context
  * of the kernel is restore. Hence, when this function returns, kernel is active
  * again, but Cp is not running any more. 
  * (See file "switch.S" for details.)
  */
extern void CSwitch();
extern void Exit_Kernel();    /* this is the same as CSwitch() */

/* Prototype */
void Task_Terminate(void);

/** 
  * This external function could be implemented in two ways:
  *  1) as an external function call, which is called by Kernel API call stubs;
  *  2) as an inline macro which maps the call into a "software interrupt";
  *       as for the AVR processor, we could use the external interrupt feature,
  *       i.e., INT0 pin.
  *  Note: Interrupts are assumed to be disabled upon calling Enter_Kernel().
  *     This is the case if it is implemented by software interrupt. However,
  *     as an external function call, it must be done explicitly. When Enter_Kernel()
  *     returns, then interrupts will be re-enabled by Enter_Kernel().
  */ 
extern void Enter_Kernel();

/**
  * This table contains ALL process descriptors. It doesn't matter what
  * state a task is in.
  */
static PD Process[MAXTHREAD];

static MTX Mutex[MAXMUTEX];

/**
  * The process descriptor of the currently RUNNING task.
  */
volatile static PD* Cp; 

/** 
  * Since this is a "full-served" model, the kernel is executing using its own
  * stack. We can allocate a new workspace for this kernel stack, or we can
  * use the stack of the "main()" function, i.e., the initial C runtime stack.
  * (Note: This and the following stack pointers are used primarily by the
  *   context switching code, i.e., CSwitch(), which is written in assembly
  *   language.)
  */         
volatile unsigned char *KernelSp;

/**
  * This is a "shadow" copy of the stack pointer of "Cp", the currently
  * running task. During context switching, we need to save and restore
  * it into the appropriate process descriptor.
  */
volatile unsigned char *CurrentSp;

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;  

/** number of tasks created so far */
volatile static unsigned int Tasks;  

// Number of mutexes created so far
volatile static unsigned int Mutexes;

// Global tick overflow count
volatile unsigned int tickOverflowCount = 0;

volatile PD *ReadyQueue[MAXTHREAD];
volatile int RQCount = 0;

volatile PD *SleepQueue[MAXTHREAD];
volatile int SQCount = 0;

volatile PD *WaitingQueue[MAXTHREAD];
volatile int WQCount = 0;

/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
PID Kernel_Create_Task_At( volatile PD *p, voidfuncptr f, PRIORITY py, int arg ) {   
	unsigned char *sp;

#ifdef DEBUG
	int counter = 0;
#endif

	//Changed -2 to -1 to fix off by one error.
	sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);



	/*----BEGIN of NEW CODE----*/
	//Initialize the workspace (i.e., stack) and PD here!

	//Clear the contents of the workspace
	memset(&(p->workSpace),0,WORKSPACE);

	//Notice that we are placing the address (16-bit) of the functions
	//onto the stack in reverse byte order (least significant first, followed
	//by most significant).  This is because the "return" assembly instructions 
	//(rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig. 
	//second), even though the AT90 is LITTLE ENDIAN machine.

	//Store terminate at the bottom of stack to protect against stack underrun.
	*(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;

	//Place return address of function at bottom of stack
	*(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
	*(unsigned char *)sp-- = 0x00; // Fix 17 bit address problem for PC

#ifdef DEBUG
   //Fill stack with initial values for development debugging
   //Registers 0 -> 31 and the status register
	for (counter = 0; counter < 34; counter++) {
		*(unsigned char *)sp-- = counter;
	}
#else
	//Place stack pointer at top of stack
	sp = sp - 34;
#endif
	  
	p->sp = sp;     /* stack pointer into the "workSpace" */
	p->code = f;        /* function to be executed as a task */
	p->request = NONE;
	p->p = Tasks;
	Tasks++;
	p->py = py;
	p->inheritedPy = py;
	p->arg = arg;

	/*----END of NEW CODE----*/

	p->state = READY;

	// Add to ready queue
	enqueueRQ(&p, &ReadyQueue, &RQCount);

	return p->p;
}

/**
  *  Create a new task
  */
static PID Kernel_Create_Task( voidfuncptr f, PRIORITY py, int arg ) {
	int x;

	if (Tasks == MAXTHREAD) return;  /* Too many task! */

	/* find a DEAD PD that we can use  */
	for (x = 0; x < MAXTHREAD; x++) {
		if (Process[x].state == DEAD) break;
	}

	unsigned int p = Kernel_Create_Task_At( &(Process[x]), f, py, arg );

	return p;
}

MUTEX Kernel_Init_Mutex_At(volatile MTX *m) {
	m->m = Mutexes;
	m->state = FREE;
	Mutexes++;

	return m->m;
}

static MUTEX Kernel_Init_Mutex() {
	int x;

	if (Mutexes == MAXMUTEX) return; // Too many mutexes!

	// find a Disabled mutex that we can use
	for (x = 0; x < MAXMUTEX; x++) {
		if (Mutex[x].state == DISABLED) break;
	}

	unsigned int m = Kernel_Init_Mutex_At( &(Mutex[x]) );

	return m;
}

static unsigned int Kernel_Lock_Mutex() {
	int i,j;
	MUTEX m = Cp->m;
	for(i=0; i<MAXMUTEX; i++) {
		if (Mutex[i].m == m) break;
	}
	if(i>=MAXMUTEX){
		return 1;
	}

	if(Mutex[i].state == FREE) {
		Mutex[i].state = LOCKED;
		Mutex[i].owner = Cp->p;
		Mutex[i].lockCount++;
	}
	else if (Mutex[i].owner == Cp->p) {
		Mutex[i].lockCount++;
	}
	else {
		for(j=0; j<MAXTHREAD; j++) {
			if (Process[j].m == m) break;
		}
		if (Process[j].inheritedPy > Cp->inheritedPy) {
			Process[j].inheritedPy = Cp->inheritedPy;
		}
		Cp->state = BLOCKED_ON_MUTEX;
		enqueueWQ(&Cp, &WaitingQueue, &WQCount);
		return 0;
	}
	return 1;
}

static void Kernel_Unlock_Mutex() {
	int i;
	MUTEX m = Cp->m;
	for(i=0; i<MAXMUTEX; i++) {
		if (Mutex[i].m == m) break;
	}
	if(i>=MAXMUTEX){
		return;
	}
	if(Mutex[i].owner != Cp->p){
		return;
	} 
	else if (Mutex[i].lockCount > 1) {
		Mutex[i].lockCount--;
	}
	else {
		volatile PD* p = dequeueWQ(&WaitingQueue, &WQCount, m);
		if(p == NULL){
			Mutex[i].state = FREE;
			Mutex[i].lockCount = 0;
			Mutex[i].owner = 99;
		}
		else {
			Mutex[i].lockCount = 0;
			Mutex[i].owner = p->p;
			enqueueRQ(&p, &ReadyQueue, &RQCount);
		}
	}
}

/**
  * This internal kernel function is a part of the "scheduler". It chooses the 
  * next task to run, i.e., Cp.
  */
static void Dispatch() {
	 /* find the next READY task
	   * Note: if there is no READY task, then this will loop forever!.
	   */

	Cp = dequeue(&ReadyQueue, &RQCount);

	CurrentSp = Cp->sp;
	Cp->state = RUNNING;
}

/**
  * This internal kernel function is the "main" driving loop of this full-served
  * model architecture. Basically, on OS_Start(), the kernel repeatedly
  * requests the next user task's next system call and then invokes the
  * corresponding kernel function on its behalf.
  *
  * This is the main loop of our kernel, called by OS_Start().
  */
static void Next_Kernel_Request() {
	Dispatch();  /* select a new task to run */

	unsigned int mutex_is_locked;

	while(1) {
		Cp->request = NONE; /* clear its request */

		/* activate this newly selected task */
		CurrentSp = Cp->sp;

		Exit_Kernel();    /* or CSwitch() */

		/* if this task makes a system call, it will return to here! */

		/* save the Cp's stack pointer */
		Cp->sp = CurrentSp;

		switch(Cp->request){
		case CREATE:
			Cp->response = Kernel_Create_Task( Cp->code, Cp->py, Cp->arg );
			break;
		case NEXT:
		case NONE:
			/* NONE could be caused by a timer interrupt */
			Cp->state = READY;
			enqueueRQ(&Cp, &ReadyQueue, &RQCount);
			Dispatch();
			break;
		case SLEEP:
			Cp->state = SLEEPING;
			enqueueSQ(&Cp, &SleepQueue, &SQCount);
			Dispatch();
			break;
		case TERMINATE:
			/* deallocate all resources used by this task */
			Cp->state = DEAD;
			Dispatch();
			break;
		case MUTEX_INIT:
			Cp->response = Kernel_Init_Mutex();
			break;
		case MUTEX_LOCK:
			mutex_is_locked = Kernel_Lock_Mutex();
			if (!mutex_is_locked) {
				Dispatch();
			}
			break;
		case MUTEX_UNLOCK:
			Kernel_Unlock_Mutex();
            break; 
		default:
			/* Houston! we have a problem here! */
			break;
		}
	} 
}

/*================
  * RTOS  API  and Stubs
  *================
  */

/**
  * This function initializes the RTOS and must be called before any other
  * system calls.
  */
void OS_Init() {
	int x;

	Tasks = 0;
	KernelActive = 0;
	Mutexes = 0;
	//Reminder: Clear the memory for the task on creation.
	for (x = 0; x < MAXTHREAD; x++) {
		memset(&(Process[x]),0,sizeof(PD));
		Process[x].state = DEAD;
	}

	for (x = 0; x < MAXMUTEX; x++) {
		memset(&(Mutex[x]),0,sizeof(MTX));
		Mutex[x].state = DISABLED;
	}
}

/**
  * This function starts the RTOS after creating a few tasks.
  */
void OS_Start() {   
	if ( (! KernelActive) && (Tasks > 0)) {
		Disable_Interrupt();
		/* we may have to initialize the interrupt vector for Enter_Kernel() here. */

		/* here we go...  */
		KernelActive = 1;
		Next_Kernel_Request();
		/* NEVER RETURNS!!! */
	}
}

MUTEX Mutex_Init() {
	if(KernelActive) {
		Disable_Interrupt();
		Cp->request = MUTEX_INIT;
		Enter_Kernel();
		return Cp->response;
	}
}

void Mutex_Lock(MUTEX m) {
	if(KernelActive) {
		Disable_Interrupt();
		Cp->request = MUTEX_LOCK;
		Cp->m = m;
		Enter_Kernel();
	}
	
}

void Mutex_Unlock(MUTEX m) {
	if(KernelActive) {
		Disable_Interrupt();
		Cp->request = MUTEX_UNLOCK;
		Cp->m = m;
		Enter_Kernel();
	}
}

/**
  * For this example, we only support cooperatively multitasking, i.e.,
  * each task gives up its share of the processor voluntarily by calling
  * Task_Next().
  */
PID Task_Create( voidfuncptr f, PRIORITY py, int arg){
	unsigned int p;

	if (KernelActive) {
		Disable_Interrupt();
		Cp->request = CREATE;
		Cp->code = f;
		Cp->py = py;
		Cp->arg = arg;
		Enter_Kernel();
		p = Cp->response;
	} else { 
	  /* call the RTOS function directly */
	  p = Kernel_Create_Task( f, py, arg );
	}
	return p;
}

/**
  * The calling task gives up its share of the processor voluntarily.
  */
void Task_Next() {
	if (KernelActive) {
		Disable_Interrupt();
		Cp->request = NEXT;
		Enter_Kernel();
	}
}

void Task_Sleep(TICK t) {
	if (KernelActive) {
		Disable_Interrupt();
		Cp->request = SLEEP;
		unsigned int clockTicks = TCNT3/625;
		Cp->wakeTickOverflow = tickOverflowCount + ((t + clockTicks) / 100);
		Cp->wakeTick = (t + clockTicks) % 100;
		Enter_Kernel();
	}
}

/**
  * The calling task terminates itself.
  */
void Task_Terminate() {
	if (KernelActive) {
		Disable_Interrupt();
		Cp -> request = TERMINATE;
		Enter_Kernel();
		/* never returns here! */
	}
}

int Task_GetArg(PID p) {
	int i;
	for (i = 0; i < MAXTHREAD; i++){
		if (Process[i].p == p) {
			return Process[i].arg;
		}
	}
	return -1;
}

void setup() {
	// pin 47
	init_LED_PORTL_pin2();

	// pin 43
	init_LED_PORTL_pin6();

	// pin 44
	init_LED_PORTL_pin5();

	// initialize Timer1 16 bit timer
	Disable_Interrupt();

	// Timer 1
	TCCR1A = 0;                 // Set TCCR1A register to 0
	TCCR1B = 0;                 // Set TCCR1B register to 0

	TCNT1 = 0;                  // Initialize counter to 0

	OCR1A = 624;                // Compare match register (TOP comparison value) [(16MHz/(100Hz*8)] - 1

	TCCR1B |= (1 << WGM12);     // Turns on CTC mode (TOP is now OCR1A)

	TCCR1B |= (1 << CS12);      // Prescaler 256

	TIMSK1 |= (1 << OCIE1A);    // Enable timer compare interrupt

	// Timer 3
	TCCR3A = 0;                 // Set TCCR0A register to 0
	TCCR3B = 0;                 // Set TCCR0B register to 0

	TCNT3 = 0;                  // Initialize counter to 0

	OCR3A = 62499;                // Compare match register (TOP comparison value) [(16MHz/(100Hz*8)] - 1

	TCCR3B |= (1 << WGM32);     // Turns on CTC mode (TOP is now OCR1A)

	TCCR3B |= (1 << CS32);      // Prescaler 1024

	TIMSK3 = (1 << OCIE3A);

	Enable_Interrupt();
}

ISR(TIMER1_COMPA_vect) {

	// This version dequeues all that need dequeuing.
	volatile int i;

	for (i = SQCount-1; i >= 0; i--) {
		if ((SleepQueue[i]->wakeTickOverflow <= tickOverflowCount) && (SleepQueue[i]->wakeTick <= (TCNT3/625))) {
			volatile PD *p = dequeue(&SleepQueue, &SQCount);
			enqueueRQ(&p, &ReadyQueue, &RQCount);
		}
		else {
			break;
		}
	}

	// This version dequeues one item.
	// if ((!isEmpty(&SQCount)) && (SleepQueue[SQCount-1]->wakeTickOverflow <= tickOverflowCount) && (SleepQueue[SQCount-1]->wakeTick <= (TCNT3/625))) {
	//     volatile PD *p = dequeue(&SleepQueue, &SQCount);
	//     enqueueRQ(&p, &ReadyQueue, &RQCount);
	// }

	Cp->request = NEXT;
	// asm ( "clr r0":: );
	// asm ( "ldi ZL, lo8(Enter_Kernel)":: );
	// asm ( "ldi ZH, hi8(Enter_Kernel)":: );
	// asm ( "ldi r16, hhi8(Enter_Kernel)":: );
	// asm ( "add ZL, r17"::);
	// asm ( "adc ZH, r0"::);
	// asm ( "adc r16, r0"::);
	// asm ( "out 0x3C, r16"::);
	// asm ( "eijmp":: );
	Task_Next();
}

ISR(TIMER3_COMPA_vect) {
	tickOverflowCount += 1;
}

/**
  * This function creates two cooperative tasks, "Ping" and "Pong". Both
  * will run forever.
  */
void main() {
	setup();

	OS_Init();
	Task_Create(a_main, 0, 1);
	OS_Start();
}

