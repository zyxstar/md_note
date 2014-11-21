#include "tetris.h"
#include <unistd.h>

const char* human_bool(BOOL val){
    if(val) return "TRUE";
    return "FALSE";
}

void each_type_do(void(*callback)(char type)){
    char *types = "TOILJSZ";
    int i = 0;
    while(types[i]!='\0'){
        callback(types[i]);
        i++;
    }
}

////////////////////////////////////////
int GLOBAL_PANEL[COLS][ROWS] = {0};
////////////////////////////////////////

void test_init_panel(){
    PANEL panel = init_panel();
    print_panel((const PANEL)panel);
    free(panel);
}
////////////////////////////////////////
void test_print_block_callback(char type){
    BLOCK b = init_block(type);
    print_block(&b);
}
void test_print_block(){
    each_type_do(test_print_block_callback);
}
////////////////////////////////////////
void _test_move(char type, char *direction_str, BOOL(*func)(PANEL, BLOCK*) ,int times){
    BLOCK b = init_block(type);
    int i;
    for(i = 1; i <= times; i++){
        BOOL ret = func((PANEL)GLOBAL_PANEL, &b);
        printf("%c: %s[%d]: %s\n",type, direction_str, i, human_bool(ret));
        if(ret) print_block(&b);
        else break;
    }
}

void test_move_callback(char type){
    // _test_move(type, "left", move_left, 10);
    // _test_move(type, "right", move_right, 10);
    _test_move(type, "down", move_down, 20);
}

void test_move(){
    each_type_do(test_move_callback);
}
////////////////////////////////////////






int main(int argc, const char* argv[]){
    // test_init_panel();
    // test_print_block();
    // test_move();

    printf ("^[D");
    printf ("^[[0;33mI love you\n");
    printf ("^[[0;33mI love you");
    printf ("^[[2K^[[1G");
    printf ("^[[1;32mI love you\n");
    printf ("^[[0;0m");
    return 0;
}

// gcc test.c tetris.c -o test
