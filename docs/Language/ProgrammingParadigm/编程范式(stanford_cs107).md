Lecture 1
========
概述

Lecture 2
========
## 数据的底层表示
C/C++关于int，float等数据的底层表示

<!--language: plain-->

    char       1 byte
    short      2 bytes
    int        4 bytes
    long       4 bytes
    long long  8 bytes
    float      4 bytes
    double     8 bytes

1byte = 8种bit的组合，即共有2的8次方这么多种选择。

## 二进制计算对应规则

<!--language: plain-->

    1    1    1    1       1    1    1    1
    128  64   32   16      8    4    2    1
    2^7  2^6  2^5  2^4     2^3  2^2  2^1  2^0

## 负数的表示
如果用反码来表示负数，试计算short数据类型的`15+(-15)`

<!--language: plain-->

    15:  00000000 00001111
    -15: 11111111 11110000
    0:   11111111 11111111

导致0有两种表示，更好的方式是使用补码，取反加1

<!--language: plain-->

    15:  00000000 00001111
    -15: 11111111 11110001
    0:  100000000 00000000

正负数相加时，因为先取反，则两者&时，会出现16位1，再加1，则发生多米诺效应，第17位进位丢失，留下16位0。同时负数取反加1即为它的绝对值（正数）

## 类型转换

### 非负数的转换

- 多byte转成少byte时，发生 __截断__(丢失高位)

<!--language: !c-->

    #include <stdio.h>

    int main(){
        short s = 321; /*00000001 01000001*/
        char ch = s;
        printf("%c", ch);
    }

- 少byte转成多byte时，__原值按位拷贝，高位用0补__

<!--language: !c-->

    #include <stdio.h>

    int main(){
        char ch = 'A'; /*01000001*/
        short s = ch;
        printf("%d", s);
    }

### 负数转换

- 多byte转成少byte时（截位最高位为1时），情况不确定(window/linux C是 __截断__，成为 __负数__ )

<!--language: !c-->

    #include <stdio.h>

    int main(){
        int i = 98305; /*00000000 00000001 10000000 00000001*/
        short s = i;
        printf("%d", s);
    }

- 少byte转成多byte时，__原值按位拷贝，高位用1补__（可以和上面[非负数的少转多]统一描述为不足的高位以符号位补[称为 __符号扩展__]）short s=-1; int i=s;

<!--language: !c-->

    #include <stdio.h>

    int main(){
        short s = -1; /*11111111 11111111*/
        int i = s;
        printf("%d", i);
    }

### 整型与浮点型转换

- 直接进行赋值，而类型不同，因此会先计算出来值，然后转换一种类型表示出来，__底层byte pattern发生变化__

<!--language: !c-->

    #include <stdio.h>

    int main(){
        short s = 5;
        float f = s;
        printf("%f", f);
    }

### 将内存中byte强制为某类型
总的来说就是 __字节拷贝__，大小尾系统 __没有__ 区别，因为 __地址&总指向 *最低* 字节__

- `int i = 98305; short s = *(short*)&i;` 保留低位，丢失高位

<!--language: !c-->

    #include <stdio.h>

    int main(){
        int i = 98305; /*00000000 00000001 10000000 00000001*/
        short s = *(short*)&i;
        printf("%x %d", s, s); /*ffff8001 -32767*/
    }

- `int i = 1078523331; float f = *(float*)&i;` 意义完全不同，变成极小的数(见下节float表示)

<!--language: !c-->

    #include <stdio.h>

    int main(){
        int i = 1078523331; /*01000000 01001000 11110101 11000011*/
        float f = *(float*)&i;
        printf("%f", f); /*3.14*/
    }

> ps. 如何得到`i = 1078523331`

<!--language: !c-->

    #include <stdio.h>
    union ufloat {
        float f;
        int i;
    };

    int main(){
        union ufloat uf;
        uf.f = 3.14;
        printf("%d", uf.i);
    }

- `double d = 3.1416; char c = *(char*)&d;` 保留1byte低位，其余丢失

