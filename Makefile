# Name: Makefile
# Author: Jon Nall
# Copyright: (c) 2009 Jon Nall, All rights reserved.
# License: LGPL http://www.gnu.org/licenses/lgpl-3.0.txt

#############################
# Begin configuration section
#
DEVICE     = attiny13
CLOCK      = 4800000
ONEWIRE_PORTIN = PINB
ONEWIRE_PORT = PORTB
ONEWIRE_IO_CTL = DDRB
ONEWIRE_PIN = 0
#
# End configuration section
#############################

COMPILE = avr-gcc -std=c99 -g -Wall -Os \
	-DF_CPU=$(CLOCK) \
	-mmcu=$(DEVICE) \
	-DONEWIRE_PORTIN=$(ONEWIRE_PORTIN) \
	-DONEWIRE_PORT=$(ONEWIRE_PORT) \
	-DONEWIRE_IO_CTL=$(ONEWIRE_IO_CTL) \
	-DONEWIRE_PIN=$(ONEWIRE_PIN)
AR = avr-ar

OBJECTS    = DS18B20.o OneWire.o

# symbolic targets:
all: maxim1wire.a

maxim1wire.a: $(OBJECTS)
	$(AR) -r -v $@ $(OBJECTS)

.c.o:
	$(COMPILE) -c $< -o $@

clean:
	rm -f maxim1wire.a $(OBJECTS)