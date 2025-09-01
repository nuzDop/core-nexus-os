# user/Makefile

CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -c
LDFLAGS = -T linker.ld -m elf_i386

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all clean

all: init.elf

init.elf: $(OBJECTS)
	@ld $(LDFLAGS) -o init.elf $(OBJECTS)

%.o: %.c
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f init.elf $(OBJECTS)
