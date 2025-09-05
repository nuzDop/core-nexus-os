CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -c
LDFLAGS = -T linker.ld -m elf_i386
SOURCES = src/main.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
.PHONY: all clean
all: guitest.elf
guitest.elf: $(OBJECTS)
	@ld $(LDFLAGS) -o guitest.elf $(OBJECTS)
%.o: %.c
	@$(CC) $(CFLAGS) $< -o $@
clean:
	@rm -f guitest.elf $(OBJECTS)
