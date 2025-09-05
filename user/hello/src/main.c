#define SYS_PRINT 1
#define SYS_YIELD 0

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

void _start() {
    syscall(SYS_PRINT, (int)"Hello from a separate application!\n", 0, 0, 0, 0);
    while(1) {
        syscall(SYS_YIELD, 0, 0, 0, 0, 0);
    }
}
