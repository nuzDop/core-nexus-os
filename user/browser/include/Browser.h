#ifndef BROWSER_H
#define BROWSER_H

#include "Renderer.h"

class Browser {
public:
    Browser();
    ~Browser();

    void run(); // Main application loop
    void navigate(const char* url);

private:
    int window_id;
    Renderer* renderer;
};

#endif
