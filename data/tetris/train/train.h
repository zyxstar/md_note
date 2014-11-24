#include "utils.h"
#include "list.h"

typedef struct{
    char *name; //char pointer, need free
    char *passwd;
} USER;

BOOL register_user(LIST *users, char *name, char *passwd);
BOOL validate_user(LIST *users, char *name, char *passwd);
void destroy_users(LIST *users);
void list_users(LIST *users);


typedef struct{
    char name[20]; //char array, need't free
    float price;
} STATION;

typedef struct{
    char no[10];
    LIST *stations;
} TRAIN;


TRAIN* add_train(LIST *trains, char *no);
void destroy_trains(LIST *trains);
void list_trains(LIST *trains);

BOOL add_station(TRAIN *train, char *name, float price);

