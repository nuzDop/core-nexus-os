#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
    virtual ~Renderer() {}

    // Renders an HTML string to the window's buffer
    virtual void renderHTML(const char* html) = 0;
};

#endif
