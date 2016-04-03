CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-c -g -Os -mmcu=atmega2560
ELFFLAGS=-g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
LOADFLAGS=-p m2560 -c stk500v2 -P /dev/cu.usbmodem1411 -b 115200 -U flash:w:rtos.hex:i -D

remote_station: compile_remote elf_remote hex load

base_station: compile_base elf_base hex load

compile_remote: rtos/cswitch.S rtos/os.c remote_station/remote.c rtos/queue.c roomba/roomba.c uart/uart.c lcd/lcd.c
	$(CC) $(FLAGS) rtos/cswitch.S rtos/os.c remote_station/remote.c rtos/queue.c roomba/roomba.c uart/uart.c lcd/lcd.c

elf_remote: cswitch.o os.o remote.o queue.o roomba.o uart.o lcd.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o remote.o queue.o roomba.o uart.o lcd.o

compile_base: rtos/cswitch.S rtos/os.c base_station/base.c rtos/queue.c roomba/roomba.c uart/uart.c lcd/lcd.c
	$(CC) $(FLAGS) rtos/cswitch.S rtos/os.c base_station/base.c rtos/queue.c roomba/roomba.c uart/uart.c lcd/lcd.c

elf_base: cswitch.o os.o base.o queue.o roomba.o uart.o lcd.o
	$(CC) $(ELFFLAGS) rtos.elf cswitch.o os.o base.o queue.o roomba.o uart.o lcd.o

hex: rtos.elf
	$(COPY) $(HEXFLAGS) rtos.elf rtos.hex

load: rtos.hex
	$(LOAD) $(LOADFLAGS)

clean:
	rm -f *.elf *.o *.hex
