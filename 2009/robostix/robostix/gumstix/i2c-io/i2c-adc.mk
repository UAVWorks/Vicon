#############################################################################
#
#	Makefile for building the i2c-io program
#
#############################################################################

ifeq ($(DEBUG),y)
	CFLAGS += -O -g		# -O is need to expand inlines
else
	CFLAGS += -O2
endif

PWD       := $(shell pwd)

GUMSTIX_BUILDROOT	= $(PWD)/../../../gumstix-buildroot
#GUMSTIX_BUILDROOT	= $(PWD)/../../../br2
BUILD_ARM		= $(GUMSTIX_BUILDROOT)/build_arm_nofpu
CROSS_COMPILE		= $(BUILD_ARM)/staging_dir/bin/arm-linux-
COMMON			= ../Common
SHARED			= ../../Shared

vpath %.c $(COMMON) $(SHARED)

CPPFLAGS += -I . -I $(COMMON) -I $(SHARED)
CFLAGS	 += -Wall

TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale
CC = $(CROSS_COMPILE)gcc

OBJS = \
	i2c-adc.o 		\
	i2c-api.o		\
	i2c-io-api.o	\
	Crc8.o			\
	DumpMem.o		\
	Log.o

all: i2c-adc

i2c-adc: $(OBJS)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions i2c-adc

depend .depend dep:
	@echo "Creating dependencies ..."
	$(CC) $(CFLAGS) $(CPPFLAGS) -M *.c $(COMMON)/*.c > .depend

FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CC) $(CPPFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI

%.pp : %.c FORCE
	$(PREPROCESS.c) $< > $@

ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif

