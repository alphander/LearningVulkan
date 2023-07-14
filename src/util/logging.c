#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void logging_print(const char* format, ...) 
{   
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void logging_finish(const char* format, ...) 
{   
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_SUCCESS);
}

void logging_error(const char* format, ...) 
{   
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}