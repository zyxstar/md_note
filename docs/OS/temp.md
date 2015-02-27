## 文件系统

- 图中有两上目录项指向同一个i节点，每一个i节点都有一个链接计数，其值指向该i节点的目录项数，只有当链接计数减少至0时，才可以删除该文件（释放该文件占用的数据块），所以，删除一个目录项称为`unlink`而不是`delete`的原因
- 在`stat`结构中，链接计数包含在`st_nlink`成员中，这种链接称为硬链接
- 另一种链接称为符号链接，该文件的实际内容（在数据块中）包含了该符号所指向的文件的名字，该i节点中的文件类型是`S_IFLNK`
- i节点包含了大多数与文件有关的信息：文件类型、文件访问权限位、文件长度、指向该文件所占用的数据块的指针等，`stat`结构中大多数信息都取自i节点，只有文件名和i节点编号（类型是`ino_t`）取自目录项
- 每个文件系统各自对它们的i节点进行编号，目录项中的i节点编号数指向同一文件系统中的相应i节点，不能使一个目录项指向另一个文件系统的i节点，这也是为什么`ln`命令不能跨越文件系统的原因
- 当不更换文件系统情况下为一个文件更名时，该文件的实际内容并未移动，只需构造一个指向现有i节点的新目录项，并解除与旧目录项的链接


对于目录文件的链接计数字段又如何？

任何一个叶目录（不包含任何其他目录的目录）的链接计数总是2，一个是命名该目录的目录项，另一个是该目录项中`.`项，如果包含子目录，则链接计数至少为3，多一个子目录，子目录中的`..`项，使父目录的链接计数增1

## link/unlink/remove/rename函数
- 创建目录项以及增加链接计数应当是个原子操作，大多数系统不允许对目录的硬链接
- 为了解除对文件的链接，必须对包含该目录项的目录具有写和执行权限，如果对该目录设置了粘住位，则对该目录必须具有写权限
- 当链接计数达到0时，该文件的内容才可被删除，另一个阻止删除文件夹的内容是，如果进程打开了该文件。
- 进程用`open/creat`创建一个文件，然后立即调用`unlink`，因为文件是打开的，所以不会将其内容删除，只有当进程关闭该文件或终止时，该文件内容才会被删除，这种性质经常被程序用来确保即使在该程序崩溃时，它所创建的临时文件也 __不会遗留__ 下来
- 如果是符号链接，那么`unlink`删除该符号链接，而不会删除由该链接所引用的文件，在给出符号链接名情况下，没有一个函数能直接删除由该链接所引用的文件
- `remove`函数，对于文件与`unlink`相同，对于目录与`rmdir`相同


rename

- 如果`oldname`指的是一个文件而不是目录，那么为该文件或符号链接更名，如果`newname`已存在（不能引用一个目录），先将该目录项删除，然后将`oldname`更名为`newname`，对于包含新旧文件的目录，调用进程必须具有写权限
- 如果`oldname`指的是目录，那么为该目录更名，如果`newname`已存在，必须引用一个目录，且目录为空目录，先将其删除，然后将`oldname`更名为`newname`，另，当为一个目录更名时，`newname`不能包含`oldname`作为其路径前缀


## 符号链接
硬链接指向文件i节点，而符号链接指向一个文件的间接指针，它避开了硬链接的一些限制：

- 硬链接通常要求链接和文件位于同一文件系统中
- 只有超级用户才能创建指向目录的硬链接

当使用以名字引用文件的函数时，需要了解该函数是否处理符号链接，也就是函数是否跟随符号链接到达它所链接的文件


table



`mkdir/mkinfo/mknod/rmdir`当路径名是符号链接时，都出错返回，`chown`是否跟随符号链接取决于实现（目前都跟随）

同时`O_CREAT`和`O_EXCL`调用`open`时，若路径名引用符号链接，将出错返回，并将`errno`设置为`EEXIST`


$ ln -s /no/such/file myfile $ ls myfile
myfile
$ cat myfile
cat: myfile: No such file or directory
$ ls -l myfile
lrwxrwxrwx 1 sar 13 Jan 22 00:26 myfile -> /no/such/file


