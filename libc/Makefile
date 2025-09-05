CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fPIC -c
LDFLAGS = -shared -m elf_i386

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all clean

all: libc.so

libc.so: $(OBJECTS)
	@ld $(LDFLAGS) -o libc.so $(OBJECTS)

%.o: %.c
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f libc.so $(OBJECTS)
