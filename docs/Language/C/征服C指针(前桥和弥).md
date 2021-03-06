> 2014-08-01

C是什么样的语言
===============

## 只能使用标量
__标量__ 就是指`char`、`int`、`double`和`枚举`等数值类型，以及`指针`，相对的，像`数组`、`结构体`和`共用体`这样的将多个标量进行组合的类型，称为 __聚合类型__(aggregate)

`if(str == "abc")`这样的代码为什么不能执行预期的动作呢，确实已经将"abc"放到了`str`中，条件表达式却不为真。

- 通常答案是，这个表达式不是在比较字符串的内容，它只是 __在比较指针__
- 另一个答案是，字符串其实就是char类型的数组，也就是说它 __不是标量__，当然在C里面不能用`==`进行比较了。

C语言是一门，不用说对输入输出，就连数组和结构体也放弃了通过语言自身进行整合利用的语言。

但如今ANSI C通过以下几个追加的功能（早期C语言不存在），能够让我们整合地使用聚合类型了：

- 结构体的一次赋值（无须再使用`memcpy()`）
- 将结构体作为函数返回值传递
- 将结构体作为函数返回值返回
- auto变量的初始化

但依然不能做到对数组的整合利用，如数组赋值给另外一个数组，或者将数组作为参数传递。所以在结构体能替换数组的情况下，应优先使用。

## 关于指针
指针类型、指针类型变量、指针类型的值，经常被简单的统称为“指针”，造成歧义，必须明确：

- 先有 __指针类型__
- 所以有了 __指针类型的 *变量*__ 和 __指针类型的 *值*__(内存的地址)

### 变量保存状况

<!--language: !c-->

    #include <stdio.h>
    int main(void){
        int hoge = 5;
        int piyo = 10;
        int *hoge_p;

        printf("&hoge..%p\n", &hoge);
        printf("&piyo..%p\n", &piyo);
        printf("&hoge_p..%p\n", &hoge_p);

        hoge_p = &hoge;
        printf("hoge_p..%p\n", hoge_p);

        return 0;
    }

发现变量按照声明的逆向顺序保存在内存中，这是常见的现象，但非一定，即变量 __不一定__按照声明顺序保存的内存中（但结构体中成员变量在内存中布局与声明顺序一致）。

<!--language: plain-->

               ┌──────────┐
               │    5     │  hoge
    0x76cd5a78 ├──────────┤
               │    10    │  piyo
    0x76cd5a74 ├──────────┤
               │0x76cd5a78│  hoge_p
    0x76cd5a70 └──────────┘

### 指针和地址之间关系
几乎所有的处理程序中，所谓的“指针类型的值”，实际上是指内存的地址。对此的疑问有：

- 归根结底，指针就是地址，地址就是内存中被分配的“门牌号”，所以，指针类型和int类型是一回事吧
    - 在C中，指针和整数是没有区别的，但运行环境不具有普适性，很多运行环境中，int类型和指针类型的长度并不相同，此外，对于分段寻址的（段地址+偏移量）也不适用。

- 指针就是地址吧，那么指向int的指针也好，指向double的指针也好，它们有什么不一样吗，有必要区分它们吗？
    - 对于大部分运行环境来说，当程序运行时，不管是指向int的指针，还是指向double的指针，都保持相同的表现形式（偶尔在一些运行环境中，指向char指针和指向int的指针有不一样的内部表示和位数）

<!--language: !c-->

    #include <stdio.h>
    int main(void){
        int hoge = 5;
        void *hoge_p;

        hoge_p = &hoge;
        /* printf("%d\n", *hoge_p);*/
        /* warning: dereferencing 'void *' pointer invalid use of void expression*/
        printf("%d\n", *(int*)hoge_p);

        return 0;
    }

<!--language: !c-->

    #include <stdio.h>
    int main(void){
        int *int_p;
        double double_variable = 5;

        int_p = &double_variable;
        /* warning: assignment from incompatible pointer type*/

        return 0;
    }

### 指针运算
指针运算是针对指针进行 __整数加减__ 运算，以及 __指针之间进行减法__ 运算的功能。

对指针加N，指针前进“当前指针指向的数据类型的长度 X N”。由此也可见“编译器记住指针指向什么样的类型”是有重要意义的

指针加减运算，标准只允许指针指向数组内的元素，或者超过数组长度的下一个元素，标准没有对除此之外的情况做出任何定义。

下例有点违反C语言标准，但在大多数环境下，是可行的

<!--language: !c-->

    #include <stdio.h>
    int main(void){
        int hoge1 = 5;
        int hoge2 = 10;
        int *hoge_p;

        hoge_p = &hoge1;
        printf("hoge_p..%p\n", hoge_p);

        hoge_p++;
        printf("hoge_p..%p\n", hoge_p);
        printf("*hoge_p..%d\n", *hoge_p);

        return 0;
    }

如果指针类型的长度与整形的长度一致的话，最后一行将输出`10`

### 什么是空指针
空指针是指可以确保没有指向任何一个对象的指针，通常使用宏定义`NULL`来表示空指针常量值

如今的操作系统下，应用程序一旦试图通过空指针引用对象，就会马上招致一个异常并且当前应用程序会被操作系统强制终止。因此，如果每次都使用`NULL`来初始化指针变量，在错误的使用了无效（未初始化）的指针时，我们就会马上发现潜在BUG

`NULL`无论对方指向什么类型的变量，都可被赋值和比较，偶尔会见到先将空指针强制转型，然后再赋值、比较的程序，不但是徒劳的，还不易于阅读。

### 函数参数
调用C的函数，参数传递往往是 __传值__，这种方式传递的是参数的 __副本__（形参是实参的副本）。

函数的 __形参__ 都和调用时被设定值的 __局部变量__ 一样，如果不这样的话，就会背负Fortran那样的危险，或者像Pascal那样在语法下的做些文章。

#### 修改参数好吗
C的形参可以和事先被设定的值的局部变量同样使用，对值进行修改在语法上没有任何问题，但却不推荐，因为参数是从调用方得到的非常重要的信息，如果修改了参数，就再也恢复不了，不利于调试。

## 关于数组
数组的内存布局大概如下：

<!--language: plain-->

               ┌──────────┐
               │          │  array[2]
    0xbfd24390 ├──────────┤
               │          │  array[1]
    0xbfd2438c ├──────────┤
               │          │  array[0]
    0xbfd24388 └──────────┘

### 数组和指针
给指针加N，指针前进“当前指针指向的变量类型的长度 X N”，因此，给指向数组的某个元素的指针加N后，指针会指向N个之后的元素

<!--language: !c-->

    #include <stdio.h>
    int main(int argc, const char* argv[]){
        int array[] = {1,2,3,4,5};
        int *p;
        for(p=&array[0]; p!=&array[5]; p++){
            printf("%d\n", *p);
        }

        return 0;
    }

通过`p++`顺序地移动指针，引导指针指向`&array[5]`(尽管它不存在)，使用`++`运算符给指针加1，指针前进`sizeof(int)`个字节

但无论写成`p++`或`*(p+i)`都不容易阅读，还是`a[i]`的方式更容易理解，也是本书推荐的方式，实际上`a[i]`正是`*(p+i)`的 __语法糖__

### 下标运算符`[]`和数组没有关系
- __表达式中__，数组可以解读成"指向它的初始元素的指针"（有三个小例外，但这和后面要不要加`[]`没有关系）
- `p[i]`是`*(p+i)`的简便写法，下标运算符`[]`只有这种用法，它和数组无关
- `p[i]`可以写成`i[p]`
- 声明中的`[]`还是表达数组的意思，但表达式中完全不同，就像`*`在声明中（指针变量）与表达式中（解引用）完全不同一样。

#### 序列化
在网络通讯中，数据交换程序中经常使用一个char数组保存各种数据类型的数据，称为序列化，在一个巨大的char数组中，参杂了各种类型的数据，当我们试图读取多少字节的数据时，使用指针运算写法，会比下标运算符更容易理解

### 数组作为函数的参数传递
<!--language: !c-->

    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>

    int get_word(char *buf, int buf_size, FILE *fp){
        int len;
        char ch;

        while((ch = getc(fp)) != EOF && !isalnum(ch))
            ;

        if(ch==EOF) return EOF;

        len = 0;
        do{
            buf[len] = ch;
            len++;
            if(len >= buf_size){
                fprintf(stderr, "word too long.\n");
                exit(1);
            }
        } while((ch = getc(fp)) != EOF && isalnum(ch));

        buf[len] = '\0';
        return len;
    }

    int main(int argc, const char* argv[]){
        char buf[256];
        while(get_word(buf, 256, stdin) != EOF){
            printf("<<%s>>\n", buf);
        }

        return 0;
    }

- 在`main()`中，`buf`作为函数的参数传递，因为这里是在表达式中，所以`buf`可以解读成"指向数组初始元素的指针"
- 在`get_word()`中，可以像`buf[len]`这样操作`buf`的内容，那是因为它只是`*(buf+len)`的语法糖
- 一旦使用`buf[len]`还真让人感觉从`main()`传递过来的是`buf`这样的数组，显然这是一个 __错觉__，传递过来的只是指向`buf`的初始元素的指针（C是只能使用标量的语言）
    - 在`main()`中如果`sizeof(buf)`将得到265，而在`get_word()`中执行同样的语句，将得到指针类型的长度。
- 在C中，函数参数传递都是传值，向函数传递的都是参数副本，向`get_word()`传递的是指向`buf`初始元素的 __指针的副本__，但在`main()`和`get_word()`引用的却是`buf`本身，而不是`buf`副本，正如此，`get_word()`才能正确向`buf`填充字符串内容

#### 如何对数组进行值传递
在迫不得已的情形下，执意要将数组的副本作为参数进行传递，可以使用替代方法，将数组的所有元素整理成结构体的成员

### 声明函数形参的方法
针对上例，有人会将`get_word()`声明成如下

```c
int get_word(char buf[], int buf_size, FILE *fp);
```

__只有声明函数形参时，数组的声明才可以被解读成指针__

即使像

```c
int func(int a[10]);
```

这样定义了元素的个数，编译器也是 __无视__ 的，还是解读成

```c
int func(int *a);
```

C是怎么使用内存的
=================

## 关于`scanf()`
在一般C语言书中，经常使用`scanf("%d", &hoge)`，`scanf()`不是以行单位对输入内容进行解释，而是对连续字符流进行解释（换行符也视为一个字符）。

`scanf()`连续的从流读入字符，并且对和格式说明符"%d"相匹配的部分进行变换处理，如输入"123\n"时，从流中取得123部分的内容，并对它进行处理，换行符依旧会残留在流中，后续如果存在`getchar()`就会吞食这个留下的换行符。

此外，当`scanf()`变换失败的时候，（比如尽管指定了%d，但输入的却是英文字符）将导致失败的部分遗留在流中。

