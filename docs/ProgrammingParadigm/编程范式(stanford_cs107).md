> 2013-08-15

Lecture 1
=========
本文是编程范式（斯坦福大学公开课）的笔记，该课程主要介绍了C、Scheme、Python这三种语言，涵盖的编程范式有：过程式、泛型式（C中实现）、代码生成（宏/元编程）、并发式（多线程）、面向对象式（少量）、函数式（scheme/haskell）、动态脚本语言（python，支持多种范式的语言）等

Lecture 2
=========
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

> p.s. 如何得到`i = 1078523331`

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
=========
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
=========

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

        int *found = lsearch(&number, array,
            sizeof(array)/sizeof(int), sizeof(int));
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
        int number = 7;

        int *found = lsearch(&number, array,
            sizeof(array)/sizeof(int), sizeof(int), IntCmp);
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

        char **found = lsearch(&favoriteNote, notes,
            sizeof(notes)/sizeof(char*),sizeof(char*), StrCmp);
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

## 泛型bsearch


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

    /* app */
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
        &s-->└─┴─┴─┴─┘


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

    /* app */
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


Lecture 7
==========

## stack的泛型版本改进

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
        void (*freefn)(void*);
    }stack;

    void StackNew(stack *s, int elemSize, void(*freefn)(void*));
    void StackDispose(stack *s);
    void StackPush(stack *s, void *elemAddr);
    void StackPop(stack *s, void *elemAddr);

    /* stack.c */
    void StackNew(stack *s, int elemSize, void(*freefn)(void*)){
        assert(elemSize > 0);
        s->elemSize = elemSize;
        s->logicLen = 0;
        s->allocLen = 4;
        s->elems = malloc(4 * elemSize);
        s->freefn = freefn;
        assert(s->elems != NULL);
    }

    void StackDispose(stack *s){
        int i;
        if(s->freefn != NULL) {
            for(i = 0; i < s->logicLen; i++) {
                s->freefn((char*)s->elems + i * s->elemSize);
            }
        }
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

    /* app */
    void stringFree(void* elemAddr) {
        char** p = (char**)elemAddr;
        free(*p);
    }

    int main(){
        const char *friends[] = {"Al", "Bob", "Carl"};
        stack stringStack;
        int i;
        StackNew(&stringStack, sizeof(char*), stringFree);
        for(i = 0; i < 3; i++) {
            char *copy = strdup(friends[i]);
            StackPush(&stringStack, &copy);
        }
        char* name;
        while(stringStack.logicLen > 1) {
            StackPop(&stringStack, &name);
            printf("%s\n", name);
            free(name);
        }
        StackDispose(&stringStack);
    }


字符串stack内存结构

<!--language: plain-->

    stack memory                         heap memory
    ───────────────────────────────────┼──────────────────

             ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
             │A│l│\│B│o│B│\│C│a│r│l│\│
    friends->└─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘

                             ┌─┬─┬─┬─┐
    allocLen                 │   4   │
                             ├─┼─┼─┼─┤
    logicLen                 │   1   │
                             ├─┼─┼─┼─┤
    elemSize                 │   4   │     ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
                             ├─┼─┼─┼─┤     │*addr1 │ empty │ empty │ empty │
    *elems                   │   *───┼────>└─┼─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
    &stringStack   --------->└─┴─┴─┴─┘       │
                                             │StackPush(&stringStack, &copy);
                             ┌─┬─┬─┬─┐       │ ┌─┬─┬─┐
                             │*addr1─┼─┐     │ │A│l│\│
    &copy          --------->└─┴─┴─┴─┘ └─────┴>└─┴─┴─┘
    char *copy = strdup(friends[i]);


- 字符串处理，在内存堆中实际存储的是`char*`
- `char *copy = strdup(friends[i]);`字符串 __深拷贝__[在堆中],如果不拷贝，让stack直接引用现存字符串[在栈中]，生命周期不一样，指针容易出现悬挂
- `StackPush(&stringStack, &copy);`在循环中每次生成一个`copy`，它存储指向堆中字符串的基地址，`StackPush`是需要将该`copy`的内容(char*)[图中`*addr1`]复制，需要传入`copy`的地址
- 如果有些数据并未`StackPop`，就没有机会将它`free`，当`StackDispose`后，将出现未释放的空间。所以需要升级`StackNew`与`StackDispose`，让它有机会处理未释放的内存空间
    - 只检查还未`StackPop`的元素，即`logicLen`个，已被`StackPop`的释放由调用方去负责，因为`strdup`是调用方使用的，所以`free`它，职责还是对称的。如果在调用`StackPop`时，就`free`掉出栈的数据，也是不对的，控制权应该返给用户了
    - 同样，`freefn`的调用，需要手要去计算每个元素的地址
    - 当然，对于int,double这些类型，甚至是没有涉及动态分配的struct，并不需要`free`
- `stringFree`中的`elemAddr`是`char**`类型，即元素的地址（如addr1的地址），而现在需要释放的是`strdup`分配的内存空间（如"Al\0"），前者指向后者，所以需要解引用，即为`free(*(char**)elemAddr)`
    - 此条结合上条，发现所有用户`strdup`的，最后还是用户`free`的，即职责对称，`stringStack`只不过是个中间介质，并未将内存分配的职责转移

## memcpy原理
- `memcpy`将使用暴力复制，每次拷贝4个字节，它 __不会检查目标与源地址重叠__ 问题，当源地址在目标地址之前，容易出问题，目标地址在源地址之前，则能正常工作，但`memmove`就没有问题，但性能稍降低。当确信没有重叠时，优先使用`memcpy`

<!--language :!c-->

    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>

    int main(){
        char *letter = strdup("ABCDEF");
        memcpy(&letter[2], letter, strlen(letter)+1);
        printf("%s\n", &letter[2]); /* ABABAF */

        char *letter2 = strdup("ABCDEF");
        memcpy(&letter2[-2], letter2, strlen(letter2)+1);
        printf("%s\n", &letter2[-2]); /* ABCDEF */

        char *letter3 = strdup("ABCDEF");
        memmove(&letter3[2], letter3, strlen(letter3)+1);
        printf("%s\n", &letter3[2]); /* ABCDEF */
    }

## rotate方法
- 实现类似STL中的rotate方法，front,middle,end（它是数组外的第一个元素地址）分别指向一个数组的不同边界，需要将front到middle之前的数据移到end的后面

<!--language :!c-->

    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>

    void rotate(void *front, void *middle, void *end){
        int frontSize=(char *)middle - (char *)front;
        int backSize=(char *)end - (char *)middle;

        char buffer[frontSize];
        memcpy(buffer,front,frontSize);
        memmove(front,middle,backSize);
        memcpy((char*)end-frontSize,buffer,frontSize);
    }

    int main(){
        char *letter = strdup("ABCDEF");
        rotate(letter,&letter[3],&letter[6]);
        printf("%s\n", letter); /* DEFABC */
    }

内存情况：

<!--language: plain-->

             ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
             │ │ │ │ │ │ │ │ │ │ │ │ │
    front->  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘ <─end
                     ^
                     └ middle

- `int fontSize=(char *)middle - (char *)front;`编译器是不允许void*进行指针算术运算的，两个地址相减，并不会返回它们之间的字节数，而是返回包含具体类型的个数，这样就和指针运算的加法相对应了
    - 如两个int*类型相减，得到是它们地址之间int元素的个数

## qsort泛型版本

<!--language :!c-->

    #include <malloc.h>
    #include <stdio.h>
    #include <string.h>

    void qsort(void *base, int size, int elemSize, int(*cmpfn)(void*,void*)){

    }

    int main(){

    }


- 栈(stack segment)的管理一般由硬件来完成，而堆(heap segment)的管理由程序来控制


Lecture 8
==========

## 堆内存分配
- `int *arr = malloc(40*sizeof(int));`其实会得到多于160bytes的空间，得到164 or 168bytes，因为会包含头部的4 or 8bytes（这和规范有关），用来记录该块内存的大小（164 or 168bytes），你得到返回地址，是在该头指针的4 or 8bytes之后，160bytes之前的那个地址。
- 当将该地址传给`free`时，内存管理器假设这个指针之前曾经处理过，将解析arr之前的4 or 8bytes，得到记录了这块内存的大小，将以某种方式将该内存放回堆中

<!--language: plain-->

             0 4                     164
             ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
             │ │                     │
    head->   └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
               ^
               └ arr

如下代码将不能正常工作

<!--language :c-->

    #include <malloc.h>
    #include <stdio.h>

    int main(){
        int *arr = malloc(100*sizeof(int));
        free(arr+60);
    }

- 申请了100个int元素，发现多申请了，需要放回部分到堆中，但这种方式不会起效，`free(arr+60)`，这取决于实现，如果不进行错误检查，将会在`arr+60-(4 or 8)`的地方去获取内存分配的信息，恰好能正确解析这些数据时，将会`free`掉意外的内存空间，同样如果修改了`arr[-1]`的值，也是有问题的，导致`free`不能正常工作，造成特殊的内存泄漏

<!--language :c-->

    #include <malloc.h>
    #include <stdio.h>

    int main(){
        int arr[100];
        free(arr);
    }

- 对栈中的空间进行`free`，`free`不会做错误检查，只是机械的将arr之前4 or 8bytes解释成内存块的大小，并将其记录了空闲表数据结构中，让内存管理器误认为该空间是可用的，下次malloc将有可能被分配出去


- 一些启发法，如果你需要160bytes，将可能给你192 or 256bytes（符合2的N次幂）
- `free`时，并不会真正将内存中数据清空，只是将其并入空闲表数据结构

## 栈段

<!--language :c-->

    void C(){
        double m[3];
        int n;
    }
    void B(){
        int x;
        char *y;
        char *z[2];
        C();
    }
    void A(){
        int i;
        short j[4];
        double k;
        B();
        C();
    }
    int main(){
        A();
    }

- 当调用A函数时，需要为的3个局部变量从栈中分配内存，它们紧密的放置在一起，并且符合某种顺序，这种内存结构称 __活动记录或栈帧__

<!--language: plain-->

       ┌───────┐
    i  │       │
       ├───┬───┤
       │   │   │
    j  ├───┼───┤
       │   │   │
       ├───┴───┤
       │       │
    k  ├       ┤
       │       │
       └───────┘

- 当调用A函数时，栈顶指针减去活动记录占用的空间大小，当再调用B或C时，栈指针会再次减小，以下以示意来表示栈空间，Ra代表A的活动记录

<!--language: plain-->

      line19   line15   line9    line4    line10    line16
      ┌────┐   ┌────┐   ┌────┐   ┌────┐   ┌────┐   ┌────┐
      │ ?? │   │ ?? │   │ ?? │   │ ?? │   │ ?? │   │ ?? │
      ├────┤   ├────┤   ├────┤   ├────┤   ├────┤   ├────┤
      │ Ra │   │ Ra │   │ Ra │   │ Ra │   │ Ra │   │ Ra │
    ->├────┤   ├────┤   ├────┤   ├────┤ ->├────┤   ├────┤
      │    │   │ Rb │   │ Rb │   │ Rb │   │ Rb │   │ Rc │
             ->├────┤   ├────┤ ->├────┤   ├────┤ ->├────┤
               │    │   │ Rc │   │ Rc │   │ Rc │   │    │
                      ->├────┤   ├────┤   ├────┤
                        │    │   │    │   │    │


- 其实A函数访问各个局部变量，是依据提供入口地址的基指针，最后一个变量C与它的偏移量为0，这个地址是靠硬件存储的，称为 __栈指针__，它总是指向最近被调用的函数的活动记录
- 当A调用B时，指针继续减小，但正常情况下B没有办法访问到A的活动记录，B调用C，在C返回时，栈指针又回到未调用C之前的位置，C的活动记录还在，只是没办法访问到，B返回后，A再次调用C时，会覆盖原先B的活动记录


Lecture 9
==========

## 局部变量操作的汇编

<!--language: c-->

    void fun(){
        int i;
        int j;
        i = 10;           M[R1+4] = 10  //store operation
        j = i + 7;        R2 = M[R1+4]  //load operation
                          R3 = R2 + 7   //ALU operation
                          M[R1] = R3    //store operation
        j++;              R2 = M[R1]
                          R2 = R2 + 1
                          M[R1] = R2
    }

- 假设寄存器大小为4bytes，硬件对该大小的操作进行了优化
- `M[R1+4] = 10`，`M`代表整个RAM的名字，将整个RAM看成一个很大的字节数组，`R1`是 __寄存器1，存储的是栈指针__，`4`是偏移量，即`i`所在地址。这是一个 __存储操作__，将RAM某块区域的值进行了更新
- `R2 = M[R1+4]`将寄存器2存储`i`变量的值
- `R3 = R2 + 7`进行ALU运算，寄存器2中值加上7，存储到寄存器3中
- `M[R1] = R3`将j的地址写入寄存器3的值，完成了`j = i + 7`的翻译
- 为什么不将汇编中第2,3行改成`R3 = 10 + 7`，其实是为了保证代码翻译的 __上下文无关性__，即使修改了`i = 10;`也不会影响第2,3行
- 倒数三行为什么不改成`M[R1]++`，因为 __汇编指令不允许对任意内存地址作为ALU运算操作的操作数__，必须先使用load，然后用寄存器作为操作数执行ALU操作，然后再将结果写回内存中（这样会让汇编指令简单，时钟频率也会更快）

<!--language: c-->

    void fun(){
        int i;
        short s1;
        short s2;
        i = 200;          M[R1+4] = 200
        s1 = i;           R2 = M[R1+4]
                          M[R1+2] = .2 R2
        s2 = s1 + 1;      R2 = .2 M[R1+2]
                          R3 = R2 + 1
                          M[R1] = .2 R3
    }

- `s1 = i;`如果翻译成`M[R1+2] = M[R1+4]`将有两个问题：
    - 不能在同一个语句中同时执行load和store操作，改成`R2 = M[R1+4]; M[R1+2] = R2`，并不能达到目的
    - 因为`M[R1+2]`的空间也是4bytes的，会更新到原先i的部分空间，会让s1变成0，而i成为一个特大的值，改成`M[R1+2] = .2 R2`，取R2低位2个字节，更新到内存中（`.4`是默认的，可以不用写）
- `R2 = .2 M[R1+2]`将M[R1+2]起的2bytes，放入到R2的低位2bytes中


## 循环与判断的汇编

<!--language: c-->

    void fun(){
        int array[4];
        int i;
        for(i=0;i<4;i++){   M[R1] = 0     //i=0
                            R2 = M[R1]    //R2得到i的值
                            BGE R2,4,PC+? //R2>=4的跳转
            array[i] = 0;   R3 = M[R1]
                            R4 = R3 * 4
                            R5 = R1 + 4
                            R6 = R4 + R5
                            M[R6] = 0
                            R2 = M[R1]    //i++
                            R2 = R2 + 1
                            M[R1] = R2
        }                   JMP PC-40     //跳至前10条指令
        i--;                R2 = M[R1]
                            R2 = R2 - 1
                            M[R1] = R2
    }

- `i<4`是一个判断语句，`BGE R2,4,PC+?`代表如果大于等于4则跳出循环，但地址目前不确定，为`PC`加上一个偏移量
    - 六种比较操作`BEQ`,`BNE`,`BLT`,`BLE`,`BGT`,`BGE`
    - `PC`是代表一个寄存器内容（现在机器通常是第27 or 29 or 31号寄存器），代表着 __程序计数器，存储当前执行的汇编代码指令的地址__
    - 所有汇编指令都是4字节宽，所以每个时钟周期PC的值增加4，即 __默认情况下，执行下一条指令__，除非遇到跳转指令，这时PC不再自增
    - 现在填上`?`，因为还不确定`array[i] = 0;`会产生多少汇编指令，跳转地址与目前PC的偏移量还不能得到，需要确定后才 __回头填写__

- `array[i] = 0;`头两句是`R3 = M[R1];R4 = R3 * 4`手工计算第i个元素的偏移地址，`4`是int类型大小；`R5 = R1 + 4`得到`array`地址，`R6 = R4 + R5`得到`array[i]`地址，`M[R6] = 0`，将`array[i]`中内容置为0

- `JMP`是无条件跳转到先前判断的地方，即`R2 = M[R1]`的位置，相对于当前PC的10条指令，乘以指令宽度4字节，即减去40bytes

- 编写`i--;`的指令，该指令的地址，即为`BGE`测试为真时的跳转位置，它相对于`BGE R2,4,PC+?`相隔10个指令，即偏移了40bytes，改写成`BGE R2,4,PC+40`

- 是否可以`JMP PC-36`，即向前9个指令，因为此是`R2`就存储着`i`的值，不推荐，不符合上下文无关的要求

## 汇编指令的结构

- 汇编指令宽度为4bytes，即32位宽，而汇编指令有59种，根据2^6=64，则需要分配出6位来表示，称为`op code`，硬件会在时钟周期前一部分查看前6位是什么，然后决定怎样解释剩下的26位

- 如果允许变长编码，如哈夫曼编码来表示`op code`，某些指令就可以留出更多的位来表示它的操作数，Mips就使用了这种方式

## 结构体强制指针类型的汇编

<!--language: c-->

    typedef struct {
        int num;
        int denom;
    } fraction;

    int main(){
        fraction pi;                            //所设R1指向pi地址
        pi.num = 22;                            M[R1]=22
        pi.denom = 7;                           M[R1+4]=22
        ((fraction *)&(pi.denom))->denom = 33;  M[R1+8]=33
    }

- 其实`((fraction *)&(pi.denom))->denom = 33;`对应的汇编就是`M[R1+8]=33`，
    - `&(pi.denom)`对应的偏移量是`R1+4`，
    - `(fraction *)`的强制转换并不会改变地址，只是让编译器对这个地址的认识发生了变化，当它解引用访问`denom`时，它认为这个地方存储的是一个`fraction`变量类型，而`denom`与变量的地址偏移量是4，即`M[R1+8]`的由来


Lecture 10
==========

## 函数参数操作的汇编

<!--language: c-->

    void foo(int bar, int *baz){
        char snink[4];                  <foo>: SP = SP - 8
        short *why;

        why = (short *)(snink + 2);            R1 = SP + 6
                                               M[SP] = R1

        *why = 50;                             R1 = M[SP]
                                               M[R1] = .2 50

                                               SP = SP + 8
                                               RET
    }

    int main(int argc, char **argv){
        int i = 4;               SP = SP - 4
                                 M[SP] = 4

        foo(i,&i);               SP = SP - 8
                                 R1 = M[SP+8]  //load i值
                                 R2 = SP + 8   //得到 i地址
                                 M[SP] = R1
                                 M[SP+4] = R2
                                 CALL <foo>
                                 SP = SP + 8   //在foo RET之后执行

        return 0;                RV = 0
                                 RET
    }


随着`foo`被调用，对应的活动记录如下，共20bytes：

<!--language: plain-->

           ┌───────┐
           │       │
    baz  ->├───────┤
           │       │  bar
           ├───────┤
           │RetLink│
    ??   ->├─┬─┬─┬─┤
           │ │ │ │ │  snink[0..3]
           ├─┴─┴─┴─┤
           │       │
    why  ->└───────┘

- 参数的在活动记录中的位置是有顺序要求的，`bar`在`baz`的下面，所有参数会根据声明 __从右至左__ 而从 __高地址向低地址__ 存储，即第0个参数，总在所有参数下面
- 局部变量比参数的地址更低，按照变量声明顺序，也是 __高地址向低地址__ 存储
- 中间的`??`，存储着该函数的调用者相关信息，方便函数返回时能正确回到调用的代码中

当`main`被调用时，活动记录如下：

<!--language: plain-->

           ┌───────┐
           │   * ──┼──> heap
    argv──>├───────┤
           │       │ argc
           ├───────┤
           │SavedPC│
    SP1 ──>├───────┤
           │   4   │
    SP2 ─┬>├───────┤
         └─┼── *   │ *baz
           ├───────┤
           │   4   │ bar
    SP3 ──>├───────┤
           │SavedPC│
    SP4 ──>├─┬─┬───┤
           │ │ │ 50│ snink[0..3]
           ├─┴─^───┤
           │   └*  │ *why
           └───────┘


- 因为只有函数自己知道需要多少局部变量来完成，根据规范，函数第一件是就是为局部变量申请内存空间，`main`在调用的时候，上面的参数空间已经存在了
- 上课使用`R1`来指向活动记录的基地址，其实是一个特定寄存器在做这件事，叫做`SP`，即stack pointer，总是指向当前执行栈的最低地址
- 当`main`被调用时，`SP`指向如上图SP1，当执行到`int i=4;`时，`SP=SP-4`，`SP`指向如上图SP2，__局部变量的空间申请由函数自己完成__
- 当调用`foo`时，`main`函数需要给`foo`的参数申请8bytes空间，`SP=SP-8`，`SP`指向如上图SP3，__参数变量的空间申请由函数的调用者完成__
- 至此，整个活动记录还是`main`的，`R1 = M[SP+8]`取得`i`的值，而`R2 = SP + 8` 则得到`i`地址，而`M[SP] = R1; M[SP+4] = R2`则是给foo传参，得到的结果如上图中`bar`与`baz`的效果

- `CALL <foo>`代表调用`foo`，将控制权交给它，执行它的第一条汇编指令，然后通过某种机制保证执行之后跳回到这里，__当`CALL`执行时，程序知道当时PC的值，它会将`SP`减去4bytes，然后将PC值放进去__（图中第二个SavedPC）。此时`SP`指向如上图SP4

- `foo`中的汇编，`SP = SP - 8`是为局部变量申请空间，后两句的翻译见上面代码，得到的效果如上图中50的部分

- `foo`中为了申请空间，做了`SP = SP - 8`，现在函数结束了，需要恢复原先状态，需要收回那些空间（像栈的malloc），需要执行`SP = SP + 8`，这样会让SP，指向图中第二个SavedPC，即返回地址，__通过`RET`，将savedPC值放到`PC`中，并且将SP加上4bytes，实现了函数返回__。（它指向调用者的某一个代码指令，savedPC其实也只是一个向上的偏移量），此时`SP`指向如上图SP3

- `main`中
，`SP = SP + 8`在`foo`的`RET`之后执行，它实际上是回收它为`foo`分配的两个参数的空间，`RV=0`,`RV`也是特殊寄存器，专门为传递返回值，调用函数会在被调用者返回后，检查该值

- 活动记录被设计成分开负责：__参数变量的空间申请由函数的调用者完成__，调用者知道函数的原型，而且只有调用者才知道如何将参数赋值，而 __局部变量的空间申请由函数自己完成__，因为调用者不知道函数的实现到底有多少局部变量

## 递归函数的汇编

<!--language: c-->

    int fact(int n){
        if(n==0)                 <fact>:  R1 = M[SP + 4]
                                          BNE R1,0 PC+?

            return 1;                     RV = 1
                                          RET

        return n * fact(n-1);             R1 = M[SP+4] //load n
                                          R1 = R1 - 1  //alu n-1
                                          SP = SP - 4  //assign space
                                          M[SP] = R1   //store
                                          CALL <fact>
                                          SP = SP + 4  //free space
                                          R1 = M[SP+4] //load n
                                          RV = RV * R1
                                          RET
    }


- `fact`是个递归函数，它没有局部变量，`BNE R1,0 PC+?`回填`12`，跳到它下面第三行，因为本身执行时PC未加上4，所以三条指令加起来为12（如果这样，岂不前面有误？）
- 当翻译`n * fact(n-1)`时，不需要将`n`先放入寄存器，因为在后面递归时它会被覆盖，现在要做的是，把`n`减1，作为参入传给`fact`

[演示过程](../../data/factorial-trace.pdf)

Lecture 11
==========

## C++地址引用类型的汇编

先编写C指针的版本

<!--language: c-->

    void swap(int *ap, int *bp){
        int temp = *ap;          <swap>:  SP = SP - 4
                                          R1 = M[SP+8]  //load ap
                                          R2 = M[R1]    //load *ap
                                          M[SP] = R2    //store

        *ap = *bp;                        R1 = M[SP+12] //load bp
                                          R2 = M[R1]    //load *bp
                                          R3 = M[SP+8]  //load ap again
                                          M[R3] = R2    //store

        *bp = temp;                       R1 = M[SP]    //load temp
                                          R2 = M[SP+8]  //load bp
                                          M[R2] = R1    //store
                                          SP = SP + 4
                                          RET
    }

    void foo(){
        int x;                         SP = SP - 8
        int y;
        x = 11;                        M[SP+4] = 11
        y = 17;                        M[SP] = 17

        swap(&x, &y);                  R1 = SP     //&y
                                       R2 = SP + 4 //&x
                                       SP = SP - 8
                                       M[SP] = R2   //pass para &x
                                       M[SP+4] = R1 //pass para &y
                                       CALL <swap>
                                       SP = SP + 8

                                       SP = SP + 8
                                       RET
    }


- 更好的编程习惯是，每逢有栈空间的分配，如`SP = SP - 8`，就需要回收它，提前写好`SP = SP + 8`，中间留出位置填写其他代码
- 汇编中，先求右值，再给左值赋上
- `swap`中`int temp = *ap;`得到`R1`存储着`ap`，但在`*ap = *bp;`时，会再次重新计算，此时`R1`已被覆盖，更为了上下文无关

接下来是C++版本：

<!--language: cpp-->

    void swap(int& a, int& b){
        int temp = a;
        a = b;
        b = temp;
    }

    void foo(){
        int x;
        int y;
        x = 11;
        y = 17;
        swap(x, y);
    }

- `a`和`b`看起来像整数，但是在内部实现上它们并不是真正的整数，引用的工作方式是，对指针进行自动解引用，即查找的是`a`存储的地址 所对应的数据
- 编译器根据`swap`原型，知道在`foo`中编写`swap(x, y);`赋的参数 __不是真正的值__，而是它们的引用（别名），__传递的还是地址__，而在`swap`中，再对指针 __自动解引用__（两头都做更改）
- 最终汇编代码将与C指针版本的一样（当然有时也不尽相同，C++语言级别的引用和引用还有不同意义，只是比较相似而已），可以理解为编译器的语法糖，只要函数声明时指定引用，而在编写函数和调用函数时，均不需要去使用`&`和`*`，认为它只是别名，直接操作

<!--language: cpp-->

    void foo(){
        int x = 11;
        int& y = x;
        //int *y = &x;
    }

- 后两句的产生的汇编是一样的，在使用时，上面一行的`y`不需要解引用，而后者需要解引用
- 一旦赋值就不能将引用重新绑定给新的左值，也就`int& y = x;`的y的 __指向不能被更改，而指针则灵活得多__，比如引用没办法构造链表

## C++类的汇编

<!--language: cpp-->

    class binky{
        public:
            int dunky(int x, int y);
            char * minky(int *z){
                int w = *z;
                return slinky + dunky(winky, winky);
            }

        private:
            int winky;
            char *blinky;
            char slinky[8];
    };

    int main(){
        int n = 17;
        binky b;
        b.minky(&n); //equals binky::minky(&b, &n);
    }

- 当声明`binky b`时，`b`的内存块构造与C的结构体类似：

<!--language: plain-->

           ┌─┬─┬─┬─┐
           │ │ │ │ │
           ├─┼─┼─┼─┤ slinky[0..7]
           │ │ │ │ │
           ├─┴─┴─┴─┤
           │       │ *blinky
           ├───────┤
           │       │ winky
    &b   ->└───────┘

- 为什么在内联函数`minky`中能访问到`slinky`和`winky`，因为它总能得到当前对象的基地址，`this`指针总是作为第0个参数隐式传入的，即`b.minky(&n)`对于编译器而言，传入的并不是一个参数，而是两个，对应的形式是`binky::minky(&b, &n);`，所以对应的活动记录就是这样

<!--language: plain-->

           ┌───────┐
           │       │ *z
           ├───────┤
           │       │ *this  指向  &b
           ├───────┤
           │SavedPC│
           ├───────┤
           │       │ w
           └───────┘

- 而`static`修饰的函数，会被认为 __只是普通函数__，恰好命名空间在class下面，不需要this的隐式传递，同样 __不能被继承__


Lecture 12
==========

## 预处理器

### 定义常量
- 定义全局常量，最常用的是`#define KW 40`

- 当预处理时，它会依次读取传入的.c文件的内容，当发现`#define key value`时，将会对文件中出现`key`替换/匹配成`value`，它甚至不会认识数字，只是文本的替换，其他的原样输出，输出的还是文本

### 宏

- `#define`的参数化形式进行扩展，称之为 __宏__，`#define MAX(a,b) ((a)>(b)?(a):(b))`，如代码中出现`MAX(10,40)`，将被处理成`((10)>(40)?(10):(40))`

- 函数的调用是需要分配参数空间，将参数值写入，需要调用和返回的，并需要清理参数空间，这可能需要花费太多额外时间与空间，而宏展开后 __不存在调用与返回__，可能只需更少汇编指令（当然取决扩展后代码的表达），如果光调用与返回占据了70%以上时间，还不如用宏

- 而且宏 __类似泛型__ 的效果，上面的`MAX`其实还可以传入`float`(重载了操作符`>`的都可以)

- `#define NthElemAddr(base, elemSize, index) ((char*)base + index*elemSize)`,定义该宏就可以替换[上面](#TOC5.1)中取元素地址的运算，__将重复的代码提取出来__

<!--language: c-->

    #ifdef NDEBUG
        #define assert(cond) (void)0
    #else
        #define assert(cond) \
        (cond)?((void)0):
               fprintf(stderr,"filename codeline ..."),exit(0)
    #endif

- `(void)0`告诉编译器别把这个0用在任何地方，不允许被赋值，只是占位表达式；`#ifdef`是在预编译时可进行控制，以产生不同的代码

- 缺点在于（这也是和函数调用不同的地方，当然调用者也有责任）：
    - `MAX`宏定义的时候，如果少写了一对括号，展开后，有可能运算符结合优先级出现问题，导致不是预想的效果
    - 预处理没有类型检查，类型错误直到编译阶段才发现；
    - 对于``MAX(fib(100),fact(400))``中的展开是这样((fib(100))>(fact(400))?(fib(100)):(fact(400)))`将会导致耗时的计算会被计算多次。
    - `int lager = MAX(m++,n++)`将导致较小的自增1次，而较大的自增2次

- 宏可认为是元编程（代码生成）的一部分

### 引入头文件
- 头文件一般只包含定义类型与定义函数原型，是不会产生汇编代码的，不能声明任何存储空间（除非定义共享全局变量）

- 如果你只想要某个c文件中的一两个函数，没必要include它的整个头文件，可以在使用之前手动声明函数原型即可

- `#include <stdio.h>`，尖括号代表这是系统头文件，编译器通过默认路径找（`/usr/include`）到这些文件，而`#include "vector.h"`，双引号引入的头文件，编译器认为这是用户编写的`.h`文件，默认会从当前工作目录查找，通过makefile可设定一些选项，在哪里查找

- 预处理器找到头文件后，把头文件的内容复制到`#include`语句处，并将`#include`语句删除

- `#include`是递归导入的，将层层的`#include`进行替换（互相导入是不会死循环的），直到生成不包含`#include`和`#define`的文本流，交给编译器进行处理

<!--language: c-->

    #ifndef _vector_h_
    #define _vector_h_
       ... /* vector.h content */
    #endif

以上是早期解决循环导入的问题，现在的预处理器应能自动解决循环导入的问题(当然你不能假设它已为你处理好)

### 预处理命令
- `gcc -E  test.c -o test.i`,只进行预编译处理，不再进行后续过程，能查看到`#define`的替换与展开，及`#include`的头文件引入


Lecture 13
==========

## 编译与链接

<!--language: !c-->

    /* main.c */
    #include <stdlib.h> /* malloc, free */
    #include <stdio.h> /* printf */
    #include <assert.h>

    int main(int argc, char *argv[]){
        void *mem = malloc(400);
        assert(mem != NULL);
        printf("Yay!\n");
        free(mem);
        return 0;
    }

<!-- - 编译阶段，输入的是中间文件`*.i`，编译后生成汇编语言文件`*.s`
    - `gcc -S test.i -o test.s`
    - `gcc -S test.c -o test.s` -->

- 上面的汇编代码中将看到`CALL malloc,CALL printf,CALL free,RV=0`等，但不会看到`CALL assert`而只有`assert`宏展开后的一些汇编指令

- 编译产生`.o`文件，然后将所有`.o`文件及其他所有来自编译器标准库的`.o`代码进行链接，链接器ld会将一个个`.o`文件拼接，生成一个更大的可执行文件，并且去掉没有用到的部分

- 如果把`#include <stdio.h>`注释掉，有些编译器将会产生编译错误，因为没有定义`printf`，
    - __但gcc不会报错__(报warning)，它会在编译时刻分析源程序，看看哪部分看函数调用，然后 __根据这个调用来推测函数原型__，这里的函数调用是一个参数，gcc将把`printf`原型定义为只有一个参数，将返回值推测为`int`（和真正原型一致），当后面还有`printf`时，它们只能有一个参数（经测试可以有多个参数）。
    - 生成的`.o`文件没有变化，因为`.h`文件是不会产生汇编代码的，只是告诉编译器一些规则，可以知道哪些语法是正确的
    - 在链接阶段，链接器会根据编译过程中出现的警告，查找标准库中`printf`对应的代码，是存在的，就会被加进来

- 如果把`#include <stdlib.h>`注释掉，gcc看到`malloc`将推测成`int malloc(int)`，将多得到一条警告，是类型不兼容（返回值是int，而赋值的类型是`void*`）
- 如果把`#include <assert.h>`注释掉，gcc将报错，它会认为`assert`也是函数调用，推测它原型，生成的`.o`文件中也会有`CALL assert`的存在，到链接时再查找标准库，但始终未定义，出现链接错误

- 函数原型的存在意义就在于，在编译器产生汇编时，让调用者和被调用者关于savedPC上面的活动记录的布局达成一致（函数调用参数符合调用参数类型规定），原型其实只涉及参数在活动记录中（位于savedPC之上）正确布局

## 当原型推测错误

<!--language: !c-->

    /* int strlen(char*, int); */
    int main(){
        int num = 65;
        int length = strlen((char*)&num, num);
        printf("Length = %d\n", length);
        return 0;
    }

不加上任何头文件，就编译这个程序，到了`strlen((char*)&num, num)`推测原型为`int strlen(char*, int)`

对应的活动记录就是：

<!--language: plain-->

      ┌───────┐
      │  65   │ num
    ┌>├───────┤
    │ │       │ length
    │ ├───────┤
    │ │  65   │
    │ ├───────┤
    └─┼── *   │
      ├───────┤
      │SavedPC│
      └───────┘

- 你觉得也许在链接时会发现`strlen`应该只有一个参数，但是不会的，在`.o`文件中并没有记录下某个调用有多少个参数，你可看到在`.o`文件中会有`SP=SP-8`，而不是减4或者16，就可以推测参数的几个，其实16也许只是一个含4个整形结构体而已，所以这种推测不准，所以在链接阶段，gcc只会检查符号名称，不会检查实参类型与个数。

- 真正strlen的定义中，它只有一个参数，所以它只会用到SavedPC上面的4bytes，

- 所以上面只会有一个警告，而不会出错（与测试不符），如果警告也不想要，可以自己声明一个`strlen`的原型（加了原型后，只警告，不报错）

- 自己加上原型（有一定危险性），减少`include`，可以省去加载多余的函数定义，减少编译时间（如果系统需要频繁编译的话）

- 最终结果，小尾系统打印1(65 00 00 00)，大尾系统打印0(00 00 00 65)

<!--language: !c-->

    int memcmp(void*);
    int main(){
        int n = 17;
        int m = memcmp(&n);
        return 0;
    }


对应的活动记录就是：

<!--language: plain-->

      ┌───────┐
      │  17   │ n
    ┌>├───────┤
    │ │       │ m
    │ ├───────┤
    └─┼── *   │
      ├───────┤
      │SavedPC│
      └───────┘


- 我们定义的`memcmp`原型认为它只能访问到SavedPC上面的4bytes，但实际上它`int memcmp(void *v1, void *v2, size)`能访问到m与n的空间，v1指向n的地址，v2的内容是m，但m还未初始化为一个随机数，有可能是不合法的一个指针，size是17
- 能编译通过，但可能运行崩溃

- C++有函数重载，而C中没有，C++会根据函数名及参数列表中的参数类型，重新构造一个更复杂的函数符号
    - 针对`int memcmp(void*)`，C中生成`CALL <memcmp>`，而C++则生成`CALL <memcmp_void_p>`
    - 针对`int memcmp(void*,void*,int)`，C中还是生成`CALL <memcmp>`，而C++则生成`CALL <memcmp_void_p_void_p_int>`
    - 所以C++更类型安全些

## 为什么程序会崩溃

- seg fault: 通常出现在对一个错误的指针解引用时出现，如尝试对NULL进行解引用，内存分数据段、栈段、堆段、程序段，NULL的地址在内存0~3对应的4bytes，操作系统知道该地址不在任何段中
- bus errors: 当你尝试对属于某个段中的地址进行解引用时，或给某个未对齐的地址写入数据时发生。为了支持操作系统或硬件某些限制，所有数据对应的地址，int的数据的地址都应该是4的倍数，而short数据类型地址是偶数，char不受限制，但为了为了简洁，编译器假设，除了short与char，__首地址都是4对齐的__

- 死循环示例1

<!--language: !c-->

    int main(){
        int i;
        int array[4];
        for(i=0;i<=4;i++)
            array[i]=0;
        return 0;
    }

<!--language: plain-->

      ┌───────┐
      │SavedPC│
      ├───────┤
      │       │ i, array[4]
      ├───────┤
      │       │ array[3]
      ├───────┤
      │       │ array[2]
      ├───────┤
      │       │ array[1]
      ├───────┤
      │       │ array[0]
      └───────┘

- 当每次执行`array[4]=0;`的时候，就修改了`i`，导致`i<=4`条件永远成立，形成死循环或缓冲区溢出

- 死循环示例2

<!--language: !c-->

    void foo(){
        int array[4];
        int i;
        for(i=0;i<=4;i++)
            array[i]-=4;
    }

    int main(){
        foo();
        return 0;
    }

<!--language: plain-->

      ┌───────┐
      │SavedPC│ array[4]
      ├───────┤
      │       │ array[3]
      ├───────┤
      │       │ array[2]
      ├───────┤
      │       │ array[1]
      ├───────┤
      │       │ array[0]
      ├───────┤
      │       │ i
      └───────┘

- 当每次执行`array[4]-=4;`的时候SavedPC的内容原本指向`CALL <foo>`这条语句的后面一条汇编指令，但减4以后，指向`CALL <foo>`指令，当foo返回后，跳到SavedPC指向的地址，则再次执行foo的调用

Lecture 14
==========

## 奇怪的数据传递

<!--language: !c-->

    #include <stdio.h>
    void DeclareAndInitArray(){
        int array[4];
        int i;
        for(i=0;i<4;i++)
            array[i]=i;
    }

    void PrintArray(){
        int array[4];
        int i;
        for(i=0;i<4;i++)
            printf("%d\n",array[i]);
    }

    int main(){
        DeclareAndInitArray();
        PrintArray();
        return 0;
    }

- `DeclareAndInitArray`与`PrintArray`并没有数组的传递，或全局变量的共享，而`PrintArray`还未初始化数组，但照样输出0~3

<!--language: plain-->

           ┌───────┐
           │       │
    argv──>├───────┤
           │       │ argc
           ├───────┤
           │SavedPC│
           ├───────┤
           │SavedPC│
           ├───────┤
           │       │ array[3]
           ├───────┤
           │       │ array[2]
           ├───────┤
           │       │ array[1]
           ├───────┤
           │       │ array[0]
           ├───────┤
           │       │ i
           └───────┘

- 因为这两个函数的局部变量的活动记录的布局是一样的，在前一个函数执行完后，后一个函数的活动记录的位置刚好覆盖了上一个，之前函数操作留在内存中的那些数据并不会在函数返回时清空，所以后一个函数直接可以访问

- 该技巧可减少数据传输浪费的时间和空间，主要是利用程序运行机制中的内存布局来灵活的操作数据（而不是抽象上的逻辑思维）

## printf的原型

- 它的参数个数是不定的，除第一个参数外，其他的类型也是不定的，第一个参数是字符串，它会根据里的占位符，决定后面的参数类型

- `int printf(const char *control, ...)`，所以第一个参数是字符串，后面指定为三个点，编译器就认为可以接受各种类型参数了，而且多数编译器不会对字符串中占位符，以及后面的参数类型进行检查，但gcc是做了检查的。它的返回值是成功解析了占位符的个数（与测试不符，测试返回的是参数个数）

- 这也解释了为什么参数，__从右至左__ 而从 __高地址向低地址__ 存储，即第0个参数，总在所有参数下面，当调用`printf("%d + %d = %d\n",4,4,8);`对应的活动记录是这样：

<!--language: plain-->

           ┌───────┐
           │   8   │
           ├───────┤
           │   4   │
           ├───────┤
           │   4   │
           ├───────┤
           │   *───┼────> "%d + %d = %d\n"
           ├───────┤
           │SavedPC│
     SP ──>├───────┤
           │       │
           ├───────┤
           │       │
           ├───────┤
           │       │
           ├───────┤
           │       │
           └───────┘

- 当`printf`被调用时，它并不知道在第一个参数上面还有多少参数，每个参数中的占位符时，它就像栈帧上方 __数据类型__ 的导航图，在解析时如遇到`%d`，就认为有一个整形数据在第一个参数上方

- 如果占位符多而参数少时，有可能拿未知内容（可能是caller的局部变量的地址作类型转换，引起崩溃）并入字符串，如果占位符少于参数时，则只是空间浪费

## 结构体的内存

- 结构体的基地址就是第一个元素的基地址，这样，如果定义两个结构体，每个结构体中第一个元素是用来指明自己的类型，当程序取得结构体第一个元素时，就知道如何解析剩下的内存是什么


Lecture 15
==========

## 多线程

- 同一个函数的两个线程，有不同的栈段（天生线程安全），共享该进程的堆段，拥有同一代码段（只读）

## 简单示例

<!--language: c-->

    #include <stdio.h>
    #include "thead_107.h"

    void SellTickets(int agentId, int numTicketsToSell){
        while(numTicketsToSell > 0){
            printf("Agent %d sells a ticket\n", agentId);
            numTicketsToSell--;
            if(RandomChance(0.1))
                TheadSleep(1000);
        }
        printf("Agent %d is done\n", agentId);
    }

    int main(){
        int numAgents = 10;
        int numTickets = 150;

        InitThreadPackage(false);
        for(int agent = 1; agent <= numAgents; agent++){
            char name[32];
            sprintf(name, "Agent %d thread", agent);
            ThreadNew(name, SellTickets, 2, agent, (numTickets / numAgents));
        }
        RunAllThreads();
    　　return 0;
    }


> [有关thead_107.h](http://stackoverflow.com/questions/11925352/cs107-assignment-file-couldnt-compile-missing-expat-h-and-thread-107-h-files)

- `InitThreadPackage(false);` false表示不要打印调试信息
- `ThreadNew(name, SellTickets, 2, agent, (numTickets / numAgents));`，第二个参数是函数名，第三个是该函数所需的参数个数，以后参数会传递给该函数
- `RunAllThreads();`运行所有定义的线程，它是阻塞的，会等所有线程运行完再结束
- `TheadSleep(1000);`当前线程暂停占用处理器至少1000毫秒，甚至从ReadyQ中删除，放到BlockQ中，一直到规定的时间过去
- `RandomChance(0.1)`表示10%的情况下返回真
- 目前的线程是没有优先级的

## 线程间共享数据
让可销售的票共享，使用`Semaphore`来模拟锁的功能

<!--language: c-->

    #include <stdio.h>
    #include "thead_107.h"

    void SellTickets(int agentId, int *numTicketsp, Semaphore lock){
        /* while(*numTicketsp > 0){
            (*numTicketsp)--;
        */
        while(true){
            SemaphoreWait(lock);
            if(*numTicketsp == 0) break;
            (*numTicketsp)--;
            SemaphoreSignal(lock);

            printf("Agent %d sells a ticket\n", agentId);
            if(RandomChance(0.1))
                TheadSleep(1000);
        }
        SemaphoreSignal(lock); //if break
        printf("Agent %d is done\n", agentId);
    }

    int main(){
        int numAgents = 10;
        int numTickets = 150;
        Semaphore lock = SemaphoreNew(-,1);
        InitThreadPackage(false);
        for(int agent = 1; agent <= numAgents; agent++){
            char name[32];
            sprintf(name, "Agent %d thread", agent);
            ThreadNew(name, SellTickets, 3, agent, &numTickets, lock);
        }
        RunAllThreads();
    　　return 0;
    }

- 共享的数据，`while(*numTicketsp > 0)(*numTicketsp)--;`并非原子操作，容易造成数据不一致，该段代码称为 __临界区__
- 为了保让临界区只有一个线程在执行，需要引入锁机制，信号量Semaphore是实现它的一种方式
- Semaphore是作为非负整数(sync counter var >=0)发挥作用，支持加1和减1 __原子操作__ 功能的变量类型
- `SemaphoreWait(lock)`执行减1动作，在需要获得资源的时候先执行，要么获得资源，要么需要等待（即Semaphore为0时会被阻塞，等待其他线程的Signal）；`SemaphoreSignal(lock)`执行加1动作，当不需要资源时，释放锁
- Semaphore类型其实是一个指向不完全类型的指针，所以传参时，并不是拷贝，而是共享某种结构体
- `SemaphoreNew(-,1);`如果第2个参数改成0，则一开始就等待某线程发出Signal，改成>1，则表示有多个资源可供使用，但如果用来限制临界区访问的，通常是代码中的模式
- 尽可能让"临界区"小，否则会影响并发性


Lecture 16
==========

## 生产者与消费者

- 模拟互联网服务器与浏览器，推送40字节

<!--language: c-->

    #include <stdio.h>
    #include "thead_107.h"

    char buffer[8];
    Semaphore emptyBuffers = SemaphoreNew(-,8);
    Semaphore fullBuffers = SemaphoreNew(-,0);

    void Writer(){
        for(int i=0; i<40; i++){
            char c = PrepareRandomChar();
            SemaphoreWait(emptyBuffers);
            buffer[i%8] = c;
            SemaphoreSignal(fullBuffers);
        }
    }

    void Reader(){
        for(int i=0; i<40; i++){
            SemaphoreWait(fullBuffers);
            char c = buffer[i%8];
            SemaphoreSignal(emptyBuffers);
            ProcessChar(c);
        }
    }

    int main(){
        InitThreadPackage(false);
        ThreadNew("Writer", Writer, 0);
        ThreadNew("Reader", Reader, 0);
        RunAllThreads();
    　　return 0;
    }

- `Writer`不能写太快，否则`Reader`还未读取，`buffer`中内容就被覆盖了，同样，`Reader`也不能读太快，否则读到`Writer`上次写的东西
- `SemaphoreWait(emptyBuffers);`确保能写，写完后`SemaphoreSignal(fullBuffers);`通知可以读了，`SemaphoreWait(fullBuffers);`确保能读，读完后`SemaphoreSignal(emptyBuffers);`通知可以写了，之间读写速度不大于8字节
- 上例是使用`Semaphore`来模拟锁，而本例则使用它来进行线程间通信
- 可以把`emptyBuffers`的设为4，不会影响代码的正确性，也不会死锁，只是影响了并发性；设为1，则相当于同步调用；设为0则会死锁；设为超过8，则会覆盖未被读的数据
- 把`fullBuffers`的设为大于0，则会读到未写的数据

## 五个哲学家就餐

<!--language: c-->

    #include <stdio.h>
    #include "thead_107.h"

    Semaphore forks[] = {1,1,1,1,1};
    Semaphore numAllowedToEat = 4;

    void Philosopher(int id){
        for(int i=0; i<3; i++){ // i没有意义，只是为了循环
            Think();

            SemaphoreWait(numAllowedToEat);

            SemaphoreWait(forks[id]);
            SemaphoreWait(forks[(id+1)%5]);
            Eat();
            SemaphoreSignal(forks[id]);
            SemaphoreSignal(forks[(id+1)%5]);

            SemaphoreSignal(numAllowedToEat);
        }
    }

    int main(){
        InitThreadPackage(false);
        for(int id=0; id<5; id++){
            char name[32];
            sprintf(name, "Philosopher %d thread", id);
            ThreadNew(name, Philosopher, 1, id);
        }
        RunAllThreads();
    　　return 0;
    }

- 如果五个哲学家同时拿起左侧的叉子，而等待右侧的时候，进入死锁
- 避免死锁的办法有：
    - 可以将`Eat()`上下各两行作为临界区进行加锁，即最多只能一个人吃饭，但这会影响并发性
    - 启发法1：可以只允许 __最多同时两个人__ 吃饭，因为三个人需要6个叉子，资源不够
    - 启发法2(本例所采用)：也可以阻止一位哲学家获得叉子，那么剩下的 __四个人尝试__ 获得叉子，至少有一个能得到两个叉子，这种并发性最好



Lecture 17
==========

## 阻塞直至全部线程发出信号

<!--language: c-->

    #include "thead_107.h"

    Semaphore forks[] = {1,1,1,1,1};
    Semaphore numAllowedToEat = 4;

    int DownloadSingleFile(const char *server, const char *path);

    void DownloadHelper(const char *server, const char *path,
        int *totalBytes, Semaphore lock, Semaphore parentToSignal){
        int bytesDownloaded = DownloadSingleFile(server,path);

        SemaphoreWait(lock);
        (*totalBytes) += bytesDownloaded;
        SemaphoreSignal(lock);

        SemaphoreSignal(parentToSignal);
    }

    int DownloadAllFiles(const char *server, const char *files[], int n){
        int totalBytes = 0;
        Semaphore childrenDone = 0;
        Semaphore lock = 1;

        for(int i=0; i<n; i++){
            ThreadNew("", DownloadHelper, 5, server, files[i],
                &totalBytes, lock, childrenDone);
        }

        for(int i=0; i<n; i++) SemaphoreWait(childrenDone);

        return totalBytes;
    }

    int main(){
        InitThreadPackage(false);
        DownloadAllFiles(...);
        RunAllThreads();
        return 0;
    }


- `int DownloadSingleFile()`，`int`是返回的字节数
- `ThreadNew`的函数指针指向的函数的返回值只能是void，所以需要`DownloadHelper`来封装，第二层原因在于`DownloadSingleFile`应该只是功能函数，它的职责应该单一，不能为了满足线程的签名而定义
- 千万不要把`SemaphoreWait(lock);`移到`int bytesDownloaded = DownloadSingleFile(server,path);`前面，这会导致下载和单线程没什么区别
- `DownloadAllFiles`需要所有的线程都下载完成后，才返回，还需要设置一个信号量`childrenDone`，`for(int i=0; i<n; i++) SemaphoreWait(childrenDone);`确保所有的线程都返回了，否则就阻塞等待信号，而每个线程中的`SemaphoreSignal(parentToSignal);`就是通知主线程。现代许多库中不需要写循环来等待信号，直接`WaitAll(WaitHandle[])`即可
- 可以不需要共享一个`totalBytes`的指针，而使用一个长度为n的数组，让每个线程完成后填充数据，再在主线程中等所有线程返回后，对数组进行累加。
- 信号量才是真正的异步机制，不需要循环侦听，不需要回调，等待到信号就往下执行


Lecture 18
==========

## 冰激凌商店模拟

- 10个Customers, 每个Customer限制购买1~4个IceCreamCones
- 1个Cashier，每个Customer都向他付钱
- 1个Manager，他检查批准是否售出IceCreamCones
- 10~40个Clerk，因为每个Customer有可能需要1~4个IceCreamCones，就需要相应的Clerk去制作
- Customer看不到Manger，只能同Clerk下单
- 每个Clerk需要带着做好的IceCreamCones向Manger送检，后者决定是否售出，如果不同意，还需要重新制作再送检
- Customer得到做好的IceCreamCones后，按FIFO的顺序排队向Cashier付款

<!--language: plain-->

    1 Manager
        |
        |竞争
        |
    10~40 Clerks
        |
        |并发
        |
    10 Customers  [1~4]IceCreamCones
        |
        |排队
        |
    1 Cashier

示意代码：

<!--language: c-->

    #include "thead_107.h"

    struct inspection{  //Manager检查的结构体
        bool passed = false;
        Semaphore requested = 0; //Clerk送检事件
        Semaphore finished = 0; //Manager检查完毕事件
        Semaphore lock = 1; //确保当前结构体只是一个Clerk在使用
    }inspection;

    struct line{
        int number=0;
        Semaphore requested = 0; //Customer付款事件
        Semaphore customers[10];
        Semaphore lock = 1; //对number的保护
    }line;

    void Manager(int totalConesNeeded){
        int numApproved = 0;
        int numInspected = 0;
        while(numApproved < totalConesNeeded){//确保Customer需要的都是检查过的
            SW(inspection.requested);
            numInspected++;
            inspection.passed=RandomChance(0,1);
            if(inspection.passed)
                numApproved++;
            SS(inspection.finished);
        }
    }

    void Clerk(Semaphore semaToSignal){
        bool passed = false;
        while(!passed){
            TakeCone();
            SW(inspection.lock);
            SS(inspection.requested);
            SW(inspection.finished);
            passed = inspection.passed;
            SS(inspection.lock);
        }
        SS(semaToSignal);
    }

    void Cashier(){
        for(int i=0;i<10;i++){
            SW(line.requested);
            Chkeckout(i);
            SS(line.customers[i]); //通知排队的customer完成了账单
        }
    }

    void WalkToCahier(){
        SW(line.number);
        line.number++;
        line.customers[line.number]
    }

    void Customer(int numCones){
        BrowX();
        Semaphore clerksDone = 0;
        for(int i=0;i<numCones;i++)
            ThreadNew("", Clerk, 1, clerksDone);
        for(i=0;i<numCones;i++) SW(clerksDone);

        SemaphoreFree(clerksDone);
        WalkToCahier();

        SW(line.lock);
        int place = line.number++;
        SS(line.lock);

        SS(line.requested);
        SW(line.customers[place]); // 等待账单完成才可以离开
    }

    int main(){
        int totalCones=0;
        InitThreadPackage(false);

        SetupSemaphores();
        for(int i=0;i<10;i++){
            int numCones = RandomInteger(1,4);
            ThreadNew("", Customer, 1, numCones);
            totalCones+=numCones;
        }

        ThreadNew("", Cashier, 0);
        ThreadNew("", Manager, 1, totalCones);

        RunAllThreads();
        FreeSemaphores();

        return 0;
    }



Lecture 19
==========

## 面向对象

<!--language: plain-->

    imperative/procedural  object-oriented
    C                      C++
    Vector v;              Vector<int> v;
    VectorNew(&v,...);     v.pushback(4);
    VectorInsert(&v,...);  v.erase(v.begin());
    VectorSort(&v,...);

- 左侧的以动词开始，面向过程的；右侧的是一个对象，主谓结构

## 函数式语言Scheme

- scheme试图强调的是一个数据集给到主函数，这个主函数应该为你做一切，它适时将任何需要来合成结果，而那个答案将通过返回值返回

<!--language: plain-->

    假设：
    f(x,y) = x^3 + y^2 +7
    g(x) = f(x, x+1) + 8
    h(x,y,z) = f(x,z) * g(x+y)
    则：
    h(x,y,z)
             = (x^3 + z^2 +7) * (f(x+y, x+y+1) + 8)


- `f`与`g`函数将自己作为函数返回值，这些返回值有助于组成更大的结果

<!--language: !scheme-->

    (define (celsius->fahrenheit temp)
        (+ 32 (* 1.8 temp)))

    (display (celsius->fahrenheit 100))
    ;;;212

<!--language: scheme-->

    > (+ 1 2 3)
    6

    > (* (+ 4 4)
         (* 5 5))
    80

- `->`是可以放入命名中的
- `+`,`*`之类的看起来像操作符，类似buildin的函数，而且参数不定
- 单个数值或字符串之类的原子类型(atom)也是表达式
- 嵌套的列表应该是递归计算
- 函数调用的思维转化为求值过程

<!--language: scheme-->

    > (> 4 2)
    #t

    > (and (> 4 2)
           (< 10 5))
    #f

- 第二个求值过程甚至会"短路求值"
- 称为Lisp的原因是，所有东西，包括函数调用，都输入列表形式，除了原子的数据类型，我们将所有数据结构认为是包装成一个列表

<!--language: scheme-->

    > (car '(1 2 3 4 5))
    1

    > (car '())
    car: expects argument of type <pair>; given empty

    > (cdr '(1 2 3 4 5))
    (2 3 4 5)

    > (car(cdr(cdr '(1 2 3 4 5))))
    3

    > (cdr '(4))
    ()

    > (cdr '())
    cdr: expects argument of type <pair>; given empty

- `car`和`cdr`是基本的列表分析器，有些版本允许使用`first`和`rest`来指代
- `car`与第一个节点的数据相关联，它总是占据一个列表里的0槽中的值
- `cdr`返回余下的列表
- 如果把列表前面`'`去掉，它将认为有一个函数叫`1`，但是找不到，就报错，加入`'`是抵制求值，当成原始数据，而不是一个递归求值。这是一个简记法，对应的函数是`quote`，`'(1 3 (4(5)))`等价于`(quote(1 3 (4(5))))`

<!--language: scheme-->

    > (cons 1 '(2 3 4 5))
    (1 2 3 4 5)

    > (cons '(1 2 3) '(4 5))
    ((1 2 3) 4 5)

    > (append '(1 2 3) '(4 5))
    (1 2 3 4 5)

    > (append '(1 2 3) '(4 5) '(6 7))
    (1 2 3 4 5 6 7)

    > (append '(1 2 3) (list 5) '(6 7))
    (1 2 3 5 6 7)


- `cons`是construct的缩写，它是`car`的反义操作
- 而`append`才是列表的合并，并且可以接受任意个列表参数


<!--language: scheme-->

    (define (add x y)
       (+ x y))

    > (add 10 7)
    7

<!--language: !scheme-->

    (define (sum-of numlist)
        (if (null? numlist) 0
            (+ (car numlist)
               (sum-of (cdr numlist)))))

    (display (sum-of '(1 2 3 4)))
    ;;;10


- `sum-of`的实现是以递归的思维实现的，以尾递归实现迭代
- `(sum-of '("hello" 1 2 3 4 5))`会报错，除非对`+`进行了重载，但报错的时间是在递归计算到`(+ "hello"` 15)`的时候发生，类型判断是在运行时发生的


Lecture 20
==========

## fibonacci

- 菲波那契序列的两种表达

<!--language: !scheme-->

    (define (fib n)
       (if (zero? n) 0
           (if (= n 1) 1
               (+ (fib (- n 1))
                  (fib (- n 2))))))

    (display (fib 10))
    ;;;55

<!--language: !scheme-->

    (define (fib n)
       (if (or (= n 0)
               (= n 1)) n
           (+ (fib (- n 1))
              (fib (- n 2)))))

    (display (fib 10))
    ;;;55


- 运行时语言，即使后面数据类型错误，也不会求值，更不会报错

<!--language: scheme-->

    > (if (zero? 0) 4
          (+ "hello" 5))
    4


## flatten

<!--language: !scheme-->

    (define (flatten sequence)
       (cond (((null? sequence) '())
              ((list? (car sequence))
                     (append (flatten (car sequence))
                             (flatten (cdr sequence))))
              (else (cons (car sequence)
                          (flatten (cdr sequence)))))))

    (display (flatten '(1 2 3 4)))
    ;;;(1 2 3 4)

    (display (flatten '(1 (2 "3") 4 ("5"))))
    ;;;(1 2 "3" 4 "5")

- 假设不会转入`'()`空列表

## sorted?

<!--language: !scheme-->

    (define (sorted? numlist)
       (or (< (length numlist) 2)
           (and (<= (car numlist)
                    (cadr numlist))
                (sorted? (cdr numlist)))))

    (display (sorted? '(1 2 2 4 7)))
    ;;;#t

    (display (sorted? '(1 0 4 7 10)))
    ;;;#f


- `cadr`嵌套的表达，相当于先`cdr`再`car`，即取第二的元素，以此类推可以有`cadadr`,`cdddr`……
- 其实Scheme中的`<`和`<=`本身支持多个参数，可以表示是否已是有序序列

<!--language: scheme-->

    > (< 1 2 3 4)
    #t

    > (<= 1 2 3 4 4)
    #t

## sorted? with compare

- `cmp`是函数对象

<!--language: !scheme-->

    (define (sorted? numlist cmp)
       (or (< (length numlist) 2)
           (and (cmp (car numlist) (cadr numlist))
                (sorted? (cdr numlist) cmp))))

    (display (sorted? '(1 2 2 4 7) <=))
    ;;;#t



Lecture 21
==========

## map

<!--language: !scheme-->

    (define (double-all numlist)
       (if (null? numlist) '()
           (cons (* 2 (car numlist))
                 (double-all (cdr numlist)))))

    (display (double-all '(1 2 5 69)))
    ;;;(2 4 10 138)

- 上面的迭代可使用`map`来统一处理，只需定义`double`即可，这样，即让`map`通用化，也让`double`职责单一化

<!--language: !scheme-->

    (define (double x)(* x 2))

    (display (map double '(1 2 5 69)))
    ;;;(2 4 10 138)

- `map`第一个参数接受函数对象，后面的序列作为要迭代处理的，甚至可以是多个序列，此时函数对象应为多元计算，以最小序列结束为结束

<!--language: !scheme-->

    (display (map cons '(1 2 8) '((4) () (2 5))))
    ;;;((1 4) (2) (8 2 5))


- 下面定义自己的`mymap`，目前只接受一个序列

<!--language: !scheme-->

    (define (mymap fn seq)
        (if (null? seq) '()
            (cons (fn (car seq))
                  (mymap fn (cdr seq)))))

    (define (double x)(* x 2))

    (display (mymap double '(1 2 5 69)))
    ;;;(2 4 10 138)

## apply

- 几个内置函数`eval`,`apply`

<!--language: scheme-->

    > (eval '(+ 1 2 3))
    6

    > (apply + '(1 2 3))
    6

- 求平均值

<!--language: !scheme-->

    (define (average numlist)
      (/ (apply + numlist)
         (length numlist)))

    (display (average '(1 2 3 4 5)))
    ;;;3

- 计算序列的和时，使用了`(apply + numlist)`，即将`+`的运算应用在后面的序列中，相当于将`+`所需的不固定参数通过一个序列来表达（同js中apply中对参数的处理效果一致）

- `map`和`apply`对序列处理是平等的，以集合处理的思维进行思考，（即使内部也是使用递归），而不像`car`,`cdr`这样有明显迭代处理方式

- `eval`一般很少用到，除非一些元编程中

## flatten with apply

- `flatten`的另一种实现，尝试使用`apply`+`map`来处理

<!--language: !scheme-->

    (define (flatten seq)
      (if (not (list? seq)) (list seq)
        (apply append
          (map flatten seq))))

    (display (flatten '((1 2) ((3) ((4) 5)) 10)))
    ;;;(1 2 3 4 5 10)

思路如下：

<!--language: plain-->

    ((1 2) ((3) ((4) 5)) 10)
     | map flatten n times
     v
    ((1 2)  (3 4 5)     (10))
     | apply append
     v
    (1 2 3 4 5 10)


## lambda

- 实现一个函数，完成以下功能

<!--language: scheme-->

    > (translate '(2 5 8 11 25) 100)
    (102 105 108 111 125)

- 函数的定义如下

<!--language: scheme-->

    (define (translate points delta)
      (map (?)
           points))

- 问题在于`map`中的函数对象`?`怎么定义，它表达的意思应为`increment_by_delta`，但这个函数需要二个参数，而不是一个，我们需要把`delta`封装到`lambda`中，通过闭包来保持它

<!--language: !scheme-->

    (define (translate points delta)
      (map (lambda (x)
             (+ x delta))
           points))

    (display (translate '(2 5 8 11 25) 100))
    ;;;(102 105 108 111 125)

- `lambda`定义的是匿名函数对象，它只存活于`map`运行期间。当然也可以定义具名函数：

<!--language: !scheme-->

    (define (translate points delta)
      (define (shift-by x)
        (+ x delta))
      (map shift-by points))

    (display (translate '(2 5 8 11 25) 100))
    ;;;(102 105 108 111 125)

- 定义`sum`两种方式，第一种符合编程习惯，而第二种是将函数名与函数体更清楚的关联起来，其实Scheme中函数都表现为一个函数符号与一个lambda的关联。即关于 符号、符号评估、函数评估 到它们在内存中的储存方式

<!--language: scheme-->

    (define (sum x y)
      (+ x y))

    (define sum
      (lambda (x y) (+ x y)))

- 所以`(define PI 3.14)`与上面第二种的表达的一样，将`PI`这个名与`3.14`对象关联起来


Lecture 22
==========

## power-set

- power-set，列举集合中的所有子集

<!--language: scheme-->

    > (power-set '(1 2 3))
    (() (3) (2) (2 3)
    (1) (1 3) (1 2) (1 2 3))

- 分析一下规律：共有8个子集，上半部分不含1，而下半部分则含有1，下半部分是对上半部分的`cons 1`的操作。
- 再分析一下上半分部，又由两个部分组成：`() (3)`和`(2) (2 3)`，后半部分是对前半部分`cons 2`的操作。
- 最后的`() (3)`亦为后半部分对前半部分的`cons 3`操作
- 至此，找出生成它们的规律，以及生成的个数为2^N

- 下例中，第一次迭代中，我们需要产生两次`() (3) (2) (2 3)`，但第二次需要`cons 1`操作

<!--language: !scheme-->

    (define (power-set set)
       (if (null? set) '(())
           (append (power-set (cdr set))
                   (map (lambda (subset)
                          (cons (car set) subset))
                        (power-set (cdr set))))))

    (display (power-set '(1 2 3)))
    ;;;(() (3) (2) (2 3) (1) (1 3) (1 2) (1 2 3))


- `append`除第一个元素外后所有子集（所以是一个递归调用，需要信念认为`power-set`已经能正常工作了），和`map`子句
- `map`子句表达为，在每一个合法的子集前面`cons`集合的第一个元素

## power-set with let

- 存在两个递归过程`(power-set (cdr set))`，对性能存在影响，需要对其进行优化，使用`let`绑定（它可以绑定多个标识，每对以`()`区分）

<!--language: !scheme-->

    (define (power-set set)
       (if (null? set) '(())
           (let ((ps-set (power-set (cdr set))))
             (append ps-set
                     (map (lambda (subset)
                            (cons (car set) subset))
                        ps-set)))))

    (display (power-set '(1 2 3)))
    ;;;(() (3) (2) (2 3) (1) (1 3) (1 2) (1 2 3))

- `let`的绑定

<!--language: scheme-->

    (let ((x expr1)
          (y expr2))
      (expr x y))

- 如果`expr2`中出现`x`是可能出错的，因为`x,y,z`的求值的顺序是不可靠的，应当做是并行评估的
- 确实需要按顺序执行时，使用`let*`，但这种情况并不多见

- 上面的`let`绑定其实相当于下面的`lambda`，是无法强制`expr1,expr2`求值的顺序的，也不希望它们之间是相互影响的

<!--language: scheme-->

    ((lambda (x y)
       (expr x y)) expr1 expr2)


## permute

- 实现`permute`，认为输入的是有序的一个集合，不存在重复元素，要求输出所有组合

<!--language: scheme-->

    > (permute '(1 2 3))
    ((1 2 3) (1 3 2)
     (2 1 3) (2 3 1)
     (3 1 2) (3 2 1))

- 分析一下规律：第一行都以1开头，第二行以2开头，第三行以3开头
- 而每一行除了第一个外，其它的是剩下的元素的全组合
- 数量为`n!`
- 可分解步骤为：

<!--language: plain-->

    首先表达为以下各组合的连接
    ((1-perms) (2-perms) (3-perms))

    而每个组合又可细分，如(1-perms)等价于
    1 cons 所有permute(items remove 1)后的集合


- 刚开始程序的框架是这样的，使用`apply append`和`map`连接各个已排好的组合：

<!--language: scheme-->

    (define (permute items)

      (apply append
             (map ??
                  items)))

- 要`cons 所有(items remove elem)后的组合`还需要一个`map`，进化成如下：

<!--language: scheme-->

    (define (permute items)

      (apply append
             (map (lambda (elem)
                    (map (lambda(permutation)
                           (cons elem permutation))
                         (permute(remove items elem))))
                  items)))

- 需要给递归加上退出的条件，并将`remove`给实现了，最终代码如下：

<!--language: !scheme-->

    (define (remove items elem)
      (apply append
             (map (lambda (el)
                    (if (= el elem) '()
                        (list el)))
                    items)))


    (define (permute items)
      (if (null? items) '(())
          (apply append
                 (map (lambda (elem)
                        (map (lambda(permutation)
                               (cons elem permutation))
                             (permute(remove items elem))))
                      items))))

    (display (permute '(1 2 3)))
    ;;;((1 2 3) (1 3 2) (2 1 3) (2 3 1) (3 1 2) (3 2 1))



Lecture 23
==========

## 内存表示

<!--language: scheme-->

    > '(1 2 3)

在内存中表示为：

<!--language: plain-->

                  ┌───┬───┐    ┌───┬───┐    ┌───┬───┐
                  │int│ 1 │    │int│ 2 │    │int│ 3 │
                  └───┴───┘    └───┴───┘    └───┴───┘
                  ^            ^            ^
                  │            │            │
               ┌──┼──┐      ┌──┼──┐      ┌──┼──┐
               │ car │      │ car │      │ car │
               ├─────┤      ├─────┤      ├─────┤      ┌────┐
               │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
    point to ->└─────┘      └─────┘      └─────┘      └────┘

同时上面表达式只是下面的简写方式：

<!--language: scheme-->

    > (cons 1 (cons 2 (cons 3 '())))


- `define`是少数有副作用的函数，它会加入一些全局符号表

<!--language: scheme-->

    > (define seq '(1 2 3))

在内存中表示为：

<!--language: plain-->

                      ┌───┬───┐    ┌───┬───┐    ┌───┬───┐
                      │int│ 1 │    │int│ 2 │    │int│ 3 │
                      └───┴───┘    └───┴───┘    └───┴───┘
                      ^            ^            ^
                      │            │            │
                   ┌──┼──┐      ┌──┼──┐      ┌──┼──┐
                   │ car │      │ car │      │ car │
    ┌───┬───┐      ├─────┤      ├─────┤      ├─────┤      ┌────┐
    │seq│ *─┼──┐   │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
    └───┴───┘  └──>└─────┘      └─────┘      └─────┘      └────┘


- `(cons '(1 2 3) '(4 5 6))`的内存表示如下，它产生一个新的列表，`'(1 2 3)`评估后的地址存放在该列表的`car`部分，`'(4 5 6)`评估后的地址存放在该列表的`cdr`部分，而整个表达式评估后的地址以左下角箭头表示

<!--language: plain-->

                    ┌───┬───┐    ┌───┬───┐    ┌───┬───┐
                    │int│ 4 │    │int│ 5 │    │int│ 6 │
                    └───┴───┘    └───┴───┘    └───┴───┘
                    ^            ^            ^
                    │            │            │
                 ┌──┼──┐      ┌──┼──┐      ┌──┼──┐
                 │ car │      │ car │      │ car │
                 ├─────┤      ├─────┤      ├─────┤      ┌────┐
                 │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
             ┌──>└─────┘      └─────┘      └─────┘      └────┘
             │
             │
             │    ┌───┬───┐    ┌───┬───┐    ┌───┬───┐
             │    │int│ 1 │    │int│ 2 │    │int│ 3 │
             │    └───┴───┘    └───┴───┘    └───┴───┘
             │    ^            ^            ^
             │    │            │            │
             │ ┌──┼──┐      ┌──┼──┐      ┌──┼──┐
             │ │ car │      │ car │      │ car │
             │ ├─────┤      ├─────┤      ├─────┤      ┌────┐
             │ │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
             │ └─────┘      └─────┘      └─────┘      └────┘
             │ ^
         ┌─────┘
      ┌──┼──┐│
      │ car ││
      ├─────┤│
      │ cdr ┼┘
    ->└─────┘

- `(cons 4 5)`的结果是`(4 . 5)`，`. 5`代表5不是一个列表，只是单独的一个元素位于`cdr`区域，称为dotted pairs，有点像关联数组的结构

- 如果想让内存结构变成如下，`car`与`cdr`指向同样的列表，输出结果依然是`((1 2) 1 2)`

<!--language: plain-->

                    ┌───┬───┐    ┌───┬───┐
                    │int│ 1 │    │int│ 2 │
                    └───┴───┘    └───┴───┘
                    ^            ^
       ┌─────┐      │            │
    ┌──┼──┐  │   ┌──┼──┐      ┌──┼──┐
    │ car │  │   │ car │      │ car │
    ├─────┤  │   ├─────┤      ├─────┤      ┌────┐
    │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
    └─────┘  │   └─────┘      └─────┘      └────┘
             │   ^
             └───┘

- 通过`lambda`中的变量来进行绑定，防止了`(cons '(1 2) '(1 2))`中对常量的依赖

<!--language: !scheme-->

    ((lambda (x) (cons x x)) '(1 2))

## map的实现

### 一元`map`

- 一元`map`（供map中运算函数迭代的只有一个列表）

<!--language: !scheme-->

    (display (map car '((1 2) (3 4) (5 6 7))))
    ;;;(1 3 5)

- 它的实现大概如下：

<!--language: !scheme-->

    (define (unary_map fn seq)
      (if (null? seq) '()
          (cons (fn (car seq))
                (unary_map fn (cdr seq)))))

    (display (unary_map car '((1 2) (3 4) (5 6 7))))
    ;;;(1 3 5)

### 多元`map`

- 多元`map`（供map中运算函数迭代的有多个列表）

<!--language: !scheme-->

    (display (map + '(1 2) '(10 20) '(100 200)))
    ;;;(111 222)

    (display (map * '(1) '(2) '(3) '(4) '(5)))
    ;;;(120)

- 首先需要明确不定参数的定义方式：

<!--language: !scheme-->

    (define (bar a b c . d)
      (list a b c d))

    (display (bar 1 2 3 4 5 6))
    ;;;(1 2 3 (4 5 6))

    (display (bar 1 2 3))
    ;;;(1 2 3 ())

- 并假设，`fist_list`与`other_lists`中列表长度均一致（真实的map一般迭代次数为最少list的length）

<!--language: !scheme-->

    (define (unary_map fn seq)
      (if (null? seq) '()
          (cons (fn (car seq))
                (unary_map fn (cdr seq)))))

    (define (multiary_map fn first_list . other_lists)
      (if (null? first_list) '()
          (cons (apply fn (cons (car first_list)
                                (unary_map car other_lists)))
                (apply multiary_map (cons fn
                                     (cons (cdr first_list)
                                           (unary_map cdr other_lists)))))))

    (display (multiary_map + '(1 2) '(10 20) '(100 200)))
    ;;;(111 222)

    (display (multiary_map * '(1) '(2) '(3) '(4) '(5)))
    ;;;(120)

- 通过`cons`与`unary_map`将嵌套列表变成单一列表，再通过`apply`将单一列表作不定参数的展开调用

## 垃圾回收

- 运行`> (cons "hello" '("there"))`的内存分配如下：

<!--language: plain-->

          hello        there
          ^            ^
          │            │
       ┌──┼──┐      ┌──┼──┐
       │ car │      │ car │
       ├─────┤      ├─────┤      ┌────┐
       │ cdr ┼─────>│ cdr ┼─────>│NULL│
    ──>└─────┘      └─────┘      └────┘

- 左下角的箭头即cons后的值的地址，打印出来后，就不被引用，这部分内存就会被回收，它所指向的`car`,`cdr`也依次通过递归的方式被回收

- 定义常量`(define x '(1 2 3))`，除非重新定义`x`，否则`x`所指向的列表地址不会被回收

- 再定义`(define y (cdr x))`

<!--language: plain-->

           1            2            3
           ^            ^            ^
           │            │            │
        ┌──┼──┐      ┌──┼──┐      ┌──┼──┐
        │ car │      │ car │      │ car │
        ├─────┤      ├─────┤      ├─────┤      ┌────┐
        │ cdr ┼─────>│ cdr ┼─────>│ cdr ┼─────>│NULL│
    x──>└─────┘      └─────┘      └─────┘      └────┘
                     ^
    y────────────────┘

- 再次定义`(define x '(4 5))`，则常量`x`并不再指向`'(1 2 3)`评估的地址，Scheme将检测出`'(1 2 3)`中的`1`不再有常量指向它，则回收这部分内存

<!--language: plain-->

                        2            3
                        ^            ^
                        │            │
                     ┌──┼──┐      ┌──┼──┐
                     │ car │      │ car │
                     ├─────┤      ├─────┤      ┌────┐
                     │ cdr ┼─────>│ cdr ┼─────>│NULL│
                     └─────┘      └─────┘      └────┘
                     ^
    y────────────────┘

- 垃圾回收可采用"引用计数"方式，但有可能存在不被回收的内存。也有采用"标记清除"

Lecture 24
==========

## python

<!--language: !python-->

    def gatherDivisors(number):
        divisors = []
        for div in range(1, number/2 + 1):
            if not number%div:
                divisors.append(div)
        return divisors

    print gatherDivisors(24)


Lecture 25
==========

## python中dict

- python中dict是异构的，甚至key也是

- Python中RSG（Random Sentence Generator）

<!--language: !python-->

    import sys
    from random import choice,seed

    grammer = {'<start>': [['this ','<object>',' is here']],
               '<object>': [['computer'],
                            ['car'],
                            ['assignment']]}

    def expend(symbol):
        if symbol.startswith('<'):
            definitions = grammer[symbol]
            expansion = choice(definitions)
            map(expend, expansion)
        else:
            sys.stdout.write(symbol)

    seed()
    expend('<start>')

- 深拷贝，对象内部结构也被很好保存下来，如下例中n中的两个m指向同一列表，深拷贝后的p[0],p[1]也应指向同一列表

<!--language: !python-->

    from copy import deepcopy

    m = [1,2,3]
    n = [m, m]
    p = deepcopy(n)
    print p is n
    p[0].append(4)
    print p

- python的面向对象是基于dict的

<!--language: !python-->

    class Cls(object): pass
    o = Cls()
    print o.__dict__
    o.name = "Bob"
    o.age = 27
    print o.__dict__
    o.__dict__['gender'] = 'M'
    print o.gender



Lecture 26
==========

## python中处理xml

- 处理XML有两种式：DOM（Document Object Model）将XML数据在内存中解析成一个树，通过对树的操作来操作XML；SAX（Simple API for XML）采用事件驱动模型，通过在解析XML的过程中触发一个个的事件并调用用户定义的回调函数来处理XML文件，流式读取XML文件，比较快，占用内存少

- 通过`xml.sax`来处理RSS feed

<!--language: !python-->

    from urllib2 import urlopen
    from xml.sax import make_parser, ContentHandler
    import sys

    def listFeedTitles(url):
        infile = urlopen(url)
        parser = make_parser()
        parser.setContentHandler(RSSHandler())
        parser.parse(infile)

    class RSSHandler(ContentHandler):
        def __init__(self):
            ContentHandler.__init__(self)
            self.__inItem = False
            self.__inTitle = False

        def characters(self, data):
            if self.__inTitle:
                sys.stdout.write(data)

        def startElement(self, tag, attrs):
            if tag == 'item': self.__inItem = True
            if tag == 'title' and self.__inItem:
                self.__inTitle = True

        def endElement(self, tag):
            if tag == 'title' and self.__inTitle:
                sys.stdout.write("\n")
                self.__inTitle = False
            if tag == 'item': self.__inItem = False

    listFeedTitles("http://news.google.com/news?pz=1&cf=all&ned=us&hl=en-US&output=rss")


Lecture 27
==========

## haskell

- 具有编译时间，会检查类型安全
- 列表中元素是同构的
- 列表解析，python也是从它借鉴过去的

- fibonacci在haskell中实现

<!--language: haskell-->

    fib = 1 : 1 : zipWith (+) fib (tail fib)


- 它是lazy的，__不是必须做的它就不会做__，`take 5 fib`相当于取下面前5个，后面的丢弃

<!--language: haskell-->

    fib = 1 : 1 : 2 : 3 : 5 : zipWith (+) fib (tail fib)

- 利用lazy可以生成无穷序列，也可让参数延后被评估

<!--language: haskell-->

    infinite = 1..
    infinitePlusOne = map(+ 1)(1..)
    infinitePlusOneMinus6 = filter(>6)(map(+ 1)(1..))

- 类型可以被推演出来，从而加快速度，`fib = 1 : 1 : zipWith (+) fib (tail fib)`可被推演出`fib`是`List<int>`类型的

- 合并两个已排序的序列

<!--language: haskell-->

    listMerge :: List Int -> List Int -> List Int

    listMerge [] [] = []
    listMerge a []  = a
    listMerge [] b  = b
    listMerge (a:as) (b:bs) = if a < b
                              then a : listMerge as (b:bs)
                              else b : listMerge (a:as) bs

- `atoi`将ascii字符表示的整形转换成真正的整形

<!--language: haskell-->

    atoi :: List Char -> Int

    dtoi = ord c - ord '0'
    atoi str = atoi' str 0
    atoi' [] acc = acc
    atoi' (c:cs) acc = atoi' cs (10 * acc + dtoi c)

等同于以下python代码（体现了函数式中对python及js的借鉴之处，以递归代替循环，但要考虑栈溢出）

<!--language: python-->

    def atoi(astr):
        def _dtoi(c):
            return ord(c) - ord('0')
        def _atoi(astr, acc):
            if not len(astr): return acc
            return _atoi(astr[1:],10 * acc + _dtoi(astr[0]))
        return _atoi(astr,0)

- 判断某字符串是否是另一个字符串的前缀

<!--language: haskell-->

    prefix :: List Char -> List Char -> Bool

    prefix (a:as) (b:bs) = if a == b
                           then prefix as bs
                           else False
    prefix [] (b:bs) = True
    prefix (a:as) [] = False














