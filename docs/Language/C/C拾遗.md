> 2014-09-04

内存相关
========

## C内存分布

```plain
    ┌──────────┐
    │          │  自动变量
    ├──────────┤
    │          │  调用函数伸长（栈）
         |
         V

         ^
         |
    │          │  通过brk()伸长（堆）
    ├──────────┤
    │          │  利用malloc分配的内存区域
    ├──────────┤
    │          │  静态变量（函数/文件内static变量、全局变量）
    ├──────────┤
    │          │  函数（程序自身）、字符串常量
    └──────────┘
```

## 常见类型的内存增长方向

```plain
                               ^                ^               ^
                               |                |               |
high    ┌──────────┐
        │    v1    │      │   arr2   │     │   arg3   │    │   mem3   │
        ├──────────┤      ├──────────┤     ├──────────┤    ├──────────┤
        │    v2    │      │   arr1   │     │   arg2   │    │   mem2   │
        ├──────────┤      ├──────────┤     ├──────────┤    ├──────────┤
        │    v3    │      │   arr0   │     │   arg1   │    │   mem1   │
low                       └──────────┘     └──────────┘    └──────────┘
             |
             v

          局部变量           数组成员          参数变量        结构体成员

```


## 常见类型内存布局示例

```plain
int hoge[2][3]  数组的数组
                适用于两个数组长度都确定时
                0   1   2   4   5   6
                ┌───┬───┬───┬───┬───┬───┐
                │           │           │
                └───┴───┴───┴───┴───┴───┘


int (*hoge)[3]   指向数组 的指针
                 指针加一即前进一个数组的长度
                 本例中即 3 * sizeof(int)，也可用来解读 数组的数组
                 适用于主数组是可变的，但 子数组 长度确定时
                 0   1   2
         hoge ──>┌───┬───┬───┐
                 │           │
       hoge+1 ──>├───┼───┼───┤
                 │           │
                 ├───┼───┼───┤
                 │           │
                 ├───┼───┼───┤
                 │           │


int *hoge[2]    指向指针 的数组
                适用于主数组长度确定，但 子数组 长度不确定
                0   1
                ┌───┬───┐
                │ *   * │
                └─┼─┴─┼─┘      ┌─┬─┬─┬─┬─┬─
                  │   └───────>└─┴─┴─┴─┴─┴─
                  │            ┌─┬─┬─┬─┬─┬─┬─┬─
                  └───────────>└─┴─┴─┴─┴─┴─┴─┴─


int **hoge      指向指针 的指针
                当上例中数组的个数不确定（可变数组）时
                适用于 两个数组均 长度不确定
                0   1
        hoge ──>┌───┬───┬─
                │ *   *
                └─┼─┴─┼─┴─     ┌─┬─┬─┬─┬─┬─
                  │   └───────>└─┴─┴─┴─┴─┴─
                  │            ┌─┬─┬─┬─┬─┬─┬─┬─
                  └───────────>└─┴─┴─┴─┴─┴─┴─┴─

```

## 其它
- `memmove`比`memcpy`更安全的，特别是源地址与目标地址，存在重叠时
- `memset`用于内存数据清空成指定的数据
- `free(NULL)`是可以的，不会报错
- 注意`realloc`重新分配内存时，有可能存在分配失败，当原始指针所指向的部分还是指针时，此时需要将原始指针保留，万一分配失败时，针对原始指针所指向的指针部分进行处理（`free`）
- 在linux相关api中，如果返回的是指针类型，并且不需要手工释放的（可能存在静态区），请不要保留该指针，再次调用时，有可能改变上次指针所指内容（被覆盖）。如`tmpnam`，`getpwent`，`getpwuid`，甚至是`gmtime/localtime`等，需要保持原因指针内容时，需要用到`memcpy`。



声明相关
=======
## C语言声明的优先级规则
1. 声明从它的名字开始读取，然后按照优先级顺序依次读取
2. 优先级从高到低依次是：
    1. 声明从被括号括起来的那部分
    2. 后缀操作符：括号`()`表示这是一个函数；方括号`[]`表示这是一个数组
    3. 前缀操作符：星号`*`表示 指向...的指针
