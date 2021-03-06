###############################################################################
# Makefile for the project mcu
###############################################################################

## General Flags
PROJECT = mcu
MCU = atmega328p
TARGET = mcu.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=mcu.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"Z:\tlmolloy\Documents\University\AHNS\code\ahns10\trunk\mcu\avr-studio\..\include" 

## Objects that must be built in order to link
OBJECTS = usart.o pwm.o pulsecapture.o modeindicator.o mcu.o mode.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) mcu.hex mcu.eep mcu.lss size

## Compile
usart.o: ../../src/usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm.o: ../../src/pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pulsecapture.o: ../../src/pulsecapture.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

modeindicator.o: ../../src/modeindicator.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mcu.o: ../../src/mcu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mode.o: ../../src/mode.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) mcu.elf dep/* mcu.hex mcu.eep mcu.lss mcu.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

