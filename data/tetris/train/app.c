#include <stdio.h>
#include <stdlib.h>
#include "train.h"


// ui
void register_user_ui(LIST *list){
    char name[10], passwd[10];
    printf("username: ");
    scanf("%s", name);
    printf("password: ");
    scanf("%s", passwd);
    if(register_user(list,name,passwd))
        printf("resigter successful\n");
    else
        printf("resigter failed: user exist\n");
}

void validate_user_ui(LIST *list){
    char name[10], passwd[10];
    printf("username: ");
    scanf("%s", name);
    printf("password: ");
    scanf("%s", passwd);
    if(validate_user(list,name,passwd))
        printf("login successful\n");
    else
        printf("login failed\n");
}


// app
int main(){
    LIST user_list = {0};
    int q,r;
    while(1){
        printf("1. manage users\n");
        printf("2. manage trains\n");
        printf("3. quit\n");
        scanf("%d", &q);
        switch(q){
            case 1:{
                printf(" 1. register\n");
                printf(" 2. login\n");
                printf(" 3. list users\n");
                scanf("%d", &r);
                switch(r){
                    case 1: register_user_ui(&user_list);break;
                    case 2: validate_user_ui(&user_list);break;
                    case 3: list_users(&user_list); break;
                    default: break;
                }
                break;
            }
            default: goto END;
        }
    }

END:
    destroy_users(&user_list);
    return 0;
}



// gcc -DDEBUG utils.c list.c train.c app.c -o app && ./app
