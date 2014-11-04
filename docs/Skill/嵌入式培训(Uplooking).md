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
