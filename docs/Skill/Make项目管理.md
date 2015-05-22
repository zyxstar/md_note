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
- `隐晦规则`由于我们的make有自动推导的功能，所以隐晦的规则可以让我们比较粗糙地简略地书写Makefile，这是由make所支持的。
- `变量的定义`在Makefile中我们要定义一系列的变量，变量一般都是字符串，这个有点C语言中的宏，当Makefile被执行时，其中的变量都会被扩展到相应的引用位置上。
- `文件指示`其包括了三个部分，一个是在一个Makefile中引用另一个Makefile，就像C语言中的include一样；另一个是指根据某些情况指定Makefile中的有效部分，就像C语言中的预编译#if一样；还有就是定义一个多行的命令。
- `注释`Makefile中只有行注释，和UNIX的Shell脚本一样，其注释是用`#`字符。如果你要在你的Makefile中使用`#`字符，可以用反斜框进行转义，如：`/#`

## Makefile的文件名
`make`命令会在当前目录下按顺序找寻文件名为`GNUmakefile`、`makefile`、`Makefile`的文件，找到了解释这个文件。在这三个文件名中，最好使用`Makefile`这个文件名

可以使用别的文件名来书写Makefile，比如：`Make.Linux`，`Make.Solaris`，`Make.AIX`等，如果要指定特定的Makefile，你可以使用make的`-f`和`--file`参数，如：`make -f Make.Linux`或`make --file Make.AIX`。

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

targets是文件名，以空格分开，可以使用通配符。一般来说，我们的目标基本上是一个文件，但也有可能是多个文件。

command是命令行，如果其不与`target:prerequisites`在一行，那么，必须以[Tab键]开头，如果和prerequisites在一行，那么可以用分号做为分隔

prerequisites也就是目标所依赖的文件（或依赖目标）。如果其中的某个文件要比目标文件要新，那么，目标就被认为是`过时的`，被认为是需要重生成的

如果命令太长，你可以使用反斜框（‘/’）作为换行符。make对一行上有多少个字符没有限制。规则告诉make两件事，文件的依赖关系和如何成成目标文件。

一般来说，make会以UNIX的标准Shell，也就是/bin/sh来执行命令。

## 规则中使用通配符
make支持三各通配符：`*`，`?`和`[...]`。这是和Unix的B-Shell是相同的。 如`*.c`表示所以后缀为c的文件。如果我们的文件名中有通配符，那么可以用转义字符`/`

```make
objects = *.o
```

通符同样可以用在变量中。并不是说`*.o`会展开，不！`objects`的值就是`*.o`。Makefile中的变量其实就是C/C++中的宏。如果你要让通配符在变量中展开，也就是让`objects`的值是所有`.o`的文件名的集合，那么，可以这样：

```make
objects := $(wildcard *.o)
```

## 文件搜寻
有大量的源文件，我们通常的做法是把这许多的源文件分类，并存放在不同的目录中。所以，当make需要去找寻文件的依赖关系时，你可以在文件前加上路径，但最好的方法是把一个路径告诉make，让make __自动__ 去找。

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

并不生成`clean`这个文件。`伪目标`并不是一个文件，只是一个标签，由于`伪目标`不是文件，所以`make`无法生成它的依赖关系和决定它是否要执行。我们只有通过显示地指明这个`目标`才能让其生效。当然，`伪目标`的取名不能和文件名重名，不然其就失去了`伪目标`的意义了

当然，为了避免和文件重名的这种情况，我们可以使用一个特殊的标记`.PHONY`来显示地指明一个目标是`伪目标`，向make说明，不管是否有这个文件，这个目标就是`伪目标`。

```make
 .PHONY: clean
clean:
    rm *.o temp
```

Makefile中的第一个目标会被作为其默认目标。我们声明了一个`all`的伪目标，其依赖于其它三个目标。由于伪目标的特性是，总是被执行的，所以其依赖的那三个目标就总是不如`all`这个目标新。所以，其它三个目标的规则总是会被决议

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

`make clean`将清除所有要被清除的文件。`cleanobj`和`cleandiff`这两个伪目标有点像`子程序`的意思。我们可以输入`make cleanall`和`make cleanobj`和`make cleandiff`命令来达到清除不同种类文件的目的。

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
Makefile的规则中的目标可以不止一个，其支持多目标，有可能我们的多个目标同时依赖于一个文件，并且其生成的命令大体类似。于是我们就能把其合并起来。

