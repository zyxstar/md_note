#include "tetris.h"

static int _get_type_idx(char type){
    char *types = "TOILJSZ";
    int i = 0;
    while(types[i]!='\0' && types[i] != type) i++;
    if(types[i] == type) return i;
    return -1;
}

static BOOL _check_hit(const PANEL panel, BLOCK *b){
    POS *pos_set = (POS*)(&(b->pos_set));
    int i, pos_y, pos_x;
    for(i = 0; i < 4; i++){
        pos_x = pos_set[i].x;
        pos_y = pos_set[i].y;
        // out of panel
        if(pos_x >= COLS || pos_x < 0 || pos_y >= ROWS) return FALSE;
        // this postion is not empty
        if(panel[pos_y][pos_x] != EMPTY) return FALSE;
    }
    return TRUE;
}

// static BOOL _is_can_move(const PANEL panel, const BLOCK *b, POS direction){
//     POS *pos_set = (POS*)(&(b->pos_set));
//     int i, pos_y, pos_x;
//     for(i = 0; i < 4; i++){
//         pos_x = pos_set[i].x + direction.x;
//         pos_y = pos_set[i].y + direction.y;
//         // out of panel
//         if(pos_x >= COLS || pos_x < 0 || pos_y >= ROWS) return FALSE;
//         // this postion is not empty
//         if(panel[pos_y][pos_x] != EMPTY) return FALSE;
//     }
//     return TRUE;
// }

static BOOL _move(PANEL panel, BLOCK *b, POS direction){
    BLOCK backup_b = *b;
    POS *pos_set = (POS*)(&(b->pos_set));
    int i;
    for(i = 0; i < 4; i++){
        pos_set[i].x += direction.x;
        pos_set[i].y += direction.y;
    }
    if(!_check_hit((const PANEL)panel, b)){
        *b = backup_b;
        return FALSE;
    }
    return TRUE;
}

static POS_SET _get_truned_offset(char type){
    static POS_SET offset_T = {{0,0},{0,0},{0,0},{0,0}};
    static POS_SET offset_O = {{0,0},{0,0},{0,0},{0,0}};
    static POS_SET offset_I = {{1,-1},{2,1},{0,0},{0,0}};
    static POS_SET offset_L = {{0,1},{1,0},{0,-1},{-1,0}};
    static POS_SET offset_J = {{-1,0},{0,-1},{1,0},{0,1}};
    static POS_SET offset_S = {{0,1},{2,0},{0,0},{0,0}};
    static POS_SET offset_Z = {{0,0},{1,0},{0,0},{0,0}};

    POS_SET offset_arr[] = {offset_T, offset_O, offset_I,
        offset_L, offset_J, offset_S, offset_Z};
    return offset_arr[_get_type_idx(type)];
}

static void _trun_block_T(BLOCK *b){
    POS base;

}
static void _trun_block_I(BLOCK *b){}
static void _trun_block_L(BLOCK *b){}
static void _trun_block_J(BLOCK *b){}
static void _trun_block_S(BLOCK *b){}
static void _trun_block_Z(BLOCK *b){}

static void _trun_block(BLOCK *b){
    switch(b->type){
        case 'T': _trun_block_T(b);return;
        case 'O': return;
        case 'I': _trun_block_I(b);return;
        case 'L': _trun_block_L(b);return;
        case 'J': _trun_block_J(b);return;
        case 'S': _trun_block_S(b);return;
        case 'Z': _trun_block_Z(b);return;
        default: assert(FALSE);
    }
}

static void _panel_be_filled(PANEL panel, BLOCK *b){
    POS *pos_set = (POS*)(&(b->pos_set));
    int i;
    for(i = 0; i < 4; i++)
        panel[pos_set[i].y][pos_set[i].x] = FILLED;
}

static int _check_eliminate(PANEL panel){
    printf("eliminate\n");
}
////////////////////////////////////////


PANEL init_panel(){
    PANEL panel = malloc(ROWS * COLS * sizeof(int));
    assert(panel != NULL);
    int row, col;
    for(row = 0; row < ROWS; row++){
        for(col = 0; col < COLS; col++){
            panel[row][col] = EMPTY;
        }
    }
    return panel;
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
    b.turn_count = 0;
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
BOOL _check_game_over(){}
BOOL move_down(PANEL panel, BLOCK *b){
    POS down = {0, 1};
    //lock
    BOOL ret = _move(panel, b, down);
    if(!ret){ //when down finished
        _panel_be_filled(panel, b);
        _check_eliminate(panel);//score
        if(!_check_game_over())
            _begin_next();
    }
    //unlock
    return ret;
}



BOOL turn(PANEL panel, BLOCK *b){
    BLOCK backup_b = *b;
    _trun_block(b);
    if(!_check_hit((const PANEL)panel, b)){
        *b = backup_b;
        return FALSE;
    }
    return TRUE;
}



////////////////////////////////////////
void print_panel(const PANEL panel){
    int row, col;
    printf("PANEL: \n");
    for(row = 0; row < ROWS; row++){
        printf("     ");
        for(col = 0; col < COLS; col++){
            printf("%c", panel[row][col] == FILLED ? '#' : '_');
        }
        printf(" : %2d\n", row);
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
        printf("%2d : ", row);
        for(col = 0; col < COLS; col++){
            if((b->pos_set.pos0.x == col && b->pos_set.pos0.y == row) ||
               (b->pos_set.pos1.x == col && b->pos_set.pos1.y == row) ||
               (b->pos_set.pos2.x == col && b->pos_set.pos2.y == row) ||
               (b->pos_set.pos3.x == col && b->pos_set.pos3.y == row))
                printf("#");
            else
                printf("_");
        }
        printf("\n");
    }
}


// int (*pnl)[COLS] = (int(*)[COLS])panel;
