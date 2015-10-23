> 2015-05-09

简单用法
==========
## 一个示例
如果一个工程有3个头文件，8个C文件

```make
edit : main.o kbd.o command.o display.o /
       insert.o search.o files.o utils.o
    cc -o edit main.o kbd.o command.o display.o /
          insert.o search.o files.o utils.o

main.o : main.c defs.h
    cc -c main.c
kbd.o : kbd.c defs.h command.h
    cc -c kbd.c
command.o : command.c defs.h command.h
    cc -c command.c
display.o : display.c defs.h buffer.h
    cc -c display.c
insert.o : insert.c defs.h buffer.h
    cc -c insert.c
search.o : search.c defs.h buffer.h
    cc -c search.c
files.o : files.c defs.h buffer.h command.h
    cc -c files.c
utils.o : utils.c defs.h
    cc -c utils.c
clean :
    rm edit main.o kbd.o command.o display.o /
       insert.o search.o files.o utils.o
```

`clean`不是一个文件，只是一个动作名字（伪目标），make就不会自动去找文件的依赖性，不会自动执行，需要`make`命令后加上它`make clean`

## 使用变量
上面的例子，`.o`文件的字符串被重复了两次，可以声明一个变量来指代它

```make
objects = main.o kbd.o command.o display.o /
    insert.o search.o files.o utils.o

edit : $(objects)
    cc -o edit $(objects)
main.o : main.c defs.h
    cc -c main.c
kbd.o : kbd.c defs.h command.h
    cc -c kbd.c
command.o : command.c defs.h command.h
    cc -c command.c
display.o : display.c defs.h buffer.h
    cc -c display.c
insert.o : insert.c defs.h buffer.h
    cc -c insert.c
search.o : search.c defs.h buffer.h
    cc -c search.c
files.o : files.c defs.h buffer.h command.h
    cc -c files.c
utils.o : utils.c defs.h
    cc -c utils.c
clean :
    rm edit $(objects)
```

## 自动推导
`make`可以自动推导文件及文件依赖关系后面的命令，没必要去在每个`.o`文件后都写上类似的命令。如果`make`找到一个`whatever.o`，那么就会依赖`whatever.c`，并且`cc -c whatever`也被推导出来

```make
objects = main.o kbd.o command.o display.o /
          insert.o search.o files.o utils.o

edit : $(objects)
    cc -o edit $(objects)

main.o : defs.h
kbd.o : defs.h command.h
command.o : defs.h command.h
display.o : defs.h buffer.h
insert.o : defs.h buffer.h
search.o : defs.h buffer.h
files.o : defs.h buffer.h command.h
utils.o : defs.h

clean :
    rm edit $(objects)
```

## 新风格(不推荐)
那堆`.o`和`.h`的依赖，有许多重复，能不能收拢

```make
objects = main.o kbd.o command.o display.o /
          insert.o search.o files.o utils.o

edit : $(objects)
    cc -o edit $(objects)

$(objects) : defs.h
kbd.o command.o files.o : command.h
display.o insert.o search.o files.o : buffer.h

clean :
    rm edit $(objects)
```

通过多个`<target>`来依赖单个头文件，减少了Makefile文件行数，但依赖关系有点凌乱，并不推荐使用

## 清空目标文件规则
```make
clean:
    rm edit $(objects)
```

更稳健的做法

```make
.PHONY : clean
clean :
    -rm edit $(objects)
```

上面文件内容中，`.PHONY`表示，`clean`是个伪目标文件，而且`rm`加减号意思是，也许某些文件出现问题，还继续后面的事


Makefile组成
============
- `显式规则`显式规则说明了，如何生成一个或多的的目标文件。这是由Makefile的书写者明显指出，要生成的文件，文件的依赖文件，生成的命令。
- `隐晦规则`由于make有自动推导的功能，所以隐晦的规则可以让比较粗糙地简略地书写Makefile，这是由make所支持的。
- `变量的定义`在Makefile中要定义一系列的变量，变量一般都是字符串，这个有点C语言中的宏，当Makefile被执行时，其中的变量都会被扩展到相应的引用位置上。
- `文件指示`其包括了三个部分，一个是在一个Makefile中引用另一个Makefile，就像C语言中的include一样；另一个是指根据某些情况指定Makefile中的有效部分，就像C语言中的预编译#if一样；还有就是定义一个多行的命令。
- `注释`Makefile中只有行注释，和UNIX的Shell脚本一样，其注释是用`#`字符。如果要在Makefile中使用`#`字符，可以用反斜框进行转义，如：`/#`

## Makefile的文件名
`make`命令会在当前目录下按顺序找寻文件名为`GNUmakefile`、`makefile`、`Makefile`的文件，找到了解释这个文件。在这三个文件名中，最好使用`Makefile`这个文件名

可以使用别的文件名来书写Makefile，比如：`Make.Linux`，`Make.Solaris`，`Make.AIX`等，如果要指定特定的Makefile，可以使用make的`-f`和`--file`参数，如：`make -f Make.Linux`或`make --file Make.AIX`。

## 引用其它Makefile
```
include <filename>
```

可使用变量

```
bar = e.mk f.mk
include foo.make *.mk $(bar)
```

如果文件都没有指定绝对路径或是相对路径的话，make会在当前目录下首先寻找，如果当前目录下没有找到，那么，make还会在下面的几个目录下找：

- 如果make执行时，有`-I`或`--include-dir`参数，那么make就会在这个参数所指定的目录下去寻找
- 如果目录`<prefix>/include`（一般是：`/usr/local/bin`或`/usr/include`）存在的话，make也会去找。


## 环境变量MAKEFILES
不建议使用，会影响全局

书写规则
==========
## 基本语法
```
<target1> <target2> <target3>: <prereq1> <prereq2>
    <command1>
    <command2>
    <command3>
```

工作目录(target)是一个必须建造的文件或进行的事情，必要条件(prereuisite)或依存对象(dependent)是工作目录得以被成功创建之前，必须事先存在的那些文件；而所要执行的命令(command)则是必要条件成立时将会创建工作目录的那些shell命令

targets是文件名，以空格分开，可以使用通配符。一般来说，目标基本上是一个文件，但也有可能是多个文件。

command是命令行，如果其不与`target:prerequisites`在一行，那么，必须以[Tab键]开头，如果和prerequisites在一行，那么可以用分号做为分隔

prerequisites也就是目标所依赖的文件（或依赖目标）。如果其中的某个文件要比目标文件要新，那么，目标就被认为是`过时的`，被认为是需要重生成的

如果命令太长，可以使用反斜框（‘/’）作为换行符。make对一行上有多少个字符没有限制。规则告诉make两件事，文件的依赖关系和如何成成目标文件。

一般来说，make会以UNIX的标准Shell，也就是/bin/sh来执行命令。

## 规则中使用通配符
make支持三各通配符：`*`，`?`和`[...]`。这是和Unix的B-Shell是相同的。 如`*.c`表示所以后缀为c的文件。如果文件名中有通配符，那么可以用转义字符`/`

```make
objects = *.o
```

通符同样可以用在变量中。并不是说`*.o`会展开，不！`objects`的值就是`*.o`。Makefile中的变量其实就是C/C++中的宏。如果要让通配符在变量中展开，也就是让`objects`的值是所有`.o`的文件名的集合，那么，可以这样：

```make
objects := $(wildcard *.o)
```

## 文件搜寻
有大量的源文件，通常的做法是把这许多的源文件分类，并存放在不同的目录中。所以，当make需要去找寻文件的依赖关系时，可以在文件前加上路径，但最好的方法是把一个路径告诉make，让make __自动__ 去找。

Makefile文件中的特殊变量`VPATH`就是完成这个功能的，如果没有指明这个变量，make只会在当前的目录中去找寻依赖文件和目标文件。如果定义了这个变量，那么，make就会在当当前目录找不到的情况下，到所指定的目录中去找寻文件了。

```make
VPATH = src:../headers
```

上面的的定义指定两个目录，`src`和`../headers`，make会按照这个顺序进行搜索。目录由`:`分隔。（当然，当前目录永远是最高优先搜索的地方）

另一个设置文件搜索路径的方法是使用make的`vpath`关键字（注意，它是全小写的），这不是变量，这是一个make的关键字，这和上面提到的那个`VPATH`变量很类似，但是它更为灵活。它可以 __指定不同的文件在不同的搜索目录__ 中。这是一个很灵活的功能。它的使用方法有三种：

- `vpath <pattern> <directories>`为符合模式`<pattern>`的文件指定搜索目录`<directories>`。
- `vpath <pattern>`清除符合模式`<pattern>`的文件的搜索目录。
- `vpath`清除所有已被设置好了的文件搜索目录。

`vapth`使用方法中的`<pattern>`需要包含`%`字符。`%`的意思是匹配零或若干字符，例如，`%.h`表示所有以`.h`结尾的文件。`<pattern>`指定了要搜索的文件集，而`<directories>`则指定了`<pattern>`的文件集的搜索的目录。例如：

```make
vpath %.h ../headers
```

要求`make`在`../headers`目录下搜索所有以`.h`结尾的文件

可以连续地使用`vpath`语句，以指定不同搜索策略。如果连续的`vpath`语句中出现了相同的`<pattern>`，或是被重复了的`<pattern>`，那么，`make`会按照`vpath`语句的先后顺序来执行搜索。如：

```make
vpath %.c foo
vpath %   blish
vpath %.c bar
```

其表示`.c`结尾的文件，先在`foo`目录，然后是`blish`，最后是`bar`目录。

```make
vpath %.c foo:bar
vpath %   blish
```

而上面的语句则表示`.c`结尾的文件，先在`foo`目录，然后是`bar`目录，最后才是`blish`目录。

## 伪目标
```make
clean:
    rm *.o temp
```

并不生成`clean`这个文件。`伪目标`并不是一个文件，只是一个标签，由于`伪目标`不是文件，所以`make`无法生成它的依赖关系和决定它是否要执行。只有通过显示地指明这个`目标`才能让其生效。当然，`伪目标`的取名不能和文件名重名，不然其就失去了`伪目标`的意义了