## symlink/readlink函数
创建一个符号链接，并不要求实际路径已经存在，也不要求位于同一文件系统中


#include <unistd.h>
int symlink(const char *actualpath, const char *sympath);
int symlinkat(const char *actualpath, int fd, const char *sympath);


#include <unistd.h>
ssize_t readlink(const char* restrict pathname, char *restrict buf, size_t bufsize);
ssize_t readlinkat(int fd, const char* restrict pathname, char *restrict buf, size_t bufsize);

此函数组合了`open`,`read`,`close`的所有操作，如果执行成功，返回读入`buf`字节数，`buf`中内容不以`null`终止

## 文件的时间

table

修改时间是文件内容最后一次被修改的时间，更改状态时间是该文件的i节点（更改文件的访问权限、用户ID、链接数等）最后一次被修改的时间

系统并不保存一个i节点的最后一次访问时间，所以`access/stat`函数并不更改这三个时间中的任一个

table

## utime函数
一个文件的访问和修改时间可用该函数

#include <sys/stat.h>
int futimens(int fd, const struct timespec times);
int utimensat(int fd, const char *path, const struct timespec times, int flag);



#include <sys/time.h>
int utimes(const char *pathname, const struct timeval times[2]);

我们不能对更改状态时间`st_ctime`指定一个值，当调用`utime`函数时，此字段将被自动更新


## mkdir/rmdir函数

#include <sys/stat.h>
int mkdir(const char *pathname, mode_t mode);
int mkdirat(int fd, const char *pathname, mode_t mode);

#include <unistd.h>
int rmdir(const char *pathname);

## 读目录

#include <dirent.h>
DIR *opendir(const char *pathname); DIR *fdopendir(int fd);
struct dirent *readdir(DIR *dp); void rewinddir(DIR *dp);
int closedir(DIR *dp);
long telldir(DIR *dp);
void seekdir(DIR *dp, long loc);

## chdir/fchdir/getcwd函数


当前工作目录是进程的一个属性，它只影响调用`chdir`的进程本身，而不影响其他进程




如果`open`或`creat`创建已经存在的文件，则该文件的访问权限位不变


标准IO库
========

## 流和FILE对象

流的定向决定了所读、写的字符是单字还是多字节的，当一个流最初被创建时，并没有定向，在其流上使用一个多字节IO函数（`wchar.h`），则将该流的定向设置为 __宽定向__ 的，如使用一个单字节IO函数，则将该流的定向设置为 __字节定向__ 的。`freopen()`清除一个流的定向，`fwide()`设置流的定向


#include <stdio.h>
#include <wchar.h>
int fwide(FILE *fp, int mode);

- `mode`为负，试图使指定的流是字节定向的
- `mode`为正，试图使指定的流是宽定向的
- `mode`为0，不试图设置流的定向，但返回该流定向的值

`fwide`并不改变 __已__ 定向流的定向，而且它无出错返回，唯一可靠是，调用它前，清除了`errno`，执行后，检查`errno`

## 缓冲
- 全缓冲，在填满标准IO缓冲区后才进行实际IO操作，对于驻留在磁盘上的文件通常是由标准IO实施全缓冲的，缓冲区可由标准IO例程自动冲洗，或者调用`fflush`冲洗一个流
- 行缓冲，在这种情况下，在输入和输出中遇到换行符时，标准IO库执行IO操作，允许我们一次输出一个字符（`fputc`），但只有在写了一行之后才进行实际IO操作。当流涉及一个终端时（标准输入和标准输出），通常使用行缓冲
> 行缓冲有两个限制，第一，因为标准IO库用来收集每一行的缓冲区的长度是固定的，只要填满了缓冲区，即使还没有写一个换行符，也进行IO操作；第二，任何时候只要通过标准IO库要求从一个不带缓冲的流，或一个行缓冲的流且要求从内核得到输入数据，那么会造成冲洗所有行缓冲输出流
- 不带缓冲，如标准IO中`fputs`写15个字符到不带缓冲的流中。标准出错流`stderr`通常是不带缓冲的


