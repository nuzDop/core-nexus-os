#
# Makefile for the LimitlessOS PolyCore Kernel (x86_64)
#

# --- Toolchain Definition ---
PREFIX = /usr/local/x86_64-elf-gcc/bin/
TARGET = x86_64-elf-
CC = $(PREFIX)$(TARGET)gcc
CXX = $(PREFIX)$(TARGET)g++
LD = $(PREFIX)$(TARGET)ld
OBJCOPY = $(PREFIX)$(TARGET)objcopy
NASM = nasm

# --- Compiler and Linker Flags ---
# -I src is the key fix: It adds the 'src' directory to the include path.
# -mcmodel=kernel, -mno-red-zone are standard for 64-bit kernel development.
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -c -I./ -I src/ \
         -mcmodel=kernel -mno-red-zone -fno-stack-protector

CXXFLAGS = $(CFLAGS) -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions

NASMFLAGS = -f elf64

LDFLAGS = -T linker.ld -nostdlib --oformat=elf64-x86-64

# --- Source Files ---
ASM_SOURCES = $(shell find arch/ -name "*.asm")
C_SOURCES = $(shell find src/ -name "*.c") $(shell find arch/ -name "*.c")
CPP_SOURCES = $(shell find src/ -name "*.cpp")

# --- User Programs to Embed ---
USER_PROGRAMS = ../user/shell/shell.elf \
                ../user/terminal/terminal.elf \
                ../user/file_manager/file_manager.elf \
                ../user/installer/installer.elf \
                ../user/sh/sh.elf \
                ../user/mkfs/mkfs.elf \
                ../user/security_center/security_center.elf

# --- Object File Generation ---
ASM_OBJECTS = $(patsubst %.asm, %.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))
USER_OBJECTS = $(patsubst %.elf, %.o, $(USER_PROGRAMS))


# --- Main Build Target ---
all: kernel.bin

kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "  LD kernel.bin"
	@$(LD) $(LDFLAGS) -o kernel.bin $^
	@echo "Kernel built successfully as kernel.bin"

# --- Rule to Embed User Programs ---
$(USER_OBJECTS): %.o: %.elf
	@echo "  OBJCOPY $<"
	@$(OBJCOPY) -I elf64-x86-64 -O binary $< $@

# --- Compilation and Assembly Rules ---
%.o: %.asm
	@echo "  NASM $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@

%.o: %.cpp
	@echo "  CXX $<"
	@$(CXX) $(CXXFLAGS) $< -o $@


# --- Housekeeping ---
.PHONY: all clean

clean:
	@rm -f kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS) $(USER_OBJECTS)
	@echo "Cleaned all kernel build artifacts."