当然，多个目标的生成规则的执行命令是同一个，这可能会可我们带来麻烦，不过好在我们的可以使用一个自动化变量`$@`，表示着目前规则中所有的目标的集合

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

如果我们的`<target-parrtern>`定义成`%.o`，意思是我们的`<target>`集合中都是以`.o`结尾的，而如果我们的`<prereq-parrterns>`定义成`%.c`，意思是对`<target-parrtern>`所形成的目标集进行二次定义，其计算方法是，取`<target-parrtern>`模式中的`%`（也就是去掉了`.o`这个结尾），并为其加上[.c]这个结尾，__形成的新集合__。

```make
objects = foo.o bar.o

all: $(objects)

$(objects): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@
```

目标从`$object`中获取，`%.o`表明要所有以`.o`结尾的目标，也就是`foo.o bar.o`，也就是变量`$object`集合的模式，而依赖模式`%.c`则取模式`%.o`的`%`，也就是`foo bar`，并为其加下`.c`的后缀，于是，我们的依赖目标就是`foo.c bar.c`。而命令中的`$<`和`$@`则是自动化变量，`$<`表示所有的依赖目标集（也就是`foo.c bar.c`），`$@`表示目标集（也就是`foo.o bar.o`）。于是，上面的规则展开后等价于下面的规则：

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
在Makefile中，我们的依赖关系可能会需要包含一系列的头文件，比如，如果我们的`main.c`中有一句`#include "defs.h"`，那么我们的依赖关系应该是：

```make
main.o : main.c defs.h
```

如果是一个比较大型的工程，你必需清楚哪些C文件包含了哪些头文件，并且，你在加入或删除头文件时，也需要小心地修改Makefile，这是一个很没有维护性的工作。为了避免这种繁重而又容易出错的事情，我们可以使用C/C++编译的一个功能。大多数的C/C++编译器都支持一个`-M`的选项，即自动找寻源文件中包含的头文件，并生成一个依赖关系。例如，如果我们执行下面的命令：

```shell
cc -M main.c
```

其输出是：

```
main.o : main.c defs.h
```

于是由编译器自动生成的依赖关系，这样一来，你就不必再手动书写若干文件的依赖关系，而由编译器自动生成了。需要提醒一句的是，如果你使用GNU的C/C++编译器，你得用`-MM`参数，不然，`-M`参数会把一些标准库的头文件也包含进来。

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

于是，我们可以写出`.c`文件和`.d`文件的依赖关系，并让make自动更新或自成`.d`文件，并把其包含在我们的主Makefile中，这样，我们就可以自动化地生成每个文件的依赖关系了。

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

于是，我们的`.d`文件也会自动更新了，并会自动生成了，当然，你还可以在这个`.d`文件中加入的不只是依赖关系，包括生成的命令也可一并加入，让每个`.d`文件都包含一个完赖的规则。一旦我们完成这个工作，接下来，我们就要把这些自动生成的规则放进我们的主Makefile中。我们可以使用Makefile的`include`命令，来引入别的Makefile文件（前面讲过），例如：

```make
sources = foo.c bar.c
include $(sources:.c=.d)
```

上述语句中的`$(sources:.c=.d)`中的`.c=.d`的意思是做一个替换，把变量`$(sources)`所有`.c`的字串都替换成`.d`。当然，你得注意次序，因为include是按次来载入文件，最先载入的`.d`文件中的目标会成为默认目标。

书写命令
==========
## 显示命令
当我们用`@`字符在命令行前，那么，这个命令将不被make显示出来，最具代表性的例子是，我们用这个功能来像屏幕显示一些信息。如：

```make
@echo 正在编译XXX模块......
```

当make执行时，会输出`正在编译XXX模块......`字串，但不会输出命令，如果没有`@`，那么，make将输出

```
echo 正在编译XXX模块......
正在编译XXX模块......
```

如果make执行时，带入make参数`-n`或`--just-print`，那么其只是显示命令，但不会执行命令，这个功能很有利于我们 __调试__ 我们的Makefile，看看我们书写的命令是执行起来是什么样子的或是什么顺序的。

