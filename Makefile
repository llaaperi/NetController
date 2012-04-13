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


OBJECTS = NetController.o NetControllerUI.o net/arp.o ds1820.o net/enc28j60.o net/eth.o net/http.o net/icmp.o net/ip.o keypad.o lcd.o net/network.o relay.o net/tcp.o time.o twi.o 
COBJECTS = $(addprefix $(OBJDIR)/, $(OBJECTS))
TARGETS = $(PROJECT).elf $(PROJECT).hex $(PROJECT).eep $(PROJECT).lss
CTARGETS = $(addprefix $(BINDIR)/, $(TARGETS))

all: buildrepo $(CTARGETS) size

buildrepo:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/net
	@mkdir -p $(BINDIR)

$(BINDIR)/%.elf: $(COBJECTS)
	$(CC) $(LFLAGS) $(COBJECTS) -o $(BINDIR)/$(PROJECT).elf

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@


$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

$(BINDIR)/%.eep: $(BINDIR)/%.elf
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

$(BINDIR)/%.lss: $(BINDIR)/%.elf
	avr-objdump -h -S $< > $@

size: $(BINDIR)/$(PROJECT).elf
	@echo
	@avr-size $(BINDIR)/$(PROJECT).elf

clean:
	rm -f $(COBJECTS)
	rmdir $(OBJDIR)/net/
	rmdir $(OBJDIR)
	rm -f $(CTARGETS)
	rmdir $(BINDIR)
	
program:
	avrdude -c avrispmkII -P usb -p $(MCU) -e -U flash:w:$(BINDIR)/$(PROJECT).hex

programeeprom:
	avrdude -c avrispmkII -P usb -p $(MCU) -U eeprom:w:$(BINDIR)/$(PROJECT).eep

programfuses:
	avrdude -c avrispmkII -P usb -p $(MCU) -U efuse:w:0x04:m -U hfuse:w:0xD1:m -U lfuse:w:0xE0:m

info:
	avrdude -vv -c avrispmkII -P usb -p $(MCU)