当然，为了避免和文件重名的这种情况，可以使用一个特殊的标记`.PHONY`来显示地指明一个目标是`伪目标`，向make说明，不管是否有这个文件，这个目标就是`伪目标`。

```make
 .PHONY: clean
clean:
    rm *.o temp
```

Makefile中的第一个目标会被作为其默认目标。声明了一个`all`的伪目标，其依赖于其它三个目标。由于伪目标的特性是，总是被执行的，所以其依赖的那三个目标就总是不如`all`这个目标新。所以，其它三个目标的规则总是会被决议

```make
all : prog1 prog2 prog3
.PHONY : all

prog1 : prog1.o utils.o
    cc -o prog1 prog1.o utils.o

prog2 : prog2.o
    cc -o prog2 prog2.o

prog3 : prog3.o sort.o utils.o
    cc -o prog3 prog3.o sort.o utils.o
```

`make clean`将清除所有要被清除的文件。`cleanobj`和`cleandiff`这两个伪目标有点像`子程序`的意思。可以输入`make cleanall`和`make cleanobj`和`make cleandiff`命令来达到清除不同种类文件的目的。

```make
.PHONY: cleanall cleanobj cleandiff

cleanall : cleanobj cleandiff
    rm program

cleanobj :
    rm *.o

cleandiff :
    rm *.diff
```

## 多目标
Makefile的规则中的目标可以不止一个，其支持多目标，有可能多个目标同时依赖于一个文件，并且其生成的命令大体类似。于是就能把其合并起来。

当然，多个目标的生成规则的执行命令是同一个，这可能会可带来麻烦，不过好在可以使用一个自动化变量`$@`，表示着目前规则中所有的目标的集合

```make
bigoutput littleoutput : text.g
    generate text.g -$(subst output,,$@) > $@
```

等价于

```make
bigoutput : text.g
    generate text.g -big > bigoutput
littleoutput : text.g
    generate text.g -little > littleoutput
```

其中，`-$(subst output,,$@)`中的`$`表示执行一个Makefile的函数，函数名为`subst`，后面的为参数。这里的这个函数是截取字符串的意思，`$@`表示目标的集合，就像一个数组，`$@`依次取出目标，并执于命令。

## 静态模式
```
<targets ...>: <target-pattern>: <prereq-patterns ...>
    <commands>
    ...
```

- `<targets>`定义了一系列的目标文件，可以有通配符。是目标的一个集合。
- `<target-parrtern>`是指明了`<targets>`的模式，也就是的目标集模式。
- `<prereq-parrterns>`是目标的依赖模式，它对`<target-parrtern>`形成的模式再进行一次依赖目标的定义。

如果`<target-parrtern>`定义成`%.o`，意思是`<target>`集合中都是以`.o`结尾的，而如果`<prereq-parrterns>`定义成`%.c`，意思是对`<target-parrtern>`所形成的目标集进行二次定义，其计算方法是，取`<target-parrtern>`模式中的`%`（也就是去掉了`.o`这个结尾），并为其加上[.c]这个结尾，__形成的新集合__。

```make
objects = foo.o bar.o

all: $(objects)

$(objects): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@
```

目标从`$object`中获取，`%.o`表明要所有以`.o`结尾的目标，也就是`foo.o bar.o`，也就是变量`$object`集合的模式，而依赖模式`%.c`则取模式`%.o`的`%`，也就是`foo bar`，并为其加下`.c`的后缀，于是，依赖目标就是`foo.c bar.c`。而命令中的`$<`和`$@`则是自动化变量，`$<`表示所有的依赖目标集（也就是`foo.c bar.c`），`$@`表示目标集（也就是`foo.o bar.o`）。于是，上面的规则展开后等价于下面的规则：

```make
foo.o : foo.c
    $(CC) -c $(CFLAGS) foo.c -o foo.o
bar.o : bar.c
    $(CC) -c $(CFLAGS) bar.c -o bar.o
```

再看一个例子：

```make
files = foo.elc bar.o lose.o

$(filter %.o,$(files)): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@
$(filter %.elc,$(files)): %.elc: %.el
    emacs -f batch-byte-compile $<
```

`$(filter %.o,$(files))`表示调用Makefile的`filter`函数，过滤`$filter`集，只要其中模式为`%.o`的内容

## 自动生成依赖性
在Makefile中，依赖关系可能会需要包含一系列的头文件，比如，如果`main.c`中有一句`#include "defs.h"`，那么依赖关系应该是：

```make
main.o : main.c defs.h
```

如果是一个比较大型的工程，必需清楚哪些C文件包含了哪些头文件，并且，在加入或删除头文件时，也需要小心地修改Makefile，这是一个很没有维护性的工作。为了避免这种繁重而又容易出错的事情，可以使用C/C++编译的一个功能。大多数的C/C++编译器都支持一个`-M`的选项，即自动找寻源文件中包含的头文件，并生成一个依赖关系。例如，如果执行下面的命令：

```shell
cc -M main.c
```

其输出是：

```
main.o : main.c defs.h
```

于是由编译器自动生成的依赖关系，这样一来，就不必再手动书写若干文件的依赖关系，而由编译器自动生成了。需要提醒一句的是，如果使用GNU的C/C++编译器，得用`-MM`参数，不然，`-M`参数会把一些标准库的头文件也包含进来。

`gcc -M main.c`的输出是：

```
main.o: main.c defs.h /usr/include/stdio.h /usr/include/features.h /
     /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h /
     /usr/lib/gcc-lib/i486-suse-linux/2.95.3/include/stddef.h /
     /usr/include/bits/types.h /usr/include/bits/pthreadtypes.h /
     /usr/include/bits/sched.h /usr/include/libio.h /
     /usr/include/_G_config.h /usr/include/wchar.h /
     /usr/include/bits/wchar.h /usr/include/gconv.h /
     /usr/lib/gcc-lib/i486-suse-linux/2.95.3/include/stdarg.h /
     /usr/include/bits/stdio_lim.h
```

`gcc -MM main.c`的输出则是：

```
main.o: main.c defs.h
```

GNU组织建议把编译器为每一个源文件的自动生成的依赖关系放到一个文件中，为每一个`name.c`的文件都生成一个`name.d`的Makefile文件，`.d`文件中就存放对应`.c`文件的依赖关系。

于是，可以写出`.c`文件和`.d`文件的依赖关系，并让make自动更新或自成`.d`文件，并把其包含在主Makefile中，这样，就可以自动化地生成每个文件的依赖关系了。

```make
%.d: %.c
    @set -e; rm -f $@; \
    $(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$
```

这个规则的意思是，所有的`.d`文件依赖于`.c`文件，`rm -f $@`的意思是删除所有的目标，也就是`.d`文件，第二行的意思是，为每个依赖文件`$<`，也就是`.c`文件生成依赖文件，`$@`表示模式`%.d`文件，如果有一个C文件是`name.c`，那么`%`就是`name`，`$$$$`意为一个随机编号，第二行生成的文件有可能是`name.d.12345`，第三行使用`sed`命令做了一个替换，第四行就是删除临时文件。

总而言之，这个模式要做的事就是在编译器生成的依赖关系中加入`.d`文件的依赖，即把依赖关系：

```
main.o : main.c defs.h
```

转成：

```
main.o main.d : main.c defs.h
```

于是，`.d`文件也会自动更新了，并会自动生成了，当然，还可以在这个`.d`文件中加入的不只是依赖关系，包括生成的命令也可一并加入，让每个`.d`文件都包含一个完赖的规则。一旦完成这个工作，接下来，就要把这些自动生成的规则放进主Makefile中。可以使用Makefile的`include`命令，来引入别的Makefile文件（前面讲过），例如：

```make
sources = foo.c bar.c
include $(sources:.c=.d)
```

上述语句中的`$(sources:.c=.d)`中的`.c=.d`的意思是做一个替换，把变量`$(sources)`所有`.c`的字串都替换成`.d`。当然，得注意次序，因为include是按次来载入文件，最先载入的`.d`文件中的目标会成为默认目标。

书写命令
==========
## 显示命令
当用`@`字符在命令行前，那么，这个命令将不被make显示出来，最具代表性的例子是，用这个功能来像屏幕显示一些信息。如：

```make
@echo 正在编译XXX模块......
```

当make执行时，会输出`正在编译XXX模块......`字串，但不会输出命令，如果没有`@`，那么，make将输出

```
echo 正在编译XXX模块......
正在编译XXX模块......
```

如果make执行时，带入make参数`-n`或`--just-print`，那么其只是显示命令，但不会执行命令，这个功能很有利于 __调试__ Makefile，看看书写的命令是执行起来是什么样子的或是什么顺序的。

而make参数`-s`或`--slient`则是全面禁止命令的显示。

## 命令执行
当依赖目标新于目标时，也就是当规则的目标需要被更新时，make会一条一条的执行其后的命令。需要注意的是，如果要让上一条命令的结果应用在下一条命令时，应该使用分号分隔这两条命令(否则make认为它们之间无依赖，会分开进程去执行它，这样才能多任务的进行make)

```make
exec1:
    cd /home/zyx
    pwd

exec2:
    cd /home/zyx; pwd
```

当执行`make exec1`时，第一个例子中的`cd`没有作用，`pwd`会打印出当前的Makefile目录，而第二个例子中，`cd`就起作用了，pwd会打印出`/home/zyx`。

## 命令出错
有些时候，命令的出错并不表示就是错误的。例如`mkdir`命令，一定需要建立一个目录，如果目录不存在，那么`mkdir`就成功执行，万事大吉，如果目录存在，那么就出错了。之所以使用`mkdir`的意思就是一定要有这样的一个目录，于是就不希望`mkdir`出错而终止规则的运行。

为了做到这一点，忽略命令的出错，可以在Makefile的命令行前加一个减号`-`（在Tab键之后），标记为不管命令出不出错都认为是成功的。如：