在读入过程中有几个对象被成功地变换，则`scanf()`的返回值就为几。

为了避免后续字符的遗留影响后面的输入时，可使用如下方式：

<!--language: c-->

    char buf[256];
    int hoge;
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &hoge);

## C的变量的种类
C语言的变量具有区间性的作用域，用语句包围的是作用域，`static`控制静态连接，`extern`控制外部连接。对于全局变量，作用域指文件作用域，链接指外部链接。对于程序员来说，这些方式都是控制命名空间的，没什么不同，统一使用“作用域”叫法


### 作用域
#### 全局变量
在函数之外声明的变量，默认的会成为全局变量。它在任何地方都是可见的。当程序被分割为多个源文件进行编译时，声明为全局变量的变量也是可以从其他源代码文件中引用的。

#### 文件内部的静态变量
就算对于像全局变量那样被定义在函数外部的变量，一旦添加了`static`，作用域只限定在当前所在的源代码文件中。通过`static`指定的变量（包括函数），对于其他源代码文件是 __不可见的__。

#### 局部变量
指在函数中声明的变量，局部变量只能在包含它的声明的语句块中被引用。局部变量通常在它所在语句块结束的时候被释放，如果 __不想释放__，可加上`static`进行声明。

### 存储期
#### 静态存储期
全局变量、文件内的`static`变量、指定`static`的局部变量都持有静态存储期，这些变量被统称为 __静态变量__，变量的寿命从程序运行时开始，到程序关闭时结束，一直存在内存的同一个地址上。

#### 自动存储期
没有指定`static`的局部变量，持有自动存储期，被称为 __自动变量__。在程序运行进入它所在语句块时（进入函数时）被分配内存区域，该语句块执行结束后这片内存区域被释放。

这个特征通常使用 __栈__ 的机制来实现。

#### 动态分配回收
通过`malloc`分配的领域，寿命一直延续到使用`free()`释放它为止，需要 __手工__ 处理。

### 变量在内存中区域
- 指向函数的指针、字符串常量 被配置在非常近的内存区域（只读）
- 函数内static变量、文件内static变量、全局变量等这些静态变量 也被配置在非常近的内存区域（只读）
- 接下来就是malloc()分配的内存区域
- 自动变量的区域离得很远，而且随函数调用（栈），会使用相同的地址

## 函数和字符串常量
### 只读内存区域
大多数操作系统都是将 __函数自身__ 和 __字符串常量__ 汇总配置在一个只读内存区域

### 指向函数的指针
将指向函数的指针保存在变量中的技术经常被运用在如下场合：

- GUI中的按钮控件记忆“当自身被按下的时候需要调用的函数”
- 根据“指向函数的指针的数组”（如下面的声明）对处理进行 __分配__

```c
int (*func_table[])(double);
```

## 静态变量
静态变量总是在虚拟地址空间上占用固定的区域。

### 分割编译和连接
关于函数和全局变量，如果它们的名称相同，即使它们跨了多个源代码文件也被作为相同的对象来对待，进行这项工作的是一个被称为“链接器”的程序

为了在链接器中将名称结合起来，各目标代码大多都具备一个 __符号表__，在unix中，可使用`nm`命令窥视符号表中内容。

## 自动变量(栈)
### 函数调用究竟发生了什么
<!--language: !c-->

    #include <stdio.h>

    void func(int arg1, int arg2){
        int var3, var4;
        printf("func:&arg1..%p &arg2..%p\n", &arg1, &arg2);
        printf("func:&var3..%p &var4..%p\n", &var3, &var4);
    }

    int main(void){
        int var1, var2;
        printf("main:&var1..%p &var2..%p\n", &var1, &var2);
        func(1, 2);
        return 0;
    }

C语言中，在现有被分配的内存区域之上以“堆积”的方式（向 __低地址位方向__ 堆积，栈不断的增长），为新的函数调用分配内存区域。在函数返回的时候，会释放这部分内存区域供下一次函数调用使用。

<!--language: plain-->

    high       ┌──────────┐
               │          │  main的局部变量 var1
    0xbfd243e4 ├──────────┤
               │          │  main的局部变量 var2
    0xbfd243e0 ├──────────┤
               │    2     │  func的形参 arg2
    0xbfd243dc ├──────────┤
               │    1     │  func的形参 arg1
    0xbfd243d8 ├──────────┤
               │          │  返回信息
    0xbfd243d4 ├──────────┤
               │          │  返回信息
    0xbfd243d0 ├──────────┤
               │          │  func的局部变量 var3
    0xbfd243cc ├──────────┤
               │          │  func的局部变量 var4
    0xbfd243c8 ├──────────┤
               │    │     │
    low             V

- 在调用方，参数 __从后往前__ 按顺序被堆积在栈中（为了支持可变长参数）
- 和函数调用关联的 __返回信息__（返回地址等）也被堆积在栈中
- 一旦函数调用结束，局部变量占用的内存区域就被释放，并且使用返回信息返回到原来地址
- __调用方__ 负责从栈中除去调用的参数
- 在函数调用时，需要为 __形参分配__ 新的内存区域，“C的参数都是传值，向函数内部传递的是实参的副本”，__其复制动作，就在这里发生__


### 常见BUG
#### 自动变量的内存区域释放
<!--language: c-->

    char *int_to_str(int int_val){
        char buf[20];
        sprintf(buf, "%d", int_value);
        return buf;
    }

像上面的代码是不能正常运行的，`buf`的内存区域在函数调用结束后就会被释放。

#### 破坏自动变量的内存区域
如果没有做数组长度检查，将数据写入了超过数组内存区域的地方，可能会破坏相邻的自动变量的内容。如果一直破坏下去，有可能将存储函数的 __返回信息给破坏掉__，导致函数不能返回。

