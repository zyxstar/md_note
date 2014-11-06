> 2014-11-01

回到索引
=======
- [嵌入式培训索引(Uplooking).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2F%25E5%25B5%258C%25E5%2585%25A5%25E5%25BC%258F%25E5%259F%25B9%25E8%25AE%25AD%25E7%25B4%25A2%25E5%25BC%2595%2528Uplooking%2529.md)

> 参考
>
> - [课件C.ppt](../../data/C.ppt)
> - [vim配置](../../data/vimrc)
> - [编程范式(stanford_cs107).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E7%25BC%2596%25E7%25A8%258B%25E8%258C%2583%25E5%25BC%258F%2528stanford_cs107%2529.md)
> - [征服C指针(前桥和弥).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E5%25BE%2581%25E6%259C%258DC%25E6%258C%2587%25E9%2592%2588%2528%25E5%2589%258D%25E6%25A1%25A5%25E5%2592%258C%25E5%25BC%25A5%2529.md)


基础测试
==========
## Test01
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

## Test02
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

- 最好使用`char str1[ARR_SIZE] = "abcd"`，将字符串分配在栈中；如果使用`strdup()`分配在堆中，有可能覆盖后面的空间；如果分配在只读空间，将报错

## Test03
- 写一个函数，能够将两个整型指针的指向交换

<!-- run -->

```c
#include <stdio.h>

void swap(int*, int*);
int main(){
    int a = 10, b = 20;
    int *p1 = &a, *p2 = &b;
    swap(p1, p2);
    printf("a=%d; b=%d", a, b);
    return 0;
}

void swap(int* p1, int* p2){
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}
```

## Test04
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


Day01
========
## 变量与内存结构
- 内存靠寻址来查找数据，一个地址只对应一个字节（8位）
- 变量用来管理和使用内存，引用相应数据类型所需内存的首地址
    - 变量就是内存中已命名的存储空间，通过引用变量就可以引用保存在该内存中的数据
- 数据类型是对变量管理内存的范围（大小）做了规范
    - `/usr/include/limits.h` 包含常用数据类型的最大最小值

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

Day02
=========
## 进制转换
- 十进制转任意进制

<!-- run -->

```c
#include <stdio.h>
#include <string.h>

void reverse(char *data){
    int i = 0, len = strlen(data);

    char temp;
    while(i < len/2){
        temp = data[i];
        data[i] = data[len - i - 1];
        data[len - i - 1] =  temp;
        i++;
    }
}
char alpha(int num){
    if (num < 10) return 48 + num;/* 0~9 */
    return (num - 10) + 65; /* A~F */
}
void trans(int num, int radix, char *out_data){
    int i = 0;
    while(num > 0){
        out_data[i++] = alpha(num % radix);
        num /= radix;
    }
    out_data[i] = '\0';
    reverse(out_data);
}

int main(){
    int num = 78;
    char data[100];

    trans(num, 2, data);
    printf("bin: %s\n", data);

    trans(num, 8, data);
    printf("oct: %s\n", data);

    trans(num, 16, data);
    printf("hex: %s\n", data);

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
> [参考cs_107](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgithub.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E7%25BC%2596%25E7%25A8%258B%25E8%258C%2583%25E5%25BC%258F%2528stanford_cs107%2529.md#TOC2.3)，采用补码，还有一个原因是方便`0`的表达

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


Day03
=========



50道题
=========
## 可重用函数
下面的解题思路，使用了一些函数式的方式，将共用的函数提炼出来

> 1. 代码更多从扩展性、重用性考虑，有些代码是牺牲了性能的，有没有坑，请看官自行判断
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
MAP_DEST_ELEM* map(MAP_DEST_ELEM(*callback)(MAP_SRC_ELEM, int), MAP_SRC_ELEM *arr, int size){
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

// take_while
int take_while(int(*predicate)(int), int(*move_next)(int), int init){
    int data = init;
    while(!(predicate(data))){
        data = move_next(data);
    }
    return data;
}

// print_arr
void print_arr(FILE *fp, int *arr, int size){
    fprintf(fp, "[");
    int i = 0;
    while(i < size){
        fprintf(fp, "%6d ", arr[i++]);
        if(i%8 == 0) fprintf(fp, "\n ");
    }
    fprintf(fp, "]\n");
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

- 如果加变成乘，只需新写一个`multi`函数

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

- 重用了Exam03的累加，新增了过滤函数，但过滤条件是可以指定的

## Exam04
- 从终端（键盘）将5个整数输入到数组a中，然后将a逆序复制到数组b中，并输出b中各元素的值。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

#define ARR_SIZE 5

//= require print_arr

void reverse_arr(int *arr1, int *arr2, int size){
    int i;
    for(i = 0; i < size; i++){
        arr2[i] = arr1[size - i - 1];
    }
}

int main(){
    int arr1[] = {1,2,3,4,5};
    int arr2[ARR_SIZE];

    reverse_arr(arr1, arr2, ARR_SIZE);
    print_arr(stdout, arr2, ARR_SIZE);
    return 0;
}

```

