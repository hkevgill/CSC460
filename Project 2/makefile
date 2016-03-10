CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-c -g -Os -mmcu=atmega2560
ELFFLAGS=-g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
LOADFLAGS=-p m2560 -c stk500v2 -P /dev/cu.usbmodem1421 -b 115200 -U flash:w:rtos.hex:i -D

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