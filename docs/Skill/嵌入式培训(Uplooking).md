> 2014-09-27

索引
======

- [Linux基础(RHEL).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2FLinux%25E5%259F%25BA%25E7%25A1%2580%2528RHEL%2529.md) <span>2014-09-27</span>

- [C语言(Uplooking).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2FC%25E8%25AF%25AD%25E8%25A8%2580%2528Uplooking%2529.md) <span>2014-11-01</span>


<style type="text/css">li span{font-size:0.95em;color:#555;font-family:'sans-serif';padding-left:5px;}</style>




cache
=========

<!--

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define COLS 10
#define ROWS 20

typedef enum{
  FALSE,
  TRUE
} BOOL;

typedef enum{
  EMPTY,
  FILLED
} POS_STAT;

typedef struct{
    int row;
  int col;
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


BLOCK init_block(char type);
int find_type_idx(char type);
BOOL move(BLOCK *b, POS direction);
BOOL is_can_move(BLOCK *b, POS direction);


void print_block(const BLOCK *b);



void test_print_block(){
  BLOCK b = init_block('T');
  print_block(&b);
}


void test_is_can_move(){
  POS left = {0, -1};
  POS right = {0, 1};
  POS down = {1, 0};
  char *types = "TOILJSZ";
  int i = 0;
  BLOCK b;
  while(types[i]!='\0'){
    b = init_block(types[i]);
    assert(move(&b, left) == FALSE);
    // printf("left\n");
    // print_block(&b);

    b = init_block(types[i]);
    assert(move(&b, right) == TRUE);
    printf("right\n");
    print_block(&b);

    b = init_block(types[i]);
    assert(move(&b, down) == TRUE);
    printf("down\n");
    print_block(&b);

    i++;
  }




}

int PANEL[COLS][ROWS] = {0};

int main(int argc, const char* argv[]){
  // test_print_block();
  test_is_can_move();


  return 0;
}



int find_type_idx(char type){
    char *types = "TOILJSZ";
    int i = 0;
    while(types[i]!='\0' && types[i] != type) i++;
    if(types[i] == type) return i;
    return -1;
}

// int find_most_left(BLOCK *b){

// }

// int find_most_right(BLOCK *b){

// }

//void mask_block(){}
BLOCK init_block(char type){
    static POS_SET pos_set_T = {{0,0},{0,1},{0,2},{1,1}};
    static POS_SET pos_set_O = {{0,0},{0,1},{1,0},{1,1}};
    static POS_SET pos_set_I = {{0,0},{1,0},{2,0},{3,0}};
    static POS_SET pos_set_L = {{0,0},{1,0},{2,0},{2,1}};
    static POS_SET pos_set_J = {{0,1},{1,1},{2,0},{2,1}};
    static POS_SET pos_set_S = {{0,1},{0,2},{1,0},{1,1}};
    static POS_SET pos_set_Z = {{0,0},{0,1},{1,1},{1,2}};

    POS_SET pos_set_arr[] = {pos_set_T, pos_set_O, pos_set_I, pos_set_L,
        pos_set_J, pos_set_S, pos_set_Z};
    BLOCK b;
    b.type = type;
    b.pos_set = pos_set_arr[find_type_idx(type)];
    return b;
}


BOOL is_can_move(BLOCK *b, POS direction){
  POS *pos_set = (POS*)(&(b->pos_set));
  int i, pos_row, pos_col;
  // print_block(b);
  for(i = 0; i < 4; i++){
    pos_row = pos_set[i].row + direction.row;
    pos_col = pos_set[i].col + direction.col;
    // printf("%d %d; ", pos_row, pos_col);
    if(pos_col >= COLS || pos_col < 0 || pos_row >= ROWS) return FALSE; // out of panel
    if(PANEL[pos_row][pos_col] != EMPTY) return FALSE; // this postion is not empty
  }
  return TRUE;
}

BOOL move(BLOCK *b, POS direction){
  if(!is_can_move(b, direction)) return FALSE;
  POS *pos_set = (POS*)(&(b->pos_set));
  int i;
  for(i = 0; i < 4; i++){
    pos_set[i].row += direction.row;
    pos_set[i].col += direction.col;
  }
  return TRUE;
}



void print_block(const BLOCK *b){
    printf("%c : ", b->type);
    printf("[{%d,%d},{%d,%d},{%d,%d},{%d,%d}]\n",
            b->pos_set.pos0.row, b->pos_set.pos0.col,
            b->pos_set.pos1.row, b->pos_set.pos1.col,
            b->pos_set.pos2.row, b->pos_set.pos2.col,
            b->pos_set.pos3.row, b->pos_set.pos3.col);
}




 -->


<!-- 周鸿真 13381031637 -->