<!--language: !c-->

    #include <stdio.h>

    int main(){
        double d = 3.1416;
        char c = *(char*)&d;
        printf("%c", c);
    }

- `short s = 45; double d = *(double*)&s;` 比较危险，越界访问s后面6byte空间

## float的表示
3部分构成：表示为`(-1)^s * 1.f * 2^(e-127)`

<!--language: table-->

    |               |长度   |表示为 |说明                        |
    |---------------|-------|-------|----------------------------|
    |符号位sign     |1 bit  |__s__  |                            |
    |指数Exponent   |8 bits |__e__  |(2^(e-127))  范围为-127~128 |
    |有效位Mantissa |23 bits|__f__  |(2^-1 + 2^-2 + ...)         |

示例：

<!--language: plain-->

    0 00010110 01100000000000000000000
    =
    (-1)^0 * 1.(2^-2+2^-3) * 2^(2^1+2^2+2^4-127)

如何分解float，假如7.0，则需要确保f为1.XXX，指数位为-127~128之间

<!--language: plain-->

    7.0
    =
    7.0 * 2^0
    =
    3.5 * 2^1
    =
    1.75 * 2^2

> double有11位exp，偏移量为1023

> 大尾存储就是最重要的数存在前部（低位），即数据的高位，保存在内存的低地址中（地址的增长顺序与值的增长顺序相反）。一双字87654321H，则大尾存储就是 87H 65H 43H 21H ，存储地址递增，判断当前系统大小尾

<!--language: !c-->

    #include <stdio.h>

    union data
    {
        short inter;
        char ch;
    };

    int main(void)
    {
        union data c;
        c.inter = 0x1122;
        if(c.ch == 0x22)
            printf("The compute is little-endian.\n");
        else
            printf("The compute is big-endian,\n");
    }


Lecture 3
========
## 结构体内存访问

<!--language: !c-->

    #include <stdio.h>

    typedef struct {
        int num;
        int denom;
    } fraction;

    int main(){
        fraction pi;
        pi.num = 22;
        pi.denom = 7;

        ((fraction *)&(pi.denom))->num = 12;
        ((fraction *)&(pi.denom))->denom = 33;
        printf("%d\n", pi.denom);
        printf("%d\n", (&pi)[1].num);
        printf("%d", (&pi+1)->num);
    }

- 当定义一个`fraction pi;`时，`pi`是个变量名，当声明后，得到存储一个`fraction`类型的内存空间，8bytes，这些成员尽可能紧密的打包起来，而 __整体的地址，与第0个域的地址一致__。

<!--language: plain-->

             ┌──────────┐
             │    33    │ ((fraction *)&(pi.denom))->denom
             ├──────────┤
    pi.denom │   7/12   │ ((fraction *)&(pi.denom))->num
             ├──────────┤
    pi.num   │    22    │
    &base -> └──────────┘

- 除非知道上下文，否则并不知道`num`存储在什么地方，当执行`pi.num=22;`时，将在基地址处存入22，`pi.denom=7;`将在 __基地址之上4字节__ 存入7。
- 接下来进行`(fraction *)&(pi.denom)`，`&(pi.denom)`得到一个`int*`类型，但现在并 __不__ 被理解成一个单独的整数了，而是地址空间为8bytes的fraction类型，则基地址向高位前进4字节，并 __越界__ 了4字节，原先denom现在成了12

## 数组内存访问
`int array[10];`声明它时，将得到40bytes的空间

- `array === &array[0]`，__数组本身与数组的第0个元素的地址等同__，其实当你传递数组给函数参数时，只是把第0个元素的地址指定了，然后再传入数组的长度
- `array[10]=1;`不会进行数组边界检查，会把它放在 __基地址偏移10*sizeof(int)字节__ 的地方(有可能崩溃)，甚至可以接受负数，表示基地址向低地址偏移
- `array+k === &array[k]`，array理解为基地址，k是int类型，参与指针算术运算，但基地址并不是直接将k加上，而是 __根据数组元素类型自动扩大寻址类型的大小(`sizeof(int)`)的k倍__，这就是上面fraction示例中`(&pi)[1].num`应用的 __原理，虽然pi _不是数组_，但同样可以使用该方式来访问内存__
- `*array === array[0]`，`*(array+k)===array[k]`，一个`array[-4]=77;`将被解引用成`*(array-4)=77;`即基地址`-sizeof(int)*4`处容纳77这个数

