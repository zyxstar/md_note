#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "train.h"



void test_1(){
    LIST user_list = {0};
    printf("reg: %s\n", human_bool(register_user(&user_list,"abc","123")));
    printf("reg: %s\n", human_bool(register_user(&user_list,"def","123")));
    printf("reg: %s\n", human_bool(register_user(&user_list,"abc","123")));

    list_users(&user_list);

    printf("login: %s\n", human_bool(validate_user(&user_list,"abc","123")));
    printf("login: %s\n", human_bool(validate_user(&user_list,"abc","124")));

    destroy_users(&user_list);
}

void test_2(){
    LIST train_list = {0};
    TRAIN *train = add_train(&train_list, "No_01");

    list_trains(&train_list);

    destroy_trains(&train_list);

}


int main(){
    // test_1();
    test_2();


    return 0;
}

// gcc -DDEBUG utils.c list.c train.c test.c -o test && ./test
// gcc utils.c list.c train.c test.c -o test && ./test
