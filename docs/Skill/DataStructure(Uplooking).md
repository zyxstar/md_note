> 2014-12-13

回到索引
=======
- [嵌入式培训索引(Uplooking).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2F%25E5%25B5%258C%25E5%2585%25A5%25E5%25BC%258F%25E5%259F%25B9%25E8%25AE%25AD%25E7%25B4%25A2%25E5%25BC%2595%2528Uplooking%2529.md)

> 参考
>
> - [课件(赵长江)](https://github.com/zyxstar/md_note/tree/master/data/DataStructure)
> - [讲师代码(刘杰)](https://github.com/zyxstar/exam_c/tree/master/data_structure)


Day01
=========
- `memmove`比`memcpy`更安全的，特别是源地址与目标地址，存在重叠时
- `memset`用于内存数据清空成指定的数据
- `free(NULL)`是可以的，不会报错
- `#if 0\#else\#endif`条件注释，有效规避多行注释不能嵌套问题
- `scanf()`是有返回值的，返回成功匹配与赋值的个数，例如`1 != scanf("%d", &tmp);`可用来判断读取是否失败了
- 对于`int arr[5]`，`arr`与`&arr`是有区别的，它们的值是一样的，但意义不一样，前者代表 __数组元素首地址__，后者为 __数组的地址__，当它们进行指针加减运算时，如`arr+1`代表前进一个`int`类型的长度，而`&arr+1`，则前进`int[5]`类型的长度
- 注意`realloc`重新分配内存时，有可能存在分配失败，当原始指针所指向的部分还是指针时，此时需要将原始指针保留，万一分配失败时，针对原始指针所指向的指针部分进行处理（`free`）
- `NULL == (void*)0`


Day02
=========
- 在泛型线性表中找到所有符合条件的元素，实现于[代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/darr/darr.c)中的`int darr_find_all(DARR head, const void *key, darr_cmp *fun, void ***find_data)`与`static void** _store_find_data(int size, void *data)`
- 读懂`const`，在纸上画出最小指针模型，左侧为指针变量域，右侧为普通变量域，看`const`离谁近，就修饰谁，`char const *p`和`const char *p`修饰的是`*p`，即右侧只读，而`char * const p`，则修饰`p`，左侧只读
- [代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/list/list.c)中`int list_insert(LIST *l, const DATA *data)`，`LIST *l`因为本例是无头链表，当链表的首节点需要变化时，需要将其地址传入`list_insert(&head, &tmp);`，方便句柄的改变
- 比较好的代码实践：[代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/list/list1/list.h)中`typedef struct list_node *LIST;`，`typedef struct list_node *PtrNode;`，`typedef struct list_node *Position;`，虽然为同一类型的重定义，但可读性增强


Day03
=========
- 可变长结构体，`ARR`本身空间其实只有`sizeof(int)`的大小（`data`不占空间），但可利用`malloc`得到更多空间，再利用`data`成员，去操作多出来的空间

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int len;
    int data[0];
} ARR;

ARR *create_array(int len){
    ARR *arr = malloc(sizeof(int) * (len + 1));
    arr->len = len;
    return arr;
}

int main(){
    ARR *arr = create_array(10);
    int i;
    for(i = 0; i < arr->len; i++)
        arr->data[i] = i * 2;
    printf("%d\n", arr->data[5]);
    free(arr);
    return 0;
}

```

利用该技术，得到 __泛型链表__：

```c
struct llist_node_st {
    struct llist_node_st *prev;
    struct llist_node_st *next;
    char data[0];
};

typedef struct {
    int size;
    struct llist_node_st head;
} LLIST;

LLIST *llist_creat(int size){
    LLIST *new;
    new = malloc(sizeof(*new));
    if (new == NULL) return NULL;
    new->size = size;
    new->head.prev = new->head.next = &new->head;
    return new;
}

int llist_insert(LLIST *ptr, const void *data, int mode){
    struct llist_node_st *newnode;
    newnode = malloc(sizeof(*newnode) + ptr->size);
    if (newnode == NULL) return -1;
    memcpy(newnode->data, data, ptr->size);

    if (mode == LLIST_FORWARD) {
        newnode->next = ptr->head.next;
        newnode->prev = &ptr->head;
    }
    else if (mode == LLIST_BACKWARD) {
        newnode->next = &ptr->head;
        newnode->prev = ptr->head.prev;
    }

    newnode->next->prev = newnode;
    newnode->prev->next = newnode;
    return 0;
}
```