## 地址与指针类型

<!--language: !c-->

    #include <stdio.h>

    int main(){
        int arr[5];
        arr[3] = 128; /*00000000 10000000*/

        ((short*)arr)[7] = 2;
        printf("%d\n", arr[3]); /*00000000 00000010 00000000 10000000*/

        ((short *)(((char *)(&arr[1]))+8))[3] = 100;
        printf("%d\n", ((short*)arr)[9]);
    }

> ps 大小尾有区别，小尾系统中`((short*)arr)[7]=2;`，大尾的则为`((short*)arr)[6]=2;`

- `(short*)arr`将认为有10个short空间，第7的位置修改了原先基于int*时的第3位置的数据。
- `
((short *)(((char *)(&arr[1]))+8))[3]=100;`可以非常灵活的控制需要访问的地址空间

<!--language: plain-->

    0       1       2       3       4       5
    ┌───────┬───────┬───────┬───────┬───────┐
    └───────┴───────┴───────┴───────┴───────┘
            ^
            |
            (char *)(&arr[1]) 即地址&arr[1] 理解成 char* 类型

            0 1 2 3 4 5 6 7 8
            ┌─┬─┬─┬─┬─┬─┬─┬─┐
            └─┴─┴─┴─┴─┴─┴─┴─┘
                            ^
                            |
                            (char *)(&arr[1]))+8，sizeof是基于char的

                            0   1   2   3   4
                            ┌───┬───┬───┬───┐
                            └───┴───┴───┴───┘
                            ^           ^
                            |           |
                            此地址理解成short*
                                        |
                                        该地址存入100


- 结构体、数组、强制类型等内存访问技巧还可以用来访问活动记录中其他的变量

## 结构体数组

<!--language: !c-->

    #include <stdio.h>
    #include <malloc.h>
    #include <string.h>

    typedef struct {
        char *name;
        char suid[8];
        int numUnits;
    } student;

    int main(){
        student pupils[4];
        pupils[0].numUnits = 21;
        pupils[2].name = strdup("Adam");
        pupils[3].name = pupils[0].suid + 6;
        strcpy(pupils[1].suid, "40415xx");
        strcpy(pupils[3].name, "123456");

        printf("%d\n", pupils[0].numUnits);
        free(pupils[2].name);
    }

- 定义了一个16bytes的结构体，name 4bytes + suid 8bytes + numUnits 4bytes。

<!--language: plain-->

             0       1       2       3
             ┌───────┬───────┬───────┬───────┐
    numUnits │  21   │       │       │       │
             ├─┬─┬─┬─┼─┬─┬─┬─┼─┬─┬─┬─┼─┬─┬─┬─┤
             │ │ │a│ │5│x│x│\│ │ │ │ │ │ │ │ │
             ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
    stuid    │ │ │ │ │4│0│4│1│ │ │ │ │ │ │ │ │
             ├─┴─┴─┴─┼─┴─┴─┴─┼─┴─┴─┴─┼─┴─┴─┴─┤
    name     │       │       │   *   │   *a  │
             └───────┴───────┴───┼───┴───────┘
                                 │
                                 │  heap
                                 │  ┌─┬─┬─┬─┬──┐
                                 │  │A│d│a│m│\0│
                                 └─>└─┴─┴─┴─┴──┘

