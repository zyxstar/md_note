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
#define ARR_SIZE 7

int is_leap_year(int);
int main(){
    int years[ARR_SIZE] = {1900,1901,1904,1996,2000,2004,2005};
    int i;
    for(i = 0; i < ARR_SIZE; i++){
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
#include <limits.h>

int add(int a, int b){return a + b;}
int minus(int a, int b){return a - b;}
int multi(int a, int b){return a * b;}
int divi(int a, int b){return a / b;}

int operator(int(*fun)(int,int), int a, int b){
    if(fun != NULL)
        return fun(a, b);
    return INT_MIN;
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


50道题
=========
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
    a = 45347;
    printf("%d\n", reverse(a));
    return 0;
}

```

## Exam02
- 计算1+2+3…+n的值，n是从键盘输入的自然数。

<!-- run -->

```c
#include <stdio.h>

int add(int a, int b){return a + b;}
int reduce(int(*callback)(int, int), int* arr, int size){
    int i, acc = 0;
    for(i = 0; i < size; i++){
        acc = callback(acc, arr[i]);
    }
    return acc;
}

int main(){
    int n = 50, i;
    int arr[n];
    for(i = 0; i < n; i++){
        arr[i] = i;
    }
    printf("%d\n", reduce(add, arr, n));
    return 0;
}

```

## Exam03
- 从终端（键盘）读入20个数据到数组中，统计其中正数的个数，并计算这些正数之和。

<!-- run -->

```c
#include <stdio.h>
#define ARR_SIZE 20

int add(int a, int b){return a + b;} /* use Exam02 code */
int reduce(int(*callback)(int, int), int* arr, int size){ /* use Exam02 code */
    int i, acc = 0;
    for(i = 0; i < size; i++){
        acc = callback(acc, arr[i]);
    }
    return acc;
}
int is_positive(num){return num > 0;}
int filter(int(*predicate)(int), int* arr, int size){
    int i, new_size = 0, temp;
    for(i = 0; i < size; i++){
        if(predicate(arr[i])){
            temp = arr[i];
            arr[new_size] = temp;
            new_size++;
        }
    }
    return new_size;
}

int main(){
    int arr[ARR_SIZE] = {1,2,3,4,5,6,-3,-4,-5,-6,11,12,13,14,15,16,-13,-14,-15,-16};
    int new_size = filter(is_positive, arr, ARR_SIZE);    
    printf("size: %d; sum: %d \n", new_size, reduce(add, arr, new_size));
    return 0;
}

```

## Exam04
- 从终端（键盘）将5个整数输入到数组a中，然后将a逆序复制到数组b中，并输出b中各元素的值。

## Exam05
- 要将5张100元的大钞票，换成等值的50元，20元，10元，5元一张的小钞票，每种面值至少1张，编程求需要多少张纸币。

## Exam06
- 求n以内（不包括n）同时能被3和7整除的所有自然数之和的平方根s，n从键盘输入。例如若n为1000时，函数值应为：s=153.909064。

## Exam07
- 一辆卡车违反交通规则，撞人后逃跑。现场有三人目击事件，但都没有记住车号，只记下车号的一些特征。甲说：牌照的前两位数字是相同的；乙说：牌照的后两位数字是相同的，但与前两位不同；丙是数学家，他说：四位的车号刚好是一个整数的平方。请根据以上线索找出车号。

## Exam08
- 输入1~10之间的一个数字，输出它对应的英文单词。

## Exam09
- 个位数为6且能被3整除但不能被5整除的三位自然数共有多少个，分别是哪些？

## Exam10
- 输入某年某月某日，判断这一天是这一年的第几天？

## Exam11
- 两个乒乓球队进行比赛，各出三人。甲队为a,b,c三人，乙队为x,y,z三人。已抽签决定比赛名单。有人向队员打听比赛的名单。a说他不和x比，c说他不和x,z比，请编程序找出三队赛手的名单。

## Exam12
- 有5个人坐在一起，问第五个人多少岁？他说比第4个人大2岁。问第4个人岁数，他说比第3个人大2岁。问第三个人，又说比第2人大两岁。问第2个人，说比第一个人大两岁。最后 问第一个人，他说是10岁。请问第五个人多大？

## Exam13
- 输入某三角形的三个边的长度，判断出这是个什么三角形（等腰、等边、任意，或不能构成）。

## Exam14
- 输入10个数，分别统计其中正数、负数、零的个数。

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

## Exam50
- 有1000发子弹 要提前装道10箱子里面，接收键盘输入，要取多少颗子弹数，只能显示整箱的个数，问这10个箱子怎么装（定义一个数组10个元素，分别装子弹的个数，比如取走100发子弹 程序运行结果，比如2箱） 
