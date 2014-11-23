#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//define
typedef enum{
    FALSE,
    TRUE
} BOOL;

typedef struct{
    char *name;
    char *passwd;
} USER;

typedef struct{
    USER *users;
    int size;
    int alloc_size;
} USER_LIST;

#define ALLOC_LEN 8

//logic
BOOL registe_user(USER_LIST *list, char *name, char *passwd){
    int i;
    for(i = 0; i < list->size; i++)
        if(strcmp(list->users[i].name, name) == 0) return FALSE;

    if(list->size == list->alloc_size){//增长策略
        if(list->alloc_size == 0)
            list->alloc_size = ALLOC_LEN;
        else
            list->alloc_size *= 2;
        list->users = realloc(list->users, sizeof(USER) * list->alloc_size);
    }

    list->users[list->size].name = strdup(name);
    list->users[list->size].passwd = strdup(passwd);

    list->size++;
    return TRUE;
}

BOOL validate_user(USER_LIST *list, char *name, char *passwd){
    int i;
    for(i = 0; i < list->size; i++)
        if(strcmp(list->users[i].name, name) == 0 && 
           strcmp(list->users[i].passwd, passwd) ==0) return TRUE;
    return FALSE;
}

void free_list(USER_LIST *list){
    int i;
    for(i = 0; i < list->size; i++){
        free(list->users[i].name);
        free(list->users[i].passwd);
    }
    free(list->users);
}

// ui
void resigte_user_ui(USER_LIST *list){
    char name[10], passwd[10];
    printf("用户名：");
    scanf("%s", name);
    printf("密码：");
    scanf("%s", passwd);
    if(registe_user(list,name,passwd))
        printf("注册成功\n");
    else
        printf("用户名重复\n");  
}

void validate_user_ui(USER_LIST *list){
    char name[10], passwd[10];    
    printf("用户名：");
    scanf("%s", name);
    printf("密码：");
    scanf("%s", passwd);
    if(validate_user(list,name,passwd))
        printf("登录成功\n");
    else
        printf("登录失败\n");                            
}

void list_user_ui(USER_LIST *list){
    int i;
    for(i = 0; i < list->size; i++)
        printf("[%2d] name: %s; passwd: %s\n",
              i, list->users[i].name, list->users[i].passwd);
}


typedef struct{
    char name[10];
    float price;
} STATION;

typedef struct{
    char no[10];
    STATION *stations;
    int size;
    int alloc_size;
} TRAIN;

typedef struct{
    TRAIN *trans;
    int size;
} TRAIN_LIST;


TRAIN* init_train(char *no){
    TRAIN *train = malloc(sizeof(TRAIN));
    strcpy(train->no, no);
    train->size = 0;
    train->alloc_size = 0;    
    train->stations = NULL;
    return train;
}

void add_station(TRAIN* train, char *name, float price){
    if(train->size == train->alloc_size){//增长策略
        if(train->alloc_size == 0)
            train->alloc_size = ALLOC_LEN;
        else
            train->alloc_size *= 2;
        train->stations = realloc(train->stations, sizeof(STATION) * train->alloc_size);
    }

    strcpy(train->stations[train->size].name, name);
    train->stations[train->size].price = price;

    train->size++;
}


// app
int main(){
    USER_LIST list = {0};
    int q,r;
    while(0){
        printf("1. 初始化系统\n");
        printf("2. 进入系统\n");
        printf("3. 退出系统\n");
        scanf("%d", &q);
        switch(q){
            case 1:
            case 2:{
                printf(" 1. 用户注册\n");
                printf(" 2. 用户登录\n");
                printf(" 3. 显示所有用户\n");
                scanf("%d", &r);
                switch(r){
                    case 1: resigte_user_ui(&list);break;
                    case 2: validate_user_ui(&list);break;
                    case 3: list_user_ui(&list); break;
                    default: break;
                }
                break;
            }
            default: goto END;
        }
    }

END:
    free_list(&list);
	return 0;
}



