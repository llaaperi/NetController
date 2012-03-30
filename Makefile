###############################################################################
# Makefile for the project NetController
###############################################################################

## Directories
OBJDIR = obj
SRCDIR = src
BINDIR = bin

## General Flags
PROJECT = NetController
MCU = atmega328p
CC = avr-gcc

CFLAGS = -mmcu=$(MCU) -Wall -Os -g -std=gnu99 -c
LFLAGS = -mmcu=$(MCU) -g


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


OBJECTS = NetController.o arp.o ds1820.o enc28j60.o eth.o http.o icmp.o ip.o keypad.o lcd.o relay.o tcp.o twi.o 
COBJECTS = $(addprefix $(OBJDIR)/, $(OBJECTS))
TARGETS = NetController.elf NetController.hex NetController.eep NetController.lss
CTARGETS = $(addprefix $(BINDIR)/, $(TARGETS))

all: buildrepo $(CTARGETS) size

buildrepo:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

$(BINDIR)/%.elf: $(COBJECTS)
	$(CC) $(LFLAGS) $(COBJECTS) -o $(BINDIR)/NetController.elf

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@


$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

$(BINDIR)/%.eep: $(BINDIR)/%.elf
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

$(BINDIR)/%.lss: $(BINDIR)/%.elf
	avr-objdump -h -S $< > $@

size: $(BINDIR)/NetController.elf
	@echo
	@avr-size $(BINDIR)/NetController.elf

clean:
	rm -f $(COBJECTS)
	rmdir $(OBJDIR)
	rm -f $(CTARGETS)
	rmdir $(BINDIR)