而make参数`-s`或`--slient`则是全面禁止命令的显示。

## 命令执行
当依赖目标新于目标时，也就是当规则的目标需要被更新时，make会一条一条的执行其后的命令。需要注意的是，如果你要让上一条命令的结果应用在下一条命令时，你应该使用分号分隔这两条命令(否则make认为它们之间无依赖，会分开进程去执行它，这样才能多任务的进行make)

```make
exec1:
    cd /home/zyx
    pwd

exec2:
    cd /home/zyx; pwd
```

当我们执行`make exec1`时，第一个例子中的`cd`没有作用，`pwd`会打印出当前的Makefile目录，而第二个例子中，`cd`就起作用了，pwd会打印出`/home/zyx`。

## 命令出错
有些时候，命令的出错并不表示就是错误的。例如`mkdir`命令，我们一定需要建立一个目录，如果目录不存在，那么`mkdir`就成功执行，万事大吉，如果目录存在，那么就出错了。我们之所以使用`mkdir`的意思就是一定要有这样的一个目录，于是我们就不希望`mkdir`出错而终止规则的运行。

为了做到这一点，忽略命令的出错，我们可以在Makefile的命令行前加一个减号`-`（在Tab键之后），标记为不管命令出不出错都认为是成功的。如：

```make
clean:
    -rm -f *.o
```

还有一个全局的办法是，给make加上`-i`或是`--ignore-errors`参数，那么，Makefile中所有命令都会忽略错误。而如果一个规则是以`.IGNORE`作为目标的，那么这个规则中的所有命令将会忽略错误。这些是不同级别的防止命令出错的方法，你可以根据你的不同喜欢设置。

还有一个要提一下的make的参数的是`-k`或是`--keep-going`，这个参数的意思是，如果某规则中的命令出错了，那么就终目该规则的执行，但继续执行其它规则。

## 嵌套执行make
在一些大的工程中，我们会把我们不同模块或是不同功能的源文件放在不同的目录中，我们可以在每个目录中都书写一个该目录的Makefile，这有利于让我们的Makefile变得更加地简洁，而不至于把所有的东西全部写在一个Makefile中，这样会很难维护我们的Makefile，这个技术对于我们模块编译和分段编译有着非常大的好处。

例如，我们有一个子目录叫subdir，这个目录下有个Makefile文件，来指明了这个目录下文件的编译规则。那么我们总控的Makefile可以这样书写：

```make
subsystem:
    cd subdir && $(MAKE)
```

其等价于：

```make
subsystem:
    $(MAKE) -C subdir
```

总控Makefile的变量可以传递到下级的Makefile中（如果你显示的声明），但是不会覆盖下层的Makefile中所定义的变量，除非指定了`-e`参数。

如果你要传递变量到下级Makefile中，那么你可以使用这样的声明：

```make
export <variable ...>
```

如果你不想让某些变量传递到下级Makefile中，那么你可以这样声明：

```make
unexport <variable ...>
```

有两个变量，一个是`SHELL`，一个是`MAKEFLAGS`，这两个变量不管你是否export，其总是要传递到下层Makefile中，特别是`MAKEFLAGS`变量，其中包含了make的 __参数信息__，如果我们执行`总控Makefile`时有make参数或是在上层Makefile中定义了这个变量，那么`MAKEFLAGS`变量将会是这些参数，并会传递到下层Makefile中，这是一个系统级的环境变量。

如果你不想往下层传递参数，那么，你可以这样来：

```make
subsystem:
    cd subdir && $(MAKE) MAKEFLAGS=
```

## 定义命令包
如果Makefile中出现一些相同命令序列，那么我们可以为这些相同的命令序列定义一个变量。定义这种命令序列的语法以`define`开始，以`endef`结束，如：

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

我们可以看见，要使用这个命令包，我们就好像使用变量一样。在这个命令包的使用中，命令包`run-yacc`中的`$^`就是`foo.y`，`$@`就是`foo.c`，make在执行命令包时，命令包中的每个命令会被依次独立执行。

使用变量
===========

在Makefile中，变量可以使用在`目标`，`依赖目标`，`命令`或是Makefile的其它部分中。

