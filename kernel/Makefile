# Makefile for the LimitlessOS PolyCore Kernel

# Define compilers and assemblers
AS = nasm
CC = gcc
LD = ld

# Compiler and linker flags
ASFLAGS = -f elf64
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -c -I./
LDFLAGS = -T linker.ld

# Define source files
ASM_SOURCES = $(shell find . -name "*.asm")
C_SOURCES = $(shell find . -name "*.c")

# Define object files
ASM_OBJECTS = $(patsubst %.asm, %.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

# Target: The final kernel binary
kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS)
	@$(LD) $(LDFLAGS) -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)
	@echo "Kernel built as kernel.bin"

# Rule to assemble .asm files
%.o: %.asm
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Rule to compile .c files
%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@

# Clean up build artifacts
clean:
	@rm -f kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)
	@echo "Cleaned build artifacts."

.PHONY: all clean
