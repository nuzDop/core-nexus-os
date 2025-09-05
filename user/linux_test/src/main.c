#include <stdio.h> // We can use this because we're linking to our libc

int main(int argc, char* argv[]) {
    printf("Hello from a dynamically linked Linux compatible application!\n");
    return 0;
}