```make
clean:
    -rm -f *.o
```

还有一个全局的办法是，给make加上`-i`或是`--ignore-errors`参数，那么，Makefile中所有命令都会忽略错误。而如果一个规则是以`.IGNORE`作为目标的，那么这个规则中的所有命令将会忽略错误。这些是不同级别的防止命令出错的方法，可以根据不同喜欢设置。

还有一个要提一下的make的参数的是`-k`或是`--keep-going`，这个参数的意思是，如果某规则中的命令出错了，那么就终目该规则的执行，但继续执行其它规则。

## 嵌套执行make
在一些大的工程中，会把不同模块或是不同功能的源文件放在不同的目录中，可以在每个目录中都书写一个该目录的Makefile，这有利于让Makefile变得更加地简洁，而不至于把所有的东西全部写在一个Makefile中，这样会很难维护Makefile，这个技术对于模块编译和分段编译有着非常大的好处。

例如，有一个子目录叫subdir，这个目录下有个Makefile文件，来指明了这个目录下文件的编译规则。那么总控的Makefile可以这样书写：

```make
subsystem:
    cd subdir && $(MAKE)
```

其等价于：

```make
subsystem:
    $(MAKE) -C subdir
```

总控Makefile的变量可以传递到下级的Makefile中（如果显示的声明），但是不会覆盖下层的Makefile中所定义的变量，除非指定了`-e`参数。

如果要传递变量到下级Makefile中，那么可以使用这样的声明：

```make
export <variable ...>
```

如果不想让某些变量传递到下级Makefile中，那么可以这样声明：

```make
unexport <variable ...>
```

有两个变量，一个是`SHELL`，一个是`MAKEFLAGS`，这两个变量不管是否export，其总是要传递到下层Makefile中，特别是`MAKEFLAGS`变量，其中包含了make的 __参数信息__，如果执行`总控Makefile`时有make参数或是在上层Makefile中定义了这个变量，那么`MAKEFLAGS`变量将会是这些参数，并会传递到下层Makefile中，这是一个系统级的环境变量。

如果不想往下层传递参数，那么，可以这样来：

```make
subsystem:
    cd subdir && $(MAKE) MAKEFLAGS=
```

## 定义命令包
如果Makefile中出现一些相同命令序列，那么可以为这些相同的命令序列定义一个变量。定义这种命令序列的语法以`define`开始，以`endef`结束，如：

```make
define run-yacc
yacc $(firstword $^)
mv y.tab.c $@
endef
```

这里，`run-yacc`是这个命令包的名字，其不要和Makefile中的变量重名。在`define`和`endef`中的两行就是命令序列。这个命令包中的第一个命令是运行Yacc程序，因为Yacc程序总是生成`y.tab.c`的文件，所以第二行的命令就是把这个文件改改名字。还是把这个命令包放到一个示例中来看看吧。

```make
foo.c : foo.y
    $(run-yacc)
```

可以看见，要使用这个命令包，就好像使用变量一样。在这个命令包的使用中，命令包`run-yacc`中的`$^`就是`foo.y`，`$@`就是`foo.c`，make在执行命令包时，命令包中的每个命令会被依次独立执行。

使用变量
===========

在Makefile中，变量可以使用在`目标`，`依赖目标`，`命令`或是Makefile的其它部分中。

传统的Makefile的变量名是全大写的命名方式，但我推荐使用大小写搭配的变量名，如：MakeFlags。这样可以避免和系统的变量冲突，而发生意外的事情。

## 变量的基础
变量在声明时需要给予初值，而在使用时，需要给在变量名前加上`$`符号，但最好用小括号`（）`或是大括号`{}`把变量给包括起来。如果要使用真实的`$`字符，那么需要用`$$`来表示。

```make
objects = program.o foo.o utils.o
program : $(objects)
    cc -o program $(objects)

$(objects) : defs.h
```

变量会在使用它的地方精确地展开，就像C/C++中的宏一样，例如：

```make
objects = program.o foo.o utils.o
program : program.o foo.o utils.o
    cc -o program program.o foo.o utils.o

program.o foo.o utils.o : defs.h
```

## 赋值类型
Makefile一共提供了四个赋值运算符 （`=`、`:=`、`？=`、`+=`）

```
# 在执行时扩展，允许递归扩展。
VARIABLE = value

# 在定义时扩展。
VARIABLE := value

# 只有在该变量为空时才设置值。
VARIABLE ?= value
# 等价
ifeq ($(origin VARIABLE), undefined)
    VARIABLE = value
endif

# 将值追加到变量的尾端。
VARIABLE += value

variable := value
variable += more
# 等价
variable := $(variable) more
```

### 变量中的变量
```make
foo = $(bar)
bar = $(ugh)
ugh = Huh?

all:
    echo $(foo)
```

执行`make all`将会打出变量`$(foo)`的值是`Huh?`（ `$(foo)`的值是`$(bar)`，`$(bar)`的值是`$(ugh)`，`$(ugh)`的值是`Huh?``）可见，变量是可以 __使用后面__ 的变量来定义的。

可以把变量的真实值推到后面来定义

```make
CFLAGS = $(include_dirs) -O
include_dirs = -Ifoo -Ibar
```

当`CFLAGS`在命令中被展开时，会是`-Ifoo -Ibar -O`

但这种形式也有不好的地方，那就是 __递归定义__，为了避免，可以使用make中的另一种用变量来定义变量的方法。这种方法使用的是`:=`操作符，在定义时就展开

```make
x := foo
y := $(x) bar
x := later
```

其等价于：

```make
y := foo bar
x := later
```

这种方法，前面的变量 __不能使用后面__ 的变量，只能使用前面已定义好了的变量，那么，`y`的值是`bar`，而不是`foo bar`。

看一个复杂的例子，其中包括了make的函数、条件表达式和一个系统变量`MAKELEVEL`的使用：

```make
ifeq (0,${MAKELEVEL})
    cur-dir   := $(shell pwd)
    whoami    := $(shell whoami)
    host-type := $(shell arch)
    MAKE := ${MAKE} host-type=${host-type} whoami=${whoami}
endif
```

## 变量高级用法
第一种是变量值的替换

可以替换变量中的共有的部分，其格式是`$(var:a=b)`或是`${var:a=b}`，其意思是，把变量`var`中所有以`a`字串`结尾`的`a`替换成`b`字串。这里的`结尾`意思是`空格`或是`结束符`。

```make
foo := a.o b.o c.o
bar := $(foo:.o=.c)
```

`$(bar)`的值就是`a.c b.c c.c`

另一种变量替换的技术是以`静态模式`（参见前面章节）定义的，如：

```make
foo := a.o b.o c.o
bar := $(foo:%.o=%.c)
```

第二种高级用法是——`把变量的值再当成变量`(类似变量指针，函数指针)。先看一个例子：

```make
x = y
y = z
a := $($(x))
```

`$(x)`的值是`y`，所以`$($(x))`就是`$(y)`，于是`$(a)`的值就是`z`。（注意，是`x=y`，而不是`x=$(y)`）

来看一个例子：

```make
x = $(y)
y = z
z = Hello
a := $($(x))
```

这里的`$($(x))`被替换成了`$($(y))`，因为`$(y)`值是`z`，所以，最终结果是`a:=$(z)`，也就是`Hello`。

再复杂一点，再加上函数：

```make
x = variable1
variable2 := Hello
y = $(subst 1,2,$(x))
z = y
a := $($($(z)))
```

这个例子中，`$($($(z)))`扩展为`$($(y))`，而其再次被扩展为`$($(subst 1,2,$(x)))`。`$(x)`的值是`variable1`，`subst`函数把`variable1`中的所有`1`字串替换成`2`字串，于是，`variable1`变成`variable2`，再取其值，所以，最终，`$(a)`的值就是$(variable2)的值——`Hello`

在这种方式中，或要可以使用多个变量来组成一个变量的名字，然后再取其值：

```make
first_second = Hello
a = first
b = second
all = $($a_$b)
```

这里的`$a_$b`组成了`first_second`，于是，`$(all)`的值就是`Hello`。

```make
a_objects := a.o b.o c.o
1_objects := 1.o 2.o 3.o

sources := $($(a1)_objects:.o=.c)
```

这个例子中，如果`$(a1)`的值是`a`的话，那么，`$(sources)`的值就是`a.c b.c c.c`；如果`$(a1)`的值是`1`，那么`$(sources)`的值是`1.c 2.c 3.c`。

这种技术和`函数`与`条件语句`一同使用的例子：

```make
ifdef do_sort
    func := sort
else
    func := strip
endif

bar := a d b g q c

