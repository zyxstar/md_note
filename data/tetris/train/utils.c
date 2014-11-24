#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"

void debug_write(char *format, ...){
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}


const char* human_bool(BOOL val){
    if(val) return "TRUE";
    return "FALSE";
}