3. 如果`const`和(或)`volatile`关键字的后面紧跟类型说明符（如`int`,`long`等），那么它作用于类型说明符，在其他情况下，`const`和(或)`volatile`关键字作用于它左边紧邻的指针星号

> 对于`char * const *(*next)();`的声明：
>
> - 首先，看变量`next`，适用规则1
> - 它直接被括住，看成一个整体，得出`next是一个指向...的指针`，适用规则2.1
> - 考虑括号外的东西，在星号前缀和括号后缀作出选择，适用规则2
> - 规则2.2，告诉我们优先级较高的是右边的函数括号，得出`next是一个函数指针，指向一个返回...的函数`
> - 然后根据规则2.3，处理前缀星号，得出指针所指的内容
> - 最后，根据规则3，把`char * const`解释为指向字符的 常量指针
> - 综合起来，这个声明表示`next是一个指针，它指向一个函数，该函数返回另一个指针，该指针指向一个类型为char的常量指针`


一个有用的程序[`cdecl`](http://www.cdecl.org/)，用于解释相关声明，或`apt-get install cdecl`

```shell
cdecl> explain char * const *(*next)()
declare next as pointer to function returning pointer to const pointer to char

cdecl> explain int(*fun())
declare fun as function returning pointer to int

cdecl> explain int(*fun())()  #返回函数指针的 一个函数
declare fun as function returning pointer to function returning int

cdecl> explain int(*foo())[]  #返回指向int数组的指针的 一个函数
declare foo as function returning pointer to array of int

cdecl> explain int(*foo[])()  #函数指针数组
declare foo as array of pointer to function returning int
```

## 容易混淆的const

<!-- run -->

```c
#include <stdio.h>

void foo(const char **p){
    printf("%s\n", *p);
}

int main(void){
    char *s[] = {
        "abc","def"
    };
    foo(s);
    return 0;
}
```

这段代码是会发出编译警告的:  warning: passing argument 1 of foo from incompatible pointer type

实参`char *s`与实参`const char *p`是相容的，但实参`char **s`与实参`const char **p` __却是不相容__ 的

参数的传递过程类似赋值，而赋值形式的合法必须满足下列条件之一：

- 两个操作数都指向有限定符或无限定符的相容类型的指针
- 左边指针所指向的类型必须具有右边指针所指向类型的全部限定符

正因为第二个条件，使得函数调用中实参`char *s`与实参`const char *p`是相容的
> 左侧是指向有`const`限定符的`char`指针，右侧是一个指向没有限定符的`char`指针，`char`类型与`char`类型是相容的，左操作数所指向的类型具有右操作数所指向类型的限定符（此处是无），再加上自身的限定符（`const`）
>
> 并且在标准库中，所有字符串处理函数就是这样的

而`const char **p`读成`p is pointer to read-only char*`，指向有`const`限定符的`char`类型的指针的指针，即为 __没有限定符的指类类型__，而`char **s`读成`s is pointer to char*`，__它们所指向的类型不一样__，因此它们是 __不__ 相容的，违反了约束条件，编译器产生警告

> `const`和`*`组合通常只用于在 __数组形式的参数中模拟传值调用__，它声明“给你一个指针，但你不能修改它”，将它想像成read-only

> 读懂`const`，在纸上画出最小指针模型，左侧为指针变量域，右侧为普通变量域，看`const`离谁近，就修饰谁，`char const *p`和`const char *p`修饰的是`*p`，即右侧只读，而`char * const p`，则修饰`p`，左侧只读

## 对无符号类型的建议
尽量不要使用无符号类型，以免增加不必要的复杂性，不要仅仅因为无符号数不存在负数而用它表示数量（如年龄）

尽量使用`int`那样有符号的类型，在涉及混合类型的复杂细节时，不必担心边界情况（如-1被翻译成非常大的正数）

只有在使用位段和二进制掩码时，才可以使用无符号数，应该在表达式中使用强制类型转换，使操作数均为有符号（或无符号），而不必由编译器来选择结果的类型

> `sizeof()`返回类型是`unsigned int`，需要小心


特殊类型使用
===========

## 可变长结构体
可变长结构体，`ARR`本身空间其实只有`sizeof(int)`的大小（`data`不占空间），但可利用`malloc`得到更多空间，再利用`data`成员，去操作多出来的空间

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

## `typeof`关键字
```c
typeof(char []) charray = "hello";
typeof(int [20]) intarr = {1,2,3,4};`
```

> - 把y定义成x指向的数据类型：`typeof(*x) y;`
> - 把y定义成x指向数据类型的数组：`typeof(*x) y[4];`
> - 把y定义成一个字符指针数组：`typeof(typeof(char *)[4]) y;`

## `sig_atomic_t`类型
在写这种类型的变量时不会被中断，意味着有具有虚拟存储器的系统中，这种变量不会跨越页边界

## `volatile`修饰符
被设计用来修饰被不同线程访问和修改的变量。如果没有volatile，基本上会导致这样的结果：要么无法编写多线程程序，要么编译器失去大量优化的机会(确保本条指令不会因编译器的优化而省略，且要求每次直接读值)

## 位图枚举
<!-- run -->

```c
#include <stdio.h>

int bin(int num, char *data){
    if (num == 0) return 0;
    int idx = bin(num / 2, data);
    data[idx] = num % 2 + '0';
    return idx + 1;
}

void show_bin(int num){
    char data[32] = {0};
    bin(num, data);
    printf("%3d : %8s\n", num, data);
}

#define set_flags(src, flags) ((src) | (flags))

#define unset_flags(src, flags) ((src) & (~(flags)))

#define has_flags(src, flags) (((src) & (flags)) == (flags))

#define has_no_flags(src, flags) (((src) & (flags)) == 0)

enum flags{
    _A  = 1,
    _B  = 1 << 1,
    _C  = 1 << 2,
    _D  = 1 << 3,
    _E  = 1 << 4
};

#define my_print(foramt, data)\
    printf("%s : ", #data);\
    printf(foramt, data);

void usage_of_flag(){
    enum flags f = 0;
    f = set_flags(f, _A | _B | _C | _D | _E);
    show_bin(f);

    f = unset_flags(f, _C | _B);
    show_bin(f);

    f = unset_flags(f, _E | _B | _A);
    show_bin(f);

    f = set_flags(f, _B | _C | _D);
    show_bin(f);

    my_print("%d\n", has_flags(f, _B));
    my_print("%d\n", has_flags(f, _B | _D));
    my_print("%d\n", has_flags(f, _B | _A));

    my_print("%d\n", has_no_flags(f, _A | _B));
    my_print("%d\n", has_no_flags(f, _A));
    my_print("%d\n", has_no_flags(f, _A | _E));

}

int main(){
    usage_of_flag();
    return 0;
}
```

也可使用 __位字段__

```c
#include <stdint.h>

struct{
    uint8_t is_keyword : 1;
    uint8_t is_extern  : 1;
    uint8_t is_static  : 1;
} flags;

int main(){
    flags.is_extern = flags.is_static = 1;
    if(flags.is_extern){
        // to do ...
    }
    return 0;
}
```

## 有关宏
- 没有数据类型，方便公用代码提炼
- 只是替换，没有栈空间分配与函数调用，速度上优于函数
- 宏的展开是在编写的地方，而不存在调用关系，所以 __能直接使用上下文__
- 宏的本质是代码生成，但却解决了，函数参数不能是类型的问题（而宏的参数却可以是类型），所以能达到　__元语言__　编程的一些效果，如内核链表中的`#define container_of(ptr, type, member)`，充分的表达了语义


有关数组
=========
- 对于`int arr[5]`，`arr`与`&arr`是有区别的，它们的值是一样的，但意义不一样，前者代表 __数组元素首地址__，后者为 __数组的地址__，当它们进行指针加减运算时，如`arr+1`代表前进一个`int`类型的长度，而`&arr+1`，则前进`int[5]`类型的长度