- pupils是 __局部变量__，将存在 __*栈*__ 中，`strdup("Adam")`是 __动态分配 _堆_ __的内存，并将地址返回给pupils[2].name，该内存是需要手工free的
- `pupils[0].suid + 6`，是上图中a所在的地址，6是基于suid数组元素类型char的长度
- `strcpy(pupils[1].suid, "40415xx");`并不申请内存，在第一个参数指定的内存地址，写入字符串。
- `strcpy(pupils[3].name, "123456");`pupils[3].name引用的地址是图中a，写入字符串，将对pupils[0].numUnits空间造成破坏，甚至影响到pupils[1]的数据。

Lecture 4
========

## swap int版本
首先实现一个基于int的版本，两个整型，__无论是包括在数组中，或结构体中__，都能被正确的交换

<!--language: !c-->

    #include <stdio.h>

    void swap(int *ap, int *bp) {
        int temp = *ap;
        *ap = *bp;
        *bp = temp;
    }

    int main(){
        int x=10, y=7;
        swap(&x,&y);
        printf("%d %d\n", x, y);
    }

- temp声明了一个4bytes的空间，将ap盒子中的内容复制过来，机器能辩明ap盒子中也是4bytes的内存布局
- 这里的空间大小都是隐式的，被限定为4bytes，如果要转换double，或自定义的结构体是做不到的

## 错误的swap泛型版本
接下来实现类似C++中泛型模板的方式

<!--language: c-->

    void swap(void *vp1, void *vp2){
        void temp = *vp1;
        *vp1 = *vp2;
        *vp2 = temp;
    }

- vp1和vp2指向需要交换的地址的起始位置，一个通用地址，也许是int,char,结构体等
- 仿造int的版本写法，但是有错误：
    1. 不能声明void型的temp变量
        - void用在函数返回值，声明没有什么可返回:`void fun(){}`
        - void可作为函数指针:`void* addr = &i;`
        - void作为方法的唯一参数说明:`int main(void){}`
    1. 不能对vp1进行解引用，因为没类型信息，机器不知道要提取多少个字节

## 正确的swap泛型版本

<!--language: !c-->

    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>

    void swap(void *vp1, void *vp2, int size){
        char buffer[size];
        /* char* buffer = malloc(size);*/
        memcpy(buffer, vp1, size);
        memcpy(vp1, vp2, size);
        memcpy(vp2, buffer, size);
        /* free(buffer);*/
    }

    int main(){
        int x=10, y=7;
        swap(&x,&y,sizeof(int));
        printf("%d %d\n", x, y);

        double a = 23.0, b = 34.0;
        swap(&a, &b, sizeof(double));
        printf("%lf %lf", a, b);
    }

- 比上个版本多出size参数，表示需要交换的byte长度
    - 不需要const修饰，参数上使用const更多用于共享信息时，起到保护作用，防止被意外修改
    - size参数可以传入一个常数，但可读性不好，而用`sizeof(int)`则明确的指出了 __需要转换的数据类型__
- `char buffer[size]`，现在版本编译器 __允许声明一个大小依赖于参数的数组__
    - 虽然声明成char，其实只是利用该字符缓冲区来留出足够内存空间存放size大小的字节，该空间位于 __栈__ 中（buffer是局部变量），函数运行后空间 __自动释放__
    - vp1并不介意解释成字符串，只是作为一个存储空间，并不利用该buffer去做字符相关操作
    - 如果不支持该 __非常数__ 作为数组长度的声明，可使用`malloc`在 __堆__ 中创建一个缓冲区，并记得最后 __释放__
- `memcpy`进行内存单元的拷贝，注意此函数并不关心你的数据类型，单纯的进行单元的拷贝而已，所以虽然编译可能通过，但是还需要自己进行判断和控制
- void*的使用，通过它能够实现泛型，即针对于int，short，char，struct等类型都能够保证能够拷贝交换成功
- C++的template和这里的区别和优缺点。使用模板的话，编译后，会为每种类型都生成一种代码，比如int对应的，float对应的，这样如果调用次数很多的话，代码体积会增大，冗余过多。而这里编译出来就一套代码，更加简洁。但因为没有类型检查，调用时需要很小心
- 但不同长度类型的vp1与vp2是不能被调用的，如int与short的类型的数据swap，简单的结果就是，截断拷贝或者多拷贝数据，大小尾表现还不一样。

