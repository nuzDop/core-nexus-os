CC = gcc
# Note: No -ffreestanding. We are linking against our libc.
CFLAGS = -m32 -O2 -Wall -Wextra -c
LDFLAGS = -m elf_i386 -dynamic-linker /lib/ld-limitless.so.1 -L../../libc -lc

SOURCES = src/main.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all clean

all: linux_test.elf

linux_test.elf: $(OBJECTS)
	@$(CC) $(LDFLAGS) -o linux_test.elf $(OBJECTS)

%.o: %.c
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f linux_test.elf $(OBJECTS)
