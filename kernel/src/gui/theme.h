#ifndef THEME_H
#define THEME_H

// Primary Palette: Very dark blue/black background with light gray/white text.

#define COLOR_BACKGROUND          0x080E1C // hsl(222.2 84% 4.9%)
#define COLOR_FOREGROUND          0xF9FAFB // hsl(210 40% 98%)

#define COLOR_TASKBAR             0x101827 // hsl(222.2 47.4% 11.2%)
#define COLOR_BORDER              0x202A3A // hsl(217.2 32.6% 17.5%)

#define COLOR_WINDOW_TITLEBAR     COLOR_TASKBAR
#define COLOR_WINDOW_TITLE_TEXT   COLOR_FOREGROUND
#define COLOR_WINDOW_BODY         COLOR_BACKGROUND
#define COLOR_WINDOW_BORDER_ACTIVE  0xF9FAFB // Use bright foreground for active window
#define COLOR_WINDOW_BORDER_INACTIVE COLOR_BORDER

#define COLOR_BUTTON_BG           COLOR_BORDER
#define COLOR_BUTTON_FG           COLOR_FOREGROUND

#endif
