# test makefile

CC = mipsel-none-elf-gcc
OBJCOPY = mipsel-none-elf-objcopy
CFLAGS = -T/usr/local/psxsdk/mipsel-unknown-elf/lib/ldscripts/playstation.x 
#CFLAGS = -T/home/bj/Documents/psxdev/libpsx/psx-exe.ld
LIBDIRS = -L/usr/local/psxchain/mipsel-none-elf/lib
LIBS = 
INCDIRS = -I/usr/local/psxchain/mipsel-none-elf/include

SOURCES = main.c psx.c
TARGET = main

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET).elf $(INCDIRS) $(LIBDIRS) $(LIBS)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	bin2exe $(TARGET).bin $(TARGET).exe

