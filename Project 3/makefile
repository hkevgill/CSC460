CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-c -g -Os -mmcu=atmega2560
ELFFLAGS=-g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
LOADFLAGS=-p m2560 -c stk500v2 -P /dev/cu.usbmodem1421 -b 115200 -U flash:w:rtos.hex:i -D

all: compile elf hex load

compile: rtos/cswitch.S rtos/os.c remote_station/remote.c rtos/queue.c roomba/roomba.c uart/uart.c
	$(CC) $(FLAGS) rtos/cswitch.S rtos/os.c remote_station/remote.c rtos/queue.c roomba/roomba.c uart/uart.c

elf: cswitch.o os.o remote.o queue.o roomba.o uart.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o remote.o queue.o roomba.o uart.o

hex: rtos.elf
	$(COPY) $(HEXFLAGS) rtos.elf rtos.hex

load: rtos.hex
	$(LOAD) $(LOADFLAGS)

clean:
	rm -f *.elf *.o *.hex
