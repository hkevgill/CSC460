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
