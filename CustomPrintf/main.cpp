#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

const int MAXBUFSIZE = 1024;

int myprintf(const char *format, ...);

int main() {
    myprintf("Moi samyh chestnih pravil;%");
    return 0;
}

int myprintf(const char *format, ...) {
    va_list ap = {};
    char buf[MAXBUFSIZE] = {};
    const char *fragstart = format;
    char* cursor = buf;
    const char *ptr = nullptr;
    char symbol;
    va_start(ap, format);
    while (*format != '\0') {
        ptr = strchr(format, '%');
        if (!ptr)
            ptr = strchr(format, '\0');
        strncpy(cursor, fragstart, (size_t) (ptr - fragstart));
        cursor += ptr - fragstart;
        symbol = *(ptr + 1);
        format = ptr + 2;
        fragstart = format;
        if(*ptr != '\0') {
            if(*ptr != '\0') {
                switch (symbol) {
                    case '%':
                    case '\0':
                        *(cursor++) = '%';
                }
            }
        }
    }
    va_end(ap);
    puts(buf);
    return 0;
}