## Exam05
- 要将5张100元的大钞票，换成等值的50元，20元，10元，5元一张的小钞票，每种面值至少1张，编程求需要多少张纸币。

<!-- run -->

```c

```



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
#include <malloc.h>
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
    for(i = 0; i < status_size ; i++){
        MEMBERSTATUS s = status[i];
        if(s.count == 1){
            is_redo = 1;
            s.my_couples[0]->is_chosen = CHOSEN;
            new_status_size -= 2;
            int nm = s.my_couples[0]->mem1;
            if(nm == s.name) nm = s.my_couples[0]->mem2;

            MEMBERSTATUS *other = get_status_by_name(status, status_size, nm);
            for(j = 0; j < other->count; j++){
                if(other->my_couples[j]->mem1 == s.name || other->my_couples[j]->mem2 == s.name)
                    continue;
                other->my_couples[j]->is_chosen = ABANDON;
            }
        }
    }
    free(status);
    if(is_redo) process(couples, couples_size, new_status_size);
}

void print_couples(FILE *fp, COUPLE *couples, int couples_size){
    int i;
    for (i = 0; i < couples_size; i++){
        if(couples[i].is_chosen == CHOSEN )
            fprintf(fp, "%c vs %c\n", couples[i].mem1, couples[i].mem2);
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

## Exam16
- 用for编程找出100~200中的完全平方数。

## Exam17
- 从终端输入三个正实数，判断这三个数能否构成直角三角形。

## Exam18
- 输入一行字符，分别统计出其中英文字母、空格、数字和其它字符的个数。

## Exam19
- 输入一个字串，判断它是否是对称串。如”abcdcba”是对称串，”123456789”不是。

## Exam20
- 随机产生N个大写字母输出，然后统计其中共有多少个元音字符。（设N为50）

## Exam21
- 找出三位自然数中的所有素数，要求判断x素数用自定义函数data(x)实现。

## Exam22
- 输出n行由“#”组成的矩形，每行“#”的个数为m个，其中n、m由终端（键盘）输入。要求输出m个“#”功能由函数satr(m)函数实现。

## Exam23
- 若ax2+bx+c=0有实根，则求之。

## Exam24
- 从键盘输入两个字串，判断它们是否相同。

## Exam25
- 从键盘输入两个字串，输出其中较短的那个字串，并输出它的长度。

## Exam26
- 从键盘输入长度不等的两个字串，将短串连接于长串之后输出。

## Exam27
- 从键盘输入长度不等的两个字串，将长串连接于短串之后输出。

## Exam28
- 随机产生N个两位自然数，降序排列后输出。（设N为20）

## Exam29
- 从键盘输入两个字串，输出其中较长的那个字串，并输出它的长度。

## Exam30
- 从键盘输入10个战士的身高，输出平均身高，并找出哪些身高低于平均身高。

## Exam31
- 从键盘输入10个战士的身高，输出平均身高，并找出哪些身高高于平均身高。

## Exam32
- 从键盘输入10个战士的身高，输出最高、最低的身高。

## Exam33
- “百钱百鸡”问题。百钱买百鸡，鸡翁一值钱三，鸡母一值钱二，鸡雏三值钱一，问鸡翁、鸡母、鸡雏各几何？

## Exam34
- 有三个小孩，一个比一个大2岁，已知其年龄之和为39，问这三个小孩各几岁？

## Exam35
- 键盘输入长方体的长宽高l、w、h，求其体积及表面积。要求用自定义函数val(l,w,h)计算长方体的体积和表面积，计算结果使用全局变量带回主函数。

## Exam36
- 计算：1/2+1/4+……+1/n，设n是由键盘输入的正偶数。

## Exam37
- 计算：1/1+1/3+……+1/n，设n是由键盘输入的正奇数。

## Exam38
- 计算：1/2-2/3+3/4-4/5……，前100项。

## Exam39
- 有一个数组{1,4,80,23,47,76,14,52}。现输入一个数，要求按原来的规律将它插入数组中并从大到小输出

## Exam40
- 公安人员审问四名窃贼嫌疑犯。已知，这四人当中仅有一名是窃贼，还知道这四人中每人要么是诚实的，要么总是说谎的。在回答公安人员的问题中：甲说“乙没有偷，是丁偷的”，  乙说“我没有偷，是丙偷的”，丙说“甲没有偷，是乙偷的”，丁说“我没有偷”。请根据这四人的答话判断谁是盗窃者。

## Exam41
- 求s=a+aa+aaa+aaaa+......+aa...a的值，其中a是1~9之间的一个数字，表达式项数从键盘输入（设不超过10项）。例如2+22+222+2222+22222(此时共有5个数相加)，几个数相加由键盘控制。

## Exam42
- 设N是一个四位数，它的9倍恰好是其反序数，求N。反序数就是将整数的数字倒过来形成的整数。例如：1234的反序数是4321。

## Exam43
- 爱因斯坦出了一道这样的数学题：有一条长阶梯，若每步跨2阶，则最后剩一阶，若每步跨3 阶，则最后剩2阶，若每步跨5阶，则最后剩4阶，若每步跨6阶则最后剩5阶。只有每次跨7阶，最后才正好一阶不剩。请问这条阶梯至少有多少阶？

<!-- run -->

```c
#include <stdio.h>

//= require take_while

int move_next(int num){ return num + 7;}
int check_num(int num){ return num%2==1 && num%3==2 && num%5==4 && num%6==5;}
int main(){
    printf("%d", take_while(check_num, move_next, 0));
    return 0;
}

```

## Exam44
- 输入任意一行字符，降序排列之。

## Exam45
- 输入一行字符串，按如下规则加密：如果是英文字母则大写变小写、小写变大写，对非英文字符则保持不变。试写加密程序。

## Exam46
- 输入一行小写英文字母，按如下规则加密：a→c、b→d、…、x→z、y→a、z→b，试写加密程序。

## Exam47
- 输入一行英文，已知各单词之间用1个空格相隔（设第一个单词前没有空格），统计这行英文有多少个单词。

## Exam48
- 求1+12+123+……前n项之和，其中n是从键盘输入的一个数字（1~9）。

## Exam49
- 随机生成10道100以内的加减乘除数学题 回答正确的加10分错误不加分 然后显示成绩 。

<!-- run -->

```c
#include <stdio.h>
#include <stdlib.h>

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

int rand_operate(int *operand1, int *operand2, char *operator){
    int(*fun)(int, int) = NULL;
    *operand1 = rand() % 100;
    *operand2 = rand() % 100;
    switch(rand() % 4){
        case 0: {fun = add; *operator = '+'; break;}
        case 1: {fun = minus; *operator = '-'; break;}
        case 2: {fun = multi; *operator = '*'; break;}
        default: {fun = divi; *operator = '/';}
    }
    return fun(*operand1, *operand2);
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
            printf(" [error] right result is %d", ret);
        }
    }
    printf("\nscore is %d\n", score);
    return 0;
}

```


## Exam50
- 有1000发子弹 要提前装道10箱子里面，接收键盘输入，要取多少颗子弹数，只能显示整箱的个数，问这10个箱子怎么装（定义一个数组10个元素，分别装子弹的个数，比如取走100发子弹 程序运行结果，比如2箱）





