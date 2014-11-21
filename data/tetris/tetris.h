#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ROWS 16
#define COLS 10

typedef enum{
    FALSE,
    TRUE
} BOOL;

typedef enum{
    EMPTY,
    FILLED
} POS_STAT;

typedef struct{
    int x; //index with column
    int y; //index with row
} POS;

typedef struct{
    POS pos0;
    POS pos1;
    POS pos2;
    POS pos3;
} POS_SET;

typedef struct{
    char type;
    char status;
    POS_SET pos_set;
} BLOCK;

typedef int (*PANEL)[COLS];



PANEL init_panel();
BLOCK init_block(char type); //type in "TOILJSZ"

BOOL move_left(PANEL panel, BLOCK *b);
BOOL move_right(PANEL panel, BLOCK *b);
BOOL move_down(PANEL panel, BLOCK *b);

BOOL turn(PANEL panel, BLOCK *b); //turn left

void print_panel(const PANEL panel);
void print_block(const BLOCK *b);
