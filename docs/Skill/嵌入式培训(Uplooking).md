> 2014-09-27

索引
======

- [Linux基础(RHEL).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2FLinux%25E5%259F%25BA%25E7%25A1%2580%2528RHEL%2529.md) <span>2014-09-27</span>

- [C语言(Uplooking).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2FC%25E8%25AF%25AD%25E8%25A8%2580%2528Uplooking%2529.md) <span>2014-11-01</span>


<style type="text/css">li span{font-size:0.95em;color:#555;font-family:'sans-serif';padding-left:5px;}</style>




cache
=========

<!-- run -->

```c
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

int* product(int *arr1, int size1, int *arr2, int size2){
    int *ret = malloc((size1 * size2) * sizeof(int[2]));
    assert(ret != NULL);
    int size = 0, i, j;
    for(i = 0; i < size1; i++){
        for(j = 0; j < size2; j++){
            ret[size++] = arr1[i];
            ret[size++] = arr2[j];
        }
    }
    return ret;
}

int check_rule(int mem1, int mem2){
    if (mem1 == 'a' && mem2 == 'x') return 0;
    if (mem1 == 'c' && mem2 == 'x') return 0;
    if (mem1 == 'c' && mem2 == 'z') return 0;
    return 1;
}

typedef struct{
    int mem;
    int count;
}MemberStatus;

int* filter(int* products, int total_size, int mem_size){
    int(*couples)[2] = (int(*)[2])products;
    int *members = malloc(sizeof(MemberStatus) * mem_size)
    int i;
    for(i = 0; i < total_size; i++){
        if(check_rule(couples[i][0], couples[i][1]))
            members[]

    }

    free(products);
}


int main(){
    int team_1[] = {'a','b','c'};
    int team_2[] = {'x','y','z'};
    int size_1 = sizeof(team_1) / sizeof(int);
    int size_2 = sizeof(team_2) / sizeof(int);

    filter(product(team_1, size_1, team_2, size_2), size_1 * size_2,  size_1 + size_2);




    return 0;
}

```
<!--

利用强制类型转换,将一维数据解释成一个结构体数组

再利用结构体可以被直接赋值

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

typedef struct{
    int mem1;
    int mem2;
} COUPLE;

#define FILTER_ELEM COUPLE

int filter(int(*predicate)(FILTER_ELEM), FILTER_ELEM* arr, int size){ /*reuse*/
    int i, new_size = 0;
    FILTER_ELEM temp;
    for(i = 0; i < size; i++){
        if(predicate(arr[i])){
            temp = arr[i];
            arr[new_size] = temp;
            new_size++;
        }
    }
    return new_size;
}

int* product(int *arr1, int size1, int *arr2, int size2){
    int *ret = malloc((size1 * size2) * sizeof(int[2]));
    assert(ret != NULL);
    int size = 0, i, j;
    for(i = 0; i < size1; i++){
        for(j = 0; j < size2; j++){
            ret[size++] = arr1[i];
            ret[size++] = arr2[j];
        }
    }
    return ret;
}

int check_rule(COUPLE couple){
    if (couple.mem1 == 'a' && couple.mem2 == 'x') return 0;
    if (couple.mem1 == 'c' && couple.mem2 == 'x') return 0;
    if (couple.mem1 == 'c' && couple.mem2 == 'z') return 0;
    return 1;
}

typedef struct{
    int name; //default is 0
    int count;//default is 0
    int competitors[3];
} MEMBERSTATUS;

MEMBERSTATUS* find_by_name(MEMBERSTATUS *status, int size, int name){
    while(size > 0){
        size--;
        if(status[size].name == name)
            return &status[size];
    }
    return NULL;
}

void increase_competitor(int name, int competitor, MEMBERSTATUS *status, int status_size){
    MEMBERSTATUS *s = find_by_name(status, status_size, name);
    if(s == NULL) {
        s = find_by_name(status, status_size, 0);
        s->name = name;
    }
    s->competitors[s->count++] = competitor;
}

void describe_status(MEMBERSTATUS *status, int status_size,
                     COUPLE *couples, int couples_size){
    while(couples_size > 0){
        couples_size--;
        COUPLE c = couples[couples_size];
        increase_competitor(c.mem1, c.mem2, status, status_size);
        increase_competitor(c.mem2, c.mem1, status, status_size);
    }
}

MEMBERSTATUS* find_by_one(MEMBERSTATUS *status, int mems_size){
    while(mems_size > 0){
        mems_size--;
        if(status[mems_size].count == 1)
            return &status[mems_size];
    }
    return NULL;
}

COUPLE* find_couple(COUPLE *couples, int couples_size, int name){
    while(couples_size > 0){
        couples_size--;
        if(couples[couples_size].mem1 == name || couples[couples_size].mem2 == name)
            return &couples[couples_size];
    }
    return NULL;
}

int check_done(MEMBERSTATUS *status, int mems_size){
    while(mems_size > 0){
        mems_size--;
        // printf("%c %d\n", status[mems_size].name, status[mems_size].count);
        if(status[mems_size].count != -1)
            return 0;
    }
    printf("%s\n", "done");
    return 1;
}

void decrease_competitor(MEMBERSTATUS *status, int mems_size, int name, int competitor){
    MEMBERSTATUS *s = find_by_name(status, mems_size, name);

    int i;
    for(i = 0; i < 3; i++){
        if(s->competitors[i] != competitor){
            s->competitors[i] = 0;

            s->count--;
        }
    }
    printf("%c %d\n", s->name, s->count);

}

void work(MEMBERSTATUS *status, int mems_size){
    COUPLE ret[3];
    int i = 0;

    while(i<3){
        MEMBERSTATUS *s = find_by_one(status, mems_size);
        ret[i].mem1 = s->name;
        ret[i].mem2 = s->competitors[0];

    }

    // while(!check_done(status, mems_size)){
    //     MEMBERSTATUS *s = find_by_one(status, mems_size);
    //     s->count = -1;
    //     ret[i].mem1 = s->name;
    //     ret[i].mem2 = s->competitors[0] + s->competitors[1] + s->competitors[2];
    //     printf("%c vs %c\n", ret[i].mem1, ret[i].mem2);

    //     decrease_competitor(status, mems_size, ret[i].mem2, ret[i].mem1);
    //     i++;
    // }
}



int main(){
    int team_1[] = {'a','b','c'};
    int team_2[] = {'x','y','z'};
    int size_1 = sizeof(team_1) / sizeof(int);
    int size_2 = sizeof(team_2) / sizeof(int);

    // int(*couples)[2] = (int(*)[2])product(team_1, size_1, team_2, size_2);

    COUPLE *couples = (COUPLE*)product(team_1, size_1, team_2, size_2);
    int couples_size = filter(check_rule, couples, (size_1 * size_2));

    int mems_size = size_1 + size_2;
    MEMBERSTATUS *status = malloc(mems_size * sizeof(MEMBERSTATUS));

    int i ;
    for(i = 0; i < mems_size ; i++){
        printf("%d : %d\n", status[i].name, status[i].count);
    }


printf("%s\n", "=============");

    describe_status(status, mems_size, couples, couples_size);


    for(i = 0; i < mems_size ; i++){
        printf("%c : %d\n", status[i].name, status[i].count);
    }

printf("%s\n", "=============");
    work(status, mems_size);

    free(couples);
    free(status);

    return 0;
}




 -->