传统的Makefile的变量名是全大写的命名方式，但我推荐使用大小写搭配的变量名，如：MakeFlags。这样可以避免和系统的变量冲突，而发生意外的事情。

## 变量的基础
变量在声明时需要给予初值，而在使用时，需要给在变量名前加上`$`符号，但最好用小括号`（）`或是大括号`{}`把变量给包括起来。如果你要使用真实的`$`字符，那么你需要用`$$`来表示。

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

我们执行`make all`将会打出变量`$(foo)`的值是`Huh?`（ `$(foo)`的值是`$(bar)`，`$(bar)`的值是`$(ugh)`，`$(ugh)`的值是`Huh?``）可见，变量是可以 __使用后面__ 的变量来定义的。

我们可以把变量的真实值推到后面来定义

```make
CFLAGS = $(include_dirs) -O
include_dirs = -Ifoo -Ibar
```

当`CFLAGS`在命令中被展开时，会是`-Ifoo -Ibar -O`

但这种形式也有不好的地方，那就是 __递归定义__，为了避免，我们可以使用make中的另一种用变量来定义变量的方法。这种方法使用的是`:=`操作符，在定义时就展开

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

我们可以替换变量中的共有的部分，其格式是`$(var:a=b)`或是`${var:a=b}`，其意思是，把变量`var`中所有以`a`字串`结尾`的`a`替换成`b`字串。这里的`结尾`意思是`空格`或是`结束符`。

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

再复杂一点，我们再加上函数：

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
如果有变量是通常make的命令行参数设置的，那么Makefile中对这个变量的赋值会被忽略。如果你想在Makefile中设置这类参数的值，那么，你可以使用`override`指示符。其语法是：

```make
override <variable> = <value>
override <variable> := <value>
```

当然，你还可以追加：

```make
override <variable> += <more text>
```

对于多行的变量定义，我们用`define`指示符，在`define`指示符前，也同样可以使用`override`指示符，如：

```make
override define foo
    bar
endef
```

## 环境变量
make运行时的系统环境变量可以在make开始运行时被载入到Makefile文件中，但是如果Makefile中已定义了这个变量，或是这个变量由make命令行带入，那么系统的环境变量的值将被覆盖。（如果make指定了`-e`参数，那么，系统环境变量将　__覆盖__ Makefile中定义的变量）

因此，如果我们在环境变量中设置了`CFLAGS`环境变量，那么我们就可以在所有的Makefile中使用这个变量了。这对于我们使用统一的编译参数有比较大的好处。如果Makefile中定义了`CFLAGS`，那么则会使用Makefile中的这个变量，如果没有定义则使用系统环境变量的值，一个共性和个性的统一，很像`全局变量`和`局部变量`的特性。

当make嵌套调用时（参见前面的`嵌套调用`章节），上层Makefile中定义的变量会以系统环境变量的方式传递到下层的Makefile中。当然，默认情况下，只有 __通过命令行设置的变量会被传递__。而定义在文件中的变量，如果要向下层Makefile传递，则需要 __使用`exprot`关键字__ 来声明。

当然，并 __不推荐把许多的变量都定义在系统环境__ 中，这样，在我们执行不用的Makefile时，拥有的是同一套系统变量，这可能会带来更多的麻烦。

## 目标变量
可以为某个目标设置局部变量，这种变量被称为`Target-specific Variable`，它可以和`全局变量`同名，因为它的作用范围只在这条规则以及连带规则中，所以其值也只在作用范围内有效。而不会影响规则链以外的全局变量的值。

其语法是：

```
<target ...> : <variable-assignment>
<target ...> : override <variable-assignment>
```

`<variable-assignment>`可以是前面讲过的各种赋值表达式，如`=`、`:=`、`+=`或是`?=`。第二个语法是针对于make命令行带入的变量，或是系统环境变量。

这个特性非常的有用，当我们设置了这样一个变量，这个变量会作用到由这个目标所引发的所有的规则中去

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
在GNU的make中，还支持模式变量（Pattern-specific Variable），通过上面的目标变量中，我们知道，变量可以定义在某个目标上。模式变量的好处就是，我们可以给定一种模式，可以把变量定义在符合这种模式的所有目标上。

make的模式一般是至少含有一个`%`的，所以，我们可以以如下方式给所有以`.o`结尾的目标定义目标变量：

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





```make

```