### 指针的swap调用

<!--language: !c-->

    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>

    void swap(void *vp1, void *vp2, int size){
        char buffer[size];
        memcpy(buffer, vp1, size);
        memcpy(vp1, vp2, size);
        memcpy(vp2, buffer, size);
    }

    int main(){
        char *husband = strdup("Fred");
        char *wife = strdup("Wilma");
        swap(&husband, &wife, sizeof(char*));
        printf("%s %s", husband, wife);
        free(husband);
        free(wife);
    }

其对应的内存状态是

<!--language: plain-->

               0 1 2 3                  heap
               ┌─┬─┬─┬─┐                ┌─┬─┬─┬─┬──┐
               │  *────┼───┐            │F│r│e│d│\0│
    &hushand ->└─┴─┴─┴─┘   └───────────>└─┴─┴─┴─┴──┘

               0 1 2 3
               ┌─┬─┬─┬─┐                ┌─┬─┬─┬─┬─┬──┐
               │  *────┼───┐            │W│i│l│m│a│\0│
    &wife    ->└─┴─┴─┴─┘   └───────────>└─┴─┴─┴─┴─┴──┘

- `swap(&husband, &wife, sizeof(char*));`说明只是交换wife和husband变量中的内容 ，而它们的类型是char*，4bytes，即只交换左侧两个变量中的4bytes数据而已（交换的是栈中数据），与右侧堆中数据无关

中间转换过程的内存状态是

<!--language: plain-->

               0 1 2 3               0 1 2 3               0 1 2 3
               ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐
               │ addr1 │             │ addr2 │             │ addr2 │
    &hushand─┬>└─┴─┴─┴─┘  &hushand─┬>└─┴─┴─┴─┘  &hushand─┬>└─┴─┴─┴─┘
             │                     │                     │
    vp1 ─────┘            vp1 ─────┘            vp1 ─────┘
               0 1 2 3               0 1 2 3               0 1 2 3
               ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐
               │ addr2 │             │ addr2 │             │ addr1 │
    &wife ───┬>└─┴─┴─┴─┘  &wife ───┬>└─┴─┴─┴─┘  &wife ───┬>└─┴─┴─┴─┘
             │                     │                     │
    vp2 ─────┘            vp2 ─────┘            vp2 ─────┘
               0 1 2 3               0 1 2 3               0 1 2 3
               ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐             ┌─┬─┬─┬─┐
               │ addr1 │             │ addr1 │             │ addr1 │
    buffer ───>└─┴─┴─┴─┘  buffer ───>└─┴─┴─┴─┘  buffer ───>└─┴─┴─┴─┘

    memcpy(buffer, vp1, size);
                          memcpy(vp1, vp2, size);
                                                memcpy(vp2, buffer, size);

- 如果将`swap(&husband, &wife, sizeof(char*));`中的&去掉，程序还得执行，只不过，交换的是堆中数据（以husband和wife为地址的堆内存的起点），并且只交换了sizeof(char*)的长度。

<!--language: !c-->

    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>

    void swap(void *vp1, void *vp2, int size){
        char buffer[size];
        memcpy(buffer, vp1, size);
        memcpy(vp1, vp2, size);
        memcpy(vp2, buffer, size);
    }

    int main(){
        char *husband = strdup("Fred");
        char *wife = strdup("Wilma");
        swap(husband, wife, sizeof(char*));
        printf("%s %s", husband, wife);
        free(husband);
        free(wife);
    }

- 如果`swap(husband, &wife, sizeof(char*));`将是堆中数据char**与栈中数据char*交换，堆中字符数据将被认做栈的内存地址，程序很可崩溃，而栈中内存地址被认为字符。

Lecture 5
==========

## 泛型lsearch

