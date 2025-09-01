#include "Browser.h"

// C-style entry point
extern "C" void _start() {
    // Create the main browser object on the stack
    Browser browser;
    // Run the browser's main loop
    browser.run();
    // Loop forever if the main loop exits
    while(1);
}
