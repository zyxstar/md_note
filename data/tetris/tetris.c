#include "tetris.h"

static int _get_type_idx(char type){
    char *types = "TOILJSZ";
    int i = 0;
    while(types[i]!='\0' && types[i] != type) i++;
    if(types[i] == type) return i;
    return -1;
}

static BOOL _is_can_move(const PANEL panel, const BLOCK *b, POS direction){
    POS *pos_set = (POS*)(&(b->pos_set));
    int i, pos_y, pos_x;
    for(i = 0; i < 4; i++){
        pos_x = pos_set[i].x + direction.x;
        pos_y = pos_set[i].y + direction.y;
        // out of panel
        if(pos_x >= COLS || pos_x < 0 || pos_y >= ROWS) return FALSE;
        // this postion is not empty
        if(panel[pos_y][pos_x] != EMPTY) return FALSE;
    }
    return TRUE;
}

static BOOL _move(PANEL panel, BLOCK *b, POS direction){
    if(!_is_can_move((const PANEL)panel, b, direction)) return FALSE;
    POS *pos_set = (POS*)(&(b->pos_set));
    int i;
    for(i = 0; i < 4; i++){
        pos_set[i].x += direction.x;
        pos_set[i].y += direction.y;
    }
    return TRUE;
}

static void _check_eliminate(PANEL panel, BLOCK *b){
    printf("eliminate\n");
}

static BOOL _is_can_turn(const PANEL panel, const BLOCK *b){
    POS *pos_set = (POS*)(&(b->pos_set));
    int i, pos_y, pos_x;

    switch(b->type){
        case 'T':
        case 'O': return TRUE;
    }

}


////////////////////////////////////////


PANEL init_panel(){
    int *panel = malloc(ROWS * COLS * sizeof(int));
    assert(panel != NULL);
    PANEL pnl = (PANEL)panel;
    int row, col;
    for(row = 0; row < ROWS; row++){
        for(col = 0; col < COLS; col++){
            pnl[row][col] = EMPTY;
        }
    }
    return pnl;
}

BLOCK init_block(char type){
    static POS_SET pos_set_T = {{0,0},{1,0},{2,0},{1,1}};
    static POS_SET pos_set_O = {{0,0},{1,0},{0,1},{1,1}};
    static POS_SET pos_set_I = {{0,0},{0,1},{0,2},{0,3}};
    static POS_SET pos_set_L = {{0,0},{0,1},{0,2},{1,2}};
    static POS_SET pos_set_J = {{1,0},{1,1},{0,2},{1,2}};
    static POS_SET pos_set_S = {{1,0},{2,0},{0,1},{1,1}};
    static POS_SET pos_set_Z = {{0,0},{1,0},{1,1},{2,1}};

    POS_SET pos_set_arr[] = {pos_set_T, pos_set_O, pos_set_I,
        pos_set_L, pos_set_J, pos_set_S, pos_set_Z};
    BLOCK b;
    b.type = type;
    b.pos_set = pos_set_arr[_get_type_idx(type)];
    return b;
}


BOOL move_left(PANEL panel, BLOCK *b){
    POS left = {-1, 0};
    return _move(panel, b, left);
}

BOOL move_right(PANEL panel, BLOCK *b){
    POS right = {1, 0};
    return _move(panel, b, right);
}

void _begin_next(){}
BOOL move_down(PANEL panel, BLOCK *b){
    POS down = {0, 1};
    //lock
    BOOL ret = _move(panel, b, down);
    if(!ret){ //when down finished
        _check_eliminate(panel, b);//score
        _begin_next();
    }
    //unlock
    return ret;
}



BOOL turn(PANEL panel, BLOCK *b){
    if(!_is_can_turn((const PANEL)panel, b)) return FALSE;

}


void print_panel(const PANEL panel){
    int row, col;
    printf("PANEL: \n");
    for(row = 0; row < ROWS; row++){
        for(col = 0; col < COLS; col++){
            printf("%d", panel[row][col]);
        }
        printf("\n");
    }
}


void print_block(const BLOCK *b){
    printf("%c: ", b->type);
    printf("[{%d,%d},{%d,%d},{%d,%d},{%d,%d}]\n",
            b->pos_set.pos0.x, b->pos_set.pos0.y,
            b->pos_set.pos1.x, b->pos_set.pos1.y,
            b->pos_set.pos2.x, b->pos_set.pos2.y,
            b->pos_set.pos3.x, b->pos_set.pos3.y);

    int row, col;
    for(row = 0; row < ROWS; row++){
        for(col = 0; col < COLS; col++){
            if((b->pos_set.pos0.x == col && b->pos_set.pos0.y == row) ||
               (b->pos_set.pos1.x == col && b->pos_set.pos1.y == row) ||
               (b->pos_set.pos2.x == col && b->pos_set.pos2.y == row) ||
               (b->pos_set.pos3.x == col && b->pos_set.pos3.y == row))
                printf("1");
            else
                printf("0");
        }
        printf("\n");
    }
}


// int (*pnl)[COLS] = (int(*)[COLS])panel;
