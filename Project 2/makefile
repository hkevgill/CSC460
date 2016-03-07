CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-c -g -Os -mmcu=atmega2560
ELFFLAGS=-g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
LOADFLAGS=-p m2560 -c stk500v2 -P /dev/cu.usbmodem1411 -b 115200 -U flash:w:main.hex:i -D

all: compile elf hex load

compile: cswitch.S os.c LED_Test.c
	$(CC) $(FLAGS) os.c cswitch.S LED_Test.c

elf: cswitch.o os.o LED_Test.o
	$(CC) $(ELFFLAGS) main.elf os.o LED_Test.o cswitch.o

hex: main.elf
	$(COPY) $(HEXFLAGS) main.elf main.hex

load: main.hex
	$(LOAD) $(LOADFLAGS)

clean:
	rm -f *.elf *.o *.hex