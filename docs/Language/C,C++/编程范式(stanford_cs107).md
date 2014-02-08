第一课
======
概述

第二课
=======
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
总的来说就是 __just copy bit pattern!__ 大小尾系统 __没有__ 区别，因为 __地址&总指向 *最低* 字节__

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


第三课
=========





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
        double a = 23.0, b = 34.0;
        swap(&a, &b, sizeof(double));
        printf("%lf %lf", a, b);
    }

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
        char *husband = strdup("Fred"); char *wife = strdup("Wilma");
        swap(&husband, &wife, sizeof(char *));
        printf("%s %s", husband, wife);
    }

http://www.cppblog.com/deercoder/archive/2012/06/24/180038.html