ISO C缓冲特征：

- 当且仅当标准输入和标准输出并不涉及交互式设备时，它们才是会缓冲的
- 标准出错不带缓冲的
- 若涉及终端设备的其他流，则它们是行缓冲的，否则是全缓冲的

#include <stdio.h>
void setbuf(FILE *restrict fp, char *restrict buf );
int setvbuf(FILE *restrict fp, char *restrict buf, int mode, size_t size);


table

一般而言，应由系统选择缓冲区的长度，并自动分配缓冲区，在此情况下关闭此流时，标准IO库将自动释放缓冲区

int fflush(FILE *fp);

此函数使该流所有未写的数据都被传送至内核，如果`fp`是`NULL`，则此函数将导致所有输出流被冲洗


## 打开流

#include <stdio.h>
FILE *fopen(const char *restrict pathname, const char *restrict type);
FILE *freopen(const char *restrict pathname, const char *restrict type, FILE *restrict fp);
FILE *fdopen(int fd, const char *type);

- `fopen`打开一个指定的文件
- `freopen`在一个指定的流上打开一个指定的文件，如流已打开，则先关闭该流，如流已定向，则清除该定向。此函数一般用于将一个指定的文件打开为一个预定义的流：标准输入、标准输出或标准出错
- `fdopen`获取一个现有的文件描述符(`open/dup/fntl/pipe/socket...`)，并使一个标准的IO流与该描述符相结合。常用于由创建管道和网络通信通道函数返回的描述符，因为这些特殊文件不能用标准的`fopen`打开

table

对于`fdopen`为`tyep="wb"`而打开，并不截短该文件，`type="a"`也不会创建文件

当添写类型打开一文件后，则每次都将数据写到文件的当前尾端处，多个进程打开了同一文件，每个进程的数据都将正确的写到文件中

当 __读和写__ 打开一个文件时，即`type`中有`+`时:

- 如果中间没有`fflush/fseek/fsetpos/rewind`，则在输出的后面不能直接跟随输入
- 如果中间没有`fseek/fsetpos/rewind`，或者一个输入操作没有到达文件尾端，则在输入之后不能直接跟参输出


table

在指定`w/a`类型创建一个新文件时，无法说明该文件的访问权限位（`open/creat`则能做到）

若流引用终端设备，则流是行缓冲的，否则按系统默认的情况，流被打开时是全缓冲的，有需要，可通过`setbuf/setvbuf`改变缓冲的类型

int fclose(FILE *fp);

在该文件被关闭之前，冲洗缓冲区的输出数据，丢弃缓冲区的任何输入数据，如果标准IO为流`malloc`了一人缓冲区，则释放此缓冲区

当一个进程正常终止时（调用`exit`或从`main`返回），则所有带未写缓冲数据的标准IO流都会被冲洗，所有打开的标准IO流都会被关闭

## 读和写流

- 每次一个字符的IO，如果流是带缓冲的，则标准IO会处理所有缓冲
- 每次一行的IO，使用`gfets/fputs`，当调用`fgets`，应能说明处理的最大行长
- 直接IO，`fread/fwrite`

### 输入函数

#include <stdio.h> 
int getc(FILE *fp); 
int fgetc(FILE *fp); 
int getchar(void);

`getchar`等价于`getc(stdin)`，`getc`可被实现为宏，而`fgetc`则是函数，后者可以作为参数传参

不管是出错还是到达文件尾端，这三个函数都返回同样的值`EOF`，为了区分这两种不同情况，必须调用`ferror/feof` 

#include <stdio.h> 
int ferror(FILE *fp); 
int feof(FILE *fp);
void clearerr(FILE *fp);


#include <stdio.h>
int ungetc(int c, FILE *fp);

回送的字符不必是上一次读到的字符，不能回送`EOF`，已到达文件尾端时，仍可以回送一字符，下次读将返回该字符，再次读则返回`EOF`

> `ungetc`并没将字符写到文件中设备中，只写回标准IO的流缓冲区中

### 输出函数