<!--language: !c-->

    #include <malloc.h>
    #include <stdio.h>

    void *lsearch(void *key, void *base, int n, int elemSize){
        int i;
        for (i = 0; i < n; i++) {
            void* elemeAddr = (char*)base + i * elemSize;
            if (memcmp(key, elemeAddr, elemSize) == 0)
                return elemeAddr;
        }
        return NULL;
    }

    int main(){
        int array[] = {4,2,3,7,11,6};
        int number = 7;

        int *found = lsearch(&number, array, 6, sizeof(int));
        if(found != NULL)
            printf("index: %d, found: %d", found-array, *found);
    }

- 在正常数组的寻址时，使用`&arr[i]`，它等效于`arr + i`，所以`void* elemeAddr = base + i;`尝试基于base向前跳i个元素，但编译器在没有base类型信息时，是不能进行指针算术运算的，所以正确的方式是纯手工计算，诱使编译器认为base是char*类型的，它的sizeof(char)为1，那么指针算术与平常算术效果就一致了，elemSize刚好模拟了每跳一步的步长
- `memcmp`是比较两个内存地址中bytes是否完全相等，对于int,long,short,char,double,float等适用，但对于自定义的结构体、字符串、指针就不能很好工作了
- `found-array`也是指针运算，会根据自动计算 地址差除以类型大小

## 泛型lsearch(带函数指针)

<!--language: !c-->

    #include <malloc.h>
    #include <stdio.h>

    void* lsearch(void *key, void *base, int n,
                  int elemSize, int(*cmpfn)(void*,void*)){
        int i;
        for(i=0; i<n; i++){
            void* elemAddr = (char*)base + i * elemSize;
            if(cmpfn(key, elemAddr) == 0)
                return elemAddr;
        }
        return NULL;
    }

    int IntCmp(void *elem1, void *elem2){
        int* ip1 = elem1;
        int* ip2 = elem2;
        return *ip1 - *ip2;
    }

    int main(){
        int array[] = {4,2,3,7,11,6};
        int size = 6;
        int number = 7;

        int *found = lsearch(&number, array, size,
                             sizeof(int), IntCmp);
        if(found != NULL)
            printf("index: %d, found: %d", found-array, *found);
    }

- `int(*cmpfn)(void*,void*)`中的cmpfn前面可以不加`*`，但加上可读性更好，表示函数指针
- `int IntCmp(void *elem1, void *elem2)`中，elem1指向&key的地址，elem2则是每次迭代数组元素的址，相当于&array[i]
- `int* ip1 = elem1;`将`void*`强制转换成`int*`类型，即它本来的类型，但在`elem1`前面并不需要显式写成`(int*)elem1`，这个转换是隐式的
- `cmpfn`应为函数，或类的静态方法，不能是实例方法，实例方法中，其实是将方法与对象相关的地址作为第一个隐含参数(即`this`指针)传递给方法

## 泛型lsearch(字符串调用)

<!--language: !c-->

    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>

    void* lsearch(void *key, void *base, int n,
                  int elemSize, int(*cmpfn)(void*,void*)){
        int i;
        for(i=0; i<n; i++){
            void* elemAddr = (char*)base + i * elemSize;
            if(cmpfn(key, elemAddr) == 0)
                return elemAddr;
        }
        return NULL;
    }

    int StrCmp(void *elem1, void *elem2){
        char* str1 = *(char**)elem1;
        char* str2 = *(char**)elem2;
        return strcmp(str1, str2);
    }

    int main(){
        char *notes[]={"Ab","F#","B","Eb","Gb","D"};
        char *favoriteNote = "Eb";
        int size = 6;

        char **found = lsearch(&favoriteNote, notes, 6,
                               sizeof(char*), StrCmp);
        if(found != NULL)
            printf("index: %d, found: %s", found-notes, *found);
    }

内存结构如下：

