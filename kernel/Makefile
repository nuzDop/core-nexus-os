CC = gcc
AS = nasm
LD = ld

CFLAGS = -Wall -Wextra -std=c11 -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -I./src -I./src/include
ASFLAGS = -f elf64
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

SRCDIR = src
OBJDIR = obj

# Find all C and assembly files in the source directory and subdirectories
SOURCES_C := $(shell find $(SRCDIR) -name '*.c')
SOURCES_ASM := $(shell find $(SRCDIR) -name '*.asm')

# Generate object file paths by replacing src/ with obj/ and .c/.asm with .o
OBJECTS_C := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES_C))
OBJECTS_ASM := $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(SOURCES_ASM))

OBJECTS = $(OBJECTS_C) $(OBJECTS_ASM)

.PHONY: all clean

all: kernel.bin

kernel.bin: $(OBJECTS)
	@mkdir -p bin
	$(LD) $(LDFLAGS) -o bin/kernel.bin $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf $(OBJDIR) bin
