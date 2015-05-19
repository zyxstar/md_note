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

通过多个<target>来依赖单个头文件，减少了Makefile文件行数，但依赖关系有点凌乱，并不推荐使用

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

## 书写规则
### 基本语法
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

### 规则中使用通配符
make支持三各通配符：`*`，`?`和`[...]`。这是和Unix的B-Shell是相同的。 如`*.c`表示所以后缀为c的文件。如果我们的文件名中有通配符，那么可以用转义字符`/`

```make
objects = *.o
```

通符同样可以用在变量中。并不是说`*.o`会展开，不！`objects`的值就是`*.o`。Makefile中的变量其实就是C/C++中的宏。如果你要让通配符在变量中展开，也就是让`objects`的值是所有`.o`的文件名的集合，那么，可以这样：

```make
objects := $(wildcard *.o)
```

### 文件搜寻
有大量的源文件，我们通常的做法是把这许多的源文件分类，并存放在不同的目录中。所以，当make需要去找寻文件的依赖关系时，你可以在文件前加上路径，但最好的方法是把一个路径告诉make，让make __自动__ 去找。

Makefile文件中的特殊变量`VPATH`就是完成这个功能的，如果没有指明这个变量，make只会在当前的目录中去找寻依赖文件和目标文件。如果定义了这个变量，那么，make就会在当当前目录找不到的情况下，到所指定的目录中去找寻文件了。

```make
VPATH = src:../headers
```

上面的的定义指定两个目录，`src`和`../headers`，make会按照这个顺序进行搜索。目录由`:`分隔。（当然，当前目录永远是最高优先搜索的地方）

另一个设置文件搜索路径的方法是使用make的`vpath`关键字（注意，它是全小写的），这不是变量，这是一个make的关键字，这和上面提到的那个`VPATH`变量很类似，但是它更为灵活。它可以指定不同的文件在不同的搜索目录中。这是一个很灵活的功能。它的使用方法有三种：

- `vpath <pattern> <directories>`为符合模式`<pattern>`的文件指定搜索目录`<directories>`。
- `vpath <pattern>`清除符合模式`<pattern>`的文件的搜索目录。
- `vpath`清除所有已被设置好了的文件搜索目录。


