#
# Makefile for the LimitlessOS PolyCore Kernel (x86_64)
#

# --- Toolchain Definition ---
# Use the PREFIX you confirmed with the 'which' command.
PREFIX = /usr/local/x86_64-elf-gcc/bin/
TARGET = x86_64-elf-
CC = $(PREFIX)$(TARGET)gcc
CXX = $(PREFIX)$(TARGET)g++
LD = $(PREFIX)$(TARGET)ld
OBJCOPY = $(PREFIX)$(TARGET)objcopy
NASM = nasm

# --- Compiler and Linker Flags ---
# CFLAGS for C files. -mcmodel=kernel places the kernel in the upper 2GB of the address space.
# -mno-red-zone is crucial for kernel code to prevent unexpected stack usage.
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -c -I./ \
         -mcmodel=kernel -mno-red-zone -fno-stack-protector

# CXXFLAGS for C++ files. Inherits CFLAGS and adds C++ specific flags.
CXXFLAGS = $(CFLAGS) -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions

# NASMFLAGS for assembly files. The output format is 64-bit ELF.
NASMFLAGS = -f elf64

# LDFLAGS for the linker. Specifies the linker script and target format.
LDFLAGS = -T linker.ld -nostdlib --oformat=elf64-x86-64

# --- Source Files ---
# Automatically find all source files in the current directory and subdirectories.
ASM_SOURCES = $(shell find . -name "*.asm")
C_SOURCES = $(shell find . -name "*.c")
CPP_SOURCES = $(shell find . -name "*.cpp")

# --- User Programs to Embed ---
# This list will be embedded as binary blobs inside the final kernel file.
USER_PROGRAMS = ../user/shell/shell.elf \
                ../user/terminal/terminal.elf \
                ../user/file_manager/file_manager.elf \
                ../user/installer/installer.elf \
                ../user/sh/sh.elf \
                ../user/mkfs/mkfs.elf \
                ../user/security_center/security_center.elf

# --- Object File Generation ---
# Generate lists of object files from the source file lists.
ASM_OBJECTS = $(patsubst %.asm, %.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))
USER_OBJECTS = $(patsubst %.elf, %.o, $(USER_PROGRAMS))

# --- Main Build Target ---
all: kernel.bin

# The final kernel binary depends on all kernel object files and embedded user programs.
kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "  LD kernel.bin"
	@$(LD) $(LDFLAGS) -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "Kernel built successfully as kernel.bin"

# --- Rule to Embed User Programs ---
# Converts a user-space ELF file into a linkable object file.
$(USER_OBJECTS): %.o: %.elf
	@echo "  OBJCOPY $<"
	@$(OBJCOPY) -I elf64-x86-64 -O binary $< $@

# --- Compilation and Assembly Rules ---

# Rule to assemble .asm files with NASM
%.o: %.asm
	@echo "  NASM $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

# Rule to compile .c files
%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@

# Rule to compile .cpp files
%.o: %.cpp
	@echo "  CXX $<"
	@$(CXX) $(CXXFLAGS) $< -o $@


# --- Housekeeping ---
.PHONY: all clean

# Clean up all build artifacts.
clean:
	@rm -f kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "Cleaned all kernel build artifacts."
