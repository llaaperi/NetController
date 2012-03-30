###############################################################################
# Makefile for the project NetController
###############################################################################

## General Flags
PROJECT = NetController
MCU = atmega328p
TARGET = NetController.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-u,vfprintf


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = NetController.o arp.o ds1820.o enc28j60.o eth.o http.o icmp.o ip.o keypad.o lcd.o relay.o tcp.o twi.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) NetController.hex NetController.eep NetController.lss size

## Compile
NetController.o: ../NetController.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

arp.o: ../arp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ds1820.o: ../ds1820.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

enc28j60.o: ../enc28j60.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eth.o: ../eth.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

http.o: ../http.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

icmp.o: ../icmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ip.o: ../ip.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

keypad.o: ../keypad.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: ../lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

relay.o: ../relay.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tcp.o: ../tcp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twi.o: ../twi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -lm -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) NetController.elf dep/* NetController.hex NetController.eep NetController.lss NetController.map

program:
	avrdude -c avrispmkII -P usb -p $(MCU) -e -U flash:w:$(PROJECT).hex

programeeprom:
	avrdude -c avrispmkII -P usb -p $(MCU) -U eeprom:w:$(PROJECT).eep

programfuses:
	avrdude -c avrispmkII -P usb -p $(MCU) -U efuse:w:0x04:m -U hfuse:w:0xD1:m -U lfuse:w:0xE0:m

info:
	avrdude -vv -c avrispmkII -P usb -p $(MCU)


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

