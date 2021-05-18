#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include "logger.h"

// 0 -> stdout
// 1 -> syslog
int log_type;

int initLogger(char *logType) {
    
    if(strcmp(logType, "syslog") == 0){
        printf("Initializing Logger on: %s\n", logType);
        log_type = 1;
    }else if(strcmp("stdout",logType) == 0 || strcmp("",logType) == 0){
        printf("Initializing Logger on: stdout\n");
        log_type = 0;
    }else{
        printf("Please select a valid mode (syslog/stdout).\n");
    }
    return 0;
}

int infof(const char *format, ...){
    printf("%c[0;32;40m",0x1B);
    va_list message;
    va_start(message,format);
    switch (log_type){
        case 1:
            vsyslog(LOG_INFO, format,message);
            closelog();
            break;
        case 0:  
            vprintf(format, message);
            fflush(stdout);
            break;
    }
    va_end(message);
    printf("%c[0;0;0m",0x1B);
    return 0;
}

int warnf(const char *format, ...){
    printf("%c[0;33;40m",0x1B);
    va_list message;
    va_start(message,format);
    switch (log_type){
        case 1:
            vsyslog(LOG_WARNING, format,message);
            closelog();
            break;
        case 0:  
            vprintf(format, message);
            fflush(stdout);
            break;
    }
    va_end(message);
    printf("%c[0;0;0m",0x1B);
    return 0;
}

int errorf(const char *format, ...){
    printf("%c[0;31;40m",0x1B);
    va_list message;
    va_start(message,format);
    switch (log_type){
        case 1:
            vsyslog(LOG_ERR, format,message);
            closelog();
            break;
        case 0:  
            vprintf(format, message);
            fflush(stdout);
            break;
    }
    va_end(message);
    printf("%c[0;0;0m",0x1B);
    return 0;
}

int panicf(const char *format, ...){
    printf("%c[0;35;40m",0x1B);
    va_list message;
    va_start(message,format);
    switch (log_type){
        case 1:
            vsyslog(LOG_EMERG, format, message);
            closelog();
            break;
        case 0:  
            vprintf(format, message);
            fflush(stdout);
            break;
    }
    va_end(message);
    printf("%c[0;0;0m",0x1B);
    return 0;
}