示例可参考[为什么程序会崩溃](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgitcafe.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E7%25BC%2596%25E7%25A8%258B%25E8%258C%2583%25E5%25BC%258F%2528stanford_cs107%2529.md#TOC13.3)

### 可变长参数
C语言的参数是 __从后往前__ 被堆积在栈中的，应该由 __调用方__ 将参数从栈中除去

> Pascal与Java中，是从 __前往后将__ 参数堆积在栈中，并且将参数从栈中除去的是 __被调用方__ 应该承担的工作

故意采取与Pascal相反的方式，主要为了实现可变长参数这功能

```c
printf("%d, %s\n", 100, str);
```

对应的栈状态如下：

<!--language: plain-->

                            ┌──────────┐
                            │          │  使用中的栈
                       ┌─   ├──────────┤
                       │    │    *     │  str
                       │    ├──────────┤
                       │    │   100    │
                       │    ├──────────┤
                       │    │    *     │  指向"%d, %s\n"
    printf() reference area ├──────────┤
                       │    │          │  返回信息
                       │    ├──────────┤
                       │    │          │  返回信息
                       │    ├──────────┤
                       │    │          │  printf()自己的局部变量
                       └─   ├──────────┤
                            │    │     │
                                 V

无论堆积多少参数，总能找到第一个参数地址（它一定存在于距离固定的场所），随着对第一个参数的占位符的解析，就可以知道后面还有几个参数

#### 使用stdarg.h
<!--language: !c-->

    #include <stdio.h>
    #include <stdarg.h>
    #include <assert.h>

    void tiny_printf(char *format, ...) {
        int i;
        va_list ap;

        va_start(ap, format);
        for (i = 0; format[i] != '\0'; i++) {
            switch (format[i]) {
                case 's':
                    printf("%s ", va_arg(ap, char*));
                    break;
                case 'd':
                    printf("%d ", va_arg(ap, int));
                    break;
                default:
                    assert(0);
            }
        }
        va_end(ap);
    }

    int main(void){
        tiny_printf("sdd", "result..", 3, 5);
        return 0;
    }

- 头文件stdarg.h提供了一组方便使用可变长参数的宏
- `va_list`一般是这样定义`typedef char *va_list;`
- `va_start(ap, format)`意味着 使指针`ap`指向参数`format`的下一个位置
- 宏`va_arg()`指定`ap`和参数类型，就可以顺序的取出可变长部分的参数
- `va_end(ap);`是一个空定义的宏，只因标准里指出了对于`va_start()`的函数需要写`va_end()`

> 在决定开发可变长参数的函数前，思考一下是否真有必要

#### 实现DEBUG_WRITE
<!--language: !c-->

    #include <stdio.h>
    #include <stdarg.h>

    #define DEBUG

    #ifdef DEBUG
    #define DEBUG_WRITE(arg) debug_write arg
    #else
    #define DEBUG_WRITE(arg)
    #endif

    #define SNAP_INT(arg) fprintf(stderr, #arg "..%d\n", arg)

    void debug_write(char *format, ...){
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
    }

    int main(void){
        DEBUG_WRITE(("\n%s..%d\n", "debug_write", 10));
        int hoge = 4;
        SNAP_INT(hoge);
        return 0;
    }

- 使用`DEBUG_WRITE`应加 __两重括号__，使用gcc时
> `gcc -DDEBUG ...`将会加上`DEBUG`的宏定义
- `vfprintf(stderr, format, ap);`输出到`stderr`不会有缓冲
- `#define SNAP_INT(arg) fprintf(stderr, #arg "..%d\n", arg)`，`#arg`将输出原文，其后只有空格，将合并两个字符串

### ANSI C初始化自动变量
ANSI C以前的C只有标量才能在声明的同时被初始化，类似初始化数组等聚合类型时，需要加上`static`作为静态变量分配内存区域，就可以在程序的 __执行前__ 被完全初始化。

现在编译器复杂了，能支持 __执行中__ 初始化数组等聚合类型，但在聚合类型初始化运算符中也只能写 __常量__，像下面的写法就违反规范了。

```c
void func(double angle){
    double hoge[] = {sin(angle), cos(angle)};
}
```

## malloc()动态内存分配(堆)
`malloc()`的返回值是`void*`，该类型的指针可以不强制转型的赋给所有指针类型变量，如下的代码其实不必要了：

```c
book_data_p = (BookData*)malloc(sizeof(BookData));
```

这点和 C++不同，后者可以将任意的指针赋值给`void*`类型的变量，但`void*`类型的值赋给通常的指针变量时，需要强制转型

### malloc()中发生了什么
`malloc()`不是"系统调用"
> 而`printf()`却最终调用系统的`write()`API。

`malloc()`大体实现是，从操作系统一次性取得比较大的内存，然后再零售给应用程序。根据操作系统不同，从操作系统取得内存的手段也不一样，在unix情况下使用`brk()`的系统调用。
> 有时也会用`mmap()`系统调用

调用函数时，__栈会向地址较小的一方伸长__，多次调用`malloc()`时，会调用一次`brk()`，__内存区域（堆）会向地址较大的一方伸长__。

<!--language: plain-->

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

实现`malloc()`时，有些实现会在分配出的各块之前加上一个管理区域，如果该区域被破坏了，以后`malloc(),free()`时，程序将有可能崩溃。

#### 建议
- 考虑给`malloc()`加一层包装，在每次分配内存的时候多留一点空间，然后在 __最前面部分__ 设定区域的大小信息，填充如0xCC的值，防止未做好初始化，并增加计数；
- 而`free()`也做个包装，在区域被释放前故意 __做一些破坏__（如填充0xCC，防止再被引用），方便BUG的查找，也增加计数，最后看是否和`malloc()`的计数一致；
- 当然这些包装只在调试模式下有效，去掉调试模式，发行版的还是直接调用底层函数。
- 谨慎使用`realloc()`，如果利用它扩展巨大内存区域，复制上花费很多时间，同时也造成大量的的空间过度活跃。如果想要动态为大量元素分配空间，最好不要使用连续的内存区域，考虑使用链表。

## 内存布局对齐
<!--language: !c-->

    #include <stdio.h>
    #include <stddef.h>

    typedef struct{
        int int1;
        double double1;
        char char1;
        double double2;
    } Hoge;

    int main(void)
    {
        printf("offset of int1 in Hoge: %d\n", offsetof(Hoge, int1));
        printf("offset of double1 in Hoge: %d\n", offsetof(Hoge, double1));
        printf("offset of char1 in Hoge: %d\n", offsetof(Hoge, char1));
        printf("offset of double2 in Hoge: %d\n", offsetof(Hoge, double2));
        return 0;
    }

`sizeof()`后的结果为24，`char1`后面空出一块来，根据CPU特征，对于不同类型的可配置地址受到一定限制，编译器会适当进行边界调整（布局对齐），在结构体内插入合适的填充物，使int,double等被配置在4的倍数的地址上（有些机器将double配置在8的倍数的地址上）。

## 字节排序(大小端)
<!--language: !c-->

    #include <stdio.h>

    int main(int argc, const char *argv[]){
        int hoge = 0x12345678;
        unsigned char *hoge_p = (unsigned char*)&hoge;

        printf("%x\n", hoge_p[0]);
        printf("%x\n", hoge_p[1]);
        printf("%x\n", hoge_p[2]);
        printf("%x\n", hoge_p[3]);

        return 0;
    }

输出78 56 34 12的，这种配置方式称为小端字节序(little-endian)，Intel,AMD均属于此类

揭秘C的语法
==========
## 解读C的声明

### 用英语来阅读
试解释下面的声明：

```c
char *color_name[] = {"red", "green", "color"};
int *func(double);
int (*func_p)(double);
```

目前不考虑`const`和`volatile`，介绍一下阅读C声明的方法：

- 首先着眼于 __标识符__（变量名或函数名）
- 从距离标识符最近的地方开始，依照优先顺序解释派生类型（指针、数组和函数）。优先顺序说明如下：
    1. 用于 __整理声明内容的括号__
    2. 用于表示 __数组的`[]`__ ，用于表示 __函数的`()`__
    3. 用于表示 __指针的`*`__
- 解释完派生类型，使用"of, to, returning"将它们 __连接__ 起来
- 最后，追加 __数据类型修饰符__（在左边，int,double等，如果函数，即为返回类型）
- 英语不好的，可以倒序用中文解释

```c
int (*func_p)(double);
```

1. 首先着眼于 __*标识符*__
> __*func_p*__ is
2. 因为 __*存在括号*__，所以 __*着眼于`*`*__
> func_p is __*pointer to*__
3. 解释用于 __*函数的`()`*__，参数是double
> func_p is pointer to __*function(double) returning*__
4. 最后 __*解释数据修饰符int*__
> func_p is pointer to function(double) returning __*int*__
5. 翻译成中文（__*倒序*__）
> func_p是指向返回int的函数(参数是double)指针

```table
C语言          |英语表达                                     |中文表达
---------------|--------------------------------------------|-------
int hoge[10][3]|hoge is array(num 10) of array(num 3) of int|hoge是int数组(元素3)的数组(元素10)（反序）
int *hoge[10]  |hoge is array(num 10) of pointer to int     |hoge是 指向int指针的 数组(元素10)
int (*hoge)[3] |hoge is pointer to array(num 3) of int      |hoge是 指向int数组(元素3)的 指针
int *func(double) |func is function(double) return int*     |func是一个返回int*的函数(参数是double)
```

### 类型名
在C中，除标识符外，有时候还必须定义“类型”：

- 在强制转型运算符中
- 类型作为`sizeof`运算符的操作数

从标识符的声明中，将标识符取出后，剩下部分自然就是类型名：

```table
声明            |声明的解释                  |类型名        |类型名的解释
---------------|----------------------------|-------------|-----------------------------
int hoge       |hoge是int                   |int          |int类型
int *hoge;     |hoge是指向int的指针          |int *        |指向int的指针类型
double *p[3]   |p是指向double指针的数组(元素3)|double *[3]  |指向double指针的数组(元素3)类型
double (*p)[3] |p是指向double数组(元素3)的指针|double(*)[3] |指向double数组(元素3)的指针类型
void (*func)() |func是指向返回void函数的指针  |void (*)()   |指向返回void函数的指针类型
```

内存布局示例:

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

## C的数据类型的模型
### 基本类型和派生类型
```c
int (*func_table[10])(int a);
```

解释成：func_table is array(num 10) of pointer to function(int) returning int，指向返回int的函数(参数为int)的指针的数组(元素为10)

<!--language: plain-->

       ┌───────┐  return  ┌───────┐  point to  ┌───────┐ element is ┌───────┐
       │  int  │<-------- │ func  │<---------- │pointer│<---------- │ array │
       └───────┘          ├───────┤            └───────┘            ├───────┤
                          │arg int│                                 │num 10 │
                          └───────┘                                 └───────┘


这种表示称为"类型链的表示"，先忽略结构体、共用体、typedef等类型，链的最后元素是基本类型，这里可能是`int`,`double`等。

从基本类型开始，第二个元素以后都是 __派生类型__（即从某些类型派生出来的），除了结构体、共用体，还有以下三种派生类型：

- 指针
- 数组（__元素个数作为它的属性__）
- 函数（__参数信息作为它的属性__）

从基本类型开始，递归地（重复的）粘附上派生类型，就可以生成无限类型。在链中，最后的类型（上例中数组(元素10)）整合了全体类型的意义，称为 __类型分类__
> 无论"指向int的指针"，还是"指向double的指针"，结果都是"指针"，无论是"int数组"，还是"指向char指针的数组"，结果都是"数组"

### 指针类型派生
由引用类型T派生的指针类型，称为指向T的指针

<!--language: plain-->

                        全体类型为 被引用类型T
       ┌────────────────────────┴────────────────────────┐
       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ point to ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          └───────┘

### 数组类型派生
和指针类型相同，数组类型也是从其元素的类型派生出来的，__元素个数作为类型的属性__ 添加在类型后面

<!--language: plain-->

                        全体类型为 元素类型
       ┌────────────────────────┴────────────────────────┐
       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ inclued  ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          ├───────┤
                                                                    │ num   │
                                                                    └───────┘

### 指向数组的指针
派生出数组之后，再派生出指针，即为指向数组的指针
> 表达式中，数组可以解读成指针，但这 __不是__ “指向数组的指针”，而是“指向数据 __初始元素__ 的指针”

```c
int array[3];
int (*array_p)[3]; /* 这才是指向int数组(元素3)的指针 */
/* array_p = array; 编译器将报警告*/
array_p = &array;
```

指向int的指针 和 指向int数组(元素为3)的指针 是完全不同的数据类型。从地址角度来看，`array`和`&array`也许指向同一地址，但它们在做指针运算时结果不同。

#### 数组的数组
C中不存在真正的多维数组，只存在数组的数组。

`int hoge[3][2]`可以通过`hoge[i][j]`的方式去访问，`hoge[i]`指“int的数组(元素2)的数组(元素3)”中的第i个元素，其 __类型为`int[2]`__

#### 参数传递
试图将数组的数组进行参数传递时，考虑“将int的数组作为参数传递时”，对应函数原型为

```c
void func(int *hoge);
```

在“int的数组(元素2)的数组(元素3)”的情况下，将后面的数组解释成指针时，表达为“指向int数组(元素2)的指针”，原型则为

```c
void func(int (*hoge)[2]);
```

> 另写成下面的方式，也都被解释成上面的

```c
void func(int hoge[3][2]);
void func(int hoge[][2]);
```

### 函数类型派生
函数类型也是一种派生类型，参数(类型)是它的属性

函数类型的其他派生类型有不太相同的一面，无论int还是double，或数组、指针、结构体，大体都可以作为变量被定义，这些变量在内存占有一定的空间，通过`sizeof`运算符可以取得它们的大小，像这样有特定长度的类型，称为 __对象类型__

可是函数类型不是对象类型，因为函数没有特定长度，C中不存在“函数类型的变量”：

- 函数类型是无法得到特定长度的，所以从函数类型派生出的数组类型是不可能的，即不可能出现 "函数的数组" 这样的类型
> 但可以存在"指向函数的指针的数组"，先派生出指针类型，再派生出数组
- 可以有“指向函数的指针”类型，但不能做运算的，因为无法得到当前指针类型的大小
- 函数类型也不能成为结构体和共用体的成员
> 但可以将“指向函数的指针”类型作为结构体成员
- 即函数类型是 __不能派生出除了指针类型之外的其他任何类型__ 的。
- 此外，数组类型是不能派生出函数类型的，在C中，数组是不能作为函数返回值返回的。

### 计算类型的大小
除了函数类型和不完全类型，其他类型都有大小，通过`sizeof(类型名)`编译器计算当前类型的大小

<!--language: !c-->

    #include <stdio.h>

    int main(int argc, const char *argv[]){
        printf("%d\n", sizeof(int(*[5])(double)));
        printf("%d\n", sizeof(main));
        return 0;
    }

- 基本类型，必定依赖处理环境进行计算
- 指针，也依赖处理环境，但它的大小 __和派生源的大小没有关系__，它的 __大小是固定__ 的。
- 数组，通过派生源类型的大小乘以元素个数得到
- 函数，大小无法计算

### 结构体和共用体
- 虽然结构体和共用体在语法上属于派生类型，但是在声明中它和数据类型修饰符（int，double）等处于相同的位置
- 只有派生指针、数组和函数的时候，类型才可以通过一维链表表示，结构体、共用体派生类型只能用树结构进行表现

### 不完全类型
不完全类型，指函数之外，类型的大小不能被确定的类型，__结构体标记的声明__ 就是一个不完全类型的例子

```c
typedef struct Woman_tag Woman;
typedef struct{
    Woman *wife;
} Man;
struct Woman_tag{
    Man *hashand;
};
```
`Woman`类型的标记被声明的时候，还不知道其内容，所以无法确定其大小。因为不能确定大小，所以不能将不完全类型变成数组，也不能作为结构体成员，只能用于取得指针，将`Woman`类型的指针作为结构体成员。
> 之后在定义`struct Woman_tag`的内容的时候，`Woman`就不是不完全类型了；
> 对于不完全类型，__只能取得其指针__
> void类型也被归类为不完全类型

## 表达式
`sizeof()`也可以用在表达式上，但唯一的用途就是从编译器获取数组的长度

### 左值是什么
作为变量，它用作为"自身的值"使用 和 作为"自身的内存区域"使用的两种情况。即使不是变量名，表达式也可以代表“某个变量的内存区域”

- 表达式代表 __某处的内存区域__ 的时候，称当前表达式为 __左值__
- 相对的，表达式只是代表 __值__ 的时候，称当前表达式为 __右值__

表达式根据上下文，可作为左值或右值使用，但像5这样的 __常量__ ，或`1+hoge`这样的表达式只能解释成右值

### 将数组解读成指针
`int hoge[10];`的声明，`hoge`等同于`&hoge[0]`，`hoge`原类型为`int的数组(元素10)`，但并不妨碍将其类型分类从"数组"变换为"指针"

<!--language: plain-->

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ inclued  ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          ├───────┤
                                                                    │ num   │
                                                                    └───────┘

                                      | 表达式中进行类型变换
                                      V

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ point to ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          └───────┘

当数组被解读成指针时，该指针不能作为左值（即不能再被赋值）[参考](#TOC3.7.2)

### 数组和指针的相关运算符
#### 解引用
返回指针所指向的对象或者函数，只要不是返回函数，运算符`*`的 __结果都是左值__。从运算符`*`的操作数的类型中仅仅 __去掉一个指针__ 后的类型，就是它返回的表达式的类型

<!--language: plain-->

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ point to ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          └───────┘

                   　　　　 | 变成从*运算符的操作数的类型中仅仅去掉一个指针后的类型
                   　　　　 V

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐
       │       │<----│       │<----│       │<----│       │
       └───────┘     └───────┘     └───────┘     └───────┘


#### 地址运算符
`&`将 __左值作为操作数__（不能将非左值作为操作数），返回指向该左值的指针，对左值的类型加上一个指针，就是`&`运算符的返回类型

<!--language: plain-->

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐
       │       │<----│       │<----│       │<----│       │
       └───────┘     └───────┘     └───────┘     └───────┘

                   　　　　 | 变成对左值的类型加上一个指针后的类型
                   　　　　 V

       ┌───────┐     ┌───────┐     ┌───────┐     ┌───────┐ point to ┌───────┐
       │       │<----│       │<----│       │<----│       │<---------│pointer│
       └───────┘     └───────┘     └───────┘     └───────┘          └───────┘

#### 下标运算符
`[]`将指针和整数作为操作数，归根结底`p[i]`就是`*(p+i)`，所以下标运算符返回的类型是，从`p`的类型去掉一个指针的类型

#### `->`运算符
通过指针访问结构体的成员的时候，会使用`->`，`p->hoge;`是`(*p).hoge;`的语法糖

### 数组的数组
`int hoge[3][5]`这样的数组的数组，使用`hoge[i][j]`这样的方式进行访问，假定`i=2,j=3`时的内存状态

<!--language: plain-->


    1 hoge is here -->┌───────┐  ─┐
                      ├─     ─┤   │
                      ├─     ─┤   ├─ int的数组(元素5)
                      ├─     ─┤   │
                      ├─     ─┤   │
                      ├───────┤  ─┘
                      ├─     ─┤
                      ├─     ─┤
                      ├─     ─┤
                      ├─     ─┤
          2.1  2.3 -->├───────┤  ─┐
                      ├─     ─┤   │
                      ├─     ─┤   ├─ 2.2 通过*(hoge+i)中的*,去掉一个指针
                 3 -->├─     ─┤   │      其类型为这样的数组
                      ├─     ─┤   │
                      └───────┘  ─┘

    2.1 hoge是int的数组(元素5)的指针,根据加法,前移至此
    2.3 可是,因为表达式中,所以被理解为指针(指向int的指针)
    3 对int的指针进行加法运算,取出实体

- `hoge`的类型为"int的数组(元素5)的数组(元素3)"
- 尽管如此，在 __表达式中数组可以被解读成指针__，因此，`hoge`的类型为"__指向int的数组(元素5)的指针__"
- `hoge[i]`是`*(hoge+i)`的语法糖
    1. 给指针加上i，意味着指地前移它指向的类型 X i的距离，即前移了`sizeof(int[5]*i)`
    2. 通过`*(hoge+i)`中的`*`，去掉一个指针，`*(hoge+i)`的类型就是"__指向int的数组(元素5)__"
    3. 尽管如此，由于在表达式中，数组可以解读成指针，所以`*(hoge+i)`的最终类型为"__指向int的指针__"
- `(*(hoge + i))[j]`和`*((*(hoge + i)) + j)`其实是相等的，就是"对指向int的指针加上j后得到的地址上的内容"，其类型为int

## 解读C的声明(续)
### const修饰符
`const`在ANSI C中追加的修饰符，它将类型修饰为　__“只读”（不一定代表常量）__。它 __主要修饰函数的参数__。

```c
char *my_strcpy(char *dest, const char *src){
    src = NULL; /* 即使对src赋值，编译器也没报错 */
    *src = 'a'  /* ERROR! */
}
```

此时成为只读的不是`src`，而是`src`所指向的对象。如果将`src`自身定义为只读，需要修改成如下：

```c
char *my_strcpy(char *dest, char * const src){
    src = NULL; /* ERROR! */
}
```

在现实中，当指针作为参数时，__`const`常用于将指针指向的对象设定为只读__，即第一种表达方式更常见

- 通常C的参数都是传值，因此无论被调用方（函数内部）对参数进行怎样的修改，都不会对调用方（函数外部）造成影响。如果想要影响调用方的变量，可以将指针作为参数传递给函数。
- 可是有时传指针，只是为了 __传递数组__ 的值（C中数组是不能作为参数传递的，不得不将指向初始元素的指针传递过去），本身并不想被修改掉，此时考虑在原型声明中加入`const`
> 尽管函数接受了作为参数的指针，但是指针指向的对象不会被修改，函数虽然接受了指针，但并不意味着要向调用方返回值。

#### 解读const声明
- 从 __标识符__ 开始，使用英语由内向外顺序的解释下去
- 一旦解释完毕的部分的 __左侧出现了`const`__，就在当前位置追加read-only
- 如果解释完毕的部分的 __左侧出现了数据类型修饰符__，并且其左侧存在`const`，姑且先去掉数据类型修饰符，追加read-only
- 在翻译成中文时，`const`修饰是紧跟在它后面的单词

```c
char * const src
```
> 解释成 src is __read-only__ pointer to char

```c
char const *src
```
> 解释成 src is __pointer__ to read-only char

```c
const char *src
```
> 解释成 src is __pointer__ to read-only char


### C语言声明的优先级规则
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




### 如何使用const
```c
void search_point(char const *name, double *x, double *y);
```

- 代表`name`是in参数，而`x`和`y`为out参数
- `char const *name`是不能将它赋值给`char*`类型的变量的（除非强制转型），因为后者是可以修改`name`所指向的对象的内容。
- 同理，`char const *`类型的指针传递给使用`char*`作为参数的函数，也是不允许的
- 一旦给指针类型的参数设定了`const`，当前层次以下的函数必须全部使用`const`，在一些通用函数中，如果本应是`const`的参数却没有加上`const`，会经常导致调用方无法使用`const`

### const能否代替`#define`
```c
const int HOGE_SIZE = 100;
int hoge[HOGE_SIZE];
```

是不行的，C中数组的元素必须为常量，`const`修饰的标识符不是常量，只是只读而已（C++另当别论）

### typedef
```c
typedef char *String;
```
- 用于给某类型定义别名，通过该声明，以后对于"指向char的指针"，可以使用"String"这个别名
- 可以 __按照普通的变量声明的顺序__ 来解释`typedef`，上面声明解读为`String is pointer to char`
- 之后在使用`String`时，可以写成这样`String hoge[10];`，解读为`hoge is array(num 10) of String`=>`hoge is array(num 10) of pointer to char`（指向`char`指针的数组）

```c
typedef struct{
} Hoge;  /*类型声明*/

struct Hoge_tag{
} hoge;  /*变量声明*/
```

- `typedef`也可一次性声明多个变量

```c
typedef struct{
} Hoge, *HogeP;
```

等同于：

```c
typedef struct{
} Hoge;
typedef Hoge *HogeP;
```

## 其他语法
### 函数的形参声明

```c
void func(int a[]);
```
对于这种写法，无论怎么看都好像要向函数的参数传递数组，可C中不能够将数组作为函数的参数进行传递，在 __声明形参的时候，作为类型分类的数组，可以被解读成指针__，也只有在这种情况下，`int a[]`和`int *a`才具有相同的意义。

```c
void func(int a[][5]);
```
`a`的类型为"int的数组(元素5)的数组(元素个数不明)"，是可以被解读成

```c
void func(int (*a)[5]);
```
"指向int的数组(元素5)的指针"

### 关于空的下标运算符[]
以下情况下标运算符`[]`可以将元素个数省略不写：

- 函数的形参的声明
- 根据初始化表达式可以确定数组大小的情况

```c
int a[] = {1, 2, 3};
char str[] = "abc";
double matric[][2] = {{1, 0}, {0, 1}};
char *color_name[] = {
    "red",
    "green",
    "blue"
};
char color_name2[][6] = {
    "red",
    "green",
    "blue"
};
int a[][3] = {
    {1, 2, 3},
    {4, 5},
    {6}
};
char str[][5] = {
    "hoge",
    "hog",
    "ho",
}
```

- 使用`extern`声明 __全局变量__ 的情况
> 全局变量在多个编译单元中的某一个中定义，然后从其他代码文件中通过`extern`进行声明，在定义的时候还是需要元素个数的，但是在使用`extern`时行声明时，在连接的时候编译器可以确定实际数组大小，所以 __可以省略 *最外层* 数组的元素个数__
>
> 但是不能将数组和指针混在一起，如file_1.c中定义了`int a[100];`，file_2.c中声明`extern int *a;`则是错误的

### 字符串常量
字符串常量类型是"char的数组"，因此在表达式中，可以解读为指针

```c
char *str;
str = "abc";
```

`char`数组的初始化是个例外，此时的字符串常量，作为花括号中分开书写的初始化表达式的省略形式，编译器会进行特殊处理，下面两句具有相同含义：

```c
char str[] = "abc";
char str[] = {'a', 'b', 'c', '\0'};
```

但下面的写法是 __非法__ 的，因为`str`并 __非标量__，虽然可以初始化，但不能直接赋值：

```c
char str[4];
str = "abc";
```

下面的例子 __不是__ 初始化`char`的数组，而是初始化指针，也是合法的，此时"abc"就是普通的`char`数组（字符串常量），然后被赋值给`str`：

```c
char *str = "abc";  /* 初始化指针 */
```

通常，字符串常量保存在 __只读的__ 内存区域（依赖具体环境），但如果在初始化`char`的数组，并不加`const`时，常量是可写的：

```c
char str[] = "abc"; /* 存在栈中, 真正的初始化char数组 */
str[0] = 'd'; 　　　/* 可写 */
```

但下面的，就会报错

```c
char *str = "abc"; /* 存在只读区域中 */
str[0] = 'd'; 　　 /* ERROR! */
```

可通过`strdup()` 动态分配 __堆__ 的内存，并将地址返回给指针，该内存是需要手工`free()`的

```c
char *str = strdup("abc"); /* 存在堆中 */
str[0] = 'd'; 　　 /* 可写 */
free(str);
```

### 指向函数的指针引起的混乱
基于C语言的声明规则：

- `int func();`这样的声明，如果一定要使用指向函数的指针，必须写成`&func`，现实中只需要传递`func`；
- 相反，像`void (*func_p)();`变量`func_p`声明为指向函数的指针，进行函数调用时，必须写成`(*func_p)();`，现实中只需要执行`func_p();`

产生混乱的原因就是，表达式中的函数可以被解读成“指向函数的指针”，为了照顾到这种混乱，ANSI标准对语法做了规定：

- 表达式中的函数自动转换成“指向函数的指针”，但当函数是地址运算符`&`或者`sizeof`运算符的操作数时，表达式的函数不能变换
- 函数调用运算符`()`的操作数不是"函数"，而是“函数的指针”

## 复杂的函数声明
### 示例1
```c
int atexit(void (*func)(void));
```

1. 首先着眼于 __*标识符*__
> __*atexit*__ is
2. 解释用于 __*函数的`()`*__
> atexit is __*function() returning*__
3. 函数的 __*参数部分*__，同样先解释 __*标识符*__
> atexit is function( __*func*__ is) returning
4. 因为有括号，这里 __*解释`*`*__
> atexit is function( func is __*pointer to*__) returning
5. 解释用于 __*函数的`()`*__，这里参数还是比较简单，是`void`
> atexit is function( func is pointer to __*function(void) returning*__) returning
6. 解释 __*类型指定符`void`*__（函数返回类型），这样就结束了`atexit`的参数部分的解释
> atexit is function( func is pointer to function(void) returning __*void*__) returning
7. 解释 __*数据类型修饰符`int`*__（函数返回类型）
> atexit is function( func is pointer to function(void) returning void) returning __*int*__
8. 翻译成中文
> atexit是返回int的函数(参数是，指向返回void没有参数的函数的指针)

### 示例2
```c
void (*signal(int sig, void (*func)(int)))(int);
```

1. 首先着眼于 __*标识符*__
> __*signal*__ is
2. 相比`*`，__*`()`*__的优先级更高，所以先解释这部分
> signal is __*function() returning*__
3. 解释参数部分，这里有两个参数，__*第一个参数是`int sig`*__
> signal is function(__*sig is int*__, ) returning
4. __*着眼另一个参数*__，先解释 __*标识符`func`*__
> signal is function(sig is int, __*func*__ is) returning
5. 因为有括号，这里 __*解释`*`*__
> signal is function(sig is int, func is __*pointer to*__) returning
6. 解释用于 __*函数的`()`*__，参数为`int`
> signal is function(sig is int, func is pointer to __*function(int) returning*__) returning
7. 解释 __*类型指定符`void`*__（函数返回类型）
> signal is function(sig is int, func is pointer to function(int) returning __*void*__) returning
8. 参数部分已经解释完。接着因为 __*有括号*__，所以这里 __*解释`*`*__
> signal is function(sig is int, func is pointer to function(int) returning void) returning __*pointer to*__
9. 解释表示 __*函数的`()`*__，参数为`int`
> signal is function(sig is int, func is pointer to function(int) returning void) returning pointer to __*function(int) returning*__
10. 解释 __*类型指定符`void`*__（函数返回类型）
> signal is function(sig is int, func is pointer to function(int) returning void) returning pointer to function(int) returning __*void*__
11. 翻译成中文
> signal是返回 "指向返回void参数为int的函数指针" 的函数，它有两个参数，一个是int，另一个是 "指向返回void参数为int的指针的函数的指针"


> 一个有用的程序[`cdecl`](http://www.cdecl.org/)，用于解释相关声明，或`apt-get install cdecl`

### 简化声明
此时用`typedef`可以让声明变得格外简洁：

```c
typedef void(*sig_t)(int);
sig_t signal(int sig, sig_t func);
```

## 数组和指针是不同的事物
数组是一些对象排列后形成的，而指针则表示指向某处，它们是完全不同的。

### 经常错误的代码
- 自动变量的指针在初期状态，值是不确定的

```c
int *p;
p[3] = 'a'; /*使用没有指向内存区域的指针*/
```

- 数组既不是标量，也不是结构体，不能临时使用

```c
char str[10];
str = "abc"; /*突然向数组赋值*/
```

### 表达式之中
在表达式中，可以写成这样

```c
int *p;
int array[10];
p = array;
```

但是反过来则不可以的：

```c
array = p;
```

表达式中`array`可以被解读成指针，本质上其实是被解释成了`&array[0]`，此时的指针是一个 __右值__（在标准中，数组也不是“可变更的左值”）（也有人称之为常量），右值没有对应的内存区域（类似`a + 1 = 10`中`a + 1`是右值，没有内存区域），不能被赋值

### 指针的数组 vs 数组的数组
指针的数组

```c
char *color_name[] = {
    "red",
    "green",
    "blue"
};
```

的内存布局如下：

<!-- language: plain -->

    ┌───────┬───────┬───────┐
    │   *   │   *   │   *   │
    └───┼───┴───┼───┴───┼───┘
        │       │       │  ┌─┬─┬─┬──┐
        │       │       │  │r│e│d│\0│  只读区域
        └───────┼───────┼─>└─┴─┴─┴──┘
                │       │  ┌─┬─┬─┬─┬─┬──┐
                │       │  │g│r│e│e│n│\0│
                └───────┼─>└─┴─┴─┴─┴─┴──┘
                        │  ┌─┬─┬─┬─┬──┐
                        │  │b│l│u│e│\0│
                        └─>└─┴─┴─┴─┴──┘

数组的数组

```c
char color_name[][6] = {
    "red",
    "green",
    "blue"
};
```

的内存布局如下：

<!-- language: plain -->

    ┌─┬─┬─┬──┬──┬──┬─┬─┬─┬─┬─┬──┬─┬─┬─┬─┬──┬──┐
    │r│e│d│\0│\0│\0│g│r│e│e│n│\0│b│l│u│e│\0│\0│
    └─┴─┴─┴──┴──┴──┴─┴─┴─┴─┴─┴──┴─┴─┴─┴─┴──┴──┘

以上两种情况都可以用`color_name[i][j]`的方式对数组进行访问，但 __内存中的数据布局完全不同__ 的：

- 第一种情况，由于指针所指向的字符串常量存在于 __只读__ 内存区域，是不能改写的，而后者的char数组一般存在 __栈__ 中，是可以被改写的。
- 第一种情况的`sizeof()`将得到3个指针的长度，而后者得到18

数组和指针的常用方法
===================

## 基本的使用方法
### 函数返值之外方式来返回值
C语言可通过函数返回值，但函数只能返回一个值，如果需要通过函数返回值以外的方式返回值，需要返回T时，将 "指向T的指针" 作为参数传递给函数（out参数）

### 将数组作为函数的参数传递
C中数组不能作为参数进行传递，但可通过传递指向数组初始元素的指针，使得在函数内部操作数组成为可能

但调用方需要将数组长度作为参数传递给被调用方，否则在被调用方中`sizeof`得到的只是指针的大小，而非数组的大小
> 字符串(char数组)则可以通过末尾的`\0`判断字符个数

### 可变长数组
即使用`malloc()`在运行时再为数组申请必要的内存区域（准确的说，得到的并不是数组，而是指针），程序员必须自己来管理数组的元素个数。

## 组合使用
### 可变长数组的数组
```c
char *slogan[7];
```

声明 一周标语 所需要可变长数组(`char*`)的数组，如果从配置文件中读取一周的标语，程序可能像下面

<!-- language: !c -->

    #include <stdio.h>
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <assert.h>

    void read_slogan(FILE *fp, char **slogan){
        char buf[1024];
        int i;
        for (i = 0; i < 7; i++){
            fgets(buf, sizeof(buf), fp);
            buf[strlen(buf)-1] = '\0'; /* remove '\n' */
            slogan[i] = malloc(sizeof(char) * (strlen(buf) + 1));
            assert(slogan[i] != NULL);
            strcpy(slogan[i], buf);
        }
    }

    int main(int argc, const char *argv[]){
        char *slogan[7];
        int i;

        read_slogan(stdin, slogan);

        for (i = 0; i < 7; i++){
            printf("%s\n", slogan[i]);
        }
        return 0;
    }

上面程序对标语的长度作了1024的限制，这让人感到不便，可使用下面程序来动态分配临时缓冲区

<!-- language: !c -->

    #include <stdio.h>
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <assert.h>

    #define INIT_ALLOC_LEN (4)

    typedef struct{
        char *buffer;
        int used_len;
        int alloc_len;
    } LineStatus;

    static void line_status_new(LineStatus *l){
        l->used_len = 0;
        l->alloc_len = INIT_ALLOC_LEN;
        l->buffer = malloc(INIT_ALLOC_LEN * sizeof(char));
        assert(l->buffer != NULL);
    }

    static void add_character(LineStatus *l, char ch){
        assert(l->alloc_len >= l->used_len);
        if(l->used_len == l->alloc_len){
            l->alloc_len *= 2;
            l->buffer = realloc(l->buffer, l->alloc_len * sizeof(char));
            assert(l->buffer != NULL);
        }
        l->buffer[l->used_len] = ch;
        l->used_len++;
    }

    static void line_status_free(LineStatus *l){
        free(l->buffer);
    }

    char *read_line(FILE *fp){
        char ch, *ret_line;
        LineStatus line_status;
        line_status_new(&line_status);

        while((ch = getc(fp)) != EOF){
            if(ch == '\n'){
                add_character(&line_status, '\0');
                break;
            }
            add_character(&line_status, ch);
        }
        if(ch == EOF){
            if(line_status.used_len > 0){
                add_character(&line_status, '\0');
            }else{
                return NULL;
            }
        }
        ret_line = malloc(line_status.used_len * sizeof(char));
        assert(ret_line != NULL);
        strcpy(ret_line, line_status.buffer);
        line_status_free(&line_status);
        return ret_line;
    }

    int main(int argc, const char *argv[]){
        char *line = read_line(stdin);
        printf("%s %d\n", line, strlen(line));
        free(line);
        return 0;
    }

- 调用方需要将输出的实参`line`进行`free()`

### 可变长数组的可变长数组
上面的标语个数固定为一周的天数（7个），如果需要在内存中加载任意行数的文本文件，可考虑使用“可变长数组的可变长数组”

“类型T的可变长数组” 是通过 “指向T的指针” 来实现的（但元素的个数需要自己来管理）

如果需要 “T的可变长数组的可变长数组”，可使用 “指向T的指针的指针”

<!-- language: plain -->

    ┌─────┐
    │  * ─┼──┐
    └─────┘  │
             V
          ┌─────┐    ┌─┬─┬─┬─┬─┐
          │  * ─┼───>└─┴─┴─┴─┴─┘
          ├─────┤    ┌─┬─┬─┬─┐
          │  * ─┼───>└─┴─┴─┴─┘
          ├─────┤    ┌─┬─┬─┬─┬─┬─┐
          │  * ─┼───>└─┴─┴─┴─┴─┴─┘
          ├─────┤    ┌─┬─┬─┬─┬─┬─┬─┐
          │  * ─┼───>└─┴─┴─┴─┴─┴─┴─┘
          ├─────┤    ┌─┬─┬─┬─┐
          │  * ─┼───>└─┴─┴─┴─┘
          └─────┘

代码如下：

<!-- language: !c -->

    #include <stdio.h>
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <assert.h>

    #define INIT_ALLOC_LEN (4)

    typedef struct{
        char *buffer;
        int used_len;
        int alloc_len;
    } LineStatus;

    static void line_status_new(LineStatus *l){
        l->used_len = 0;
        l->alloc_len = INIT_ALLOC_LEN;
        l->buffer = malloc(INIT_ALLOC_LEN * sizeof(char));
        assert(l->buffer != NULL);
    }

    static void add_character(LineStatus *l, char ch){
        assert(l->alloc_len >= l->used_len);
        if(l->used_len == l->alloc_len){
            l->alloc_len *= 2;
            l->buffer = realloc(l->buffer, l->alloc_len * sizeof(char));
            assert(l->buffer != NULL);
        }
        l->buffer[l->used_len] = ch;
        l->used_len++;
    }

    static void line_status_free(LineStatus *l){
        free(l->buffer);
    }

    char *read_line(FILE *fp){
        char ch, *ret_line;
        LineStatus line_status;
        line_status_new(&line_status);

        while((ch = getc(fp)) != EOF){
            if(ch == '\n'){
                add_character(&line_status, '\0');
                break;
            }
            add_character(&line_status, ch);
        }
        if(ch == EOF){
            if(line_status.used_len > 0){
                add_character(&line_status, '\0');
            }else{
                return NULL;
            }
        }
        ret_line = malloc(line_status.used_len * sizeof(char));
        assert(ret_line != NULL);
        strcpy(ret_line, line_status.buffer);
        line_status_free(&line_status);
        return ret_line;
    }

    char **add_line(char **text_data, char *line,
                    int *line_alloc_num, int *line_num){
        assert(*line_alloc_num >= *line_num);
        if(*line_alloc_num == *line_num){
            text_data = realloc(text_data, (*line_alloc_num + INIT_ALLOC_LEN) * sizeof(char*));
            *line_alloc_num += INIT_ALLOC_LEN;
        }
        text_data[*line_num] = line;
        (*line_num)++;
        return text_data;
    }

    char **read_file(FILE *fp, int *ret_line_num_p){
        char **text_data = NULL;
        int line_num = 0;
        int line_alloc_num = 0;
        char *line;

        while((line = read_line(fp)) != NULL){
            text_data = add_line(text_data, line, &line_alloc_num, &line_num);
        }
        *ret_line_num_p = line_num;
        return text_data;
    }

    int main(int argc, const char *argv[]){
        char **text_data;
        int line_num;
        int i = 0;
        FILE *fp = fopen("input.txt", "r");

        text_data = read_file(fp, &line_num);

        for(; i < line_num; i++){
            printf("%s\n", text_data[i]);
            free(text_data[i]);
        }
        free(text_data);
        fclose(fp);
        return 0;
    }


`add_line()`及`read_file()`中通过参数指针的方式，达到共享一些变量。

#### 重构为泛型
上面的代码中，`add_line()`与`add_character()`的逻辑结构十分相似(当分配的空间不够时，需要增长空间，并记录相应的状态)，只是数据类型有所不同，可运用泛型思维，重构代码如下：

<!-- language: !c -->

    #include <stdio.h>
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <assert.h>
    #include <stdarg.h>

    #ifdef DEBUG
    #define DEBUG_WRITE(arg) debug_write arg
    #else
    #define DEBUG_WRITE(arg)
    #endif

    #define INIT_ALLOC_LEN (4)

    void debug_write(char *format, ...){
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
    }

    typedef struct{
        void *buffer;
        int elem_size;
        int used_len;
        int alloc_len;
        void(*free_fn)(void*);
    } Cache;

    static void cache_new(Cache *c, int elem_size, void(*free_fn)(void*)) {
        assert(elem_size > 0);
        c->elem_size = elem_size;
        c->used_len = 0;
        c->alloc_len = INIT_ALLOC_LEN;
        c->buffer = malloc(INIT_ALLOC_LEN * elem_size);
        c->free_fn = free_fn;
        assert(c->buffer != NULL);
    }

    static void cache_grow(Cache *c){
        DEBUG_WRITE(("-cache_grow_b: %d\n", c->alloc_len));
        c->alloc_len *= 2;
        c->buffer = realloc(c->buffer, c->alloc_len * sizeof(c->elem_size));
        assert(c->buffer != NULL);
    }

    static void cache_add(Cache *c, void *data_addr){
        assert(c->alloc_len >= c->used_len);
        if(c->used_len == c->alloc_len){
            cache_grow(c);
        }
        void *target = (char*)c->buffer + c->used_len * c->elem_size;
        memcpy(target, data_addr, c->elem_size);

        #ifdef DEBUG //debug write
            if(c->elem_size == sizeof(char)){
                DEBUG_WRITE(("-cache_add_char_s: %c\n", *(char*)data_addr));
                DEBUG_WRITE(("-cache_add_char_t: %c\n", ((char*)c->buffer)[c->used_len]));
            }
            else if (c->elem_size == sizeof(char*)){
                DEBUG_WRITE(("-cache_add_char*_s: %s\n", *(char**)data_addr));
                DEBUG_WRITE(("-cache_add_char*_t: %s\n", ((char**)c->buffer)[c->used_len]));
            }
        #endif

        c->used_len++;
    }

    static void cache_free(Cache *c){
        int i;
        if(c->free_fn != NULL){
            for(i = 0; i < c->used_len; i++){
                c->free_fn((char)c->buffer + i * c->elem_size);
            }
        }
        #ifdef DEBUG
            if(c->elem_size == sizeof(char)){
                DEBUG_WRITE(("-cache_free_char*: %s\n", (char*)c->buffer));
            }
            else if (c->elem_size == sizeof(char*)){
                DEBUG_WRITE(("-cache_free_char**\n"));
            }
        #endif
        free(c->buffer);
    }

    char *read_line(FILE *fp){
        char ch, temp_ch, *ret_line;
        Cache cache;
        cache_new(&cache, sizeof(char), NULL);

        while((ch = getc(fp)) != EOF){
            if(ch == '\n'){
                temp_ch = '\0';
                cache_add(&cache, &temp_ch);
                break;
            }
            cache_add(&cache, &ch);
        }
        if(ch == EOF){
            if(cache.used_len > 0){
                temp_ch = '\0';
                cache_add(&cache, &temp_ch);
            }else{
                return NULL;
            }
        }
        ret_line = malloc(cache.used_len * sizeof(char));
        assert(ret_line != NULL);
        strcpy(ret_line, (char*)cache.buffer);
        cache_free(&cache);
        return ret_line;
    }

    void string_free(void* elemAddr) {
        char** p = (char**)elemAddr;
        DEBUG_WRITE(("-string_free: %s\n", *p));
        free(*p);
    }

    char **read_file(FILE *fp, int *ret_line_num){
        char *temp_line, **ret_file_data;
        Cache cache;

        // cache_new(&cache, sizeof(char*), string_free);
        cache_new(&cache, sizeof(char*), NULL);
        while((temp_line = read_line(fp)) != NULL){
            // must use &, so add `char**`
            cache_add(&cache, &temp_line);
        }
        // not *ret_file_data = ...
        ret_file_data = malloc(cache.used_len * sizeof(char*));
        assert(ret_file_data != NULL);

        memcpy(ret_file_data, cache.buffer, cache.used_len * sizeof(char*));
        *ret_line_num = cache.used_len;
        cache_free(&cache);

        return ret_file_data;
    }

    int main(int argc, const char *argv[]){
        char **file_data;
        int i, line_num;
        FILE *fp = fopen("input.txt", "r");

        file_data = read_file(fp, &line_num);

        for(i = 0; i < line_num; i++){
            printf("%s\n", file_data[i]);
            free(file_data[i]);
        }
        free(file_data);
        fclose(fp);
        return 0;
    }

> 也可以使用 [泛型栈](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgitcafe.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FLanguage%2FC%2F%25E7%25BC%2596%25E7%25A8%258B%25E8%258C%2583%25E5%25BC%258F%2528stanford_cs107%2529.md#TOC7.1) 来进行编码，`push()`即相当于此处的`add_data()`

### 命令行参数
实现`cat`命令

<!-- language: !c -->

    #include <stdio.h>
    #include <stdlib.h>

    void type_one_file(FILE *fp){
        int ch;
        while((ch = getc(fp)) != EOF){
            putchar(ch);
        }
    }

    int main(int argc, const char *argv[]){
        if(argc == 1){
            type_one_file(stdin);
        }
        else{
            int i;
            FILE *fp;
            for(i = 1; i < argc; i++){
                fp = fopen(argv[i], "rb");
                if(fp == NULL){
                    fprintf(stderr, "%s:%s can not open\n", argv[0], argv[i]);
                    exit(1);
                }
                type_one_file(fp);
                fclose(fp);
            }
        }

        return 0;
    }

### 通过参数返回指针
前两节中`read_line()`，将读取的行作为返回值，如果到达文件的终点则返回`NULL`，可是，作为返回值的形式，`read_line()`返回的是通过`malloc()`分配的内存区域，前代码中并没有对返回值做检查。

如果想让其做为通用函数，必须好好的对返回值做检查，并能向调用方返回函数的处理状态

```c
typedef enum{
    READ_LINE_SUCCESS,
    READ_LINE_EOF,
    READ_LINE_OUT_OF_MEMORY
} ReadLineStatus;
```

函数的签名将变为

```c
char *read_line(FILE *fp, ReadLineStatus *status);
```

- 但有些项目坚持 “应该通过返回值返回处理状态” 的观点，就需要改造函数的签名，将`ReadLineStatus`作为函数返回类型，而原来返回的`char*`则作为out参数来处理。
- 需要返回T时，将 "指向T的指针" 作为参数传递给函数，因为需要返回`char*`所以形参就为`char**`

```c
ReadLineStatus read_line(FILE *fp, char **line);
```

代码改造如下：

```c
ReadLineStatus read_line(FILE *fp, char **line){
    char ch, temp_ch;
    Cache cache;
    ReadLineStatus status = READ_LINE_SUCCESS;

    cache_new(&cache, sizeof(char), NULL);

    while((ch = getc(fp)) != EOF){
        if(ch == '\n'){
            temp_ch = '\0';
            cache_add(&cache, &temp_ch);
            break;
        }
        cache_add(&cache, &ch);
    }
    if(ch == EOF){
        if(cache.used_len > 0){
            temp_ch = '\0';
            cache_add(&cache, &temp_ch);
            goto FUNC_END;
        }else{
            status = READ_LINE_EOF;
            goto FUNC_END;
        }
    }
    *line = malloc(cache.used_len * sizeof(char));
    assert(*line != NULL);
    strcpy(*line, (char*)cache.buffer);

  FUNC_END:
    cache_free(&cache);
    return status;
}
```

- 异常处理中使用`goto`，反而可以让程序更整洁

### 数组的可变长数组
假设开发一个支持多点折线的画笔工具，使用变长数组来表现多点折线中的“点”，并且使用 "double的数组(元素2)" 来记录一个 "点"

可使用 "指向类型T的指针" 实现 "类型T的可变长数组"

```c
double (*polyline)[2]; // polyline is pointer to array(num 2) of double
polyline = malloc(sizeof(double[2] * npoints));
```

如果感觉这有些困难，不妨通过类型定义：

```c
typedef double Point[2];
Point *polyline;
polyline = malloc(sizeof(Point * npoints));
```

无论哪种方式，取第i个点的X与Y坐标如下：

```c
double x = polyline[i][0],
       y = polyline[i][1];
```

### 考虑使用结构体
上面的例子中，如果有5条多点折线，应该通过什么方式来管理呢？

“多点折线”是“指向double的数组(元素2)的指针”（需要自己管理元素个数），因此，如果是“多点折线的数组(元素5)”，就可以解释为“指向double数组(元素2)的指针的数组(元素5)”

```c
double (*polyline[5])[2]; // polyline is array(num5) of pointer to array(num 2) of double
```

如果不是5根，而是任意数量的折线时，则参数含：

- 折线个数`int polyline_num`,
- 每条折线中点的个数`int *npoints`，表示为数组，其长度为`polyline_num`

其函数原型：

```c
func(int polyline_num, double(**polylines)[2], int *npoints);
```

如果做一个类型定义，则函数原型为：

```c
typedef double Point[2];
func(int polyline_num, Point **polylines, int *npoints);
```

再顺便定义下面的类型：

```c
typedef Point *Polyline;
func(int polyline_num, Polyline *polylines, int *npoints);
```

尽管`typedef`能简化声明，但不能使声明变得更容易理解，可考虑使用结构体：

```c
typedef struct{
    double x;
    double y;
} Point;

typedef struct{
    int npoints;
    Point *point;
} Polyline;

func(Polyline *polylines);
```

可见，通过结构体的封装，`fun`所需的参数就减少了，并且语义也更清楚了

### __*可变长*__ 的结构体
```c
typedef struct{
    int npoints;
    Point *point;
} Polyline;
```

当使用`malloc()`动态的为`Polyline *polyline`分配内存时，需要调用两次，即两次在堆空间上分配内存（第一次是`Polyline *polyline`的分配，第二次是`Point *point`的分配）

<!--language: plain-->

    Polyline         Point的数组
    ┌────┬────┐     ┌────┬────┬────┬────┬────┐
    │    │  *─┼────>│    │    │    │    │    │
    └────┴────┘     └────┴────┴────┴────┴────┘

不妨使用下面的方式：

```c
typedef struct{
    int npoints;
    Point point[1];
} Polyline;

Polyline *polyline;
polyline = malloc(sizeof(Polyline) + sizeof(Point) * (npoints - 1));
```

> `Point point[1];`完全可以声明成`Point point[0];`，这样`Polyline`类型的大小仅是`int`的大小

如果使用`polyline->point[3]`进行引用，会发生地址越界引用，好在大多数C处理环境不做数组范围检查，我们使用`malloc()`在`Polyline`后面追加分配必要的内存空间

<!--language: plain-->

      Polyline
    ┌────────────┐  ┐
    │  npoints   │  │
    ├────────────┤  ├─真正的Polyline只到这里
    │  point[0]  │  │
    ├────────────┤  ┘ ┐
    │  point[1]  │    │
    ├────────────┤    │
    │  point[2]  │    │
    ├────────────┤    │
    │  point[3]  │    ├─该区域引用，如point[3]，虽超出了数组的范围，
    ├────────────┤    │ 但malloc()为此分配了空间，一般情况能正常执行
    │  point[4]  │    │
    └────────────┘    ┘

这种写法，结构体的 __*最后*__ 成员 __不使用指针__ 也可以直接保存 __可变长__ 数组
> 一定是最后成员，否则会覆盖到其它成员
>
> ISO C99可变长结构体已被正式承认

数据结构中的指针
===============
## 案例:计算单词的出现频率
### 需求
设计开发一个将文本文件作为输入，计算出其中各单词出现频率的程序，将英文单词按照字母顺序排序后输出，并且在各单词的后面显示当前单词的出现次数，如果省略参数，就将标准输入的内容作为输入进行处理

### 设计
![c_pointer_01](../../../imgs/c_pointer_01.jpg)

1. 取得单词部分，从输入流（文件等）一个个地取得单词
2. 管理单词部分，包括输出功能
3. 主处理过程，统一管理以上两个模块

#### 取得单词
对于取得单词部分，可直接使用[TOC1.3.3](#TOC1.3.3)的实现，假设已提供了`get_word.h`这个头文件

```c
#ifndef GET_WORD_H_INCLUEDE
#define GET_WORD_H_INCLUEDE
#include <stdio.h>

int get_word(char *buf, int size, FILE *stream);

#endif /*GET_WORD_H_INCLUEDE*/
```
#### 管理单词
管理单词部分，提供了4个函数作为对外接口

- 初始化

```c
void word_initalize(void);
```

- 单词的追加，为传入的字符串动态分配内存区域，并且将字符串保存在其中

```c
void add_word(char *word);
```

- 输出单词的出现频率

```c
void dump_word(FILE *fp);
```

- 结束处理

```c
void word_finalize(void);
```

整理到 __公有__ 头文件`word_manage.h`中

```c
#ifndef WORD_MANAGE_H_INCLUEDE
#define WORD_MANAGE_H_INCLUEDE
#include <stdio.h>

void word_initalize(void);
void add_word(char *word);
void dump_word(FILE *fp);
void word_finalize(void);

#endif /*WORD_MANAGE_H_INCLUEDE*/
```

#### 主处理过程
在"主处理过程"中，使用`get_word()`努力地从输入流读取单词，然后通过调用`add_word()`，将每次读取到的单词，不断地加入到"单词管理部分"，最后再调用`dump_word()`将最终结果输出

```c
#include <stdio.h>
#include <stdlib.h>
#include "get_word.h"
#include "word_manage.h"

#define WORD_LEN_MAX (1024)

int main(int argc, char **argv){
    char buf[WORD_LEN_MAX];
    FILE *fp;
    if(argc == 1){
        fp = stdin;
    }else{
        fp = fopen(argv[1], "r");
        if(fp == NULL){
            fprintf(stderr, "%s:%s can not open.\n", argv[0], argv[1]);
            exit(1);
        }
    }

    word_initalize();
    while(get_word(buf, WORD_LEN_MAX, fp) != EOF){
        add_word(buf);
    }
    dump_word(stdout);
    word_finalize();

    return 0;
}
```

### 实现（数组版）
使用数组管理单词时，采取以下方式：

- 将单词和其出现的次数整理到结构体中
- 将这些结构体组织成数组，并且管理各单词的出现频率
- 为了将单词的追加和结果输出变得更加简单，使用将数组的元素按照单词的字母顺序排序的方式进行管理

据此写的 __私有__ 头文件`word_manage_with_array.h`如下：

```c
#ifndef WORD_MANAGE_WITH_ARRAY_H_INCLUEDE
#define WORD_MANAGE_WITH_ARRAY_H_INCLUEDE
#include "word_manage.h"

typedef struct{
    char *name;
    int count;
} Word;

#define WORD_NUM_MAX (10000)

extern Word word_array[];
extern int num_of_word;

#endif /*WORD_MANAGE_WITH_ARRAY_H_INCLUEDE*/
```

每当加入一个新单词时：

- 从数组的初始元素开始遍历，如发现同样的单词，将此单词出现的次数加1
- 如没发现，就在行进到比当前单词“大的单词”的时候，将当前单词插入到“大的单词”的前面
> 插入操作：将插入点后方的元素顺序后移，新的元素保存在空出来的位置上

代码文件`word_manage_with_array.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "word_manage_with_array.h"

Word word_array[WORD_NUM_MAX];
int num_of_word;

void word_initalize(void){
    num_of_word = 0;
}

static void shift_array(int index){
    int src;
    for(src = num_of_word-1; src>=index; src--){
        word_array[src+1] = word_array[src];
    }
    num_of_word++;
}

static char *my_strdup(char *src){
    char *dest;
    dest = malloc(sizeof(char) * (strlen(src) + 1));
    strcpy(dest, src);
    return dest;
}

void add_word(char *word){
    int i;
    int result;

    for (i = 0; i < num_of_word; ++i) {
        result = strcmp(word_array[i].name, word);
        if(result >= 0) break;
    }
    if(num_of_word !=0 && result ==0){
        word_array[i].count++;
    }else{
        shift_array(i);
        word_array[i].name = my_strdup(word);
        word_array[i].count = 1;
    }
}

void dump_word(FILE *fp){
    int i;
    for (i = 0; i < num_of_word; ++i) {
        fprintf(fp, "%-20s%5d\n", word_array[i].name, word_array[i].count);
    }
}

void word_finalize(void){
    int i;
    for (i = 0; i < num_of_word; ++i) {
        free(word_array[i].name);
    }
    num_of_word = 0;
}
```

### 实现（链表版）
数组版存以下问题：

- 中途向数组插入元素，后面的元素就必须依次向后方移动，导致效率低下
- 数组初始化时就需要决定元素个数。

使用链表的数据结构，可以避免这些问题

__私有__ 头文件`word_manage_with_chain.h`

```c
#ifndef WORD_MANAGE_WITH_CHAIN_H_INCLUEDE
#define WORD_MANAGE_WITH_CHAIN_H_INCLUEDE
#include "word_manage.h"

typedef struct Word_tag{
    char *name;
    int count;
    struct Word_tag *next;
} Word;

extern Word *word_header;

#endif /*WORD_MANAGE_WITH_CHAIN_H_INCLUEDE*/
```

代码实现`word_manage_with_chain.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "word_manage_with_chain.h"

Word *word_header = NULL;

void word_initalize(void){
    word_header = NULL;
}

static char *my_strdup(char *src){
    char *dest;
    dest = malloc(sizeof(char) * (strlen(src) + 1));
    strcpy(dest, src);
    return dest;
}

static Word *create_word(char *name){
    Word *new_word;
    new_word = malloc(sizeof(Word));
    new_word->name = my_strdup(name);
    new_word->count = 1;
    new_word->next = NULL;
    return new_word;
}

void add_word(char *word){
    Word *pos;
    Word *prev;
    Word *new_word;
    int result;

    prev = NULL;
    for(pos = word_header; pos != NULL; pos = pos->next){
        result = strcmp(pos->name, word);
        if(result >=0) break;

        prev = pos;
    }
    if(word_header != NULL && result ==0){
        pos->count++;
    }else{
        new_word = create_word(word);
        if(prev == NULL){
            new_word->next = word_header;
            word_header = new_word;
        }else{
            new_word->next = pos;
            prev->next = new_word;
        }
    }
}

void dump_word(FILE *fp){
    Word *pos;
    for(pos = word_header; pos != NULL; pos = pos->next){
        fprintf(fp, "%-20s%5d\n", pos->name, pos->count);
    }
}

void word_finalize(void){
    Word *temp;
    while(word_header != NULL){
        temp = word_header;
        word_header = word_header->next;

        free(temp->name);
        free(temp);
    }
}
```

上面的代码使用了全局变量，是不合适的，为了管理多份数据，__公有__ 头文件`word_manage.h`应声明如下：

```c
#ifndef WORD_MANAGE_H_INCLUEDE
#define WORD_MANAGE_H_INCLUEDE
#include <stdio.h>

typedef struct WordManager_tag WordManager;

WordManager *word_initalize(void);
void add_word(WordManager *word_manager, char *word);
void dump_word(WordManager *word_manager, FILE *fp);
void word_finalize(WordManager *word_manager);

#endif /*WORD_MANAGE_H_INCLUEDE*/
```

- 具体实现是用数组还是链表，对于公有头文件是无关的
- 声明一个 __*不完全类型*__ `WordManager_tag`，然后在私有头文件中，将实体填充到`struct WordManager_tag`中

对应的数组的私有头文件`word_manage_with_array.h`为：

```c
#ifndef WORD_MANAGE_WITH_ARRAY_H_INCLUEDE
#define WORD_MANAGE_WITH_ARRAY_H_INCLUEDE
#include "word_manage.h"
#define WORD_NUM_MAX (10000)

typedef struct{
    char *name;
    int count;
} Word;

struct WordManager_tag{
    Word *word_array;
    int num_of_word;
};

#endif /*WORD_MANAGE_WITH_ARRAY_H_INCLUEDE*/
```

对应的链表的私有头文件`word_manage_with_chain.h`为：

```c
#ifndef WORD_MANAGE_WITH_CHAIN_H_INCLUEDE
#define WORD_MANAGE_WITH_CHAIN_H_INCLUEDE
#include "word_manage.h"

typedef struct Word_tag{
    char *name;
    int count;
    struct Word_tag *next;
} Word;

struct WordManager_tag{
    Word *word_header;
};

#endif /*WORD_MANAGE_WITH_CHAIN_H_INCLUEDE*/
```

## 案例:绘图工具的数据结构
### 实现各种图形的数据模型
```c
typedef struct{
    double x;
    double y;
} Point;

typedef struct{
    int npoints;
    Point *point;
} Polyline;

typedef struct{
    Point minPoint;//左下角坐标
    Point maxPoint;//右上角坐标
} Rectangle;

typedef struct{
    Point center;
    double radius;
} Circle;
```

### Shape型
```c
typedef enum{
    COLOR_BLACK;
    COLOR_BLUE;
    COLOR_RED;
    COLOR_MAGENTA;
    COLOR_GREEN;
    COLOR_CYAN;
    COLOR_YELLOW;
    COLOR_WHITE;
} Color;

typedef enum{
    FILL_NONE,
    FILL_SOLID;
    FILL_HATCH;
    FILL_CROSSHATCH;
} FillPattern;
```

我们无法预测图形的数量，考虑使用`malloc()`为`Shape`动态的分配内存区域，然后通过链表对它们进行管理。我们选择两向链表：

- 对所有的图形进行重绘的时候，“从后面的图形开始顺序的地” 绘制，能够以正确的顺序表示各图形。
- 使用鼠标的方式选择图形的时候，需要 “从前面的图形开始顺序的” 检查哪个图形才是选择对象。
- 选择后图形，可以将它移动到最前面或最后面，此时需要将图形对应的元素在链表中进行移动

```c
typedef enum{
    POLYLINE_SHAPE;
    RECTANGLE_SHAPE;
    CIRCLE_SHAPE;
} ShapeType;

typedef struct Shape_tag{
    Color pen_color;
    FillPattern fill_pattern;
    Color fill_color;
    ShapeType type;
    union {
        Polyline polyline;
        Rectangle rectangle;
        Circle circle;
    } u;
    struct Shape_tag *prev;
    struct Shape_tag *next;
} Shape;
```

### 讨论

- 是否有必要存在`Line`类型
> 从应用角度而言是需要的，且是频繁的，但功能角度来言，可使用只有两个点的`Polyline`来代替，但不能使用`Polyline`来代替`Rectangle`，因为语义不同
- `Polyline`不能使用[可变长结构体](#TOC4.2.7)，因为使用后，会破坏`prev`和`next`的数据
> 即使调整了结构体的位置，`Polyline`的折线个数发生变化时，会发生`realloc()`，`Shape`地址就会发生变化（因为`Shape`中直接包含了`Polyline`，而不是它的指针），就需要调用前后两个结点对它的指向。
- 使用了共用体，会根据最大成员来分配内存空间，是不是会浪费内存
> 是的，但如果成员间的大小差别不大，是可接受的，或者使用如下方案：

```c
typedef struct Shape_tag{
    Color pen_color;
    FillPattern fill_pattern;
    Color fill_color;
    ShapeType type;
    union {
        Polyline *polyline;
        Rectangle *rectangle;
        Circle *circle;
    } u;
    struct Shape_tag *prev;
    struct Shape_tag *next;
} Shape;
```

> 使用"指针共同体"，这样也使`Polyline`使用可变长结构体具有了现实意义。但需要额外的`malloc()`与`free()`的管理。当共用体成员大小差别不大时，还是推荐先前的方案

- `Shape`放进`prev`和`next`做法很不自然，它只是一个图形，使用何种方式来管理并不是它的职责，更好的方式是：

```c
typedef struct LinkableShape_tag{
    Shape shape; //Shape *shape;
    struct LinkableShape_tag *prev;
    struct LinkableShape_tag *next;
} LinkableShape;
```

> 这种做法，也是增加了额外的`malloc()`与`free()`的管理

- 由上面问题引发一个通用的方案，形成 __泛型__

```c
typedef struct Linkable_tag{
    void *object;
    struct Linkable_tag *prev;
    struct Linkable_tag *next;
} Linkable;
typedef struct{
    Linkable *head;
    Linkable *tail;
} LinkedList;
```

> 但会影响代码的可读性，并且C的通用类型是不做编译检查的，许多BUG会延迟到运行时发生

### 图形的组合
将几个图形进行组合，然后当做一个图形去操作，定义一个`Group`类型，它包含多个`Shape`，但后者自身实现了双向链表，只需要`Group`中定义成如下：

```c
typedef struct{
    Shape *head;
    Shape *tail;
} Group;
```

更新`ShapeType`

```c
typedef enum{
    POLYLINE_SHAPE;
    RECTANGLE_SHAPE;
    CIRCLE_SHAPE;
    GROUP_SHAPE;
} ShapeType;
```

也许不够和谐，重构类型的定义：

```c
typedef enum{
    POLYLINE_SHAPE;
    RECTANGLE_SHAPE;
    CIRCLE_SHAPE;
} PrimitiveType;

typedef struct {
    Color pen_color;
    FillPattern fill_pattern;
    Color fill_color;
    PrimitiveType type;
    union {
        Polyline *polyline;
        Rectangle *rectangle;
        Circle *circle;
    } u;
} Primitive;

typedef struct Shape_tag Shape;

typedef struct{
    Shape *head;
    Shape *tail;
} Group;

typedef enum{
    PRIMITIVE_SHAPE;
    GROUP_SHAPE;
} ShapeType;

typedef struct Shape_tag{
    ShapeType type;
    union {
        Primitive primitive;
        Group group;
    } u;
    struct Shape_tag *prev;
    struct Shape_tag *next;
};

```

### 对指针恐惧

```c
Shape *new_shape;
new_shape = malloc(sizeof(Shape));
*new_shape = *shape;
```

如果复制的是长方形或圆形，没有问题，但如果是折线时，`Polyline`自身的`*points`并未复制，会导致它被多方引用，而任一方的修改，都会导致其它地方的变化。

拾遗
=====
- ANSI C会擅自将相邻的两个字符串连接起来

```c
char *color_name[] = {
    "red",
    "green",
    "blue"
    "yellow"
}
```

会构成只有"red","green","blueyellow"3个元素的数组

- 共用体的初始化针对第一个成员实施的
- 全局变量的声明
> 建立全局变量命名规则
> 建立工具去检查，确保只有一个地方定义，其它文件中使用`extern`，使用宏来帮助：

```c
#ifdef GLOBAL_VARIABLE_DEFINE
#define GLOBAL /* 定义无 */
#else
#define GLOBAL extern
#endif /* GLOBAL_VARIABLE_DEFINE */

GLOBAL int global_variable;
```

> 将头文件写成这样，只有某一个文件使用了`#define GLOBAL_VARIABLE_DEFINE`，从而保证了只有一个地方定义，而其它地方是`extern`

> 使用了这个技巧，就无法使用初始化表达式，对于全局变量，最好少用初始化表达式，而借助一些函数来完成，因为前者只能发生一次，而函数调用却可以多次发生。

> 但如果确实需要对数组进行初始化时，可使用下面方式：

```c
GLOBAL char *color_name[]
#ifdef GLOBAL_VARIABLE_DEFINE
= {
    "red",
    "green",
    "blue",
}
#endif /* GLOBAL_VARIABLE_DEFINE */
;
```

<!--
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


 -->
