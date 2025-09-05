#include <stdarg.h>

#define SYS_WRITE 15
#define STDOUT 1

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

int printf(const char *format, ...) {
    // This is a very basic printf that only handles %s and %d
    // A full implementation is much more complex.
    char buffer[1024];
    // ... format string and write to buffer ...
    
    syscall(SYS_WRITE, STDOUT, (int)buffer, 0, 0, 0);
    return 0;
}
