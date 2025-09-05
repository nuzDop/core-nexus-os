#include <stdint.h>

// This global variable is expected by the compiler when -fstack-protector is used.
// It holds the "canary" value that is checked before a function returns.
// We declare it here and define its initial value in the linker script.
uintptr_t __stack_chk_guard = 0;

// This function is called by the compiler-generated code when a stack
// canary has been corrupted. It should not return.
void __stack_chk_fail(void) {
    // A real implementation would trigger a kernel panic.
    // For now, we'll enter an infinite loop.
    // print("STACK SMASHING DETECTED! System halted.\n");
    for (;;);
}
