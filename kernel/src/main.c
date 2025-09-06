#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <acpi/acpi.h>
#include <drivers/pci.h>
#include <proc/task.h>
#include <gui/compositor.h>
#include <lib/print.h>

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

static volatile LIMINE_BASE_REVISION(1);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void) {
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (size_t i = 0; i < 100; i++) {
        uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }
    
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    acpi_init();
    pci_init();
    task_init();
    compositor_init();
    
    kprintf("Hello, world!\n");

    hcf();
}
