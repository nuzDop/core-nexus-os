# Makefile for the LimitlessOS PolyCore Kernel

# Define the cross-compiler tools
PREFIX = /usr/local/cross/bin/
TARGET = x86_64-elf-
AS = $(PREFIX)$(TARGET)as
CC = $(PREFIX)$(TARGET)gcc
CXX = $(PREFIX)$(TARGET)g++
LD = $(PREFIX)$(TARGET)ld

# Compiler and linker flags
ASFLAGS = --32
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -c -I./ -fstack-protector-all
CXXFLAGS = -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -I./include -c
LDFLAGS = -T linker.ld --oformat=elf32-i386

# Define source files to be linked, including user programs as binary blobs
USER_PROGRAMS = ../user/shell/shell.elf ../user/hello/hello.elf ../user/net_test/net_test.elf ../user/guitest/guitest.elf
USER_OBJECTS = $(patsubst %.elf, %.o, $(USER_PROGRAMS))

ASM_SOURCES = $(shell find . -name "*.asm")
C_SOURCES = $(shell find . -name "*.c")
CPP_SOURCES = $(shell find . -name "*.cpp")

# Define object files
ASM_OBJECTS = $(patsubst %.asm, %.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

.PHONY: all clean

all: kernel.bin

# Rule to embed user programs into the kernel binary
$(USER_OBJECTS): %.o: %.elf
	@$(PREFIX)$(TARGET)objcopy -I elf32-i386 -O binary $< $@

kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@$(LD) $(LDFLAGS) -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "Kernel built as kernel.bin"

%.o: %.asm
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@
	
%.o: %.cpp
	@echo "  CXX $<"
	@$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "Cleaned kernel build artifacts."
