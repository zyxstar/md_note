> 2014-11-01

- 参考
> - [C课件.ppt](https://gitcafe.com/zyxstar/md_note/tree/master/data/C.ppt)
> - [C资源](https://gitcafe.com/zyxstar/md_note/tree/master/data/lessonplans)
> - [数据结构课件](https://gitcafe.com/zyxstar/md_note/tree/master/data/DataStructure)
> - [讲师代码](https://github.com/zyxstar/exam_c/tree/master/data_structure)


基础测试
==========
- 闰年判断，四年一闰，百年不闰，四百年一闰（if,  else if）

<!-- run -->

```c
#include <stdio.h>

int is_leap_year(int);
int main(){
    int years[] = {1900,1901,1904,1996,2000,2004,2005};
    int i, count = sizeof(years)/sizeof(int);
    for(i = 0; i < count; i++){
        if(is_leap_year(years[i]))
            printf("%d is a leap year\n", years[i]);
        else
            printf("%d is not a leap year\n", years[i]);
    }
    return 0;
}

int is_leap_year(int year){
    if(year % 400 == 0)  return 1;
    else if((year % 4 == 0) && (year % 100 != 0)) return 1;
    return 0;
}
```

- 写一个函数，能够将两个字符窜连接，并测试函数

<!-- run -->

```c
#include <stdio.h>
#define ARR_SIZE 20

char* my_strcat(char*, char*);
int main(){
    char str1[ARR_SIZE] = "abcd";
    char* str2 = "012345";
    printf("%s", my_strcat(str1, str2));
    return 0;
}

char* my_strcat(char *str1, char *str2){
    char* p = str1, *q = str2;
    while(*p != '\0') p++;
    while(*q != '\0'){
        *p = *q;
        p++;
        q++;
    }
    *p = '\0';
    return str1;
}
```

> - 最好使用`char str1[ARR_SIZE] = "abcd"`，将字符串分配在栈中；如果使用`strdup()`分配在堆中，有可能覆盖后面的空间；如果分配在只读空间，将报错

- 写一个函数，能够将两个整型指针的指向交换

<!-- run -->

```c
#include <stdio.h>

void swap(int**, int**);
int main(){
    int a = 10, b = 20;
    int *p1 = &a, *p2 = &b;
    swap(&p1, &p2);
    printf("*p1=%d; *p2=%d\n", *p1, *p2);
    return 0;
}

void swap(int **p1, int **p2){
    int *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}
```

- 书写四个函数，分别计算两个整数 + - * /，书写第五个函数，使用参数型函数指针完成对以上四个函数的调用。

<!-- run -->

```c
#include <stdio.h>
#include <assert.h>

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

int operator(int(*fun)(int,int), int a, int b){
    assert(fun != NULL);
    return fun(a, b);
}

int main(){
    int a = 100, b = 6;
    printf("add %d\n", operator(add, a, b));
    printf("minus %d\n", operator(minus, a, b));
    printf("multi %d\n", operator(multi, a, b));
    printf("divi %d\n", operator(divi, a, b));
    return 0;
}
```

课堂笔记
========

## 变量与内存结构
- 内存靠寻址来查找数据，一个地址只对应一个字节（8位）
- 变量用来管理和使用内存，引用相应数据类型所需内存的首地址
    - 变量就是内存中已命名的存储空间，通过引用变量就可以引用保存在该内存中的数据
- 数据类型是对变量管理内存的范围（大小）做了规范
    - `/usr/include/limits.h` 包含常用数据类型的最大最小值
- 有关`switch`不要在内部再嵌套一个`switch`会产生混乱，第二个可改成`if/else`

## 产生随机数
假设需要产生5个，50~100的随机数

<!-- run -->
```c
#include <stdio.h>
#include <stdlib.h>

int main(){
    int a, i;
    srand(time(NULL));
    for(i = 0; i < 5; i++){
        a = rand() % 51 + 50;
        printf("%d\n", a);
    }
    return 0;
}
```

- 注意`%`的使用，将数值求模，可以将固定数值的取值范围
- `srand(time(NULL))`用于产生随机数的种子

## 进制转换
- 十进制转任意进制

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

char alpha(int num){
    if (num < 10) return '0' + num;/* 0~9 */
    return (num - 10) + 'A'; /* A~F */
}

int trans(int num, int radix, char *out_data){
    if (num == 0) return 0;
    int idx = trans(num / radix, radix, out_data);
    out_data[idx] = alpha(num % radix);
    return idx + 1;
}

int main(){
    int num = 78;
    char data[100] = {0};

    trans(num, 2, data);
    printf("bin: %s\n", data);

    memset(data, 0, 100);
    trans(num, 8, data);
    printf("oct: %s\n", data);

    memset(data, 0, 100);
    trans(num, 16, data);
    printf("hex: %s\n", data);

    memset(data, 0, 100);
    trans(num, 10, data);
    printf("den: %s\n", data);
    return 0;
}
```

- 任意进制转十进制

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

int digit(char ch){
    if(isdigit(ch)) return ch - 48;
    if(isupper(ch)) return ch - 65 + 10;
    if(islower(ch)) return ch - 97 + 10;
    return 0;
}
int trans(char *in_data, int radix){
    int i, ret = 0, len = strlen(in_data);
    for(i = 0; i < len; i++){
        ret = ret * radix + digit(in_data[i]);
    }
    return ret;
}

int main(){
    printf("from bin: %d\n", trans("1001100",2));
    printf("from oct: %d\n", trans("75765",8));
    printf("from hex: %d\n", trans("B4",16));
    printf("from hex: %d\n", trans("b4",16));
    printf("from den: %d\n", trans("180",10));

    return 0;
}
```

## 有关补码
> [参考cs_107](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgitcafe.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E7%25BC%2596%25E7%25A8%258B%25E8%258C%2583%25E5%25BC%258F%2528stanford_cs107%2529.md#TOC2.3)，采用补码，还有一个原因是方便`0`的表达

- 计算机内部全部用补码来计算
- 正数的补码就是原码
- 负数的补码为：除去符号位，先取反码，再加1
> 其实负数的补码 转换到 原码，也可采取 `取反再+1`，这样比`-1再取反`容易操作

## 位移
- 右移，高位补符号位（少数机器高位补零）
- 左移，右侧空位补零，左侧符号位移出，原先的某数据位将成为符号位（少数机器符号位被固定，是不能位移的）

<!-- run -->

```c
#include <stdio.h>

int main(){
    char ch = 127;
    char ret = ch << 1;
    printf("result: %d\n", ret); /* -2 */

    ch = -65;
    ret = ch << 1;
    printf("result: %d\n", ret); /* 126 */

    return 0;
}
```


## 效率tip
编码时，要考虑数据的普遍情况，将能过滤掉大量无效数据的条件前置，可有效提高程序效率

## 指针初始化
- `int *p = &a;`, 定义变量`p`，`*`说明`p`是一个指针，`int`说明`p`可以控制内存的范围
- `*p = 10;`, `*`是寻址操作，该地址可读可写
- 指针定义时 __必须初始化__，或初始化`NULL`，或初始化某个具体地址

## 二级指针
```c
int a = 100;
int *p = &a;
int **pp = &p;

assert(pp == &p);
assert(*pp == p && p == &a);
assert(**pp == *p && *p == a);
```
## 数组是特殊的指针
- `int d[100] = {0};`第一个元素初始化了，后面所有元素初始化为0，二维数组也适用
- `a`相当于二级指针，`a[0]`是一个指针，`a[0]..a[2]`组成一个指针数组
- 与一般的二级指针不一样，`a`是只读的，甚至`a[0]..a[2]`也是只读的


## 二维数组与二级指针

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

int main(){
    int a[3][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
    // int a[3][4] = {1,2,3,4,5,6,7,8,9,10,11,12};  //same as above

    printf("a           = %p\n", a);
    printf("*a          = %p\n", *a);
    printf("** a        = %d\n", **a);

    printf("a[0]        = %p\n", a[0]);
    printf("&a[0][0]    = %p\n", &a[0][0]);
    printf("a[1]        = %p\n", a[1]);
    printf("&a[1][0]    = %p\n", &a[1][0]);

    printf("*(*(a+2)+3) = %d\n", *(*(a+2)+3)); //same as a[2][3]
    return 0;
}
```

## 常数区字符串复用
- 如果在常数区存在同一字符串，那么可以根据编译器不同有可能 __复用__(基本上来讲，常数区有一份可以利用的字符数组，就会复用)

## 数组vs指针
- c语言不允许数组首地址转向，所以数组名是一种特殊的指针，只能取值，不能赋值
- `sizeof`运算，如果是数组，得到的是数组的长度，而指针得到是指针类型的长度（32是4字节，64位8字节）

## 内存划分
1. 代码区
1. 全局区（静态区）
1. 栈区
1. 堆区
1. 常数区

## 堆中分配二维数组
- 除`pp`在栈中，其余都在堆中

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

int main(){
    int rows = 2, cols = 4;
    int **pp = NULL;
    pp = malloc(sizeof(int*) * rows);
    printf("pp: %p\n", pp);
    int i, j;
    for(i = 0; i < rows; i++){
        // *(pp+i) = malloc(sizeof(int) * cols);
        pp[i] = malloc(sizeof(int) * cols);
        printf("pp[%d]: %p\n",i, pp[i]);
        for(j = 0; j < cols; j++){
            *(*(pp+i)+j) = i * 10 + j;
            printf("pp[%d][%d]: %2d\n", i, j, pp[i][j]);
        }
    }

    for(i = 0; i < rows; i++) free(*(pp+i));
    free(pp);

    return 0;
}
```

## 二维数组传参

```c
void fun1(int a[][5], int rows);
void fun2(int **a, int rows, int cols);
```

## 示例(注册登录)

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

BOOL registe_user(USER_LIST *list, char *name, char *passwd){
    int i;
    for(i = 0; i < list->size; i++)
        if(strcmp(list->users[i].name, name) == 0) return FALSE;

    if(list->size == list->alloc_size){
        if(list->alloc_size == 0)
            list->alloc_size = ALLOC_LEN;
        else
            list->alloc_size *= 2;
        list->users = realloc(list->users, sizeof(USER) * list->alloc_size);
    }

    // char *_name = malloc(sizeof(char) * (strlen(name) + 1));
    // char *_passwd = malloc(sizeof(char) * (strlen(passwd) + 1));
    // strcpy(_name, name);
    // strcpy(_passwd, passwd);
    // list->users[list->size].name = _name;
    // list->users[list->size].passwd = _passwd;

    list->users[list->size].name = strdup(name);
    list->users[list->size].passwd = strdup(passwd);

    list->size++;
    return TRUE;
}

void list_user(USER_LIST *list){
    int i;
    for(i = 0; i < list->size; i++)
        printf("[%2d] name: %s; passwd: %s\n",
              i, list->users[i].name, list->users[i].passwd);
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

int main(){
    USER_LIST list = {0};
    // list.users = NULL;
    // list.size = 0;
    // list.alloc_size = 0;

    printf("registe: %d\n", registe_user(&list,"abc","123"));
    printf("registe: %d\n", registe_user(&list,"def","345"));
    printf("registe: %d\n", registe_user(&list,"def","345"));

    list_user(&list);

    printf("validate: %d\n", validate_user(&list,"def","345"));
    printf("validate: %d\n", validate_user(&list,"def","344"));

    free_list(&list);

    return 0;
}
```

## 数组名不能做左值

```c
char name[10];
name = "abc"; //ERROR
strcpy(name, "abc"); //RIGHT
// or
char *name;
name = strdup("abc");
free(name);
```

## 共用体
- 当参数类型不确定时，可以使用共用体来传递

## 函数指针
- 定义的函数名就是一个只读的函数指针

<!-- run -->

```c
void fun1(){}

int main(){
    printf("%p\n", fun1);
}
```

- 函数指针就成为一种类型了，可作为参数、变量、返回值、结构体成员等的类型，达到 __接口契约__ 的功能
- 函数指针作为返回值:

<!-- run -->

```c
#include <stdio.h>
#include <assert.h>

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

int(*route_fn(char operator))(int, int){
    static int(*funs[])(int, int) = {add, minus, multi, divi};
    static char operators[] = {'+', '-',   '*',   '/'};
    int i = 0;
    while(operators[i] != operator) i++;
    assert(i<4);
    return funs[i];
}
int main(){
    printf("%d\n", route_fn('+')(10,20));
    return 0;
}
```

或者定义`typedef`一个类型`FUN`来表示函数指针，增加可读性

<!-- run -->

```c
#include <stdio.h>
#include <assert.h>

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

typedef int(*FUN)(int, int);

FUN route_fn(char operator){
    static FUN funs[] = {add, minus, multi, divi};
    static char operators[] = {'+', '-',   '*',   '/'};
    int i = 0;
    while(operators[i] != operator) i++;
    assert(i<4);
    return funs[i];
}
int main(){
    printf("%d\n", route_fn('+')(10,20));
    return 0;
}
```


## 指针函数
- 即返回指针的函数，需要注意返回的不能是无效的地址，如函数内部栈中定义的地址

## 变量
### 全局变量
- 存储区域：全局区（静态区）
- 作用域：整个程序
- 生命周期：与程序生命周期一致

### 局部变量
- 存储区域：栈区
- 作用域：当前函数
- 生命周期：当前函数运行周期

### 块级变量
- 存储区域：栈区
- 作用域：当前语句块
- 生命周期：当前语句块运行周期

### 静态局部变量
- 存储区域：全局区（静态区）
- 作用域：当前函数
- 生命周期：与程序生命周期一致

### 静态全局变量
- 存储区域：全局区（静态区）
- 作用域：本文件（但不能被其他文件`extern`）
- 生命周期：与程序生命周期一致

> 静态变量和全局变量，不初始化时，系统会自动初始化，局部变量则不会

## 头文件
一般将外部全局变量声明，函数声明放在此处，如必须要存在定义（struct,typedef）时，则需`#ifndef/#endif`


## GCC
```shell
gcc -E hello.c -o hello.i          #预处理
gcc -S hello.i                     #编译，产行hello.s汇编文件
gcc -c hello.s                     #汇编，产生hello.o二进制目标文件
gcc -o hello.out hello.o           #链接
```

## GDB
- [linux下GDB教程.pdf](https://gitcafe.com/zyxstar/md_note/blob/master/data/linux下GDB教程.pdf)

## Make
- [什么是makefile.doc](https://gitcafe.com/zyxstar/md_note/blob/master/data/什么是makefile.doc)


## 数据结构
- `#if 0\#else\#endif`条件注释，有效规避多行注释不能嵌套问题
- `scanf()`是有返回值的，返回成功匹配与赋值的个数，例如`1 != scanf("%d", &tmp);`可用来判断读取是否失败了
- `NULL == (void*)0`
- 在泛型线性表中找到所有符合条件的元素，实现于[代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/darr/darr.c)中的`int darr_find_all(DARR head, const void *key, darr_cmp *fun, void ***find_data)`与`static void** _store_find_data(int size, void *data)`
- [代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/list/list.c)中`int list_insert(LIST *l, const DATA *data)`，`LIST *l`因为本例是无头链表，当链表的首节点需要变化时，需要将其地址传入`list_insert(&head, &tmp);`，方便句柄的改变
- 比较好的代码实践：[代码](https://github.com/zyxstar/exam_c/blob/master/data_structure/list/list1/list.h)中`typedef struct list_node *LIST;`，`typedef struct list_node *PtrNode;`，`typedef struct list_node *Position;`，虽然为同一类型的重定义，但可读性增强

50道题
=========
## 可重用函数
下面的解题思路，使用了一些函数式的方式，将共用的函数提炼出来

> 1. 代码更多从扩展性、重用性考虑，有些代码是牺牲了性能的
> 2. 迫不得已，不会在函数内部使用`printf`，`scanf`等函数：
>
>> - 方法之一，借由参数与返回值的方式来保持数据的获取与返回；
>> - 方法之二，是使用`FILE*`来代替直接对`stdin`和`stdout`的操作

形成的重用头文件`utils.h`：

<!-- utils -->

```c
#ifndef UTILS_H_INCLUEDE
#  define UTILS_H_INCLUEDE

#  ifndef REDUCE_CACHE
#     define REDUCE_CACHE int
#  endif

#  ifndef REDUCE_ELEM
#     define REDUCE_ELEM int
#  endif

#  ifndef MAP_SRC_ELEM
#     define MAP_SRC_ELEM int
#  endif

#  ifndef MAP_DEST_ELEM
#     define MAP_DEST_ELEM int
#  endif

#  ifndef EACH_ELEM
#     define EACH_ELEM int
#  endif

#  ifndef FILTER_ELEM
#     define FILTER_ELEM int
#  endif

#  ifndef TAKE_ELEM
#     define TAKE_ELEM int
#  endif

#  ifndef Q_SORT_ELEM
#     define Q_SORT_ELEM int
#  endif

// reduce
REDUCE_CACHE reduce(REDUCE_CACHE(*callback)(REDUCE_CACHE, REDUCE_ELEM, int),
                    REDUCE_ELEM *arr, int size, REDUCE_CACHE init){
    int i;
    REDUCE_CACHE acc = init;
    for(i = 0; i < size; i++){
        acc = callback(acc, arr[i], i);
    }
    return acc;
}

// map
MAP_DEST_ELEM* map(MAP_DEST_ELEM(*callback)(MAP_SRC_ELEM, int),
                   MAP_SRC_ELEM *arr, int size){
    int i;
    MAP_DEST_ELEM* ret_arr = malloc(sizeof(MAP_DEST_ELEM) * size);
    assert(ret_arr != NULL);
    for(i = 0; i < size; i++){
        ret_arr[i] = callback(arr[i], i);
    }
    return ret_arr;
}

// each
void each(void(*callback)(EACH_ELEM*, int), EACH_ELEM *arr, int size){
    int i;
    for(i = 0; i < size; i++){
        callback(&arr[i], i);
    }
}

// filter
int filter(int(*predicate)(FILTER_ELEM, int), FILTER_ELEM *arr, int size){
    int i, new_size = 0;
    FILTER_ELEM temp;
    for(i = 0; i < size; i++){
        if(predicate(arr[i], i)){
            temp = arr[i];
            arr[new_size] = temp;
            new_size++;
        }
    }
    return new_size;
}

// take_while
TAKE_ELEM take_while(TAKE_ELEM(*predicate)(TAKE_ELEM, int),
                     TAKE_ELEM(*move_next)(TAKE_ELEM, int), TAKE_ELEM init){
    TAKE_ELEM data = init;
    int i = 0;
    while(!(predicate(data, i))){
        data = move_next(data, i);
        i++;
    }
    return data;
}

// range
int range(int start, int end, int step, int* arr){
    if(step == 0) return 0;
    if(step > 0 && end < start) return 0;
    if(step < 0 && start < end) return 0;
    int new_size = 0;
    while(1){
        if(step > 0 && start >= end) break;
        if(step < 0 && start <= end) break;
        arr[new_size++] = start;
        start += step;
    }
    return new_size;
}

// q_sort
    void swap(Q_SORT_ELEM *a, Q_SORT_ELEM *b){
        Q_SORT_ELEM temp = *a;
        *a = *b;
        *b = temp;
    }

    int partition(int(*cmp)(Q_SORT_ELEM, Q_SORT_ELEM),
                  Q_SORT_ELEM *arr, int size, int pivoit_idx){
        int i, left_idx = 0;
        Q_SORT_ELEM pivot = arr[pivoit_idx];
        swap(&arr[pivoit_idx], &arr[size - 1]);
        for(i = 0; i < size; i++){
            if(cmp(arr[i], pivot) == -1){
                swap(&arr[i], &arr[left_idx]);
                left_idx++;
            }
        }
        swap(&arr[size - 1], &arr[left_idx]);
        return left_idx;
    }

void q_sort(int(*cmp)(Q_SORT_ELEM, Q_SORT_ELEM), Q_SORT_ELEM *arr, int size){
    if(size <=1 ) return;
    int left_idx = partition(cmp, arr, size, 0);
    q_sort(cmp, arr, left_idx);
    q_sort(cmp, &arr[left_idx+1], size - left_idx -1);
}

// print_arr
void print_arr(FILE *fp, int *arr, int size){
    fprintf(fp, "[ ");
    int i = 0;
    while(i < size){
        fprintf(fp, "%6d ", arr[i++]);
        if(i == size){
            fprintf(fp, " ]\n");
            return;
        }
        if(i%10 == 0) fprintf(fp, "\n");
    }
}

// my_strcmp
int my_strcmp(const char *str1, const char *str2){
    const char *p1 = str1, *p2 = str2;
    while(*p1 != '\0' && *p2 != '\0' && *p1 == *p2){
        p1++; p2++;
    }
    if(*p1 == '\0' && *p2 == '\0') return 0;
    return *p1 > *p2 ? 1 : -1;
}

// my_strlen
int my_strlen(const char *str){
    const char *p = str;
    while(*p != '\0') p++;
    return p - str;
}

// my_strcat
char* my_strcat(char *str1, const char *str2){
    char* p = str1, *q = str2;
    while(*p != '\0') p++;
    while(*q != '\0'){
        *p = *q;
        p++;
        q++;
    }
    *p = '\0';
    return str1;
}

#endif /*UTILS_H_INCLUEDE*/
```

## Exam01
- 输入一个不超过五位的正整数，输出其逆数。例如输入12345，输出应为54321。

<!-- run -->
```c
#include <stdio.h>

int reverse(int num){
    int ret = 0;
    while(num > 0){
        ret = ret * 10 + num % 10;
        num /= 10;
    }
    return ret;
}

int main(){
    int a = 12345;
    printf("%d\n", reverse(a));
    a = 45040;
    printf("%d\n", reverse(a));
    return 0;
}

```

## Exam02
- 计算1+2+3…+n的值，n是从键盘输入的自然数。

<!-- run -->

```c
#include <stdio.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE int

//= require reduce
//= require range

int add(int acc, int b, int idx){return acc + b;}
int main(){
    int n = 50;
    int arr[n];
    int size = range(1, n+1, 1, arr);
    printf("%d\n", reduce(add, arr, size, 0));
    return 0;
}
```

## Exam03
- 从终端（键盘）读入20个数据到数组中，统计其中正数的个数，并计算这些正数之和。

<!-- run -->

```c
#include <stdio.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE int
#define FILTER_ELEM int

//= require reduce
//= require filter

int add(int acc, int num, int idx){return acc + num;}
int is_positive(int num, int idx){return num > 0;}
int main(){
    int arr[] = {1,2,3,4,5,6,-3,-4,-5,-6,11,12,13,14,15,16,-13,-14,-15,-16};
    int size = sizeof(arr) / sizeof(int);
    size = filter(is_positive, arr, size);
    printf("size: %d; sum: %d \n", size, reduce(add, arr, size, 0));
    return 0;
}
```


## Exam04
- 从终端（键盘）将5个整数输入到数组a中，然后将a逆序复制到数组b中，并输出b中各元素的值。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

//= require print_arr

void reverse_arr(int *arr1, int *arr2, int size){
    int i;
    for(i = 0; i < size; i++){
        arr2[i] = arr1[size - i - 1];
    }
}

int main(){
    int arr1[] = {1,2,3,4,5};
    int size = sizeof(arr1) / sizeof(int);
    int arr2[size];

    reverse_arr(arr1, arr2, size);
    print_arr(stdout, arr2, size);
    return 0;
}

```

## Exam05
- 要将5张100元的大钞票，换成等值的50元，20元，10元，5元一张的小钞票，每种面值至少1张，编程求需要多少张纸币。

<!-- run -->

```c
#include <stdio.h>

int main(){
    int target = 500;
    int i,j,k,m,count=0;
    for(i = 1; i < target/50 + 1; i++){
        for(j = 1; j < (target - i*50)/20 + 1; j++){
            for(k = 1; k < (target - i*50 - j*20)/10 + 1; k++){
                for(m = 1; m < (target - i*50 - j*20 - k*10)/5 + 1; m++){
                    if(i*50 + j*20 + k*10 + m*5 == target){
                        printf("%d %d %d %d\n", i, j, k, m);
                        count++;
                    }
                }
            }
        }
    }
    printf("count: %d\n", count);
    return 0;
}
```

使用递归，能支持更多面值，只需修改`STAT`定义，同时增加相应的面值到`rules`中即可：

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

int TARGET = 500;
int COUNT = 0;

typedef struct{
    int cnt_50;
    int cnt_20;
    int cnt_10;
    int cnt_5;
}STAT;

int rules[] = {50, 20, 10, 5};

int _get_total(STAT stat){
    int i = 0, ret = 0, size = sizeof(rules) / sizeof(int);
    while(i < size){
        ret += ((int*)&stat)[i] * rules[i];
        i++;
    }
    return ret;
}

int _all_count_than_zero(STAT stat){
    int i = 0, ret = 0, size = sizeof(rules) / sizeof(int);
    while(i < size){
        if (((int*)&stat)[i] == 0) return 0;
        i++;
    }
    return 1;
}

int check_is_done(STAT stat){
    return _get_total(stat) > TARGET;
}

int check_accord(STAT stat){
    return _get_total(stat) == TARGET && _all_count_than_zero(stat);
}

void push_result(STAT stat){
    printf("[%4d](", ++COUNT);
    int i = 0, size = sizeof(rules) / sizeof(int);
    while(i < size){
        printf("%4d", ((int*)&stat)[i]);
        i++;
    }
    printf(")\n");
}

void product(int size, int idx, STAT stat){
    if (idx == size) return;
    while(!check_is_done(stat)){
        product(size, idx + 1, stat);
        ((int*)&stat)[idx] += 1;
        if(check_accord(stat))
            push_result(stat);
    }
}

int main(){
    int size = sizeof(rules) / sizeof(int);
    int stat[size];
    int i = 0;
    while(i < size) stat[i++] = 0;
    product(size, 0, *((STAT*)stat));
    return 0;
}
```

- `((int*)&stat)[idx] += 1`利用了结构体地址强制转换成数组指针，达到对结构体元素按索引操作，同时又利用了结构体能相互赋值，方便值传递

## Exam06
- 求n以内（不包括n）同时能被3和7整除的所有自然数之和的平方根s，n从键盘输入。例如若n为1000时，函数值应为：s=153.909064。

<!-- run -->

```c
#include <stdio.h>
#include <math.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE int
#define FILTER_ELEM int

//= require reduce
//= require filter
//= require range

int add(int acc, int num, int idx){return acc + num;}
int check_num(int num, int idx){return (num % 3 == 0) && (num % 7 == 0);}
int main(){
    int n = 1000;
    int arr[n];

    int size = range(1, n, 1, arr);
    size = filter(check_num, arr, size);
    int sum = reduce(add, arr, size, 0);
    double ret = sqrt(sum);

    printf("%f\n", ret);
    return 0;
}

```

- 需要引入`math.h`，并在gcc时，需要 `-lm`

## Exam07
- 一辆卡车违反交通规则，撞人后逃跑。现场有三人目击事件，但都没有记住车号，只记下车号的一些特征。甲说：牌照的前两位数字是相同的；乙说：牌照的后两位数字是相同的，但与前两位不同；丙是数学家，他说：四位的车号刚好是一个整数的平方。请根据以上线索找出车号。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define MAP_SRC_ELEM int
#define MAP_DEST_ELEM int
#define FILTER_ELEM int

//= require map
//= require filter
//= require range
//= require print_arr

int square_num(int num, int idx){return num * num;}
int check_num(int num, int idx){
    int a = num % 10;
    int b = (num/10) % 10;
    int c = (num/100) % 10;
    int d = (num/1000) % 10;
    return a==b && c==d && a!=c;
}
int main(){
    int arr[100];
    int size = range((int)sqrt(1000), (int)sqrt(10000), 1, arr);
    int *ret_arr = map(square_num, arr, size);
    size = filter(check_num, ret_arr, size);
    print_arr(stdout, ret_arr, size);
    free(ret_arr);

    return 0;
}
```

- 需要引入`math.h`，并在gcc时，需要 `-lm`

## Exam08
- 输入1~10之间的一个数字，输出它对应的英文单词。

<!-- run -->

```c
#include <stdio.h>

const char* mapper(int digit){
    if(digit < 0 || digit > 10) return NULL;
    static char *arr[] = {"zero", "one", "two",
                          "three", "four", "five",
                          "six", "seven", "eight",
                          "nine", "ten" };
    return arr[digit];
}

int main(){
    int i;
    for(i = 1; i <= 10; i++){
        printf("%d : %s\n", i, mapper(i));
    }
    return 0;
}
```

## Exam09
- 个位数为6且能被3整除但不能被5整除的三位自然数共有多少个，分别是哪些？

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define FILTER_ELEM int

//= require filter
//= require range
//= require print_arr

int check_num(int num, int idx){
    return (num%3 == 0) && (num%5 != 0) && (num%10 == 6);
}
int main(){
    int arr[999];
    int size = range(100, 1000, 1, arr);
    size = filter(check_num, arr, size);
    printf("count is %d\n", size);
    print_arr(stdout, arr, size);
    return 0;
}
```

## Exam10
- 输入某年某月某日，判断这一天是这一年的第几天？

<!-- run -->

```c
#include <stdio.h>

#define MONTH_SIZE 12
#define REDUCE_ELEM int
#define REDUCE_CACHE int

//= require reduce

int is_leap_year(int year){
    if(year % 400 == 0)  return 1;
    if((year % 4 == 0) && (year % 100 != 0)) return 1;
    return 0;
}
void copy_arr(int *souce, int *dest, int size){
    while(size >= 0) {dest[size-1] = souce[size-1]; size--;}
}
void get_months(int year, int *month_days){
    static int s_month_days[] = {31,0,31,30,31,30,31,31,30,31,30,31};
    copy_arr(s_month_days, month_days, MONTH_SIZE);
    month_days[1] = is_leap_year(year) ? 29 : 28;
}
int add(int acc, int num, int idx){return acc + num;}
int day_in_year(int year, int month, int day){
    int month_days[MONTH_SIZE];
    get_months(year, month_days);
    return reduce(add, month_days, month-1, 0) + day;
}
int main(){
    printf("%d\n", day_in_year(1998,1,1));
    printf("%d\n", day_in_year(1998,2,1));
    printf("%d\n", day_in_year(1998,3,1));

    printf("%d\n", day_in_year(2000,1,1));
    printf("%d\n", day_in_year(2000,2,1));
    printf("%d\n", day_in_year(2000,3,1));
    printf("%d\n", day_in_year(2000,12,31));

    return 0;
}
```

## Exam11
- 两个乒乓球队进行比赛，各出三人。甲队为a,b,c三人，乙队为x,y,z三人。已抽签决定比赛名单。有人向队员打听比赛的名单。a说他不和x比，c说他不和x,z比，请编程序找出三队赛手的名单。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum{
    ALTERNATIVE,
    CHOSEN,
    ABANDON
} CHOSENTYPE;

typedef struct{
    int mem1;
    int mem2;
    CHOSENTYPE is_chosen;
} COUPLE;

typedef struct{
    int name;
    int count;
    COUPLE *my_couples[3];
} MEMBERSTATUS;

#define FILTER_ELEM COUPLE
#define REDUCE_ELEM COUPLE
#define REDUCE_CACHE int

//= require filter
//= require reduce

int* product(int *arr1, int size1, int *arr2, int size2){
    int *ret = malloc((size1 * size2) * sizeof(int[3]));
    assert(ret != NULL);
    int size = 0, i, j;
    for(i = 0; i < size1; i++){
        for(j = 0; j < size2; j++){
            ret[size++] = arr1[i]; //mem1
            ret[size++] = arr2[j]; //mem2
            ret[size++] = 0;       //is_chosen
        }
    }
    return ret;
}

int check_rule(COUPLE couple, int idx){
    if (couple.mem1 == 'a' && couple.mem2 == 'x') return 0;
    if (couple.mem1 == 'c' && couple.mem2 == 'x') return 0;
    if (couple.mem1 == 'c' && couple.mem2 == 'z') return 0;
    return 1;
}

MEMBERSTATUS* init_status(int size){
    MEMBERSTATUS *status = malloc(size * sizeof(MEMBERSTATUS));
    while(size > 0){
        size--;
        status[size].name = 0;
        status[size].count = 0;
    }
    return status;
}

MEMBERSTATUS* get_status_by_name(MEMBERSTATUS *status, int size, int name){
    while(size > 0){
        size--;
        if(status[size].name == name) return &status[size];
    }
    return NULL;
}

void increase_my_couples(int name, COUPLE *couple,
                         MEMBERSTATUS *status, int status_size){
    MEMBERSTATUS *s = get_status_by_name(status, status_size, name);
    if(s == NULL) {
        s = get_status_by_name(status, status_size, 0);
        s->name = name;
    }
    s->my_couples[s->count++] = couple;
}

void describe_status(MEMBERSTATUS *status, int status_size,
                     COUPLE *couples, int couples_size){
    while(couples_size > 0){
        couples_size--;
        COUPLE *c = &couples[couples_size];
        if(c->is_chosen == ALTERNATIVE){
            increase_my_couples(c->mem1, c, status, status_size);
            increase_my_couples(c->mem2, c, status, status_size);
        }
    }
}

void process(COUPLE *couples, int couples_size, int status_size){
    if(status_size == 0) return;
    int is_redo = 0;

    MEMBERSTATUS *status = init_status(status_size);
    describe_status(status, status_size, couples, couples_size);

    int i,j,new_status_size = status_size;
    printf("Process:\n");
    for(i = 0; i < status_size ; i++){
        MEMBERSTATUS s = status[i];
        if(s.count == 1 && s.my_couples[0]->is_chosen != CHOSEN){
            is_redo = 1;
            s.my_couples[0]->is_chosen = CHOSEN;
            printf("  %c vs %c CHOSEN\n", s.my_couples[0]->mem1, s.my_couples[0]->mem2);
            new_status_size -= 2;
            int nm = s.my_couples[0]->mem1;
            if(nm == s.name) nm = s.my_couples[0]->mem2;

            MEMBERSTATUS *other = get_status_by_name(status, status_size, nm);
            for(j = 0; j < other->count; j++){
                if(other->my_couples[j]->mem1 == s.name || other->my_couples[j]->mem2 == s.name)
                    continue;
                other->my_couples[j]->is_chosen = ABANDON;
                printf("  %c vs %c ABANDON\n", other->my_couples[j]->mem1, other->my_couples[j]->mem2);
            }
        }
    }
    free(status);
    if(is_redo) process(couples, couples_size, new_status_size);
}

void print_couples(FILE *fp, COUPLE *couples, int couples_size){
    int i;
    fprintf(fp,"Result: \n");
    for (i = 0; i < couples_size; i++){
        if(couples[i].is_chosen != ABANDON )
            fprintf(fp, "  %c vs %c\n", couples[i].mem1, couples[i].mem2);
    }
}

int main(){
    int team_1[] = {'a','b','c'};
    int team_2[] = {'x','y','z'};
    int size_1 = sizeof(team_1) / sizeof(int);
    int size_2 = sizeof(team_2) / sizeof(int);

    // int(*couples)[3] = (int(*)[3])product(team_1, size_1, team_2, size_2);
    COUPLE *couples = (COUPLE*)product(team_1, size_1, team_2, size_2);
    int couples_size = filter(check_rule, couples, size_1 * size_2);
    process(couples, couples_size, size_1 + size_2);
    print_couples(stdout, couples, couples_size);

    free(couples);
    return 0;
}
```

- 利用`product`产生一个数组的数组，再强制转换为一个结构体数组，而结构体可以互相赋值，方便了子数组的操作

## Exam12
- 有5个人坐在一起，问第五个人多少岁？他说比第4个人大2岁。问第4个人岁数，他说比第3个人大2岁。问第三个人，又说比第2人大两岁。问第2个人，说比第一个人大两岁。最后 问第一个人，他说是10岁。请问第五个人多大？

<!-- run -->

```c
#include <stdio.h>

int fac(int n){
    if(n == 1) return 10;
    return 2 + fac(n - 1);
}

int main(){
    int n = 5;
    printf("%d\n", fac(n));
    return 0;
}
```

## Exam13
- 输入某三角形的三个边的长度，判断出这是个什么三角形（等腰、等边、任意，或不能构成）。

<!-- run -->

```c
#include <stdio.h>

typedef enum {
    NONE,
    GENERAL,
    ISOSCELES,
    EQUILATERAL
} TriangleType;

const char* describe_triangle(TriangleType type){
    static char *arr[] = {"NONE", "GENERAL", "ISOSCELES", "EQUILATERAL"};
    return arr[type];
}

int check_positive(int a, int b, int c){
    return (a>0 && b>0 && c>0);
}
int check_general(int a, int b, int c){
    return (a + b > c) && (a + c > b) && (b + c > a);
}
int check_isosceles(int a, int b, int c){
    return (a==b || b==c || c==a);
}
int check_equilateral(int a, int b, int c){
    return (a==b && b==c);
}
TriangleType get_triangle_type(int a, int b, int c){
    if(!check_positive(a,b,c) || !check_general(a,b,c)) return NONE;
    if(check_equilateral(a,b,c)) return EQUILATERAL;
    if(check_isosceles(a,b,c)) return ISOSCELES;
    return GENERAL;
}

int main(){
    int a = 10, b = 20, c = 25;
    printf("%s\n", describe_triangle(get_triangle_type(a,b,c)));

    a = 10, b = 20, c = 35;
    printf("%s\n", describe_triangle(get_triangle_type(a,b,c)));

    a = 10, b = 20, c = 20;
    printf("%s\n", describe_triangle(get_triangle_type(a,b,c)));

    a = 10, b = 10, c = 10;
    printf("%s\n", describe_triangle(get_triangle_type(a,b,c)));
    return 0;
}
```

## Exam14
- 输入10个数，分别统计其中正数、负数、零的个数。

<!-- run -->

```c
#include <stdio.h>

typedef struct{
    int zero;
    int positive;
    int negative;
} STATUS;

#define REDUCE_ELEM int
#define REDUCE_CACHE STATUS*

//= require reduce

STATUS* callback(STATUS *acc, int num, int idx){
    if(num == 0) acc->zero++;
    else if(num > 0) acc->positive++;
    else acc->negative++;
    return acc;
}
int main(){
    int arr[] = {1,2,3,4,0,0,-5,-6,0,-7,-9};
    STATUS status = {0,0,0};
    reduce(callback, arr, sizeof(arr) / sizeof(int), &status);
    printf("zero: %d; positive: %d; negative: %d\n",
        status.zero, status.positive, status.negative);
    return 0;
}
```

## Exam15
- 先随机产生N个三位自然数输出，然后再输出其中同时是3、5、7倍数的数。（设N为100）

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define N 100
#define FILTER_ELEM int

//= require filter
//= require print_arr

int check_num(int num, int idx){return num%3==0 && num%5==0 && num%7==0;}

int main(){
    int arr[N], i, size;
    srand(time(NULL));
    for(i = 0; i < N; i++){
        arr[i] = rand()%900 + 100;
    }
    print_arr(stdout, arr, N);
    size = filter(check_num, arr, N);
    print_arr(stdout, arr, size);
    return;
}
```

## Exam16
- 用for编程找出100~200中的完全平方数。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAP_SRC_ELEM int
#define MAP_DEST_ELEM int

//= require range
//= require map
//= require print_arr

int square(int num, int idx){return num * num;}
int main(){
    int arr[10];
    int size = range(sqrt(100), sqrt(200)+1, 1, arr);
    int *ret = map(square, arr, size);
    print_arr(stdout, ret, size);
    return 0;
}
```

## Exam17
- 从终端输入三个正实数，判断这三个数能否构成直角三角形。

<!-- run -->

```c
#include <stdio.h>

int check(int a, int b, int c){
    int square_a = a * a;
    int square_b = b * b;
    int square_c = c * c;
    if(square_a == square_b + square_c) return 1;
    if(square_b == square_a + square_c) return 1;
    if(square_c == square_a + square_b) return 1;
    return 0;
}

int main(){
    printf("%d\n", check(3,4,5));
    printf("%d\n", check(4,5,6));
    printf("%d\n", check(6,7,8));
    printf("%d\n", check(30,40,50));
    return 0;
}
```

## Exam18
- 输入一行字符，分别统计出其中英文字母、空格、数字和其它字符的个数。

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

typedef struct{
    int alpha;
    int digit;
    int space;
    int other;
} STAT;

#define REDUCE_CACHE STAT*
#define REDUCE_ELEM char

//= require reduce

STAT* statistics(STAT* acc,  char ch, int idx){
    if(isalpha(ch)) acc->alpha++;
    else if(isdigit(ch)) acc->digit++;
    else if(isspace(ch)) acc->space++;
    else acc->other++;
    return acc;
}

int main(){
    STAT stat = {0,0,0,0};
    char* str = "this is test, 1237 !";
    reduce(statistics, str, strlen(str), &stat);
    printf("alpha: %d\ndigit: %d\nspace: %d\nother: %d\n",
            stat.alpha, stat.digit, stat.space, stat.other);
    return 0;
}
```

## Exam19
- 输入一个字串，判断它是否是对称串。如”abcdcba”是对称串，”123456789”不是。

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

int check(char *str){
    int len = strlen(str);
    int i = 0;
    while(i < len/2){
        if(str[i] != str[len-1-i]) return 0;
        i++;
    }
    return 1;
}
int main(){
    printf("%d\n", check("abcdcba"));
    printf("%d\n", check("123456789"));
    printf("%d\n", check("abccba"));
    printf("%d\n", check("abbccbba"));
    return 0;
}
```

## Exam20
- 随机产生N个大写字母输出，然后统计其中共有多少个元音字符。（设N为50）

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define N 50
#define REDUCE_CACHE int
#define REDUCE_ELEM char

//= require reduce

void gen_alpha(char *arr, int size){
    srand(time(NULL));
    int i;
    for(i = 0; i < size; i++){
        arr[i] = rand()%26 + 'A';
    }
}

int count(int acc, char ch, int idx){
    return acc + (ch == 'A' || ch == 'E' ||
            ch == 'I' || ch == 'O' || ch == 'U');
}

int main(){
    char str[N+1];
    gen_alpha(str, N);
    str[N] = '\0';
    printf("%s\n", str);
    printf("%d\n", reduce(count,str,N,0));
    return 0;
}
```

## Exam21
- 找出三位自然数中的所有素数，要求判断x素数用自定义函数data(x)实现。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FILTER_ELEM int

//= require range
//= require filter
//= require print_arr

int data(int num, int idx){
    if(num == 1 || num == 2 || num == 3) return 1;
    int i = 2, max = sqrt(num);
    while(i < max){
        if(num % i == 0) return 0;
        i++;
    }
    return 1;
}

int main(){
    int nums[1000];
    int size = range(100, 1000, 1, nums);
    size = filter(data, nums, size);
    printf("count: %d\n", size);
    print_arr(stdout, nums, size);

    return 0;
}
```


## Exam22
- 输出n行由“#”组成的矩形，每行“#”的个数为m个，其中n、m由终端（键盘）输入。要求输出m个“#”功能由函数satr(m)函数实现。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAP_SRC_ELEM int
#define MAP_DEST_ELEM char

//= require map
//= require range

char change(int num, int idx) { return '#'; }

void star(FILE *fp, int m){
    int src[m+1];
    range(0, m+1, 1, src);
    char *str = map(change, src, m+1);
    str[m+1] = '\0';
    fprintf(fp, "%s\n", str);
    free(str);
}

int main(){
    int m = 5, n = 7;
    int rows[n];
    int i;
    for(i = 0; i < n; i++)
        star(stdout, m);

    return 0;
}
```

## Exam23
- 若ax2+bx+c=0有实根，则求之。

<!-- run -->

```c
#include <stdio.h>
#include <math.h>

int calc_x(double a, double b, double c, double *result){
    if (a == 0) return 0;
    double delta = b * b - 4 * a *c;
    if(delta < 0) return 0;
    double temp = sqrt(delta);
    result[0] = (-1 * b + temp) / (2 * a);
    result[1] = (-1 * b - temp) / (2 * a);
    return 1;
}

int main(){
    double a = 2, b = -20, c = 5;
    double result[2];
    if(calc_x(a,b,c,result))
        printf("%.2f %.2f", result[0], result[1]);
}
```

## Exam24
- 从键盘输入两个字串，判断它们是否相同。

<!-- run -->

```c
#include <stdio.h>

//= require my_strcmp

int main(){
    printf("%d\n", my_strcmp("abc",""));
    printf("%d\n", my_strcmp("","abc"));
    printf("%d\n", my_strcmp("abc","ab"));
    printf("%d\n", my_strcmp("ab","abc"));
    printf("%d\n", my_strcmp("abc","abd"));
    printf("%d\n", my_strcmp("abc","abc"));
    return 0;
}
```

## Exam25
- 从键盘输入两个字串，输出其中较短的那个字串，并输出它的长度。

<!-- run -->

```c
#include <stdio.h>

//= require my_strlen

int main(){
    char *str1 = "abcd";
    char *str2 = "abcdef";
    int len1 = my_strlen(str1);
    int len2 = my_strlen(str2);
    printf("%d %d %s\n", len1, len2, len1 <= len2 ? str1 : str2);
    return 0;
}
```

## Exam26
- 从键盘输入长度不等的两个字串，将短串连接于长串之后输出。

<!-- run -->

```c
#include <stdio.h>

//= require my_strlen
//= require my_strcat

int main(){
    char str1[20] = "abcd";
    char str2[20] = "123";
    int len1 = my_strlen(str1);
    int len2 = my_strlen(str2);
    printf("%d %d %s\n", len1, len2,
        len1 <= len2 ? my_strcat(str1, str2) : my_strcat(str2,str1));
    return 0;
}
```

## Exam27
- 从键盘输入长度不等的两个字串，将长串连接于短串之后输出。

<!-- run -->

```c
#include <stdio.h>

//= require my_strlen
//= require my_strcat

int main(){
    char str1[20] = "abcd";
    char str2[20] = "123";
    int len1 = my_strlen(str1);
    int len2 = my_strlen(str2);
    printf("%d %d %s\n", len1, len2,
        len1 <= len2 ? my_strcat(str2, str1) : my_strcat(str1,str2));
    return 0;
}
```

## Exam28
- 随机产生N个两位自然数，降序排列后输出。（设N为20）

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define Q_SORT_ELEM int

//= require q_sort
//= require print_arr

int my_cmp(int a, int b){
    return a == b ? 0 : (a > b ? -1 : 1);
}

void fill_data(int *arr, int size){
    srand(time(NULL));
    while(size > 0) arr[--size] = rand()%90 + 10;
}

int main(void) {
    int n = 10;
    int arr[n];
    fill_data(arr, n);
    q_sort(my_cmp, arr, n);
    print_arr(stdout, arr, n);

    return 0;
}
```

## Exam29
- 从键盘输入两个字串，输出其中较长的那个字串，并输出它的长度。

<!-- run -->

```c
#include <stdio.h>

//= require my_strlen

int main(){
    char *str1 = "abcd";
    char *str2 = "abcdef";
    int len1 = my_strlen(str1);
    int len2 = my_strlen(str2);
    printf("%d %d %s\n", len1, len2, len1 <= len2 ? str2 : str1);
    return 0;
}
```

## Exam30
- 从键盘输入10个战士的身高，输出平均身高，并找出哪些身高低于平均身高。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define REDUCE_CACHE int
#define REDUCE_ELEM int
#define FILTER_ELEM int

//= require reduce
//= require filter
//= require print_arr

int avg = 0;
int add(int acc, int num, int idx){return acc + num;}
int check(int num, int idx){return num < avg;}

int main(){
    int soldiers[] = {171, 172, 173, 174, 181, 183, 190, 190, 176, 168};
    int count = sizeof(soldiers) / sizeof(int);
    avg = reduce(add, soldiers, count, 0) / count;
    count = filter(check, soldiers, count);
    printf("average: %d", avg);
    print_arr(stdout, soldiers, count);

    return 0;
}
```

## Exam31
- 从键盘输入10个战士的身高，输出平均身高，并找出哪些身高高于平均身高。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define REDUCE_CACHE int
#define REDUCE_ELEM int
#define FILTER_ELEM int

//= require reduce
//= require filter
//= require print_arr

int avg = 0;
int add(int acc, int num, int idx){return acc + num;}
int check(int num, int idx){return num > avg;}

int main(){
    int soldiers[] = {171, 172, 173, 174, 181, 183, 190, 190, 176, 168};
    int count = sizeof(soldiers) / sizeof(int);
    avg = reduce(add, soldiers, count, 0) / count;
    count = filter(check, soldiers, count);
    printf("average: %d", avg);
    print_arr(stdout, soldiers, count);

    return 0;
}
```

## Exam32
- 从键盘输入10个战士的身高，输出最高、最低的身高。

<!-- run -->

```c
#include <stdio.h>

typedef struct{
    int min;
    int max;
} STAT;

#define REDUCE_CACHE STAT*
#define REDUCE_ELEM int

//= require reduce

STAT* pipe(STAT* acc, int num, int idx){
    if(num < acc->min) acc -> min = num;
    else if(num > acc->max) acc -> max = num;
    return acc;
}

int main(){
    int soldiers[] = {171, 172, 173, 174, 181, 183, 190, 190, 176, 168};
    int count = sizeof(soldiers) / sizeof(int);
    STAT stat = {soldiers[0], soldiers[0]};
    reduce(pipe, soldiers, count, &stat);
    printf("min: %d; max: %d\n", stat.min, stat.max);

    return 0;
}
```

## Exam33
- “百钱百鸡”问题。百钱买百鸡，鸡翁一值钱三，鸡母一值钱二，鸡雏三值钱一，问鸡翁、鸡母、鸡雏各几何？

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int count;
    int value;
} CHICKEN;

int main(){
    CHICKEN a = {1, 3};
    CHICKEN b = {1, 2};
    CHICKEN c = {3, 1};

    int target_count = 100;
    int target_value = 100;

    int i, j, k;
    for(i = 0; i < target_value/a.value + 1; i++){
      for(j = 0; j < (target_value-i*a.value)/b.value + 1; j++){
        for(k = 0; k < (target_value-i*a.value-j*b.value)/c.value + 1; k++){
          if(i * a.value + j * b.value + k * c.value == target_value &&
            i * a.count + j * b.count + k * c.count == target_count){
            printf("%3d %3d %3d\n", i * a.count, j * b.count, k * c.count);
            break;
          }
        }
      }
    }
    return 0;
}
```

- 递归版本，达到不限制循环次数，可增加任意规则

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

int TARGET_COUNT = 100;
int TARGET_VALUE = 100;
int COUNT = 0;

typedef struct{
    int cnt_male;
    int cnt_female;
    int cnt_children;
}STAT;

typedef struct
{
    int count;
    int value;
}RULE;

RULE rules[] = {{1,3}, {1,2}, {3,1}};

void _get_total(STAT stat, int *total){
    int i = 0, size = sizeof(rules) / sizeof(RULE);
    while(i < size){
        total[0] += ((int*)&stat)[i] * rules[i].count;
        total[1] += ((int*)&stat)[i] * rules[i].value;
        i++;
    }
}

int check_is_done(STAT stat){
    int total[] = {0,0};
    _get_total(stat, total);
    return total[0] > TARGET_COUNT || total[1] > TARGET_VALUE;
}

int check_accord(STAT stat){
    int total[] = {0,0};
    _get_total(stat, total);
    return total[0] == TARGET_COUNT && total[1] == TARGET_VALUE;
}

void push_result(STAT stat){
    printf("[%4d](", ++COUNT);
    int i = 0, size = sizeof(rules) / sizeof(RULE);
    while(i < size){
        printf("%4d", ((int*)&stat)[i] * rules[i].count);
        i++;
    }
    printf(")\n");
}

void product(int size, int idx, STAT stat){
    if (idx == size) return;
    while(!check_is_done(stat)){
        product(size, idx + 1, stat);
        ((int*)&stat)[idx] += 1;
        if(check_accord(stat))
            push_result(stat);
    }
}

int main(){
    int size = sizeof(rules) / sizeof(RULE);
    int stat[size];
    int i = 0;
    while(i < size) stat[i++] = 0;
    product(size, 0, *((STAT*)stat));
    return 0;
}
```

- 利用了结构体与数组互相指代，与Exam05有许多可重用部分，只是判定规则不同

## Exam34
- 有三个小孩，一个比一个大2岁，已知其年龄之和为39，问这三个小孩各几岁？

<!-- run -->

```c
#include <stdio.h>

int main(){
    int age = (39 - 6) / 3;
    printf("%d %d %d\n", age, age+2, age+4);
    return 0;
}
```

## Exam35
- 键盘输入长方体的长宽高l、w、h，求其体积及表面积。要求用自定义函数val(l,w,h)计算长方体的体积和表面积，计算结果使用全局变量带回主函数。

<!-- run -->

```c
#include <stdio.h>

void calc(double l, double w, double h, double *result){
    result[0] = (l * w + l * h + w * h) * 2;
    result[1] = l * w * h;
}

int main(){
    double result[2];
    calc(2,3,4,result);
    printf("%.2f %.2f\n", result[0], result[1]);
    return 0;
}
```

## Exam36
- 计算：1/2+1/4+……+1/n，设n是由键盘输入的正偶数。

<!-- run -->

```c
#include <stdio.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE double

//= require reduce
//= require range

double calc(double acc, int num, int idx){return acc + 1.0/num;}

int main(){
    int n = 6;
    int arr[n];
    int count = range(2, n+1, 2, arr);
    double ret = reduce(calc, arr, count, 0.0);
    printf("%.8f\n", ret);
    return 0;
}
```

## Exam37
- 计算：1/1+1/3+……+1/n，设n是由键盘输入的正奇数。

<!-- run -->

```c
#include <stdio.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE double

//= require reduce
//= require range

double calc(double acc, int num, int idx){return acc + 1.0/num;}

int main(){
    int n = 6;
    int arr[n];
    int count = range(1, n+1, 2, arr);
    double ret = reduce(calc, arr, count, 0.0);
    printf("%.8f\n", ret);
    return 0;
}
```

## Exam38
- 计算：1/2-2/3+3/4-4/5……，前100项。

<!-- run -->

```c
#include <stdio.h>

#define REDUCE_ELEM int
#define REDUCE_CACHE double

//= require reduce
//= require range

double calc(double acc, int num, int idx){
    if(num%2 == 1) return acc + num/(num+1.0);
    return acc - num/(num+1.0);
}

int main(){
    int n = 100;
    int arr[n];
    int count = range(1, n+1, 1, arr);
    double ret = reduce(calc, arr, count, 0.0);
    printf("%.8f\n", ret);
    return 0;
}
```

## Exam39
- 有一个数组{1,4,80,23,47,76,14,52}。现输入一个数，要求按原来的规律将它插入数组中并从大到小输出

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define Q_SORT_ELEM int

//= require q_sort
//= require print_arr

int my_cmp(int a, int b){
    return a == b ? 0 : (a > b ? -1 : 1);
}

int main(void) {
    int arr[] = {1,4,80,23,47,76,14,52};
    int count = sizeof(arr) / sizeof(int);
    q_sort(my_cmp, arr, count);
    print_arr(stdout, arr, count);

    return 0;
}
```

## Exam40
- 公安人员审问四名窃贼嫌疑犯。已知，这四人当中仅有一名是窃贼，还知道这四人中每人要么是诚实的，要么总是说谎的。在回答公安人员的问题中：甲说“乙没有偷，是丁偷的”，  乙说“我没有偷，是丙偷的”，丙说“甲没有偷，是乙偷的”，丁说“我没有偷”。请根据这四人的答话判断谁是盗窃者。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

typedef enum{
    IS_THIEF = -1,
    NOT_SURE = 0,
    NOT_THIEF = 1
} DESCRIBE;

typedef struct{
    DESCRIBE jia;
    DESCRIBE yi;
    DESCRIBE bing;
    DESCRIBE ding;
} SAY;

SAY _reverse(SAY say){
    SAY ret = say;
    int i;
    for(i = 0; i < 4; i++) ((int*)&ret)[i] *= -1;
    return ret;
}

int _check_rule(SAY say){
    int i, count=0;
    for(i=0; i<4; i++){
        if(((int*)&say)[i] == IS_THIEF) count++;
    }
    return count == 1; // only one thief
}

int check_said_if_person_is_thief(int person, SAY say){
    SAY say_if_real = say;
    ((int*)&say_if_real)[person] = IS_THIEF; //set thief

    SAY say_if_lie =  _reverse(say); // all real or all lies, so use reverse
    ((int*)&say_if_lie)[person] = IS_THIEF;

    return _check_rule(say_if_real) || _check_rule(say_if_lie);
}

const char* get_person_name(int person){
    const char *str[] = {"jia", "yi", "bing", "ding"};
    return str[person];
}

int main(){
    SAY says[] = {{NOT_SURE,NOT_THIEF,NOT_SURE,IS_THIEF}, {NOT_SURE,NOT_THIEF,IS_THIEF,NOT_SURE},
                 {NOT_THIEF,IS_THIEF,NOT_SURE,NOT_SURE}, {NOT_SURE,NOT_SURE,NOT_SURE,NOT_THIEF}};
    int says_count = sizeof(says) / sizeof(SAY);
    int person_count = sizeof(SAY) / sizeof(int);
    int person, says_idx;

    for(person = 0; person < person_count; person++){
        for(says_idx = 0; says_idx < says_count; says_idx++){
            if(!check_said_if_person_is_thief(person, says[says_idx]))
                break;
            if(says_idx == says_count-1)
                printf("%s\n", get_person_name(person));
            continue;
        }
    }
    return 0;
}
```

## Exam41
- 求s=a+aa+aaa+aaaa+......+aa...a的值，其中a是1~9之间的一个数字，表达式项数从键盘输入（设不超过10项）。例如2+22+222+2222+22222(此时共有5个数相加)，几个数相加由键盘控制。

<!-- run -->

```c
#include <stdio.h>

typedef struct{
    int num;
    long last;
    long sum;
}STAT;

#define REDUCE_ELEM int
#define REDUCE_CACHE STAT

//= require range
//= require reduce

STAT calc(STAT acc, int num, int idx){
    acc.last = acc.last * 10 + acc.num;
    acc.sum = acc.sum + acc.last;
    // printf("=%ld %ld %d\n", acc.last, acc.sum, acc.num);
    return acc;
}

int main(){
    int num = 5;
    int times = 5;
    int arr[times];
    range(0, times, 1, arr);
    STAT stat = {num,0,0};
    stat = reduce(calc, arr, times, stat);
    printf("%ld\n",stat.sum);
    return 0;
}
```

## Exam42
- 设N是一个四位数，它的9倍恰好是其反序数，求N。反序数就是将整数的数字倒过来形成的整数。例如：1234的反序数是4321。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define FILTER_ELEM int

//= require range
//= require filter
//= require print_arr

int reverse(int num){
    int ret = 0;
    while(num > 0){
        ret = ret * 10 + num % 10;
        num /= 10;
    }
    return ret;
}

int check(int num, int idx){
    return num * 9 == reverse(num);
}

int main(){
    int arr[2000];
    int count = range(1000, 9999/9 + 1, 1, arr);
    count = filter(check, arr, count);
    print_arr(stdout, arr, count);
    return 0;
}
```

## Exam43
- 爱因斯坦出了一道这样的数学题：有一条长阶梯，若每步跨2阶，则最后剩一阶，若每步跨3 阶，则最后剩2阶，若每步跨5阶，则最后剩4阶，若每步跨6阶则最后剩5阶。只有每次跨7阶，最后才正好一阶不剩。请问这条阶梯至少有多少阶？

<!-- run -->

```c
#include <stdio.h>

#define TAKE_ELEM int

//= require take_while

int move_next(int num, int idx){ return num + 7;}
int check_num(int num, int idx){ return num%2==1 && num%3==2 && num%5==4 && num%6==5;}
int main(){
    printf("%d", take_while(check_num, move_next, 0));
    return 0;
}

```

## Exam44
- 输入任意一行字符，降序排列之。

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

#define Q_SORT_ELEM char

//= require q_sort

int my_cmp(char a, char b){
    return a == b ? 0 : (a > b ? -1 : 1);
}

int main(void) {
    char str[] = "abefdcbgdf";
    q_sort(my_cmp, str, strlen(str));
    printf("%s", str);
    return 0;
}
```

## Exam45
- 输入一行字符串，按如下规则加密：如果是英文字母则大写变小写、小写变大写，对非英文字符则保持不变。试写加密程序。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAP_SRC_ELEM char
#define MAP_DEST_ELEM char

//= require map

char change(char ch, int idx){
    if(isupper(ch)) return ch + ('a' - 'A');
    if(islower(ch)) return ch - ('a' - 'A');
    return ch;
}

int main(){
    char *str = "abc123 67EFD a!";
    char *str2 = map(change, str, strlen(str) + 1);
    printf("%s\n", str2);
    free(str2);
    return 0;
}
```

## Exam46
- 输入一行小写英文字母，按如下规则加密：a→c、b→d、…、x→z、y→a、z→b，试写加密程序。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAP_SRC_ELEM char
#define MAP_DEST_ELEM char

//= require map

char change(char ch, int idx){
    if(isupper(ch)) return (ch - 'A' + 2) % 26 + 'A';
    if(islower(ch)) return (ch - 'a' + 2) % 26 + 'a';
    return ch;
}

int main(){
    char *str = "abc123 67EFD axyz XYZ!";
    char *str2 = map(change, str, strlen(str) + 1);
    printf("%s\n", str2);
    free(str2);
    return 0;
}
```

## Exam47
- 输入一行英文，已知各单词之间用1个空格相隔（设第一个单词前没有空格），统计这行英文有多少个单词。

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

typedef struct{
    int count;
    int before_is_space;
} STAT;

#define REDUCE_ELEM char
#define REDUCE_CACHE STAT*

//= require reduce

STAT* calc(STAT* acc, char ch, int idx){
    if(isspace(ch)){
        acc->before_is_space = 1;
        return acc;
    }
    else{
        if(acc->before_is_space)
            acc->count++;
        acc->before_is_space = 0;
        return acc;
    }
}

int main(){
    char *str = "this is new  line. with  some num:  123!";
    STAT stat = {0,1};
    reduce(calc, str, strlen(str), &stat);
    printf("%s\ncount: %d\n",str, stat.count);
    return 0;
}
```

## Exam48
- 求1+12+123+……前n项之和，其中n是从键盘输入的一个数字（1~9）。

<!-- run -->

```c
#include <stdio.h>

typedef struct{
    long last;
    long sum;
}STAT;

#define REDUCE_ELEM int
#define REDUCE_CACHE STAT

//= require range
//= require reduce

STAT calc(STAT acc, int num, int idx){
    acc.last = acc.last * 10 + num;
    acc.sum = acc.sum + acc.last;
    // printf("%ld %ld\n", acc.last, acc.sum);
    return acc;
}

int main(){
    int n = 5;
    int arr[n];
    range(1, n+1, 1, arr);
    STAT stat = {0,0};
    stat = reduce(calc, arr, n, stat);
    printf("%ld\n",stat.sum);
    return 0;
}
```

## Exam49
- 随机生成10道100以内的加减乘除数学题 回答正确的加10分错误不加分 然后显示成绩 。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

typedef int(*FUN)(int, int);

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

int rand_operate(int *operand1, int *operand2, char *operator){
    //static int(*funs[])(int, int) = {add, minus, multi, divi};
    static FUN funs[] = {add, minus, multi, divi};
    static char operators[] = {'+', '-',   '*',   '/'};
    int fun_count = sizeof(funs) / sizeof(FUN);
    *operand1 = rand() % 100;
    *operand2 = rand() % 100;
    int choice = rand() % fun_count;
    *operator = operators[choice];
    return funs[choice](*operand1, *operand2);
}

int main(){
    int score = 0, i, operand1, operand2, guess, ret;
    char operator;
    srand(time(NULL));
    for(i = 0; i < 10; i++){
        printf("\nquestion %d : ", i + 1);
        ret = rand_operate(&operand1, &operand2, &operator);
        printf("%d %c %d = ", operand1, operator, operand2);
        scanf("%d", &guess);
        if(guess == ret) {
            printf(" [right]");
            score += 10;
        }
        else{
            printf(" [error] correct is %d", ret);
        }
    }
    printf("\nscore is %d\n", score);
    return 0;
}
```

## Exam50
- 有1000发子弹 要提前装道10箱子里面，接收键盘输入，要取多少颗子弹数，只能显示整箱的个数，问这10个箱子怎么装（定义一个数组10个元素，分别装子弹的个数，比如取走100发子弹 程序运行结果，比如2箱）

> 可理解为，1000之内任意数可被N个箱子组合出来

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//= require print_arr

int* fill_box(int total, int *size){
    *size = log2(total) + 1;
    int *arr = malloc(sizeof(int) * (*size));
    int i, sum = 0, tmp;
    for(i = 0; i < *size; i++){
        if (i == 0) tmp = 1;
        else tmp = arr[i - 1] * 2;
        if(sum + tmp > total)
            tmp = total - sum;
        arr[i] = tmp;
        sum += tmp;
    }
    return arr;
}

int get_nearest_idx(int *arr, int size, int num){
    int i;
    for(i = 0; i < size; i++){
        if(arr[i] > num) return i-1;
    }
    return size-1;
}

void want(int *arr, int box_count, int want_num){
    int give, idx;
    printf("%d: ", want_num);
    while(want_num > 0){
        idx = get_nearest_idx(arr, box_count, want_num);
        give = arr[idx];
        printf("[%d](%3d) ", idx, give);
        want_num -= give;
    }
    printf("\n");
}

void want_2(int want_num){
    int n, give;
    printf("%d: ", want_num);
    while(want_num > 0){
        n = log2(want_num);
        give = pow(2, n);
        printf("[%d](%3d) ", n, give);
        want_num -= give;
    }
    printf("\n");
}

int main(){
    int box_count;
    int *arr =  fill_box(1000, &box_count);
    print_arr(stdout, arr, box_count);

    want(arr, box_count, 800);
    want_2(800);
    want(arr, box_count, 700);
    want(arr, box_count, 500);
    want(arr, box_count, 300);
    want_2(300);
    want(arr, box_count, 117);

    free(arr);
    return 0;
}
```

火车订票系统
==========
[Code in Github](https://github.com/zyxstar/exam_c/tree/master/train)

1. 用户注册功能
    - 注册新用户，输入用户名密码。
        - 用户名检测，用户名重复，重新注册。
        - 用户注册成功。
2. 用户登录
    - 检测用户名和密码是否正确，正确，登录成功，否则失败。
3. 后台管理
    - 列车信息录入
4. 列车时刻表查询
    - 车次查询: 输入车次，列出本次车所有到站及价格
    - 发到站查询: 输入发站 输入到站，列出所有车次及价格
5. 订票功能
    - 输入发车站，输入到站，列出所有车次，同时显示车上是否有票
    - 订票： 显示座位号，订票成功，订票信息记入个人账户


实现说明

- 基础 __helper工具__ 类，与具体应用无关，可重用，在`..\utils\`中
- __通用类型__ 的`LIST`泛型的实现，与具体应用无关，可重用，在`..\utils\list.c`中
    + 在它的查找、释放空间、读写文件时，不确定的部分以回调函数的方式，依赖调用方传入相应逻辑
- 具体 __模型__ `USER`，`TRAIN`都依赖于`LIST`来实现，它们只有逻辑部分，不包含UI交互
- 依赖`LIST`实现了`MENU`(`..\utils\menu.c`)的相关逻辑，达到在该项目中，对菜单项目配置可扩展性
- __视图逻辑__ 在最上层，放置在`app.c`中，它们依赖于应用逻辑，注册到`MENU`的回调函数中
- 编写应用主函数，存放会话所需的数据，避免放置到全局空间，展示`MENU`入口即可

学生管理系统
==========
[Code in Github](https://github.com/zyxstar/exam_c/tree/master/school)

- 能够管理一个学校的学生
- 学校的年届可以增加和删除
- 每一年级的班级可以增加和删除
- 每个班级的学生也可以增加和删除
- 对班级的学生可以根据学号和姓名进行查找
- 对班级的学生分数具有排序功能
- 对同一年级的学生具有按成绩排序的功能
- 要求使用有头双向循环链表完成
- 能够把年级，班级，学生存储到文件中，下次打开系统可以直接载入文件。
- 每次修改都必须对文件进行保存

实现说明

- 本项目使用内核链表来实现
- `STUDENT`只有一个`struct list_head`，用于编成链，`CLASS`，`GRADE`均有两个`struct list_head`，一个表示下级的链表，另一个用于自身编成链
- 用 __宏__(`../utils/kernel_list_helper.h`) 封装了内核链表在 `each`、`find`、`save`、`load`方面的通用实现(因为如果用函数封装，不能模拟泛型的要求)

> 使用如下，第一行是声明，第二行是实现，将来宏会被展开

```c
// declare
void class_each_student(CLASS *, void *env, void(*each_do)(STUDENT *, int idx, void *env));
// implement
_implement_list_each(class_each_student, CLASS, student_list, STUDENT, node);
```

- 排序功能使用了标准库的`qsort()`，返回的是一个指向`STUDENT *`的数组，相当于一个视图，并未真正改变他们在各自班级中的顺序，盖因为排序的依据是可变的，以这种视图的方式呈现可能更好些

- 简单的实现了一个事件机制(位于`../utils/event.h`，内部的事件列表也是使用内核链表实现的)，`CLASS`中`STUDENT`的增删能够被`GRADE`所监听，后者可据此做一些事情（如增减学生的计数）


俄罗斯方块
==========
[Code in Github](https://github.com/zyxstar/exam_c/tree/master/tetris)

- 基础helper工具类，与具体应用无关，可重用，在`..\utils`中
    + `console.c`的实现，用于输出调试信息
    + `simple_timer.c`的实现，实现了可注册多个时间响应函数，响应函数可接受一个`void*`的数据，由于C不存在内嵌函数及闭包，必要的上下文则由该参数来保持（该上下文由创建者来指定）
    + `vt_code`，`termios`的相关封装，参考了网上的例子
- __模型层__：`BLOCK`，`GAME`的实现，它们只有逻辑部分，不包含UI交互
    + 它们可被单独测试，测试代码在`test_game.c`中
    + 它们不依赖于具体的UI技术，比如使用qt，只需能完成`GAME_UI`适配即可
    + 高层函数，即开放在`game.h`中的，抽象一致，并且所有逻辑判断不超过两层，基本达到可读性
    + 计分规则`_calc_score`、升级规则`_check_level_up`、加速规则`_speed_up`等由单独函数来完成，方便修改
    + <del>使用了`mutex`，防止在手工的与`timer`激发的`move_down`动作同时发生时，产生资源竞争，导致数据不一致</del>采用信号屏蔽，防止资源竞争
    + 如`POS *set = (POS*)(&pos_set);`，采用结构体，利用了它能被整体赋值的优势，必要时，将它再强制转换成数组，方便数据计算
    + 使用函数指针数组`void(*config_set_fn[])(POS_SET*)`，充当路由的功能，类似高级语言中的多态，避免了过多的`swich/if`
    + 原则上来说，`GAME`中不能依赖`SIMPER_TIMER`（破坏了可测性），而在控制层将两者联系到一起，达到解耦，但此处选择了相对方便的方式将两者耦合在一起了
- __视图层__：`view.c`存放具体的UI绘制的部分，它依赖于工具类中的`vt_code`
    + 它也可被单独运行，在`test_view.c`中用于测试单个绘制是否有效
- __控制层__：
    + `ctrl.c`，接收UI的交互（键盘输入），并将行为委托给模型层的具体方法。
    + 模型层发生的变化，借由`GAME_UI`来绘制UI，后者承担`GAME`与`view`之间的联系，由一系列函数指针组成的结构体，充担高级语言中的接口的概念
- 避免了全局变量的存在，方便在单人应用`tetris_single`与双人应用`tetris_double`使用同一套逻辑

实现ls(1)
===========
[Code in Github](https://github.com/zyxstar/exam_c/blob/master/apue/study/myls.c)

- 具备`-l`,`-i`,`-a`的选项，并且可组合配置，如`-li`,`-al`
- 要求根据环境变量`LS_COLORS`，使用来颜色打印文件名

函数调用关系

```
main()
    parse_argv()
    load_ls_colors()
    trave_dir()
        foreach:
            process_file()
                get_file_type()
                get_file_perm()
                get_file_username()
                get_file_groupname()
                get_file_time()
                get_link_filenm()
                get_file_colors()
                    get_ls_color_item()
        show_ls_entries()
            qsort()
            foreach:
                print_ls_entry()

```

实现说明

- 解释选项，并存储到位图中，并定义了一个判断选项位的宏`HAS_BIT`
- 加载环境变量`LS_COLORS`，使用`strtok_r`来分解分割符
- > 使用`strtok_r`分割后，会影响被分割的字符串，所以会影响再次使用，并且该字符串不能存在只读区
- 使用`setjmp/longjmp`来捕获异常，确保异常时，资源也得到回收
- > 在`setjmp`函数内部，如果是异常返回来的，并且需要使用的变量，最好是`static`的
- 使用内核链表来存储和`qsort`来排序
- 注意`localtime`重复调用时，会覆盖上次指针指向内容




<script>

(function(){
    if(typeof expand_toc !== 'function') setTimeout(arguments.callee,500);
    else expand_toc('md_toc',1);
})();

</script>
