# Makefile for the LimitlessOS PolyCore Kernel

# Define the cross-compiler tools
PREFIX = /usr/local/cross/bin/
TARGET = x86_64-elf-
AS = $(PREFIX)$(TARGET)as
CC = $(PREFIX)$(TARGET)gcc
CXX = $(PREFIX)$(TARGET)g++
LD = $(PREFIX)$(TARGET)ld

# Compiler and linker flags
ASFLAGS = --32 # Assemble in 32-bit mode
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -c -I./
CXXFLAGS = -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -I./include -c
LDFLAGS = -T linker.ld

# Define source files
ASM_SOURCES = $(shell find . -name "*.asm")
C_SOURCES = $(shell find . -name "*.c")
CPP_SOURCES = $(shell find . -name "*.cpp")

# Define object files
ASM_OBJECTS = $(patsubst %.asm, %.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

# Target: The final kernel binary
kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS)
	@$(LD) $(LDFLAGS) -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS)
	@echo "Kernel built as kernel.bin"

# Rule to assemble .asm files
%.o: %.asm
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Rule to compile .c files
%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@
	
# Rule to compile .cpp files
%.o: %.cpp
	@echo "  CXX $<"
	@$(CXX) $(CXXFLAGS) $< -o $@


# Clean up build artifacts
clean:
	@rm -f kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS)
	@echo "Cleaned build artifacts."

.PHONY: all clean
