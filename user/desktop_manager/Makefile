# user/desktop_manager/Makefile

CXX = g++
CXXFLAGS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c
LDFLAGS = -T linker.ld

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all clean

all: desktop_manager.elf

desktop_manager.elf: $(OBJECTS)
	@ld $(LDFLAGS) -o desktop_manager.elf $(OBJECTS)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f desktop_manager.elf $(OBJECTS)
