# user/browser/Makefile

CXX = g++
CXXFLAGS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -I./include -c
LDFLAGS = -T linker.ld

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all clean

all: browser.elf

browser.elf: $(OBJECTS)
	@ld $(LDFLAGS) -o browser.elf $(OBJECTS)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f browser.elf $(OBJECTS)