foo := $($(func) $(bar))
```

这个示例中，如果定义了`do_sort`，那么：`foo := $(sort a d b g q c)`，于是`$(foo)`的值就是`a b c d g q`，而如果没有定义`do_sort`，那么：`foo := $(strip a d b g q c)`，调用的就是`strip`函数。

当然，`把变量的值再当成变量`这种技术，同样可以用在操作符的左边：

```make
dir = foo
$(dir)_sources := $(wildcard $(dir)/*.c)
define $(dir)_print
    lpr $($(dir)_sources)
endef
```


## override 指示符
如果有变量是通常make的命令行参数设置的，那么Makefile中对这个变量的赋值会被忽略。如果想在Makefile中设置这类参数的值，那么，可以使用`override`指示符。其语法是：

```make
override <variable> = <value>
override <variable> := <value>
```

当然，还可以追加：

```make
override <variable> += <more text>
```

对于多行的变量定义，用`define`指示符，在`define`指示符前，也同样可以使用`override`指示符，如：

```make
override define foo
    bar
endef
```

## 环境变量
make运行时的系统环境变量可以在make开始运行时被载入到Makefile文件中，但是如果Makefile中已定义了这个变量，或是这个变量由make命令行带入，那么系统的环境变量的值将被覆盖。（如果make指定了`-e`参数，那么，系统环境变量将　__覆盖__ Makefile中定义的变量）

因此，如果在环境变量中设置了`CFLAGS`环境变量，那么就可以在所有的Makefile中使用这个变量了。这对于使用统一的编译参数有比较大的好处。如果Makefile中定义了`CFLAGS`，那么则会使用Makefile中的这个变量，如果没有定义则使用系统环境变量的值，一个共性和个性的统一，很像`全局变量`和`局部变量`的特性。

当make嵌套调用时（参见前面的`嵌套调用`章节），上层Makefile中定义的变量会以系统环境变量的方式传递到下层的Makefile中。当然，默认情况下，只有 __通过命令行设置的变量会被传递__。而定义在文件中的变量，如果要向下层Makefile传递，则需要 __使用`exprot`关键字__ 来声明。

当然，并 __不推荐把许多的变量都定义在系统环境__ 中，这样，在执行不用的Makefile时，拥有的是同一套系统变量，这可能会带来更多的麻烦。

## 目标变量
可以为某个目标设置局部变量，这种变量被称为`Target-specific Variable`，它可以和`全局变量`同名，因为它的作用范围只在这条规则以及连带规则中，所以其值也只在作用范围内有效。而不会影响规则链以外的全局变量的值。

其语法是：

```
<target ...> : <variable-assignment>
<target ...> : override <variable-assignment>
```

`<variable-assignment>`可以是前面讲过的各种赋值表达式，如`=`、`:=`、`+=`或是`?=`。第二个语法是针对于make命令行带入的变量，或是系统环境变量。

这个特性非常的有用，当设置了这样一个变量，这个变量会作用到由这个目标所引发的所有的规则中去

```make
prog : CFLAGS = -g
prog : prog.o foo.o bar.o
    $(CC) $(CFLAGS) prog.o foo.o bar.o

prog.o : prog.c
    $(CC) $(CFLAGS) prog.c

foo.o : foo.c
    $(CC) $(CFLAGS) foo.c

bar.o : bar.c
    $(CC) $(CFLAGS) bar.c
```

在这个示例中，不管全局的`$(CFLAGS)`的值是什么，在`prog`目标，以及其所引发的所有规则中（`prog.o foo.o bar.o`的规则），`$(CFLAGS)`的值都是`-g`

## 模式变量
在GNU的make中，还支持模式变量（Pattern-specific Variable），通过上面的目标变量中，知道，变量可以定义在某个目标上。模式变量的好处就是，可以给定一种模式，可以把变量定义在符合这种模式的所有目标上。

make的模式一般是至少含有一个`%`的，所以，可以以如下方式给所有以`.o`结尾的目标定义目标变量：

```make
%.o : CFLAGS = -O
```

同样，模式变量的语法和`目标变量`一样：

```make
<pattern ...> : <variable-assignment>
<pattern ...> : override <variable-assignment>
```

`override`同样是针对于系统环境传入的变量，或是make命令行指定的变量。

使用条件判断
============

## 示例
```make
libs_for_gcc = -lgnu
normal_libs =

foo: $(objects)
ifeq ($(CC),gcc)
    $(CC) -o foo $(objects) $(libs_for_gcc)
else
    $(CC) -o foo $(objects) $(normal_libs)
endif
```

上面示例的这个规则中，目标`foo`可以根据变量`$(CC)`值来选取不同的函数库来编译程序。可以简化一下

```make
libs_for_gcc = -lgnu
normal_libs =

ifeq ($(CC),gcc)
    libs=$(libs_for_gcc)
else
    libs=$(normal_libs)
endif

foo: $(objects)
    $(CC) -o foo $(objects) $(libs)
```

## 语法

```make
ifeq (<arg1>, <arg2>)
ifneq (<arg1>, <arg2>)
ifdef <variable-name>
ifndef <variable-name>
```

使用函数
==========
## 函数的调用语法
```make
$(<function> <arguments>)
${<function> <arguments>}
```

为了风格的统一，函数和变量的括号最好一样

```make
comma:= ,
empty:=
space:= $(empty) $(empty)
foo:= a b c
bar:= $(subst $(space),$(comma),$(foo))
```

`$(comma)`的值是一个逗号。`$(space)`使用了`$(empty)`定义了一个空格，`$(foo)`的值是`a b c`，`$(bar)`的定义用，调用了函数`subst`，这是一个替换函数，这个函数有三个参数，第一个参数是被替换字串，第二个参数是替换字串，第三个参数是替换操作作用的字串。这个函数也就是把`$(foo)`中的空格替换成逗号，所以`$(bar)`的值是`a,b,c`。

## 字符串处理函数
### subst
```make
$(subst <from>,<to>,<text>)
```

- 名称：字符串替换函数
- 功能：把字串`<text>`中的`<from>`字符串替换成`<to>`。
- 返回：函数返回被替换过后的字符串。
- 示例：

```make
$(subst ee,EE,feet on the street)
#把"feet on the street"中的"ee"替换成"EE"，返回结果是"fEEt on the strEEt"。
```

### patsubst
```make
$(patsubst <pattern>,<replacement>,<text>)
```

- 名称：模式字符串替换函数
- 功能：查找`<text>`中的单词（单词以`空格`、`Tab`或`回车`,`换行`分隔）是否符合模式`<pattern>`，如果匹配的话，则以`<replacement>`替换。这里，`<pattern>`可以包括通配符`%`，表示任意长度的字串。如果`<replacement>`中也包含`%`，那么，`<replacement>`中的这个`%`将是`<pattern>`中的那个`%`所代表的字串。（可以用`/`来转义，以`/%`来表示真实含义的`%`字符）
- 返回：函数返回被替换过后的字符串。
- 示例：

```make
$(patsubst %.c,%.o,x.c.c bar.c)
#把字串"x.c.c bar.c"符合模式[%.c]的单词替换成[%.o]，返回结果是"x.c.o bar.o"

#和前面 变量章节 说过的相关知识有点相似

$(var:<pattern>=<replacement>)
#相当于
$(patsubst <pattern>,<replacement>,$(var))

$(var: <suffix>=<replacement>)
#相当于
$(patsubst %<suffix>,%<replacement>,$(var))

#例如有：
objects = foo.o bar.o baz.o，
#那么
$(objects:.o=.c)
#相当于
$(patsubst %.o,%.c,$(objects))
```

### strip
```make
$(strip <string>)
```

- 名称：去空格函数
- 功能：去掉`<string>`字串中开头和结尾的空字符。
- 返回：返回被去掉空格的字符串值。
- 示例：

```make
$(strip a b c )
#把字串"a b c "去到开头和结尾的空格，结果是"a b c"。
```

### findstring
```make
$(findstring <find>,<in>)
```

- 名称：查找字符串函数
- 功能：在字串`<in>`中查找`<find>`字串。
- 返回：如果找到，那么返回`<find>`，否则返回空字符串。
- 示例：

```make
$(findstring a,a b c)
$(findstring a,b c)
#第一个函数返回"a"字符串，第二个返回""字符串（空字符串）
```

### filter
```make
$(filter <pattern...>,<text>)
```

- 名称：过滤函数
- 功能：以`<pattern>`模式过滤`<text>`字符串中的单词，保留符合模式`<pattern>`的单词。可以有多个模式
- 返回：返回符合模式`<pattern>`的字串。
- 示例：

```make
sources := foo.c bar.c baz.s ugh.h
foo: $(sources)
    cc $(filter %.c %.s,$(sources)) -o foo

#$(filter %.c %.s,$(sources))返回的值是"foo.c bar.c baz.s"。
```

### filter-out
```make
$(filter-out <pattern...>,<text>)
```

- 名称：反过滤函数
- 功能：以`<pattern>`模式过滤`<text>`字符串中的单词，去除符合模式`<pattern>`的单词。可以有多个模式。
- 返回：返回不符合模式`<pattern>`的字串。
- 示例：

```make
objects=main1.o foo.o main2.o bar.o
mains=main1.o main2.o

#$(filter-out $(mains),$(objects)) 返回值是"foo.o bar.o"。
```

### sort
```make
$(sort <list>)
```

- 名称：排序函数
- 功能：给字符串`<list>`中的单词排序（升序）。
- 返回：返回排序后的字符串。函数会去掉`<list>`中相同的单词
- 示例：

```make
$(sort foo bar lose)
#返回"bar foo lose"
```
### word
```make
$(word <n>,<text>)
```

- 名称：取单词函数。
- 功能：取字符串`<text>`中第`<n>`个单词。（从一开始）
- 返回：返回字符串`<text>`中第`<n>`个单词。如果`<n>`比`<text>`中的单词数要大，那么返回空字符串。
- 示例：`$(word 2, foo bar baz)`返回值是"bar"。

### wordlist
```make
$(wordlist <s>,<e>,<text>)
```

- 名称：取单词串函数。
- 功能：从字符串`<text>`中取从`<s>`开始到`<e>`的单词串。`<s>`和`<e>`是一个数字。
- 返回：返回字符串`<text>`中从`<s>`到`<e>`的单词字串。如果`<s>`比`<text>`中的单词数要大，那么返回空字符串。如果`<e>`大于`<text>`的单词数，那么返回从`<s>`开始，到`<text>`结束的单词串。
- 示例： `$(wordlist 2, 3, foo bar baz)`返回值是"bar baz"。

### words
```make
$(words <text>)
```

- 名称：单词个数统计函数。
- 功能：统计`<text>`中字符串中的单词个数。
- 返回：返回`<text>`中的单词数。
- 示例：`$(words, foo bar baz)`返回值是"3"。
- 备注：如果要取`<text>`中最后的一个单词，可以这样：`$(word $(words <text>),<text>)`。

### firstword
```make
$(firstword <text>)
```

- 名称：首单词函数。
- 功能：取字符串`<text>`中的第一个单词。
- 返回：返回字符串`<text>`的第一个单词。
- 示例：`$(firstword foo bar)`返回值是"foo"。
- 备注：这个函数可以用`word`函数来实现：`$(word 1,<text>)`。

### 小结
如果搭配混合使用，可以完成比较复杂的功能。这里，举一个现实中应用的例子。知道，make使用`VPATH`变量来指定"依赖文件"的搜索路径。于是，可以利用这个搜索路径来指定编译器对头文件的搜索路径参数`CFLAGS`，如：

```make
override CFLAGS += $(patsubst %,-I%,$(subst :, ,$(VPATH)))
```

如果`$(VPATH)`值是`src:../headers`，那么`$(patsubst %,-I%,$(subst :, ,$(VPATH)))`将返回`-Isrc -I../headers`，这正是cc或gcc搜索头文件路径的参数。

## 文件名操作函数
下面介绍的函数主要是处理文件名的。每个函数的参数字符串都会被当做一个或是一系列的文件名来对待。

### dir
```make
$(dir <names...>)
```

- 名称：取目录函数
- 功能：从文件名序列`<names>`中取出目录部分。目录部分是指最后一个反斜杠（"/"）之前的部分。如果没有反斜杠，那么返回"./"。
- 返回：返回文件名序列`<names>`的目录部分。
- 示例： `$(dir src/foo.c hacks)`返回值是"src/ ./"。

### notdir
```make
$(notdir <names...>)
```

- 名称：取文件函数
- 功能：从文件名序列`<names>`中取出非目录部分。非目录部分是指最后一个反斜杠（"/"）之后的部分。
- 返回：返回文件名序列`<names>`的非目录部分。
- 示例： `$(notdir src/foo.c hacks)`返回值是"foo.c hacks"。

### suffix
```make
$(suffix <names...>)
```

- 名称：取后缀函数
- 功能：从文件名序列`<names>`中取出各个文件名的后缀。
- 返回：返回文件名序列`<names>`的后缀序列，如果文件没有后缀，则返回空字串。
- 示例：`$(suffix src/foo.c src-1.0/bar.c hacks)`返回值是".c .c"。

### basename
```make
$(basename <names...>)
```

- 名称：取前缀函数
- 功能：从文件名序列`<names>`中取出各个文件名的前缀部分。
- 返回：返回文件名序列`<names>`的前缀序列，如果文件没有前缀，则返回空字串。
- 示例：`$(basename src/foo.c src-1.0/bar.c hacks)`返回值是"src/foo src-1.0/bar hacks"。

### addsuffix
```make
$(addsuffix <suffix>,<names...>)
```

- 名称：加后缀函数
- 功能：把后缀`<suffix>`加到`<names>`中的每个单词后面。
- 返回：返回加过后缀的文件名序列。
- 示例：`$(addsuffix .c,foo bar)`返回值是"foo.c bar.c"。

### addprefix
```make
$(addprefix <prefix>,<names...>)
```

- 名称：加前缀函数
- 功能：把前缀`<prefix>`加到`<names>`中的每个单词后面。
- 返回：返回加过前缀的文件名序列。
- 示例：`$(addprefix src/,foo bar)`返回值是"src/foo src/bar"。

### join
```make
$(join <list1>,<list2>)
```

- 名称：连接函数
- 功能：把`<list2>`中的单词对应地加到`<list1>`的单词后面。如果`<list1>`的单词个数要比`<list2>`的多，那么，`<list1>`中的多出来的单词将保持原样。如果`<list2>`的单词个数要比`<list1>`多，那么，`<list2>`多出来的单词将被复制到`<list2>`中。
- 返回：返回连接过后的字符串。
- 示例：`$(join aaa bbb , 111 222 333)`返回值是"aaa111 bbb222 333"。

## foreach 函数
```make
$(foreach <var>,<list>,<text>)
```

把参数`<list>`中的单词逐一取出放到参数`<var>`所指定的变量中，然后再执行`<text>`所包含的表达式。每一次`<text>`会返回一个字符串，循环过程中，`<text>`的所返回的每个字符串会以空格分隔，最后当整个循环结束时，`<text>`所返回的每个字符串所组成的整个字符串（以空格分隔）将会是`foreach`函数的返回值。

```make
names := a b c d
files := $(foreach n,$(names),$(n).o)
#值是a.o b.o c.o d.o
```

`foreach`中的`<var>`参数是一个临时的局部变量，`foreach`函数执行完后，参数`<var>`的变量将不在作用，其作用域只在`foreach`函数当中。

## if 函数
```make
$(if <condition>,<then-part>)
$(if <condition>,<then-part>,<else-part>)
```

## call 函数
`call`函数是唯一一个可以用来创建新的参数化的函数。可以写一个非常复杂的表达式，这个表达式中，可以定义许多参数，然后可以用`call`函数来向这个表达式传递参数。其语法是：

```make
$(call <expression>,<parm1>,<parm2>,<parm3>...)
```

当make执行这个函数时，`<expression>`参数中的变量，如`$(1)`，`$(2)`，`$(3)`等，会被参数`<parm1>`，`<parm2>`，`<parm3>`依次取代。而`<expression>`的返回值就是`call`函数的返回值

```make
reverse =  $(2) $(1)
foo = $(call reverse,a,b)
```

## origin函数
他并不操作变量的值，他只是告诉这个变量是哪里来的？其语法是

```make
$(origin <variable>)
```

注意，`<variable>`是变量的名字，不应该是引用。所以最好不要在`<variable>`中使用`$`字符。`origin`函数会以其返回值来告诉这个变量的`出生情况`，下面，是`origin`函数的返回值:

- `undefined` 如果`<variable>`从来没有定义过，函数返回这个值`undefined`
- `default` 如果`<variable>`是一个默认的定义，比如`CC`这个变量，这种变量将在后面讲述
- `environment` 如果`<variable>`是一个环境变量，并且当Makefile被执行时，`-e`参数没有被打开
- `file` 如果`<variable>`这个变量被定义在Makefile中
- `command line` 如果`<variable>`这个变量是被命令行定义的
- `override` 如果`<variable>`是被`override`指示符重新定义的
- `automatic` 如果`<variable>`是一个命令运行中的自动化变量。关于自动化变量将在后面讲述

这些信息对于编写Makefile是非常有用的，例如，假设有一个Makefile其包了一个定义文件`Make.def`，在`Make.def`中定义了一个变量`bletch`，而环境中也有一个环境变量`bletch`，此时，想判断一下，如果变量来源于环境，那么就把之重定义了，如果来源`于Make.def`或是命令行等非环境的，那么就不重新定义它。于是，在Makefile中，可以这样写：

```make
ifdef bletch
    ifeq "$(origin bletch)" "environment"
        bletch = barf, gag, etc.
    endif
endif
```

是的，用`override`是可以达到这样的效果，可是`override`过于粗暴，它同时会把从命令行定义的变量也覆盖了，而只想重新定义环境传来的，而不想重新定义命令行传来的。

## shell函数
它的参数应该就是操作系统Shell的命令。它和<code>反引号｀</code>是相同的功能。这就是说，shell函数把执行操作系统命令后的输出作为函数返回。于是，可以用操作系统命令以及字符串处理命令`awk`，`sed`等等命令来生成一个变量，如：

```make
contents := $(shell cat foo)
files := $(shell echo *.c)
```

这个函数会新生成一个Shell程序来执行命令，所以要注意其运行性能，如果Makefile中有一些比较复杂的规则，并大量使用了这个函数，那么对于系统性能是有害的。特别是Makefile的隐晦的规则可能会让shell函数执行的次数比想像的多得多。

## 控制make的函数
```make
$(error <text ...>)
```

产生一个致命的错误，`<text ...>`是错误信息。注意，`error`函数不会在一被使用就会产生错误信息，所以如果把其定义在某个变量中，并在后续的脚本中使用这个变量，那么也是可以的。例如：

```make
ifdef ERROR_001
    $(error error is $(ERROR_001))
endif

ERR = $(error found an error!)
.PHONY: err
err: ; $(ERR)
```

示例一会在变量`ERROR_001`定义了后执行时产生`error`调用，而示例二则在目录`err`被执行时才发生`error`调用。

```make
$(warning <text ...>)
```

这个函数很像`error`函数，只是它并不会让make退出，只是输出一段警告信息，而make继续执行。

make的运行
===========
一般来说，最简单的就是直接在命令行下输入make命令，make命令会找当前目录的makefile来执行，一切都是自动的。但也有时也许只想让make重编译某些文件，而不是整个工程，而又有的时候有几套编译规则，想在不同的时候使用不同的编译规则，等等。本章节就是讲述如何使用make命令的。

## make的退出码
- 0 —— 表示成功执行。
- 1 —— 如果make运行时出现任何错误，其返回1。
- 2 —— 如果使用了make的`-q`选项，并且make使得一些目标不需要更新，那么返回2。

## 指定Makefile
GNU make找寻默认的Makefile的规则是在当前目录下依次找三个文件——`GNUmakefile`、`makefile`和`Makefile`。其按顺序找这三个文件，一旦找到，就开始读取这个文件并执行。

当前，也可以给make命令指定一个特殊名字的Makefile。要达到这个功能，要使用make的`-f`或是`--file`参数（`--makefile`参数也行）。例如，有个makefile的名字是`aaa.mk`，那么，可以这样来让make来执行这个文件：

```shell
make –f aaa.mk
```
如果在make的命令行是，不只一次地使用了`-f`参数，那么，所有指定的makefile将会被连在一起传递给make执行。

## 指定目标
一般来说，make的最终目标是makefile中的第一个目标，而其它目标一般是由这个目标连带出来的。这是make的默认行为。当然，一般来说，makefile中的第一个目标是由许多个目标组成，可以指示make，让其完成所指定的目标。

任何在makefile中的目标都可以被指定成终极目标，但是除了以`-`打头，或是包含了`=`的目标，因为有这些字符的目标，会被解析成`命令行参数`或是`变量`。甚至没有被明确写出来的目标也可以成为make的终极目标，也就是说，只要make可以找到其隐含规则推导规则，那么这个隐含目标同样可以被指定成终极目标。

有一个make的环境变量叫`MAKECMDGOALS`，这个变量中会 __存放所指定的终极目标的列表__，如果在命令行上，没有指定目标，那么，这个变量是空值。这个变量可以让使用在一些比较特殊的情形下。比如下面的例子：


```make
sources = foo.c bar.c
ifneq ( $(MAKECMDGOALS),clean)
    include $(sources:.c=.d)
endif
```

只要输入的命令不是`make clean`，那么makefile会自动包含`foo.d`和`bar.d`这两个makefile。

即然make可以指定所有makefile中的目标，那么也包括`伪目标`，于是可以根据这种性质来让makefile根据指定的不同的目标来完成不同的事。在Unix世界中，软件发布时，特别是GNU这种开源软件的发布时，其makefile都包含了编译、安装、打包等功能。可以参照这种规则来书写makefile中的目标。

- `all`这个伪目标是所有目标的目标，其功能一般是编译所有的目标。
- `clean`这个伪目标功能是删除所有被make创建的文件。
- `install`这个伪目标功能是安装已编译好的程序，其实就是把目标执行文件拷贝到指定的目标中去。
- `print`这个伪目标的功能是例出改变过的源文件。
- `tar`这个伪目标功能是把源程序打包备份。也就是一个tar文件。
- `dist`这个伪目标功能是创建一个压缩文件，一般是把tar文件压成Z文件。或是gz文件。
- `TAGS`这个伪目标功能是更新所有的目标，以备完整地重编译使用。
- `check`和`test`这两个伪目标一般用来测试makefile的流程。

## 检查规则
有时候，不想让makefile中的规则执行起来，只想检查一下命令，或是执行的序列。于是可以使用make命令的下述参数：

```shell
-n
--just-print
--dry-run
--recon
```

不执行参数，这些参数只是打印命令，不管目标是否更新，把规则和连带规则下的命令打印出来，但不执行，这些参数对于调试makefile很有用处。

```shell
-t
--touch
```

这个参数的意思就是把目标文件的时间更新，但不更改目标文件。也就是说，make假装编译目标，但不是真正的编译目标，只是把目标变成已编译过的状态。

```shell
-q
--question
```

这个参数的行为是找目标的意思，也就是说，如果目标存在，那么其什么也不会输出，当然也不会执行编译，如果目标不存在，其会打印出一条出错信息。

```shell
-W <file>
--what-if=<file>
--assume-new=<file>
--new-file=<file>
```

这个参数需要指定一个文件。一般是是源文件（或依赖文件），Make会根据规则推导来运行依赖于这个文件的命令，一般来说，可以和`-n`参数一同使用，来查看这个依赖文件所发生的规则命令。

另外一个很有意思的用法是结合`-p`和`-v`来输出makefile被执行时的信息

## make参数
下面列举了所有GNU make 3.80版的参数定义。其它版本和产商的make大同小异，不过其它产商的make的具体参数还是请参考各自的产品文档。

```shell
-b
-m
```

这两个参数的作用是忽略和其它版本make的兼容性。

```shell
-B
--always-make
```

认为所有的目标都需要更新（重编译）。

```shell
-C <dir>
--directory=<dir>
```

指定读取makefile的目录。如果有多个`-C`参数，make的解释是后面的路径以前面的作为相对路径，并以最后的目录作为被指定目录。如：`make –C ~zyx/test –C prog`等价于`make –C ~zyx/test/prog`。

```shell
`—debug[=<options>]`
```

输出make的调试信息。它有几种不同的级别可供选择，如果没有参数，那就是输出最简单的调试信息。下面是<options>的取值：

- `a` —— 也就是all，输出所有的调试信息。（会非常的多）
- `b` —— 也就是basic，只输出简单的调试信息。即输出不需要重编译的目标。
- `v` —— 也就是verbose，在b选项的级别之上。输出的信息包括哪个makefile被解析，不需要被重编译的依赖文件（或是依赖目标）等。
- `i` —— 也就是implicit，输出所以的隐含规则。
- `j` —— 也就是jobs，输出执行规则中命令的详细信息，如命令的PID、返回码等。
- `m` —— 也就是makefile，输出make读取makefile，更新makefile，执行makefile的信息。

```shell
-d
```

相当于`--debug=a`。

```shell
-e
--environment-overrides
```

指明环境变量的值覆盖makefile中定义的变量的值。

```shell
-f=<file>
--file=<file>
--makefile=<file>
```

指定需要执行的makefile。

```shell
-h
--help
```

显示帮助信息。

```shell
-i
--ignore-errors
```

在执行时忽略所有的错误。

```shell
-I <dir>
--include-dir=<dir>
```

指定一个被包含makefile的搜索目标。可以使用多个`-I`参数来指定多个目录。

```shell
-j [<jobsnum>]
--jobs[=<jobsnum>]
```

指同时运行命令的个数。如果没有这个参数，make运行命令时能运行多少就运行多少。如果有一个以上的`-j`参数，那么仅最后一个`-j`才是有效的。（注意这个参数在MS-DOS中是无用的）

```shell
-k
--keep-going
```

出错也不停止运行。如果生成一个目标失败了，那么依赖于其上的目标就不会被执行了。

```shell
-l <load>
--load-average[=<load]
—max-load[=<load>]
```

指定make运行命令的负载。

```shell
-n
--just-print
--dry-run
--recon
```

仅输出执行过程中的命令序列，但并不执行。

```shell
-o <file>
--old-file=<file>
--assume-old=<file>
```

不重新生成的指定的<file>，即使这个目标的依赖文件新于它。

```shell
-p
--print-data-base
```

输出makefile中的所有数据，包括所有的规则和变量。这个参数会让一个简单的makefile都会输出一堆信息。如果只是想输出信息而不想执行makefile，可以使用`make -qp`命令。如果想查看执行makefile前的预设变量和规则，可以使用`make –p –f /dev/null`。这个参数输出的信息会包含着makefile文件的文件名和行号，所以，用这个参数来调试makefile会是很有用的，特别是当环境变量很复杂的时候。

```shell
-q
--question
```

不运行命令，也不输出。仅仅是检查所指定的目标是否需要更新。如果是0则说明要更新，如果是2则说明有错误发生。

```shell
-r
--no-builtin-rules
```

禁止make使用任何隐含规则。

```shell
-R
--no-builtin-variabes
```

禁止make使用任何作用于变量上的隐含规则。

```shell
-s
--silent
--quiet
```

在命令运行时不输出命令的输出。

```shell
-S
--no-keep-going
--stop
```

取消`-k`选项的作用。因为有些时候，make的选项是从环境变量`MAKEFLAGS`中继承下来的。所以可以在命令行中使用这个参数来让环境变量中的`-k`选项失效。

```shell
-t
--touch
```

相当于UNIX的touch命令，只是把目标的修改日期变成最新的，也就是阻止生成目标的命令运行。

```shell
-v
--version
```

输出make程序的版本、版权等关于make的信息。

```shell
-w
--print-directory
```

输出运行makefile之前和之后的信息。这个参数对于跟踪嵌套式调用make时很有用。

```shell
--no-print-directory
```

禁止`-w`选项。

```shell
-W <file>
--what-if=<file>
--new-file=<file>
--assume-file=<file>
```

假定目标`<file>`需要更新，如果和`-n`选项使用，那么这个参数会输出该目标更新时的运行动作。如果没有`-n`那么就像运行UNIX的`touch`命令一样，使得`<file>`的修改时间为当前时间。

```shell
--warn-undefined-variables
```

只要make发现有未定义的变量，那么就输出警告信息。

隐含规则
========
`隐含规则`也就是一种惯例，make会按照这种`惯例`心照不喧地来运行，那怕Makefile中没有书写这样的规则。例如，把[.c]文件编译成[.o]文件这一规则，根本就不用写出来，make会自动推导出这种规则，并生成需要的[.o]文件。

`隐含规则`会使用一些系统变量，可以改变这些系统变量的值来定制隐含规则的运行时的参数。如系统变量`CFLAGS`可以控制编译时的编译器参数。

## 使用隐含规则
如果要使用隐含规则生成需要的目标，所需要做的就是不要写出这个目标的规则。那么，make会试图去自动推导产生这个目标的规则和命令，如果make可以自动推导生成这个目标的规则和命令，那么这个行为就是隐含规则的自动推导。当然，隐含规则是make事先约定好的一些东西。例如，有下面的一个Makefile：

```make
foo : foo.o bar.o
    cc –o foo foo.o bar.o $(CFLAGS) $(LDFLAGS)
```

这个Makefile中并没有写下如何生成`foo.o`和`bar.o`这两目标的规则和命令。因为make的`隐含规则`功能会自动为自动去推导这两个目标的依赖目标和生成命令。

在上面的那个例子中，make调用的隐含规则是，把[.o]的目标的依赖文件置成[.c]，并使用C的编译命令`cc –c $(CFLAGS) [.c]`来生成[.o]的目标。也就是说，完全没有必要写下下面的两条规则：

```make
foo.o : foo.c
    cc –c foo.c $(CFLAGS)
bar.o : bar.c
    cc –c bar.c $(CFLAGS)
```

在make的`隐含规则库`中，每一条隐含规则都在库中有其顺序，越靠前的则是越被经常使用的，所以，这会导致有些时候即使显示地指定了目标依赖，make也不会管。如下面这条规则（没有命令）：

```make
foo.o : foo.p
```

依赖文件`foo.p`（Pascal程序的源文件）有可能变得没有意义。如果目录下存在了`foo.c`文件，那么隐含规则一样会生效，并会通过`foo.c`调用C的编译器生成`foo.o`文件。因为，在隐含规则中，Pascal的规则出现在C的规则之后，所以，make找到可以生成`foo.o`的C的规则就不再寻找下一条规则了。如果确实不希望任何隐含规则推导，那么，就不要只写出`依赖规则`，而不写命令。

## 隐含规则一览
如果不明确地写下规则，那么，make就会在这些规则中寻找所需要规则和命令。当然，也可以使用make的参数`-r`或`--no-builtin-rules`选项来取消所有的预设置的隐含规则。

即使是指定了`-r`参数，某些隐含规则还是会生效，因为有许多的隐含规则都是使用了`后缀规则`来定义的，所以，只要隐含规则中有`后缀列表`（也就一系统定义在目标`.SUFFIXES`的依赖目标），那么隐含规则就会生效。默认的后缀列表是：`.out`, `.a`, `.ln`, `.o`, `.c`, `.cc`, `.C`, `.p`, `.f`, `.F`, `.r`, `.y`, `.l`, `.s`, `.S`, `.mod`, `.sym`, `.def`, `.h`, `.info`, `.dvi`, `.tex`, `.texinfo`, `.texi`, `.txinfo`, `.w`, `.ch .web`, `.sh`, `.elc`, `.el`

- 编译C程序的隐含规则

`<n>.o`的目标的依赖目标会自动推导为`<n>.c`，并且其生成命令是`$(CC) –c $(CPPFLAGS) $(CFLAGS)`

- 编译C++程序的隐含规则

`<n>.o`的目标的依赖目标会自动推导为`<n>.cc`或是`<n>.C`，并且其生成命令是`$(CXX) –c $(CPPFLAGS) $(CFLAGS)`。（建议使用`.cc`作为C++源文件的后缀，而不是`.C`）

- 汇编和汇编预处理的隐含规则

`<n>.o` 的目标的依赖目标会自动推导为`<n>.s`，默认使用编译品`as`，并且其生成命令是：`$(AS) $(ASFLAGS)`。`<n>.s` 的目标的依赖目标会自动推导为`<n>.S`，默认使用C预编译器`cpp`，并且其生成命令是：`$(AS) $(ASFLAGS)`。

- 链接Object文件的隐含规则

`<n>`目标依赖于`<n>.o`，通过运行C的编译器来运行链接程序生成（一般是`ld`），其生成命令是：`$(CC) $(LDFLAGS) <n>.o $(LOADLIBES) $(LDLIBS)`。这个规则对于只有一个源文件的工程有效，同时也对多个Object文件（由不同的源文件生成）的也有效。例如如下规则：

```make
x : y.o z.o
```

并且`x.c`、`y.c`和`z.c`都存在时，隐含规则将执行如下命令：

```make
cc -c x.c -o x.o
cc -c y.c -o y.o
cc -c z.c -o z.o
cc x.o y.o z.o -o x
rm -f x.o
rm -f y.o
rm -f z.o
```

如果没有一个源文件（如上例中的`x.c`）和目标名字（如上例中的`x`）相关联，那么，最好写出自己的生成规则，不然，隐含规则会报错的。

- Yacc C程序时的隐含规则

`<n>.c`的依赖文件被自动推导为`n.y`（Yacc生成的文件），其生成命令是：`$(YACC) $(YFALGS)`

- Lex C程序时的隐含规则

`<n>.c`的依赖文件被自动推导为`n.l`（Lex生成的文件），其生成命令是：`$(LEX) $(LFALGS)`

- Lex Ratfor程序时的隐含规则

`<n>.r`的依赖文件被自动推导为`n.l`（Lex生成的文件），其生成命令是：`$(LEX) $(LFALGS)`

- 从C程序、Yacc文件或Lex文件创建Lint库的隐含规则

`<n>.ln` （lint生成的文件）的依赖文件被自动推导为`n.c`，其生成命令是：`$(LINT) $(LINTFALGS) $(CPPFLAGS) -i`。对于`<n>.y`和`<n>.l`也是同样的规则。

## 隐含规则使用的变量
在隐含规则中的命令中，基本上都是使用了一些预先设置的变量。可以在makefile中改变这些变量的值，或是在make的命令行中传入这些值，或是在环境变量中设置这些值，无论怎么样，只要设置了这些特定的变量，那么其就会对隐含规则起作用。当然，也可以利用make的`-R`或`--no–builtin-variables`参数来取消所定义的变量对隐含规则的作用。

### 关于命令的变量
- `AR`函数库打包程序 默认命令是`ar`
- `AS`汇编语言编译程序 默认命令是`as`
- `CC`C语言编译程序 默认命令是`cc`
- `CXX`C++语言编译程序 默认命令是`g++`
- `CO`从 RCS文件中扩展文件程序 默认命令是`co`
- `CPP`C程序的预处理器（输出是标准输出设备） 默认命令是`$(CC) –E`
- `FC`Fortran 和 Ratfor 的编译器和预处理程序 默认命令是`f77`
- `GET`从SCCS文件中扩展文件的程序 默认命令是`get`
- `LEX`Lex方法分析器程序（针对于C或Ratfor） 默认命令是`lex`
- `PC`Pascal语言编译程序 默认命令是`pc`
- `YACC`Yacc文法分析器（针对于C程序） 默认命令是`yacc`
- `YACCR`Yacc文法分析器（针对于Ratfor程序） 默认命令是`yacc –r`
- `MAKEINFO`转换Texinfo源文件（.texi）到Info文件程序 默认命令是`makeinfo`
- `TEX`从TeX源文件创建TeX DVI文件的程序 默认命令是`tex`
- `TEXI2DVI`从Texinfo源文件创建军TeX DVI 文件的程序 默认命令是`texi2dvi`
- `WEAVE`转换Web到TeX的程序 默认命令是`weave`
- `CWEAVE`转换C Web 到 TeX的程序 默认命令是`cweave`
- `TANGLE`转换Web到Pascal语言的程序 默认命令是`tangle`
- `CTANGLE`转换C Web 到 C 默认命令是`ctangle`
- `RM`删除文件命令 默认命令是`rm –f`

### 关于命令参数的变量
下面的这些变量都是相关上面的命令的参数。如果没有指明其默认值，那么其默认值都是空

- `ARFLAGS`函数库打包程序AR命令的参数。默认值是`rv`。
- `ASFLAGS`汇编语言编译器参数。（当明显地调用`.s`或`.S`文件时）。
- `CFLAGS`C语言编译器参数。
- `CXXFLAGS`C++语言编译器参数。
- `COFLAGS`RCS命令参数。
- `CPPFLAGS`C预处理器参数。（ C 和 Fortran 编译器也会用到）。
- `FFLAGS`Fortran语言编译器参数。
- `GFLAGS`SCCS `get`程序参数。
- `LDFLAGS`链接器参数。（如：`ld`）
- `LFLAGS`Lex文法分析器参数。
- `PFLAGS`Pascal语言编译器参数。
- `RFLAGS`Ratfor 程序的Fortran 编译器参数。
- `YFLAGS`Yacc文法分析器参数。

### 隐含规则链
有些时候，一个目标可能被一系列的隐含规则所作用。例如，一个[.o]的文件生成，可能会是先被Yacc的[.y]文件先成[.c]，然后再被C的编译器生成。把这一系列的隐含规则叫做`隐含规则链`。

在上面的例子中，如果文件[.c]存在，那么就直接调用C的编译器的隐含规则，如果没有[.c]文件，但有一个[.y]文件，那么Yacc的隐含规则会被调用，生成[.c]文件，然后，再调用C编译的隐含规则最终由[.c]生成[.o]文件，达到目标。

把这种[.c]的文件（或是目标），叫做中间目标。不管怎么样，make会努力自动推导生成目标的一切方法，不管中间目标有多少，其都会执着地把所有的隐含规则和书写的规则全部合起来分析，努力达到目标，所以，有些时候，可能会让觉得奇怪，怎么我的目标会这样生成？怎么我的makefile发疯了？

在默认情况下，对于中间目标，它和一般的目标有两个地方所不同：第一个不同是除非中间的目标不存在，才会引发中间规则。第二个不同的是，只要目标成功产生，那么，产生最终目标过程中，所产生的中间目标文件会被以`rm -f`删除。

通常，一个被makefile指定成目标或是依赖目标的文件不能被当作中介。然而，可以明显地说明一个文件或是目标是中介目标，可以使用伪目标`.INTERMEDIATE`来强制声明。（如`.INTERMEDIATE : mid` ）

也可以阻止make自动删除中间目标，要做到这一点，可以使用伪目标`.SECONDARY`来强制声明（如：`.SECONDARY : sec`）。还可以把目标，以模式的方式来指定（如：`%.o`）成伪目标`.PRECIOUS`的依赖目标，以保存被隐含规则所生成的中间文件。

在`隐含规则链`中，禁止同一个目标出现两次或两次以上，这样一来，就可防止在make自动推导时出现无限递归的情况。

Make会优化一些特殊的隐含规则，而不生成中间文件。如，从文件`foo.c`生成目标程序`foo`，按道理，make会编译生成中间文件`foo.o`，然后链接成`foo`，但在实际情况下，这一动作可以被一条`cc`的命令完成（cc –o foo foo.c），于是优化过的规则就不会生成中间文件。


定义模式规则
============
## 模式规则介绍
模式规则中，至少在规则的目标定义中要包含`%`，否则，就是一般的规则。目标中的`%`定义表示对文件名的匹配，`%`表示长度任意的非空字符串。例如：`%.c`表示以`.c`结尾的文件名（文件名的长度至少为3），而`s.%.c`则表示以`s.`开头，`.c`结尾的文件名（文件名的长度至少为5）

如果`%`定义在目标中，那么，目标中的`%`的值决定了依赖目标中的`%`的值，也就是说，目标中的模式的`%`决定了依赖目标中`%`的样子

```make
%.o : %.c
    <command ......>
```

指出了怎么从所有的[.c]文件生成相应的[.o]文件的规则。如果要生成的目标是`a.o b.o`，那么`%c`就是`a.c b.c`。

一旦依赖目标中的`%`模式被确定，那么，make会被要求去匹配 __当前目录__ 下所有的文件名，一旦找到，make就会规则下的命令，所以，在模式规则中，目标可能会是多个的，如果有模式匹配出多个目标，make就会产生所有的模式目标，此时，make关心的是依赖的文件名和生成目标的命令这两件事。


### 自动化变量
- `$@` 表示规则中的目标文件集。在模式规则中，如果有多个目标，那么，`$@`就是匹配于目标中模式定义的 __集合__。

- `$%` 仅当目标是函数库文件中，表示规则中的目标成员名。例如，如果一个目标是`foo.a(bar.o)`，那么，`$%`就是`bar.o`，`$@`就是`foo.a`。如果目标不是函数库文件（Unix下是[.a]，Windows下是[.lib]），那么，其值为空。

- `$<` 依赖目标中的 __第一个目标__ 名字。如果依赖目标是以模式（即`%`）定义的，那么`$<`将是符合模式的一系列的文件集。注意，其是一个一个取出来的。

对于`$<`，为了避免产生不必要的麻烦，最好给`$`后面的那个特定字符都加上圆括号，比如，`$(<)`就要比`$<`要好一些。

- `$?` 所有比目标新的依赖目标的集合。以空格分隔。

当希望只对更新过的依赖文件进行操作时，`$?`在显式规则中很有用，例如，假设有一个函数库文件叫`lib`，其由其它几个object文件更新。那么把object文件打包的比较有效率的Makefile规则是：

```make
lib : foo.o bar.o lose.o win.o
    ar r lib $?
```

- `$^` 所有的依赖目标的集合。以空格分隔。如果在依赖目标中有多个重复的，那个这个变量会去除重复的依赖目标，只保留一份。

- `$+` 这个变量很像`$^`，也是所有依赖目标的集合。只是它不去除重复的依赖目标。

- `$*` 这个变量表示目标模式中`%`及其之前的部分。

如果目标是`dir/a.foo.b`，并且目标的模式是`a.%.b`，那么，`$*`的值就是`dir/a.foo`。这个变量对于构造有关联的文件名是比较有较。如果目标中没有模式的定义，那么`$*`也就不能被推导出，但是，如果目标文件的后缀是make所识别的，那么`$*`就是除了后缀的那一部分。例如：如果目标是`foo.c`，因为`.c`是make所能识别的后缀名，所以，`$*`的值就是`foo`。这个特性是GNU make的，很有可能不兼容于其它版本的make，所以，应该尽量避免使用`$*`，除非是在隐含规则或是静态模式中。如果目标中的后缀是make所不能识别的，那么`$*`就是空值。


在上述所列出来的自动量变量中。四个变量（`$@`、`$<`、`$%`、`$*`）在扩展时只会有一个文件，而另三个的值是一个文件列表。这七个自动化变量还可以取得文件的目录名或是在当前目录下的符合模式的文件名，只需要搭配上`D`或`F`字样。这是GNU make中老版本的特性，在新版本中，使用函数`dir`或`notdir`就可以做到了。`D`的含义就是Directory，就是目录，`F`的含义就是File，就是文件。

下面是对于上面的七个变量分别加上`D`或是`F`的含义：

- `$(@D)` 表示`$@`的目录部分（不以斜杠作为结尾），如果`$@`值是`dir/foo.o`，那么`$(@D)`就是`dir`，而如果`$@`中没有包含斜杠的话，其值就是`.`（当前目录）。

- `$(@F)` 表示`$@`的文件部分，如果`$@`值是`dir/foo.o`，那么`$(@F)`就是`foo.o`，`$(@F)`相当于函数`$(notdir $@)`。

- `$(*D)`/`$(*F)` 和上面所述的同理，也是取文件的目录部分和文件部分。对于上面的那个例子，`$(*D)`返回`dir`，而`$(*F)`返回`foo`

- `$(%D)`/`$(%F)` 分别表示了函数包文件成员的目录部分和文件部分。这对于形同`archive(member)`形式的目标中的`member`中包含了不同的目录很有用。

- `$(<D)`/`$(<F)` 分别表示依赖文件的目录部分和文件部分。

- `$(^D)`/`$(^F)` 分别表示所有依赖文件的目录部分和文件部分。（无相同的）

- `$(+D)`/`$(+F)` 分别表示所有依赖文件的目录部分和文件部分。（可以有相同的）

- `$(?D)`/`$(?F)` 分别表示被更新的依赖文件的目录部分和文件部分。

还得要注意的是，这些变量只使用在规则的命令中，而且一般都是`显式规则`和`静态模式规则`

## 模式的匹配
一般来说，一个目标的模式有一个有前缀或是后缀的`%`，或是没有前后缀，直接就是一个`%`。因为`%`代表一个或多个字符，所以在定义好了的模式中，把`%`所匹配的内容叫做`茎`，例如`%.c`所匹配的文件`test.c`中`test`就是`茎`。因为在目标和依赖目标中同时有`%`时，依赖目标的`茎`会传给目标，当做目标中的`茎`。

当一个模式匹配包含有斜杠（实际也不经常包含）的文件时，那么在进行模式匹配时，目录部分会首先被移开，然后进行匹配，成功后，再把目录加回去。在进行`茎`的传递时，需要知道这个步骤。例如有一个模式`e%t`，文件`src/eat`匹配于该模式，于是`src/a`就是其`茎`，如果这个模式定义在依赖目标中，而被依赖于这个模式的目标中又有个模式`c%r`，那么，目标就是`src/car`。（`茎`被传递）

## 重载内建隐含规则
可以重载内建的隐含规则（或是定义一个全新的），例如可以重新构造和内建隐含规则不同的命令，如：

```make
%.o : %.c
    $(CC) -c $(CPPFLAGS) $(CFLAGS) -D$(date)
```

可以取消内建的隐含规则，只要不在后面写命令就行。如：

```make
%.o : %.s
```

同样，可以重新定义一个全新的隐含规则，其在隐含规则中的位置取决于在哪里写下这个规则。朝前的位置就靠前。

## 隐含规则搜索算法
比如我们有一个目标叫 `T`。下面是搜索目标`T`的规则的算法。请注意，在下面，我们没有提到后缀规则，原因是，所有的后缀规则在Makefile被载入内存时，会被转换成模式规则。如果目标是`archive(member)`的函数库文件模式，那么这个算法会被运行两次，第一次是找目标`T`，如果没有找到的话，那么进入第二次，第二次会把`member`当作`T`来搜索。

1. 把`T`的目录部分分离出来。叫`D`，而剩余部分叫`N`。（如：如果`T`是`src/foo.o`，那么，`D`就是`src/`，`N`就是`foo.o`）
1. 创建所有匹配于`T`或是`N`的模式规则列表。
1. 如果在模式规则列表中有匹配所有文件的模式，如`%`，那么从列表中移除其它的模式。
1. 移除列表中没有命令的规则。
1. 对于第一个在列表中的模式规则：
    1. 推导其`茎`(`S`)，`S`应该是`T`或是`N`匹配于模式中`%`非空的部分。
    1. 计算依赖文件。把依赖文件中的`%`都替换成`茎`(`S`)。如果目标模式中没有包含斜框字符，而把`D`加在第一个依赖文件的开头。
    1. 测试是否所有的依赖文件都存在或是理当存在。（如果有一个文件被定义成另外一个规则的目标文件，或者是一个显式规则的依赖文件，那么这个文件就叫`理当存在`）
    1. 如果所有的依赖文件存在或是理当存在，或是就没有依赖文件。那么这条规则将被采用，退出该算法。
1. 如果经过第5步，没有模式规则被找到，那么就做更进一步的搜索。对于存在于列表中的第一个模式规则：
    1. 如果规则是终止规则，那就忽略它，继续下一条模式规则。
    1. 计算依赖文件。（同第5步）
    1. 测试所有的依赖文件是否存在或是理当存在。
    1. 对于不存在的依赖文件，递归调用这个算法查找他是否可以被隐含规则找到。
    1. 如果所有的依赖文件存在或是理当存在，或是就根本没有依赖文件。那么这条规则被采用，退出该算法。
1. 如果没有隐含规则可以使用，查看`.DEFAULT`规则，如果有，采用，把`.DEFAULT`的命令给`T`使用。

一旦规则被找到，就会执行其相当的命令，而此时，我们的自动化变量的值才会生成。

使用make更新函数库
================
函数库文件也就是对Object文件（程序编译的中间文件）的打包文件。在Unix下，一般是由命令`ar`来完成打包工作。

## 函数库文件的成员
一个函数库文件由多个文件组成。你可以以如下格式指定函数库文件及其组成：

```make
archive(member)
```

这个不是一个命令，而一个目标和依赖的定义。一般来说，这种用法基本上就是为了`ar`命令来服务的。如：

```make
foolib(hack.o) : hack.o
    ar cr foolib hack.o
```

如果要指定多个member，那就以空格分开，如：

```make
foolib(hack.o kludge.o)
```

其等价于：

```make
foolib(hack.o) foolib(kludge.o)
```

你还可以使用Shell的文件通配符来定义，如：

```make
foolib(*.o)
```

## 函数库成员的隐含规则
当make搜索一个目标的隐含规则时，一个特殊的特性是，如果这个目标是`a(m)`形式的，其会把目标变成`(m)`。于是，如果我们的成员是`%.o`的模式定义，并且如果我们使用`make 'foo.a(bar.o)'`的形式调用Makefile时，隐含规则会去找`bar.o`的规则，如果没有定义`bar.o`的规则，那么内建隐含规则生效，make会去找`bar.c`文件来生成`bar.o`，如果找得到的话，make执行的命令大致如下：

```shell
cc -c bar.c -o bar.o
ar r foo.a bar.o
rm -f bar.o
```

## 函数库文件的后缀规则
可以使用`后缀规则`和`隐含规则`来生成函数库打包文件，如：

```make
.c.a:
    $(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $*.o
    $(AR) r $@ $*.o
    $(RM) $*.o
```

其等效于：

```make
(%.o) : %.c
    $(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $*.o
    $(AR) r $@ $*.o
    $(RM) $*.o
```

> 在进行函数库打包文件生成时，请小心使用make的并行机制（`-j`参数）。如果多个ar命令在同一时间运行在同一个函数库打包文件上，就很有可以损坏这个函数库文件。所以，在make未来的版本中，应该提供一种机制来避免并行操作发生在函数打包文件上。


<!-- http://blog.csdn.net/haoel/article/details/2897 -->