#include <stdio.h>
int putc(int c, FILE *fp); 
int fputc(int c, FILE *fp); 
int putchar(int c);

同理，`putchar(c)`等价于`putc(c, stdout)`，`putc`可实现为宏，而`fputc`则为函数


## 每次一行IO

#include <stdio.h>
char *fgets(char *restrict buf, int n, FILE *restrict fp); 
char *gets(char *buf);

对于`fgets`，必须指定缓冲区的长度`n`，一直读到下一个换行符为止，但不超过`n-1`个字符，缓冲区以`null`字符结尾，若该行（包括换行符）的字符数超过`n-1`，则返回一个不完整的行，下次调用继续读该行

`gets` __不推荐__ 使用，容易造成缓冲区溢出，与`fgets`另一个区别是，它不将换行符存入缓冲区中


#include <stdio.h>
int fputs(const char *restrict str, FILE *restrict fp); 
int puts(const char *str);

`fputs`将一个`null`符终止的字符串写到指定的流，尾端`null`不写出，通常`null`之前是一个换行符

`puts`会自动将换行符写到标准输出，也 __不推荐__ 使用，推荐总是使用`fgets/fputs`，手工处理换行符


## 标准IO的效率

如果`fgets/fputs`是用`getc/putc`实现的，那么，可以预期`fgets`版本的时间会和`getc`相接近，但`fgets`是用`memccpy`（使用汇编）实现的，会有较高的速度


## 二进制IO

