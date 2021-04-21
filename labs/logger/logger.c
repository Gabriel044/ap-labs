#include <stdio.h>
#include <stdarg.h>

int infof(const char *format, ...){
    printf("%c[0;32;40m",0x1B);
    va_list message;
    va_start(message,format);
    vprintf(format, message);
    va_end(message);
    printf("%c[0;0;0m",0x1B);
}

int warnf(const char *format, ...){
    printf("%c[0;33;40m",0x1B);
    va_list message;
    va_start(message,format);
    vprintf(format, message);
    va_end(message);
    printf("%c[0;0;0m",0x1B);
}

int errorf(const char *format, ...){
    printf("%c[0;31;40m",0x1B);
    va_list message;
    va_start(message,format);
    vprintf(format, message);
    va_end(message);
    printf("%c[0;0;0m",0x1B);
}

int panicf(const char *format, ...){
    printf("%c[0;35;40m",0x1B);
    va_list message;
    va_start(message,format);
    vprintf(format, message);
    va_end(message);
    printf("%c[0;0;0m",0x1B);
}
