#include <stdio.h>
#include <stdlib.h>

// For this example, we'll just create a simple window and draw a colored rectangle.
// A real GUI test would be much more involved, testing various widgets and features.
// For now, we will just create a simple window and draw a colored rectangle.

// Define a simple window structure
typedef struct {
    int x, y;
    int width, height;
    char* title;
} Window;

// Function to create a new window
Window* create_window(int x, int y, int width, int height, char* title) {
    Window* win = (Window*)malloc(sizeof(Window));
    if (win) {
        win->x = x;
        win->y = y;
        win->width = width;
        win->height = height;
        win->title = title;
    }
    return win;
}

// Function to draw a window
void draw_window(Window* win) {
    if (!win) return;
    // In a real scenario, this would involve system calls to the window manager
    printf("Drawing window '%s' at (%d, %d) with size %dx%d\n",
           win->title, win->x, win->y, win->width, win->height);
    // Simulate drawing a rectangle
    printf("Drawing a red rectangle inside the window.\n");
}

int main() {
    // Create a new window
    Window* main_window = create_window(100, 100, 400, 300, "GUI Test Window");

    if (!main_window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    // Draw the window
    draw_window(main_window);

    // Clean up
    free(main_window);

    return 0;
}
