#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "train.h"

static void _destroy_user(void *data){
    USER *u = (USER*)data;
    DEBUG_WRITE(("  # _destroy_user: %s[%d] %s[%d]\n",
        u->name,strlen(u->name),u->passwd,strlen(u->passwd)));
    free(u->name);
    free(u->passwd);
}

static BOOL _cmp_user_name(void *exist, void *data){
    USER *e = (USER*)exist;
    USER *d = (USER*)data;
    return strcmp(e->name, d->name) == 0;
}

static BOOL _cmp_user(void *exist, void *data){
    USER *e = (USER*)exist;
    USER *d = (USER*)data;
    return strcmp(e->name, d->name) == 0 && strcmp(e->passwd, d->passwd) == 0;
}

static void _print_user(void *data, int idx){
    USER *u = (USER*)data;
    printf("[%2d] [name] %-10s [passwd] %-10s\n", idx, u->name, u->passwd);
}


BOOL register_user(LIST *users, char *name, char *passwd){
    if(users->header == NULL)
        list_new(users, sizeof(USER), _destroy_user);//need free
    USER nu;
    nu.name = strdup(name);
    nu.passwd = strdup(passwd);
    return list_add_elem_with_unique(users, &nu, _cmp_user_name) != NULL;
}

BOOL validate_user(LIST *users, char *name, char *passwd){
    USER nu;
    nu.name = strdup(name);
    nu.passwd = strdup(passwd);
    return list_find_idx(users, &nu, _cmp_user) != -1;
}

void list_users(LIST *users){
    list_each_elem_do(users, _print_user);
}

void destroy_users(LIST *users){
    list_free(users);
}

//////////////////////////////////////////////
static void _destroy_train(void *data){
    TRAIN *train = (TRAIN*)data;
    DEBUG_WRITE(("  # _destroy_train: %s[%d]\n",
        train->no, train->stations->used_len));
    list_free(train->stations);
    free(train->stations);
}

static BOOL _cmp_train(void *exist, void *data){
    TRAIN *e = (TRAIN*)exist;
    TRAIN *d = (TRAIN*)data;
    return strcmp(e->no, d->no) == 0;
}

static void _print_train(void *data, int idx){
    TRAIN *t = (TRAIN*)data;
    printf("%p\n", t->stations);
    printf("[%2d] [no] %-10s [stations] %d\n", idx, t->no, t->stations->used_len);
}

TRAIN* add_train(LIST *trains, char *no){
    if(trains->header == NULL){
        list_new(trains, sizeof(TRAIN), _destroy_train); //need free
    }

    TRAIN nt = {0};
    strcpy(nt.no, no);
    TRAIN *ret =(TRAIN*)list_add_elem_with_unique(trains, &nt, _cmp_train);
    if(ret != NULL){// init stations
        ret->stations = malloc(sizeof(LIST));
        list_new(ret->stations, sizeof(STATION), NULL); //need't free
        return ret;
    }
    return NULL;
}

void list_trains(LIST *trains){
    list_each_elem_do(trains, _print_train);
}

void destroy_trains(LIST *trains){
    list_free(trains);
}

// BOOL add_station(TRAIN *train, char *name, float price){
//     LIST *stations = train->stations;

//     int i;
//     STATION *s;
//     for(i = 0; i < stations->used_len; i++){
//         s = (STATION*)list_get_elem(stations, i);
//         if(strcmp(s->name, name) == 0) return FALSE;
//     }

//     STATION ns;
//     strcpy(ns.name, name);
//     ns.price = price;
//     list_add_elem(stations, &ns);
//     return TRUE;
// }




