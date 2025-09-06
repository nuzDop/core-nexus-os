#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

static void print_number(int n, int base) {
    if (n < 0) {
        putchar('-');
        n = -n;
    }
    if (n / base) {
        print_number(n / base, base);
    }
    putchar("0123456789abcdef"[n % base]);
}

int printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (int i = 0; fmt[i]; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 'c':
                    putchar(va_arg(ap, int));
                    break;
                case 's':
                    for (char* s = va_arg(ap, char*); *s; s++) {
                        putchar(*s);
                    }
                    break;
                case 'd':
                    print_number(va_arg(ap, int), 10);
                    break;
                case 'x':
                    print_number(va_arg(ap, int), 16);
                    break;
                case '%':
                    putchar('%');
                    break;
            }
        } else {
            putchar(fmt[i]);
        }
    }

    va_end(ap);
    return 0; // Simplified return value
}