#include <stdio.h>
size_t fread(void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
size_t fwrite(const void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);


在一个系统上写的数据，要在另一个系统上进行处理，可能不能正常工作：

- 在一个结构中，同一成员的偏移量可能因编译器和系统而异：如紧密包装、内存对齐
- 用来存储多字节整数和浮点值的二进制格式在不同机器体系结构间也可能不同

## 定位流
- `ftell/fseek`，假定文件的位置可以存放在一个长整型中
- `ftello/fseeko`，使用`off_t`数据类型代替了长整型
- `fgetpos/fsetpos`，使用一个抽象数据类型`fpos_t`记录文件的位置，移植到非UNIX系统上应当使用它

#include <stdio.h> 
long ftell(FILE *fp);
int fseek(FILE *fp, long offset, int whence);
void rewind(FILE *fp);


off_t ftello(FILE *fp);
int fseeko(FILE *fp, off_t offset, int whence);


int fgetpos(FILE *restrict fp, fpos_t *restrict pos); 
int fsetpos(FILE *fp, const fpos_t *pos);


## 格式化IO

### 格式化输出

#include <stdio.h>
int printf(const char *restrict format, ...);
int fprintf(FILE *restrict fp, const char *restrict format, ...); 
int dprintf(int fd, const char *restrict format, ...);
int sprintf(char *restrict buf, const char *restrict format, ...); 
int snprintf(char *restrict buf, size_t n, const char *restrict format, ...);


`sprintf`在数组的尾端自动加一个`null`字节，但该字节不包括在返回值中。该函数有可能造成`buf`指向的缓冲区的溢出，调用者有责任确保该缓冲区足够大

`snprintf`解决缓冲区溢出问题，缓冲区长度是一个显式参数，超过的任何字符都会被丢弃，返回的是写入缓冲区的字符数，同上，不包括`null`字节，若发生错误，则返回负值

`%[flags][fldwidth][precision][lenmodifier]convtype`

table flag

- `fldwidth`说明转换的最小字段宽度，字符少时用空格填充它，是一个非负十进制数，或一个星号(`*`，占位符，具体值由后续的参数给出)
- `precision`说明整型转换后最小输出数字位数、浮点数转换后小数点后的最少位数、字符转换后的最大字符数。精度是一个句点(`.`)，后接一个可选的非负十进制整数或一个星号(`*`)
- 宽度和精度字段两者皆可为`*`，此时，一个整型参数指定宽度或精度的值。该整型参数正好位于被转换的参数之前
- `lenmodifier`说明参数长度

table


- `convtype`是必选的，它控制如何解释参数

table


//cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
ntpdate us.pool.ntp.org



下面是变体，可变参数代换成了`arg`

#include <stdarg.h>
#include <stdio.h>
int vprintf(const char *restrict format, va_list arg);
int vfprintf(FILE *restrict fp, const char *restrict format, va_list arg);
int vdprintf(int fd, const char *restrict format, va_list arg);
int vsprintf(char *restrict buf, const char *restrict format, va_list arg);
int vsnprintf(char *restrict buf, size_t n, const char *restrict format, va_list arg);


### 格式化输入

#include <stdio.h>
int scanf(const char *restrict format, ...);
int fscanf(FILE *restrict fp, const char *restrict format, ...);
int sscanf(const char *restrict buf, const char *restrict format, ...);


除转换说明和空白字符外，格式字符串中的其他字符必须与输入匹配，若有一个字符不匹配，则停止后续处理，不再读入输入的其余部分

`%[*][fldwidth][lenmodifier]convtype`

- `*`用于抑制转换，按照转换说明的其余部分对输入进行转换，但转换结果并不存放在参数中
- `fldwidth`说明最大宽度
- `lenmodifier`说明要用转换结果初始化的参数大小，同输出函数的长度修饰符
- `convtype`类似输出函数，但还有些差别，如存储在无符号类型中的结果可输入时带上符号，如-1可被转换成4294967295赋予无符号整型变量

table


支持的可变参数

#include <stdarg.h>
#include <stdio.h>
int vscanf(const char *restrict format, va_list arg);
int vfscanf(FILE *restrict fp, const char *restrict format, va_list arg);
int vsscanf(const char *restrict buf, const char *restrict format, va_list arg);


## 实现细节
对一个流调用`fileno()`得到其描述符

#include <stdio.h> 
int fileno(FILE *fp);

在打印缓冲状态信息之前，需要对每个流执行IO操作，第一个IO操作通常就造成为该流分配缓冲。结构体成员`_IO_file_flags/_IO_buf_base/_IO_buf_end/_IO_UNBUFFERED/_IO_LINE_BUFFERED`由GUN标准IO库定义的

当标准输入、输出连至终端时，它们是行缓冲的，长度是1024，当重定向到普通文件时，则变成全缓冲的，长度是`stat.st_blksize`(4096)

## 临时文件

#include <stdio.h>
char *tmpnam(char *ptr);
FILE *tmpfile(void);

若`ptr`是`NULL`，则所产生的路径名存放在一个静态区，并将指向该静态区的指针返回，下一次调用时，会 __重写__ 该静态区（想保存路径名，则应保存路径名的副本），若不是`NULL`，则认为它指向长度至少是`L_tmpnam`个字符的数组，所产生的路径名存放在该数组中，`ptr`也作为函数值返回。

`tmpfile`创建一个临时二进制文件（类型`wb+`），在关闭该文件或程序结束将自动删除这种文件


#include <stdlib.h>
char *mkdtemp(char *template);
int mkstemp(char *template);

与`tempfile`不同，它们所创建的临时文件不会自动删除(需要手工`unlink`)，`tmpnam/tempnam`不足之处是，在返回唯一路径名和应用程序用该路径创建文件之间有一个时间窗口，不同进程可能创建同名文件，而`mkstemp`不会产生此问题


系统数据文件和信息
===============

## 口令文件
`<pwd.h>`中定义的`passwd`结构

table

为了阻止一个特定用户登录系统:

- 使用`/dev/null`作为登录shell，如`squid`用户，它是一个设备（很多服务对帮助它们得以实现的不同守护进程使用不同的用户ID，`squid`项为实现squid代理缓冲服务而设置的），阻止任何人以该用户名义登录到系统
- 将`/bin/false`用作登录shell，将以不成功（非0）状态终止，该shell将此种终止状态判断为假
- 将`/bin/true`禁止一个账户，将以成功（0）状态终止
- 某此系统提供`nologin`命令，打印可自定义的出错信息，然后以非0状态终止


`nobody`用户名的目的是，使任何人都可登录至系统，但其用户和组不提供任何特权





























































