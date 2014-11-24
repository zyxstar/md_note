#ifndef CONSOLE_H_INCLUEDE
#define CONSOLE_H_INCLUEDE

#ifdef DEBUG
#define DEBUG_WRITE(arg) debug_write arg
#else
#define DEBUG_WRITE(arg)
#endif

void debug_write(char *format, ...);



#define SNAP_INT(arg) fprintf(stderr, "[" #arg "] %d\n", arg)
#define SNAP_CHAR(arg) fprintf(stderr, "[" #arg "] %c\n", arg)
#define SNAP_FLOAT(arg) fprintf(stderr, "[" #arg "] %f\n", arg)
#define SNAP_STRING(arg) fprintf(stderr, "[" #arg "] %s\n", arg)


typedef enum{
    FALSE,
    TRUE
} BOOL;

const char* human_bool(BOOL val);

#endif /*CONSOLE_H_INCLUEDE*/