<!--language: plain-->

                Ab\0            B\0             Gb\0
                ^               ^               ^
                │               │               │
            ┌─┬─┼─┬─┬─┬─┬─┬─┬─┬─┼─┬─┬─┬─┬─┬─┬─┬─┼─┬─┬─┬─┬─┬─┐
            │   *   │   *   │   *   │   *   │   *   │   *   │
    notes ->└─┴─┴─┴─┴─┴─┼─┴─┴─┴─┴─┴─┴─┴─┼─┴─┴─┴─┴─┴─┴─┴─┼─┴─┘
                        │               │               │
                        v               v               v
                        F#\0            Eb\0            D\0

                     ┌─┬─┬─┬─┐
                     │   *───┼──> Eb\0
    &favoriteNote -> └─┴─┴─┴─┘
    char**            char*       char

- `*notes[]={"Ab","F#","B","Eb","Gb","D"};`字符串作为常量进行存储
- 基于`notes`数组元素的地址(类型为`char**` 即存储char*地址)比较字符串，离真正的字符串，还有2步之遥，第1步，解引用得到`char*`，第2步，再解引用得到字符串数据
- 为什么不直接将数组元素中`char*`传递给`StrCmp`作比较，因为`lsearch`中无法得知数组元素类型，它只能得到数组元素的地址，只不过此时它为`char**`
- `char **found`指出返回的数组元素的地址的类型
- `lsearch`中`elemSize`参数为`sizeof(char*)`代表数组元素空间大小
- `lsearch`中`key`参数为`&favoriteNote`是为了保持与数组元素的地址的类型(`char**`)相一致，方便在`StrCmp`中对`void *`的参数理解一致，当然可以只传入`favoriteNote`，那样在`StrCmp`中对`elem1`则理解为`char*`，代码对称性差
- `char* str1 = *(char**)elem1;`，`elem1`本身为`char**`，为了比较`strcmp`需要`char*`，所以`elem1`需要解引用一次

Lecture 6
==========

## stack的int版本

<!--language: !c-->

    /* stack.h */
    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>

    typedef struct{
        int *elems;
        int logicLen;
        int allocLen;
    }stack;

    void StackNew(stack *s);
    void StackDispose(stack *s);
    void StackPush(stack *s, int value);
    int StackPop(stack *s);

    /* stack.c */
    void StackNew(stack *s){
        s->logicLen = 0;
        s->allocLen = 4;
        s->elems = malloc(4 * sizeof(int));
        assert(s->elems != NULL);
    }

    void StackDispose(stack *s){
        free(s->elems);
    }

    void StackPush(stack *s, int value){
        if(s->logicLen == s->allocLen){
            s->allocLen *= 2;
            s->elems = realloc(s->elems,s->allocLen*sizeof(int));
            assert(s->elems != NULL);
        }
        s->elems[s->logicLen] = value;
        s->logicLen++;
    }

    int StackPop(stack *s){
        assert(s->logicLen > 0);
        s->logicLen--;
        return s->elems[s->logicLen];
    }

    int main(){
        int i;
        stack s;
        StackNew(&s);
        for(i=0; i<5; i++){
            StackPush(&s,i);
        }
        for(i--; i>=0; i--){
            printf("%d\n", StackPop(&s));
        }
        StackDispose(&s);
    }


stack的内存结构

<!--language: plain-->

             ┌─┬─┬─┬─┐
    allocLen │   4   │
             ├─┼─┼─┼─┤     heap
    logicLen │   2   │     ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
             ├─┼─┼─┼─┤     │   0   │   1   │ empty │ empty │
    *elems   │   *───┼───> └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
         s-->└─┴─┴─┴─┘


