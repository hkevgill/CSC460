CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-c -g -Os -mmcu=atmega2560
ELFFLAGS=-g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
LOADFLAGS=-p m2560 -c stk500v2 -P /dev/cu.usbmodem1411 -b 115200 -U flash:w:rtos.hex:i -D

all: compile elf hex load

compile: cswitch.S os.c main.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c main.c queue.c LED_Test.c

elf: cswitch.o os.o main.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o main.o queue.o LED_Test.o

hex: rtos.elf
	$(COPY) $(HEXFLAGS) rtos.elf rtos.hex

load: rtos.hex
	$(LOAD) $(LOADFLAGS)

clean:
	rm -f *.elf *.o *.hex

# Test 1

t1: compile_t1 elf_t1 hex load

compile_t1: cswitch.S os.c test1.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test1.c queue.c LED_Test.c

elf_t1: cswitch.o os.o test1.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test1.o queue.o LED_Test.o

# Test 2

t2: compile_t2 elf_t2 hex load

compile_t2: cswitch.S os.c test2.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test2.c queue.c LED_Test.c

elf_t2: cswitch.o os.o test2.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test2.o queue.o LED_Test.o

# Test 3

t3: compile_t3 elf_t3 hex load

compile_t3: cswitch.S os.c test3.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test3.c queue.c LED_Test.c

elf_t3: cswitch.o os.o test3.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test3.o queue.o LED_Test.o

# Test 4

t4: compile_t4 elf_t4 hex load

compile_t4: cswitch.S os.c test4.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test4.c queue.c LED_Test.c

elf_t4: cswitch.o os.o test4.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test4.o queue.o LED_Test.o

# Test Terminate With Mutex

t5: compile_t5 elf_t5 hex load

compile_t5: cswitch.S os.c test_terminate_with_mutex.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_terminate_with_mutex.c queue.c LED_Test.c

elf_t5: cswitch.o os.o test_terminate_with_mutex.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_terminate_with_mutex.o queue.o LED_Test.o

# Test Terminate

t6: compile_t6 elf_t6 hex load

compile_t6: cswitch.S os.c test_terminate.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_terminate.c queue.c LED_Test.c

elf_t6: cswitch.o os.o test_terminate.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_terminate.o queue.o LED_Test.o

# Test Mutex with Mutual Exclusion

t7: compile_t7 elf_t7 hex load

compile_t7: cswitch.S os.c test_mutex_mutual_exclusion.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_mutual_exclusion.c queue.c LED_Test.c

elf_t7: cswitch.o os.o test_mutex_mutual_exclusion.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_mutual_exclusion.o queue.o LED_Test.o

# Test Mutex with Ownership

t8: compile_t8 elf_t8 hex load

compile_t8: cswitch.S os.c test_mutex_ownership.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_ownership.c queue.c LED_Test.c

elf_t8: cswitch.o os.o test_mutex_ownership.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_ownership.o queue.o LED_Test.o

# Test Mutex with Recursiveness

t9: compile_t9 elf_t9 hex load

compile_t9: cswitch.S os.c test_mutex_recursiveness.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_recursiveness.c queue.c LED_Test.c

elf_t9: cswitch.o os.o test_mutex_recursiveness.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_recursiveness.o queue.o LED_Test.o

# Test Mutex with Priority Inheritance

t10: compile_t10 elf_t10 hex load

compile_t10: cswitch.S os.c test_mutex_priority_inheritance.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_priority_inheritance.c queue.c LED_Test.c

elf_t10: cswitch.o os.o test_mutex_priority_inheritance.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_priority_inheritance.o queue.o LED_Test.o

# Test Mutex with Priority Inheritance 2

t11: compile_t11 elf_t11 hex load

compile_t11: cswitch.S os.c test_mutex_priority_inheritance2.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_priority_inheritance2.c queue.c LED_Test.c

elf_t11: cswitch.o os.o test_mutex_priority_inheritance2.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_priority_inheritance2.o queue.o LED_Test.o

# Test Mutex with Priority Inheritance 3

t12: compile_t12 elf_t12 hex load

compile_t12: cswitch.S os.c test_mutex_priority_inheritance3.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_mutex_priority_inheritance3.c queue.c LED_Test.c

elf_t12: cswitch.o os.o test_mutex_priority_inheritance3.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_mutex_priority_inheritance3.o queue.o LED_Test.o

# Test Sleep

t13: compile_t13 elf_t13 hex load

compile_t13: cswitch.S os.c test_sleep.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_sleep.c queue.c LED_Test.c

elf_t13: cswitch.o os.o test_sleep.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_sleep.o queue.o LED_Test.o

# Test Suspend Resume

t14: compile_t14 elf_t14 hex load

compile_t14: cswitch.S os.c test_suspend_resume.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_suspend_resume.c queue.c LED_Test.c

elf_t14: cswitch.o os.o test_suspend_resume.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_suspend_resume.o queue.o LED_Test.o

# Test Suspend Resume 2

t15: compile_t15 elf_t15 hex load

compile_t15: cswitch.S os.c test_suspend_resume2.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_suspend_resume2.c queue.c LED_Test.c

elf_t15: cswitch.o os.o test_suspend_resume2.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_suspend_resume2.o queue.o LED_Test.o

# Test Events

t16: compile_t16 elf_t16 hex load

compile_t16: cswitch.S os.c test_events.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_events.c queue.c LED_Test.c

elf_t16: cswitch.o os.o test_events.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_events.o queue.o LED_Test.o

# Test Events 2

t17: compile_t17 elf_t17 hex load

compile_t17: cswitch.S os.c test_events2.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_events2.c queue.c LED_Test.c

elf_t17: cswitch.o os.o test_events2.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_events2.o queue.o LED_Test.o

# Test Events 3

t18: compile_t18 elf_t18 hex load

compile_t18: cswitch.S os.c test_events3.c queue.c LED_Test.c
	$(CC) $(FLAGS) cswitch.S os.c test_events3.c queue.c LED_Test.c

elf_t18: cswitch.o os.o test_events3.o queue.o LED_Test.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o test_events3.o queue.o LED_Test.o