- 为了stack的泛型版本清理思路
- 不应该直接操作`stack`这个结构体中的成员，而应该通过函数来操作
- `malloc(4 * sizeof(int));`需要手工指定分配空间大小(堆中)，不像C++中的`new`，`new double[20]`那会隐式的考虑数据类型
- `assert(s->elems != NULL);`，`assert`是宏，如果条件为真，什么都不做，但为假时(申请内存失败)，会终止程序，并告诉终止的地方，能有效防止错误漫延，快速定位错误。在编译时很容易去掉该宏。
- `free(s->elems);`是释放在堆中分配的内存，不需要`free(s);`，我们假设它是已分配好的(本例中它是局部变量，已在栈中，完全不是动态申请的)，而且将它的地址传给`StackNew`的，也不需要做`s->allocLen=0;`的工作，因为调用`StackDispose`时，代表该`s`已被销毁，即使下次想再启动，也得先`StackNew`
- `StackPush`需要做好存储分配工作，C++的做法是：当`logicLen == allocLen`需要重新分配2倍于`allocLen`的内存空间(加倍策略)，将老内存地址内的内容复制过来，将`elems`指向新分配的地址，再将老内存地址给`free`掉；但C还有更底层的内存分配方式：`realloc`，它会检查先前分配的内存是否可以调整大小，即这块内存的后面没有被占用话，可以扩展它(查找堆中合适内存是比较耗时的)，就没必要找一块符合条件的内存，而是返回扩大了内存空间的同一地址，如果被占用的话，会自动查找符合条件的内存，并复制内容，返回新地址。如果`realloc`第一个参数传入`NULL`它就等同于`malloc`


## stack的泛型版本

<!--language: !c-->

    /* stack.h */
    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>

    typedef struct{
        void *elems;
        int elemSize;
        int logicLen;
        int allocLen;
    }stack;

    void StackNew(stack *s, int elemSize);
    void StackDispose(stack *s);
    void StackPush(stack *s, void *elemAddr);
    void StackPop(stack *s, void *elemAddr);

    /* stack.c */
    void StackNew(stack *s, int elemSize){
        assert(elemSize > 0);
        s->elemSize = elemSize;
        s->logicLen = 0;
        s->allocLen = 4;
        s->elems = malloc(4 * elemSize);
        assert(s->elems != NULL);
    }

    void StackDispose(stack *s){
        free(s->elems);
    }

    static void StackGrow(stack *s){
        s->allocLen *= 2;
        s->elems = realloc(s->elems,s->allocLen*s->elemSize);
        assert(s->elems != NULL);
    }

    void StackPush(stack *s, void *elemAddr){
        if(s->logicLen == s->allocLen){
            StackGrow(s);
        }
        void *target = (char*)s->elems+s->logicLen*s->elemSize;
        memcpy(target, elemAddr, s->elemSize);
        s->logicLen++;
    }

    void StackPop(stack *s, void *elemAddr){
        assert(s->logicLen > 0);
        s->logicLen--;
        void *source = (char*)s->elems+s->logicLen*s->elemSize;
        memcpy(elemAddr, source, s->elemSize);
    }

    int main(){
        int i;
        stack s;
        StackNew(&s, sizeof(int));
        for(i=0; i<5; i++){
            StackPush(&s,&i);
        }
        for(i--; i>=0; i--){
            StackPop(&s,&i);
            printf("%d\n", i);
        }
        StackDispose(&s);
    }

- `void StackPop(stack *s, void *elemAddr);`出栈的数据不是通过返回值得到的，而是先提供一个地址，像直升机放下软梯，在该地址放入出栈的数据
- 如果声明成`void* StackPop(stack *s)`，意味着将返回在堆中创建的内存地址，除了我们明确动态申请（如malloc,realloc,strdup）分配堆内存，否则不要在一般函数中返回在堆中创建的内存地址，因为：
    - 这意味着调用者需要对它进行释放，这种责任不对称，更好的方式是 __谁申请的，谁就负责这些空间的释放__
    - 如果`s`被`StackDispose`掉，则调用者得到的地址空间是无效的
- `void *target = (char*)s->elems+s->logicLen*s->elemSize;`因为没有类型信息，无法有数组下标来定位地址，必须手工计算
- `static`修改C或C++的函数时，说明这是一个私有函数，不能在函数所在文件之外对其引用，在技术上叫内链接，生成的.o文件中会被标示为局部的。
- `static void StackGrow(stack *s)`的抽出，为了使`StackPush`更集中相同抽象级别的逻辑








