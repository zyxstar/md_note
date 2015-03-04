> 2015-02-08

文件IO
===========
本章所说的函数经常被称为 __不带缓冲的IO__，指每个`read`和`write`都调用内核中的一个 __系统调用__，其实还存在一个内核的缓冲区高速缓存，`read`和`write`的数据都要被内核缓冲，此处的不带缓冲，指的是在用户的进程中对这两个函数不会自动缓冲

## 文件描述符
在依从POSIX的应用程序中，幻数0,1,2应当替换成符号常量`STDIN_FILENO`,`STDOUT_FILENO`,`STDERR_FILENO`(被定义在`<unistd.h>`中)

文件描述符的变化范围为0~`OPEN_MAX`

## open函数
```c
#include <fcntl.h>
int open(const char *path, int oflag, ... /* mode_t mode*/ );
int openat(int fd, const char *path, int oflag, ... /* mode_t mode*/ );
//Both return: file descriptor if OK, −1 on error
```

- `O_CREAT` 若文件不存在，则创建它，需要第三个参数`mode`，用于指定该新文件的访问权限位
- `O_CLOEXEC` 设置文件描述符为`FD_CLOEXEC`
- `O_DIRECTORY` 如果`path`引用的不是目录，则出错
- `O_EXCL` 如果同时指定了`O_CREAT`，而文件存在，则会出错，可以测试一个文件是否存在，如果不存在则创建，测试和创建是一个 __原子__ 操作
- `O_NOFOLLOW` 如果`path`引用的是一个符号链接，则出错
- `O_NONBLOCK` 如果`path`引用的是一个FIFO、一个块特殊文件或一个字符特殊文件，则此选项为文件的本次打开操作和后续IO操作设置为 __非阻塞__ 方式
- `O_DSYNC` 使每次`write`等待物理IO操作完成，但是如果写操作并不影响读取刚写入的数据，则不等待文件属性被更新
- `O_SYNC` 使每次`write`等待物理IO操作完成，包括由`write`操作引起的文件属性更新所需的IO
- `O_RSYNC` 进行`read`操作等待，直至所有对文件同一部分挂起的写操作都完成

关于`openat`:

- `path`参数指定的是绝对路径，这种情况下`fd`参数被忽略，`openat`相当于`open`
- `path`参数指定的是相对路径，`fd`参数指出了相对路径名在文件系统中的开始地址，`fd`参数是通过打开相对路径名所在的目录来获取（即通过以前`open`过的目录所得到的`fd`）
- `path`参数指定的是相对路径，`fd`参数具有特殊值`AT_FDCWD`，路径名在当前工作目录中获取

引入它，为了解决：

- 让线程可以使用相对路径名打开目录中的文件，而不再只能打开当前工作目录
- 避免time-of-check-to-time-of-use(TOCTTOU)错误

使用`O_APPEND`打开一个文件以便读写，无论`lseek`到任意位置，`write`都从文件末尾开始写。不用该标志打开文件，就可`lseek`到任意位置，然后`write`，但会覆盖之前内容

## creat函数
```c
#include <fcntl.h>
int creat(const char *path, mode_tmode);
//Returns: file descriptor opened for write-only if OK, −1 on error
```

此函数等效于

```c
open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
```

`creat`不足之外它以 __只写__ 方式打开所创建的文件，如果需要创建一个临时文件，并先写，然后再读，推荐以下方式

```c
open(path, O_RDWR | O_CREAT | O_TRUNC, mode);
```

## close函数
```c
#include <unistd.h>
int close(int fd );
//Returns: 0 if OK,−1 on error
```

关闭一个文件时还会释放该进程加在该文件上的所有记录锁。当一个进程终止时，内核自动关闭它所有打开的文件。

## lseek函数
```c
#include <unistd.h>
off_t lseek(intfd, off_t offset,int whence );
Returns: new file offset if OK,−1 on error
```

当打开一个文件时，除非指定`O_APPEND`选项，否则该偏移量被设置为0

若`lseek`成功，则返回新的文件偏移量，为此可以用以下方式确定打开文件的当前偏移量(相当于标准库中的`ftell`?)

```c
off_t currpos;
currpos = lseek(fd, 0, SEEK_CUR);
```

这种方法也可用来确定所涉及的文件是否可以设置偏移量，如果文件描述符引用的是一个管道、FIFO、网络套接字，则`lseek`返回-1，并将`error`设置为`ESPIPE`

```c
#include "apue.h"
int main(void){
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
        printf("cannot seek\n");
    else
        printf("seek OK\n");
    exit(0);
}
```

```shell
$ ./a.out < /etc/passwd
seek OK
$ cat < /etc/passwd | ./a.out
cannot seek
$ ./a.out < /var/spool/cron/FIFO
cannot seek
```

`lseek`仅将当前的文件偏移量记录在内核中，并不引起任何IO操作

文件偏移量可以大于文件的当前长度，在这种情况下，对该文件的下一次写将加长该文件，并在文件中构成一个 __空洞__，位于文件中但没有写过的字节都被读为0

> 文件中的空洞并不要求在磁盘上占用存储区，具体处理方式与文件系统的实现有关，当定位超出文件尾端之后写时，对新写的数据需要分配磁盘块，但于对于原文件尾端和新开始写位置之间发则不需要分配磁盘块

```c
#include "apue.h"
#include <fcntl.h>
char  buf1[] ="abcdefghij";
char  buf2[] ="ABCDEFGHIJ";
int main(void){
    int  fd;
    if ((fd = creat("file.hole", FILE_MODE)) < 0)
        err_sys("creat error");
    if (write(fd, buf1, 10) != 10)
        err_sys("buf1 write error");
    /* offset now = 10 */
    if (lseek(fd, 16384, SEEK_SET) == -1)
        err_sys("lseek error");
    /* offset now = 16384 */
    if (write(fd, buf2, 10) != 10)
        err_sys("buf2 write error");
    /* offset now = 16394 */
    exit(0);
}
```

```shell
$ ./a.out
$ ls -l file.hole         #check its size
-rw-r--r--  1 sar  16394 Nov 25 01:01 file.hole
$ od -c file.hole         #let’sl ook at the actual contents
0000000  a bcdefghij\0 \0 \0 \0 \0 \0
0000020  \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0
*
0040000  A BCDEFGHIJ
0040012
```

如果比较无空洞但同样长度的文件，后者占用的磁盘块会大

```shell
$ ls -ls file.hole file.nohole      #compar esizes
8  -rw-r--r--  1 sar  16394 Nov 25 01:01 file.hole
20 -rw-r--r--  1 sar  16394 Nov 25 01:03 file.nohole
```

## read函数
```c
#include <unistd.h>
ssize_t read(int fd, void *buf,size_tnbytes );
//Returns: number of bytes read, 0 if end of file,−1 on error
```

有多种情况可使读到的字节数少于要求读的字节数：

- 读普通文件时，在读到要求字节数之前已到达了文件尾端。且下次调用`read`时，它将返回0
- 当从终端设备读时，通常一次最多读一行
- 当从网络读时，网络中的缓冲机构可能造成返回值小于所要求读的字节数
- 当从管道或FIFO读时，如若管道包含的字节少于所需的数量，那么`read`将只返回实际可用的字节数
- 当从某些面向记录的设备(如磁带)，一次最多返回一个记录
- 当某一信号造成中断，而已经读了部分数据量时

### 有关错误处理
- `EAGAIN` 提示没有数据可读，可以重试，也许下次能成功
- `EBADF`,`EISDIR` 等属于程序错误，不应做错误处理（测试就能发现）
- `EFAULT`,`EIO`　等错误，无法处理，只能出错返回
- `EINTR` 信号中断，需要重试

```c
while (1) {
    retr = read(fdr, buf, BUFSIZE);
    if (retr == -1) {
        if (errno == EINTR) {
            continue;
        }
        perror("read()");
        goto read_err;
    }
    if (retr == 0) {
        break;
    }

    retw = 0;
    while (retw < retr) {
        ret = write(fdw, buf + retw, retr - retw);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("write()");
            goto write_err;
        }
        retw += ret;
    }
}
```

所以上述的代码除了检查`errno == EINTR`，其它出错均不必或无法处理

## write函数
```c
#include <unistd.h>
ssize_t write(int fd, const void *buf,size_tnbytes );
Returns: number of bytes written if OK, −1 on error
```

`write`出错的常见原因是：磁盘已写满，或者超过了一个给定进程的文件长度限制

## IO效率
磁盘块长度为4096字节(由`st_blksize`表示)，当缓存区大小设置为该大小时，系统CPU时间最小

文件系统为改善性能都会采用某种 __预读__ 技术，当检测到正在进行顺序读取时，系统就试图读入比应用所要求的更多数据，所以当缓存区即使设置为32时与设置较大缓存区，最后的墙上时间几乎一样

程序执行时，设置`fd`的文件状态标志为`O_SYNC`时（使用`fcntl`），每次`write`都要等待，会增加系统时间和时钟时间（墙上时间）

![img](../../imgs/apue_03.png)

同步写入文件之后，`fsync`的调用并不明显


## 文件共享
即支持在 __不同进程间共享__ 打开的文件

内核使用三种数据结构表示打开的文件，它们之间的关系决定了在文件共享方面一个进程对另一个进行可能产生的影响

- 每个进程在 __进程表中都有一个记录项__(process table entry)，记录项中包含有一张 __打开文件描述符表__，可将其视为一个矢量，每个描述符占用一项，与之相关联的是
    + __文件描述符标志__(fd flags)
    + 指向一个 __文件表项的指针__(file pointer)
- 内核为所有打开文件维持一张文件表(file table entry)，每个文件表包含
    + __文件状态标志__(file status flags，读、写、添写、同步和非阻塞等)
    + 当前文件 __偏移量__(current file offset)
    + 指向该文件 __v节点表项的指针__(v-node pointer)
- 每个打开文件(或设备)都有一个 __v节点__(v-node)结构，包含了 __文件类型__ 和对此文件进行各种 __操作的函数的指针__。对于大多数文件，v节点还包含了该文件的 __i节点__(i-node,索引节点)，这些信息是在打开文件时从磁盘上读入内存的，包含文件所有者、文件长度、文件所在的设备、指向文件实际数据块在磁盘上所在位置的指针等

> linux没用使用v节点，而使用了通用i节点结构

![img](../../imgs/apue_01.png)

可能有多个文件描述符项指向 __同一文件表项__，如下面讨论的`dup`函数。在`fork`后也会发生同样的情况，此时父、子进程对于每一个打开文件描述符 __共享同一个文件表项__

## 原子操作
### 添写至一个文件
`open`时使用`O_APPEND`选项

### pread和pwrite函数
```c
#include <unistd.h>
ssize_t pread(int fd, void *buf,size_tnbytes, off_t offset);
//Returns: number of bytes read, 0 if end of file,−1 on error
ssize_t pwrite(int fd, const void *buf,size_tnbytes, off_t offset);
Returns: number of bytes written if OK, −1 on error
```

相当于顺序调用`lseek`和`read`/`write`，但它是 __原子操作，且不更新文件偏移量__

### 创建一个文件
对`open`函数同时使用`O_CREAT`和`O_EXCL`，否则如果先`open`再`creat`间，另一个进程创建了该文件，就会引起问题

## dup和dup2函数
```c
#include <unistd.h>
int dup(int fd );
int dup2(int fd, int fd2);
//Both return: new file descriptor if OK, −1 on error
```

- 由`dup`返回的新文件描述符一定是当前可用文件描述符中的最小数值，
- 用`dup2`则可以用参数`fd2`指定新描述符的数值，
    + 如果`fd2`已打开，则先将其关闭
    + 如果`fd`等于`fd2`，则返回`fd2`，而不关闭它
    + 否则，`fd2`的`FD_CLOEXEC`文件描述标志就清除，这样`fd2`在进程调用`exec`时是打开的

> `FD_CLOEXEC` close on exec, not on-fork, 意为如果对描述符设置了FD_CLOEXEC，使用execl执行的程序里，此描述符被关闭，不能再使用它，但是在使用fork调用的子进程中，此描述符并不关闭，仍可使用


当两个描述符指向 __同一文件表项__，它们 __共享__ 同一文件状态标志(file status flags)，以及同一当前文件偏移量(current file offset)，__但__ 却有自己的一套文件描述符标志(fd flags)，新描述符的执行时关闭(`close-on-exec`)标志总是由`dup`函数清除

![img](../../imgs/apue_02.png)

复制一个描述符的另一种方法是使用`fcntl`

```c
dup(fd);
// is equivalent to
fcntl(fd, F_DUPFD, 0);

dup2(fd, fd2);
// is equivalent to
close(fd2);
fcntl(fd, F_DUPFD, fd2);
```

在后一种情况，`dup2`是一个原子操作，且两者有某些不同的`errno`

说明下面命令区别

```shell
./a.out > outfile 2>&1
./a.out 2>&1 > outfile
```

第一条，设置标准输出到outfile，然后执行`dup`将 __标准输出复制到描述符2上__，其结果是将标准输出和标准错误设置为同一个文件

第二条，首先执行`dup`，描述符2成为终端（标准输入？），而标准输出重定向到outfile



## sync/fsync/fdatasync函数
```c
#include <unistd.h>
int fsync(int fd );
int fdatasync(int fd );
//Returns: 0 if OK,−1 on error
void sync(void);
```

`sync`函数将所有修改过的缓冲区排入写队列，然后就返回，所以它　__并不等于__ 实际写硬盘操作结束

通常称为update的系统守护进程会周期性的调用`sync`，保证了定期冲洗内核的块缓冲区

`fsync`只对由文件描述符指定的单一文件起作用，并且 __等待写磁盘操作结束__，然后返回，可用于数据库这样的应用程序

`fdatasync`类似于`fsync`，但它只影响文件的数据部分，而`fsync`还会同步更新文件的属性

## fcntl函数
```c
#include <fcntl.h>
int fcntl(int fd, int cmd, ... /* int arg */ );
Returns: depends on cmd if OK (see following), −1 on error
```

如果出错，返回-1，如果成功则返回某个其他值

该函数有5种功能

- 复制一个现有的描述符(cmd = `F_DUPFD`　or `F_DUPFD_CLOEXEC`)
    + `F_DUPFD`，复制文件描述符，新文件描述符作为函数值返回，它是尚未打开的各描述符中，大于或等于第3个参数中各值的最小值（对照`dup`），新描述符与`fd`共享同一文件表项，但其文件描述符中`FD_CLOEXEC`标志被清除
    + `F_DUPFD_CLOEXEC`，复制文件描述符，设置与新描述符关联的`FD_CLOEXEC`文件描述符标志的值，返回新文件描述符
- 获得、设置文件描述符标记(cmd = `F_GETFD`　or `F_SETFD`)
    + `F_GETFD`，对应于`fd`的文件描述符标志作为函数值返回（目前只有一个文件文件描述符标志`FD_CLOEXEC`）
    + `F_SETFD`，设置对于`fd`的文件描述符标志，一般不用常量`FD_CLOEXEC`，而使用0，`exec`时不关闭，1时关闭
- 获得、设置文件状态标志(cmd = `F_GETFL`　or `F_SETFL`)
    + `F_GETFL`，对应于`fd`的文件状态标志作为函数值返回
        * `O_RDONLY` open for reading only
        * `O_WRONLY` open for writing only
        * `O_RDWR` open for reading and writing
        * `O_EXEC` open for execute only
        * `O_SEARCH` open directory for searching only
        * `O_APPEND` append on each write
        * `O_NONBLOCK` nonblocking mode
        * `O_SYNC` wait for writes to complete (data and attributes)
        * `O_DSYNC` wait for writes to complete (data only)
        * `O_RSYNC` synchronize reads and writes
        * `O_FSYNC` wait for writes to complete (FreeBSD and Mac OS X only)
        * `O_ASYNC` asynchronous I/O (FreeBSD and Mac OS X only)
        * 前五个访问标志`O_RDONLY`、`O_WRONLY`、`O_RDWR`、`O_EXEC`、`O_SEARCH`，并不各占一位，它们互斥，只能取其一，需用屏蔽字`O_ACCMODE`取得访问方式位，然后将结果与这五个值一一比较(见下代码)
    + `F_SETFL`，可以更改的值为`O_APPEND`、`O_NONBLOCK`、`O_SYNC`、`O_DSYNC`、`O_RSYNC`、`O_FSYNC`、`O_ASYNC`
- 获得、设置异步IO所有权(cmd = `F_GETOWN` or `F_SETOWN`)
    + `F_GETOWN`，获取当前接收`SIGIO`和`SIGURG`信号的进程ID或进程组ID
    + `F_SETOWN`，设置，正的`arg`指定一个进程ID，负的`arg`表示等于`arg`绝对值的一个进程组ID
- 获得、设置记录锁(cmd = `F_GETLK`, `F_SETLK`,　or `F_SETLKW`)

### 取得文件描述符

```c
#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[])
{
    int     val;

    if (argc != 2)
        err_quit("usage: a.out <descriptor#>");

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
        err_sys("fcntl error for fd %d", atoi(argv[1]));

    switch (val & O_ACCMODE) {
    case O_RDONLY:
        printf("read only");
        break;

    case O_WRONLY:
        printf("write only");
        break;

    case O_RDWR:
        printf("read write");
        break;

    default:
        err_dump("unknown access mode");
    }

    if (val & O_APPEND)
        printf(", append");
    if (val & O_NONBLOCK)
        printf(", nonblocking");
    if (val & O_SYNC)
        printf(", synchronous writes");

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC)
        printf(", synchronous writes");
#endif

    putchar('\n');
    exit(0);
}
```

```shell
$./a.out 0 < /dev/tty
read only
$./a.out 1 > temp.foo
$cat temp.foo
write only
$./a.out 2 2>>temp.foo
write only, append
$./a.out 5 5<>temp.foo
read write
```

程序在一个描述符上进行操作，但根本不知道由shell打开的相应文件的名称，因为这是shell打开的，不能打开时设置，就只能使用`fcntl`了

`5<>temp.foo`表示在文件描述符5上打开文件temp.foo以供读写

### 设置/取消状态标志
```c
#include "apue.h"
#include <fcntl.h>

void
set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int     val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val |= flags;       /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}

void
clr_fl(int fd, int flags) /* flags are file status flags to turn off */
{
    int     val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val &= ~flags;       /* turn flags off */

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}
```

先要获得现有标志值，然后按照期望修改它，最后修改设置新标志值，不能只执行`F_SETFL`(或`F_SETFD`)，这样会关闭以前设置的标志位

## ioctl函数
```c
#include <unistd.h> /* System V */
#include <sys/ioctl.h> /* BSD and Linux */
int ioctl(int fd, int request, ...);
//Returns: −1 on error,s omething else if OK
```

## /dev/fd
系统提供的`/dev/fd`目录下，有名为0、1、2等的文件，打开它们相当于复制描述符

```c
fd = open("/dev/fd/0", mode);
// equals
fd = dup(0);
```

但描述符之前打开时是只读的，那么也只能对`fd`进行读操作，下面的调用即使成功，也不能写操作

```c
fd = open("/dev/fd/0", O_RDWR);
```

> linux的实现是个例外，它把文件描述符映射成指向底层物理文件的符号链接，因此返回的新文件描述符的模式与`/dev/fd`文件描述符其实并不相关

也可使用`/dev/fd`作为路径名参数调用`creat`

> 但linux上，因为指向实际文件的符号链接，在`/dev/fd`上使用`creat`会导致底层文件被截断

系统提供的`/dev/stdin`, `/dev/stdout`, `/dev/stderr`等效于`/dev/fd/0`, `/dev/fd/1`, `/dev/fd/2`

`/dev/fd`主要由shell使用，它允许使用路径名称为调用参数的程序，__能用处理其他路径名的相同方式处理标准输入和标准输出__

```shell
filter file2 | cat file1 /dev/fd/0 file3 | lpr
```

文件和目录
=================
## stat/fstat/fstatat/lstat函数
```c
#include <sys/stat.h>
int stat(const char *restrict pathname, struct stat *restrict buf );
int fstat(int fd, struct stat *buf );
int lstat(const char *restrict pathname, struct stat *restrict buf );
int fstatat(int fd, const char *restrict pathname, struct stat *restrict buf, int flag);
//All four return: 0 if OK,−1 on error
```

`fstatat`为相对于当前打开目录（`fd`指定）的路径名返回文件统计信息，`flag`决定是否跟随一个符号链接，默认是跟随。如果`fd`值为`AT_FDCWD`，且`pathname`是相对路径，则计算具体的路径，如果`pathname`是绝对路径，`fd`被忽略

`stat`返回与此命名文件有关的信息结构，`fstat`获取已在描述符`fd`上打开文件的有关信息，当命名文件是一个符号连接时，`lstat`返回该符号链接的有关信息，而不是由该符号链接引用文件的信息，除此之外，同于`stat`

第二个参数`buf`是指针，指向一个结构体，这些函数填写该结构体，该结构在不同实现中有所不同，但基本形式为：

```c
struct stat {
    mode_t st_mode; /* file type & mode (permissions) */
    ino_t st_ino; /* i-node number (serial number) */
    dev_t st_dev; /* device number (file system) */
    dev_t st_rdev; /* device number for special files */
    nlink_t st_nlink; /* number of links */
    uid_t st_uid; /* user ID of owner */
    gid_t st_gid; /* group ID of owner */
    off_t st_size; /* size in bytes, for regular files */
    struct timespec st_atim; /* time of last access */
    struct timespec st_mtim; /* time of last modification */
    struct timespec st_ctim; /* time of last file status change */
    blksize_t st_blksize; /* best I/O block size */
    blkcnt_t st_blocks; /* number of disk blocks allocated */
};

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

```

> 2008版本以前，时间字段为`st_atime/st_mtime/st_ctime`，都是`time_t`类型的（以秒来表示），`timespec`提供了更高精度的时间戳，为了保持兼容，旧的名字可以定义成`tv_sec`成员，如`st_atime`定义为`st_atim.tv_sec`


## 文件类型
- __普通文件__，至于是文本还是二进制，对内核无区别，对普通文件内容的解释由应用程序进行
> 一个例外是二进制可执行文件，为了执行它，内核必须理解其格式，所有二进制可执行文件都遵循一种格式，这种格式能使内核确定程序文件和数据的加载位置

- __目录文件__，包含了其他文件的名字以及指向这些文件有信息的指针，具有读权限的任一进程可以读该目录内容，但只有内核可以直接写目录
- __块特殊文件__，提供对设备（如磁盘）带缓冲的访问，每次访问以固定长度为单位进行
- __字符特殊文件__，提供对设备不带缓冲的访问，每次访问长度可变，系统中 __所有设备__ 要么是字符特殊文件，要么是块特殊文件
- __FIFO__，用于进程间通信，有时也称为命名管道
- __套接字__，用于进程间的网络网络通信，也可用于一台宿主上进程之间的非网络通信
- __符号链接__，这种文件类型指向另一个文件

可用下面的宏来确定文件类型，宏的参数即`stat`结构的`st_mode`成员

```c
S_ISREG()      // regular file
S_ISDIR()      // directory file
S_ISCHR()      // character special file
S_ISBLK()      // block special file
S_ISFIFO()     // pipe or FIFO
S_ISLNK()      // symbolic link
S_ISSOCK()     // socket
```

```shell
$ ./a.out /etc/passwd /etc /dev/log /dev/tty \
> /var/lib/oprofile/opd_pipe /dev/sr0 /dev/cdrom
/etc/passwd: regular
/etc: directory
/dev/log: socket
/dev/tty: character special
/var/lib/oprofile/opd_pipe: fifo
/dev/sr0: block special
/dev/cdrom: symbolic link
```

在linux上编译该程序，必须定义`_GNU_SOURCE`，这样就能包括`S_ISSOCK`宏定义

> 早期版本并不提供宏，需要与屏蔽字`S_IFMT`进行逻辑与运算，然后与`S_IFxxx`常量进行比较

```
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
```

## 设置用户ID和组ID
与一个进程相关的ID：

- 我们实际上谁，这两个字段在登录时取自口令文件中的登录项，通常一个登录会话间这些值并不改变，但是超级用户进程有方法改变它们
    - 实际用户ID(real user ID)
    - 实际组ID(real group ID)
- 用于文件访问权限检查
    - 有效用户ID(effective user ID)
    - 有效组ID(effective group ID)
    - 附加组ID(supplementary group IDs)
- 由exec函数保存
    - 保存的设置用户ID(saved set-user-ID)
    - 保存的设置组ID(saved set-group-ID)

当执行一个程序文件时，进程的有效用户ID等于实际用户ID，有效组ID等于实际组ID，但是可以在文件模式字`st_mode`中设置一个特殊标志，其含义是，当执行此文件时，将进程的有效用户ID设置为`st_uid`，(shell中为`chmod u+s xxx`)，同理，文件模式中可以设置另一位，将执行此文件的进程的有效组ID为文件的组所有者ID`st_gid`

若文件所有者`st_uid`是超级用户，并且设置了`st_mode`中的`set-user-ID`bit，然后该程序由另一进程执行时，则该进程具有超级用户特权，不管执行此文件的进程的实际用户ID是什么，如`/usr/bin/passwd`，允许任一用户改变口令

`set-user-ID`和`set-group-ID`bit包含在`st_mode`中，通过宏`S_ISUID`和`S_ISGID`测试

## 文件访问权限
`st_mode`值也包含了针对文件的访问权限位

```table

st_mode mask | Meaning
---------|--------------
S_IRUSR  |  user-read
S_IWUSR  |  user-write
S_IXUSR  |  user-execute
S_IRGRP  |  group-read
S_IWGRP  |  group-write
S_IXGRP  |  group-execute
S_IROTH  |  other-read
S_IWOTH  |  other-write
S_IXOTH  |  other-execute

```

```c
assert((buf.st_mode & S_IXUSR) == S_IXUSR);
```

- 用名字打开任一类型的文件时，对该名字中的 __包含每一个目录，以及当前工作目录__，都应具有 __执行权限__（执行权限位常被称为搜索位）

> 对于目录的读权限和执行权限的意义是 __不同__ 的，读权限允许我们读目录，获得在该目录中所有文件名的列表。当一个目录是要访问文件的路径名的一个组成部分时，对该目录的执行权限可使我们通过该目录

> 如果`PATH`环境变量指定了一个我们不具有执行权限的目录，那么shell决不会在该目录下找到可执行文件

- 对于一个读权限决定了我们是否能够打开该文件进行读操作，与`open`的`O_RDONLY`和`O_RDWR`标志相关
- 对于一个写权限决定了我们是否能够打开该文件进行写操作，与`open`的`O_WRONLY`和`O_RDWR`标志相关
- 为了在`open`中对一个文件指定`O_TRUNC`标志，必须对该文件具有写权限
- 为了在一个目录创建一个新文件，必须对该目录具有写权限和执行权限
- 为了删除一个现有文件，必须对包含该文件的目录具有写权限和执行权限。对该文件本身则不需要有读、写权限
- 如果用`exec`族函数中任何一个执行某个文件，都必须对该文件具有执行权限，该文件还必须是一个普通文件

进程每次打开、创建或删除一个文件时，内核就进行文件访问权限测试，而这种测试可能涉及文件的所有者（`st_uid`或`st_gid`）、进程的有效ID（有效用户ID和有效组ID）以及进程的附加组ID。__两个所有者ID是 *文件的性质*，而两个有效ID和附加组ID则是 *进程的性质*__

- 若进程的有效用户ID是0（超级用户），则允许访问
- 若进程的有效用户ID等于文件的所有者ID（也就是该进程拥有此文件），那么，若所有者适当的访问权限被设置，则允许访问，否则拒绝访问
- 若进程的有效组ID或进程的附加组ID之一等于文件的组ID，那么，若组适当的访问限位被设置，则允许访问，否则拒绝访问
- 若其他用户适当的访问权限位被设置，则允许访问，否则拒绝访问


## 新文件和目录的所有权
新文件的用户ID设置为进程的有效用户ID，新文件的组ID可以是进程的有效组ID，也可以是它所在目录的组ID

> As we mentioned earlier, this option for group ownership is the default for FreeBSD 8.0 and Mac OS X 10.6.8, but an option for Linux and Solaris. Under Solaris 10, and by default under Linux 3.2.0, we have to enable the set-group-ID bit, and the mkdir function has to propagate a directory’s set-group-ID bit automatically for this to work.

## access/faccessat函数
```c
#include <unistd.h>
int access(const char *pathname, int mode);
int faccessat(int fd, const char *pathname, int mode, int flag);
//Both return: 0 if OK, −1 on error
```

其中`mode`为

```table
mode | Description
-----|--------------
R_OK | test for read permission
W_OK | test for write permission
X_OK | test for execute permission
```

## umask函数
```c
#include <sys/stat.h>
mode_t umask(mode_t cmask);
//Returns: previous file mode creation mask
```

为进程设置文件模式创建屏蔽字，并返回以前的值，这是少数几个没有出错返回的，其中`cmask`是9个常量`S_IRUSR，S_IWUSR，S_IXUSR，S_IRGRP，S_IWGRP，S_IXGRP，S_IROTH，S_IWOTH，S_IXOTH`中的若干个按位 或 构成的，对于任何在文件模式创建屏蔽字中为1的位，在文件`mode`中的相应位则一定被关闭

unix系统大多数用户从不处理他们的`umask`值，通常在登录时，由shell启动文件设置一次，然后从不改娈。尽管如此，当编写创建新文件的程序时，还是确保`umask(0);`，否则，当我们进程运行时，有效的`umask`值有可能关闭该权限位

更改进程的文件模式创建屏蔽字，不会影响父进程的屏蔽字

常用的几种`umask`值为002,022,027，`umask -S`打印符号形式

```table
Mask bit | Meaning
---------|-----------
0400     | user-read
0200     | user-write
0100     | user-execute
0040     | group-read
0020     | group-write
0010     | group-execute
0004     | other-read
0002     | other-write
0001     | other-execute
```

## chmod/fchmod/fchmodat函数
```c
#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int fd, const char *pathname, mode_t mode, int flag);
//All three return: 0 if OK, −1 on error
```

```table
mode       |  Description
-----------|-------------------------
S_ISUID    |  set-user-ID on execution
S_ISGID    |  set-group-ID on execution
S_ISVTX    |  saved-text (sticky bit)
S_IRWXU    |  read, write, and execute by user (owner)
   S_IRUSR |  read by user (owner)
   S_IWUSR |  write by user (owner)
   S_IXUSR |  execute by user (owner)
S_IRWXG    |  read, write, and execute by group
   S_IRGRP |  read by group
   S_IWGRP |  write by group
   S_IXGRP |  execute by group
S_IRWXO    |  read, write, and execute by other (world)
   S_IROTH |  read by other (world)
   S_IWOTH |  write by other (world)
   S_IXOTH |  execute by other (world)
```

![img](../../imgs/apue_08.png)

除了以前的9个文件访问权限位，另外加了6项，两个设置id常量（`S_ISUID/S_ISGID`），保存正文常量（`S_ISVTX`），以及三个组合常量（`S_IRWXU/S_IRWXG/S_IRWXO`）

`chmod`函数更新的只是i节点最近一次被更改的时间，而`ls`命令列出的是最后修改文件内容的时间

## 粘住位
`S_ISVTX`位被称为粘住位

目录`/tmp`为`drwxrwxrwt`，任何用户都可以在这目录中创建文件，任一用户对这目录都是读、写和执行，但用户不能删除或更名属于其他人的文件

> linux中粘住位对普通文件无意义


## chown/fchown/fchownat/lchown函数
```c
#include <unistd.h>
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int fchownat(int fd, const char *pathname, uid_t owner, gid_t group, int flag);
int lchown(const char *pathname, uid_t owner, gid_t group);
//All four return: 0 if OK,−1 on error
```

在符号链接的情况下，`lchown`更改符号链接本身的所有者，而不是该符号所指向的文件

若两个参数`owner`或`group`中任意一个是-1，则对应ID不变

## 文件长度
`stat`结构成员`st_size`表示以字节为单位的文件长度，只对普通文件、目录文件和符号链接有意义

对于目录，文件长度通常是一个数（16或256)的倍数，对于符号链接，文件长度是 文件名 中的实际字节数

```
lrwxrwxrwx  1 root            7 Sep 25 07:14 lib -> usr/lib
```

其中，文件长度7就是路径名`usr/lib`的长度

`stat`结构成员`st_blksize`是对文件IO较合适的块长度，当将它用于读操作时，读一个文件所需的时间最少；`st_blocks`是所分配的实际512字节块数量


### 文件中的空洞
`ls -l`得到文件长度，但`ls -ls`或`du -s`可以得到实际占磁盘多少个字节块(512大小的)

`wc -c`得到文件中字符（字节）数，包括没写过的字节位置（空洞），使用`cat`再重定向输出，则把所有空洞填满


## 文件截短
```c
#include <unistd.h>
int truncate(const char *pathname, off_t length);
int ftruncate(int fd, off_t length);
//Both return: 0 if OK, −1 on error
```

当`length`大于以前文件长度，遵循XSI的系统将增加文件长度，即创建一个空洞

## 文件系统

![img](../../imgs/apue_04.png)

![img](../../imgs/apue_05.png)

- 图中有两上目录项指向同一个i节点，每一个i节点都有一个链接计数，其值指向该i节点的目录项数，只有当链接计数减少至0时，才可以删除该文件（释放该文件占用的数据块），所以，删除一个目录项称为`unlink`而不是`delete`的原因
- 在`stat`结构中，链接计数包含在`st_nlink`成员中，这种链接称为硬链接
- 另一种链接称为符号链接，该文件的实际内容（在数据块中）包含了该符号所指向的文件的名字，该i节点中的文件类型是`S_IFLNK`
- i节点包含了大多数与文件有关的信息：文件类型、文件访问权限位、文件长度、指向该文件所占用的数据块的指针等，`stat`结构中大多数信息都取自i节点，只有文件名和i节点编号（类型是`ino_t`）取自目录项
- 每个文件系统各自对它们的i节点进行编号，目录项中的i节点编号数指向同一文件系统中的相应i节点，不能使一个目录项指向另一个文件系统的i节点，这也是为什么`ln`命令不能跨越文件系统的原因
- 当不更换文件系统情况下为一个文件更名时，该文件的实际内容并未移动，只需构造一个指向现有i节点的新目录项，并解除与旧目录项的链接


对于目录文件的链接计数字段又如何？

![img](../../imgs/apue_06.png)

任何一个叶目录（不包含任何其他目录的目录）的链接计数总是2，一个是命名该目录的目录项，另一个是该目录项中`.`项，如果包含子目录，则链接计数至少为3，多一个子目录，子目录中的`..`项，使父目录的链接计数增1

## link/linkat/unlink/unlinkat/remove/rename函数

```c
#include <unistd.h>
int link(const char *existingpath, const char *newpath);
int linkat(int efd,const char *existingpath, int nfd, const char *newpath, int flag);
//Both return: 0 if OK, −1 on error
```

```c
#include <unistd.h>
int unlink(const char * pathname );
int unlinkat(int fd, const char *pathname, int flag);
//Both return: 0 if OK, −1 on error
```

```c
#include <stdio.h>
int remove(const char * pathname );
//Returns: 0 if OK,−1 on error
```

- 创建目录项以及增加链接计数应当是个原子操作，大多数系统不允许对目录的硬链接
- 为了解除对文件的链接，必须对包含该目录项的目录具有写和执行权限，如果对该目录设置了粘住位，则对该目录必须具有写权限
- 当链接计数达到0时，该文件的内容才可被删除，另一个阻止删除文件夹的内容是，如果进程打开了该文件。
- 进程用`open/creat`创建一个文件，然后立即调用`unlink`，因为文件是打开的，所以不会将其内容删除，只有当进程关闭该文件或终止时，该文件内容才会被删除，这种性质经常被程序用来确保即使在该程序崩溃时，它所创建的临时文件也 __不会遗留__ 下来
- 如果是符号链接，那么`unlink`删除该符号链接，而不会删除由该链接所引用的文件，在给出符号链接名情况下，没有一个函数能直接删除由该链接所引用的文件
- `remove`函数，对于文件与`unlink`相同，对于目录与`rmdir`相同


## rename/renameat函数
```c
#include <stdio.h>
int rename(const char * oldname, const char *newname );
int renameat(int oldfd, const char *oldname, int newfd, const char *newname );
//Both return: 0 if OK, −1 on error
```

- 如果`oldname`指的是一个文件而不是目录，那么为该文件或符号链接更名，如果`newname`已存在（不能引用一个目录），先将该目录项删除，然后将`oldname`更名为`newname`，对于包含新旧文件的目录，调用进程必须具有写权限
- 如果`oldname`指的是目录，那么为该目录更名，如果`newname`已存在，必须引用一个目录，且目录为空目录，先将其删除，然后将`oldname`更名为`newname`，另，当为一个目录更名时，`newname`不能包含`oldname`作为其路径前缀
- 如果引用符号链接，则处理符号链接本身，而不是它所引用的文件



## 符号链接
硬链接指向文件i节点，而符号链接指向一个文件的间接指针，它避开了硬链接的一些限制：

- 硬链接通常要求链接和文件位于同一文件系统中
- 只有超级用户才能创建指向目录的硬链接

当使用以名字引用文件的函数时，需要了解该函数是否处理符号链接，也就是函数是否跟随符号链接到达它所链接的文件

```table
Function |Does not follow symbolic link |Follows symbolic link
---------|------------------------------|---------------------
access   |                              | •
chdir    |                              | •
chmod    |                              | •
chown    |                              | •
creat    |                              | •
exec     |                              | •
lchown   | •                            |
link     |                              | •
lstat    | •                            |
open     |                              | •
opendir  |                              | •
pathconf |                              | •
readlink | •                            |
remove   | •                            |
rename   | •                            |
stat     |                              | •
truncate |                              | •
unlink   | •                            |
```

`mkdir/mkinfo/mknod/rmdir`当路径名是符号链接时，都出错返回，`chown`是否跟随符号链接取决于实现（目前都跟随）

同时`O_CREAT`和`O_EXCL`调用`open`时，若路径名引用符号链接，将出错返回，并将`errno`设置为`EEXIST`

```shell
$ ln -s /no/such/file myfile
$ ls myfile
myfile
$ cat myfile
cat: myfile: No such file or directory
$ ls -l myfile
lrwxrwxrwx 1 sar 13 Jan 22 00:26 myfile -> /no/such/file
```

## symlink/readlink函数
创建一个符号链接，并不要求实际路径已经存在，也不要求位于同一文件系统中

```c
#include <unistd.h>
int symlink(const char *actualpath, const char *sympath);
int symlinkat(const char *actualpath, int fd, const char *sympath);
//Both return: 0 if OK, −1 on error
```

```c
#include <unistd.h>
ssize_t readlink(const char* restrict pathname, char *restrict buf, size_t bufsize);
ssize_t readlinkat(int fd, const char* restrict pathname, char *restrict buf, size_t bufsize);
//Both return: number of bytes read if OK, −1 on error
```

此函数组合了`open`,`read`,`close`的所有操作，如果执行成功，返回读入`buf`字节数，`buf`中内容不以`null`终止

## 文件的时间
```table
Field   |Description                         |Example      |ls(1) option
--------|------------------------------------|-------------|---------------
st_atim |last-access time of file data       |read         | -u
st_mtim |last-modification time of file data |write        | default
st_ctim |last-change time of i-node status   |chmod, chown | -c
```

修改时间`st_mtim`是文件内容最后一次被修改的时间，更改状态时间`st_ctim`是该文件的i节点（更改文件的访问权限、用户ID、链接数等）最后一次被修改的时间

系统并不保存一个i节点的最后一次访问时间，所以`access/stat`函数并不更改这三个时间中的任一个

![img](../../imgs/apue_07.png)

## futimens/utimensat/utimes函数
一个文件的访问和修改时间可用该函数，下面函数可以指定纳秒级精度的时间戳

```c
#include <sys/stat.h>
int futimens(int fd, const struct timespec times[2]);
int utimensat(int fd, const char *path, const struct timespec times[2], int flag);
//Both return: 0 if OK, −1 on error
```

`times`数组第一个元素包含访问时间，第二包包含修改时间

- 如果`times`参数是一个空指针，则访问时间和修改时间两者设置为当前时间
- 如果`times`参数某一元素中的`tv_nsec`字段的值为`UTIME_NOW`，相应的时间戳为当前时间，忽略`tv_sec`
- 如果`times`参数某一元素中的`tv_nsec`字段的值为`UTIME_OMIT`，相应的时间戳不变，忽略`tv_sec`
- 除维持时间戳不变，即`times`的两个元素中的`tv_nsec`字段的值均为`UTIME_OMIT`，否则，要么为超级用户进程，要么进程的有效用户ID等于该文件的所有者ID，则该进程对该文件必须具有写权限


```c
#include <sys/time.h>
int utimes(const char *pathname, const struct timeval times[2]);
//Returns: 0 if OK,−1 on error
```

我们不能对更改状态时间`st_ctim`指定一个值，当调用`utime`函数时，此字段将被自动更新


## mkdir/mkdirat/rmdir函数
```c
#include <sys/stat.h>
int mkdir(const char *pathname, mode_t mode);
int mkdirat(int fd, const char *pathname, mode_t mode);
//Both return: 0 if OK, −1 on error
```

所指定的文件访问权限mode，由进程的文件模式mask修改

```c
#include <unistd.h>
int rmdir(const char *pathname);
//Returns: 0 if OK,−1 on error
```

如果调用此函数使目录的链接计数成0，并且也没有其它进程打开此目录，则释放占用的空间。为了执行成功，目录必须是空的

## 读目录
为了防止文件系统产生混乱，只有内核才能写目录。一个目录的写权限位和执行权限位，并不代表能否写目录本身

```c
#include <dirent.h>
DIR *opendir(const char *pathname );
DIR *fdopendir(int fd );
//Both return: pointer if OK, NULL on error
struct dirent *readdir(DIR * dp);
//Returns: pointer if OK,NULL at end of directory or error
void rewinddir(DIR *dp);
int closedir(DIR * dp);
//Returns: 0 if OK,−1 on error
long telldir(DIR * dp);
//Returns: current location in directory associated with dp
void seekdir(DIR * dp,long loc);
```

## chdir/fchdir/getcwd函数
```c
#include <unistd.h>
int chdir(const char *pathname );
int fchdir(int fd );
//Both return: 0 if OK, −1 on error
```

当前工作目录是进程的一个属性，它只影响调用`chdir`的进程本身，而不影响其他进程

如果`open`或`creat`创建已经存在的文件，则该文件的访问权限位不变

```c
#include <unistd.h>
char *getcwd(char *buf,size_tsize);
//Returns:bufif OK,NULLon error
```

取得当前工作目录的绝对路径

标准IO库
========
## 流和FILE对象
流的定向决定了所读、写的字符是单字还是多字节的，当一个流最初被创建时，并没有定向，在其流上使用一个多字节IO函数（`wchar.h`），则将该流的定向设置为 __宽定向__ 的，如使用一个单字节IO函数，则将该流的定向设置为 __字节定向__ 的。`freopen()`清除一个流的定向，`fwide()`设置流的定向

```c
#include <stdio.h>
#include <wchar.h>
int fwide(FILE *fp, int mode);
//Returns: positive if stream is wide oriented,
//negative if stream is byte oriented,
//or 0 if stream has no orientation
```

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

```c
#include <stdio.h>
void setbuf(FILE *restrict fp, char *restrict buf );
int setvbuf(FILE *restrict fp, char *restrict buf, int mode, size_t size);
//Returns: 0 if OK, nonzero on error
```

mode参数：

- `_IOFBF`全缓冲
- `_IOLBF`行缓冲
- `_IONBF`不带缓冲

![img](../../imgs/apue_09.png)

一般而言，应由系统选择缓冲区的长度，并自动分配缓冲区，在此情况下关闭此流时，标准IO库将自动释放缓冲区

```c
#include <stdio.h>
int fflush(FILE *fp);
//Returns: 0 if OK,EOF on error
```

此函数使该流所有未写的数据都被传送至内核，如果`fp`是`NULL`，则此函数将导致所有输出流被冲洗

## 打开流
```c
#include <stdio.h>
FILE *fopen(const char *restrict pathname, const char *restrict type);
FILE *freopen(const char *restrict pathname, const char *restrict type, FILE *restrict fp);
FILE *fdopen(int fd, const char *type);
//All three return: file pointer if OK,NULL on error
```

- `fopen`打开一个指定的文件
- `freopen`在一个指定的流上打开一个指定的文件，如流已打开，则先关闭该流，如流已定向，则清除该定向。此函数一般用于将一个指定的文件打开为一个预定义的流：标准输入、标准输出或标准出错
- `fdopen`获取一个现有的文件描述符(`open/dup/fntl/pipe/socket...`)，并使一个标准的IO流与该描述符相结合。常用于由创建管道和网络通信通道函数返回的描述符，因为这些特殊文件不能用标准的`fopen`打开

```table
type              |open(2) Flags                 |Description
------------------|------------------------------|---------------
r or rb           |`O_RDONLY`                    |open for reading
w or wb           |`O_WRONLY｜O_CREAT｜O_TRUNC ` |truncate to 0 length or create for writing
a or ab           |`O_WRONLY｜O_CREAT｜O_APPEND` |append; open for writing at end of file, or create for writing
r+ or r+b or rb+  |`O_RDWR`                      |open for reading and writing
w+ or w+b or wb+  |`O_RDWR｜O_CREAT｜O_TRUNC`    |truncate to 0 length or create for reading and writing
a+ or a+b or ab+  |`O_RDWR｜O_CREAT｜O_APPEND`   |open or create for reading and writing at end of file
```

对于`fdopen`为`tyep="wb"`而打开，并不截短该文件，`type="a"`也不会创建文件

当添写类型打开一文件后，则每次都将数据写到文件的当前尾端处，多个进程打开了同一文件，每个进程的数据都将正确的写到文件中

当 __读和写__ 打开一个文件时，即`type`中有`+`时:

- 如果中间没有`fflush/fseek/fsetpos/rewind`，则在输出的后面不能直接跟随输入
- 如果中间没有`fseek/fsetpos/rewind`，或者一个输入操作没有到达文件尾端，则在输入之后不能直接跟参输出

```table
Restriction                          |r  |w  |a   |r+  |w+  |a+
-------------------------------------|---|---|----|----|----|-----
file must already exist              |•  |   |    |•   |    |
previous contents of file discarded  |   |•  |    |    |•   |
stream can be read                   |•  |   |    |•   |•   |•
stream can be written                |   |•  |•   |•   |•   |•
stream can be written only at end    |   |   |•   |    |    |•
```

在指定`w/a`类型创建一个新文件时，无法说明该文件的访问权限位（`open/creat`则能做到）

若流引用终端设备，则流是行缓冲的，否则按系统默认的情况，流被打开时是全缓冲的，有需要，可通过`setbuf/setvbuf`改变缓冲的类型

```c
#include <stdio.h>
int fclose(FILE *fp );
//Returns: 0 if OK,EOF on error
```

在该文件被关闭之前，冲洗缓冲区的输出数据，丢弃缓冲区的任何输入数据，如果标准IO为流`malloc`了一人缓冲区，则释放此缓冲区

当一个进程正常终止时（调用`exit`或从`main`返回），则所有带未写缓冲数据的标准IO流都会被冲洗，所有打开的标准IO流都会被关闭

## 读和写流

- 每次一个字符的IO，如果流是带缓冲的，则标准IO会处理所有缓冲
- 每次一行的IO，使用`gfets/fputs`，当调用`fgets`，应能说明处理的最大行长
- 直接IO，`fread/fwrite`

### 输入函数
```c
#include <stdio.h>
int getc(FILE *fp );
int fgetc(FILE *fp );
int getchar(void);
//All three return: next character if OK,EOF on end of file or error
```

`getchar`等价于`getc(stdin)`，`getc`可被实现为宏，而`fgetc`则是函数，后者可以作为参数传参

不管是出错还是到达文件尾端，这三个函数都返回同样的值`EOF`，为了区分这两种不同情况，必须调用`ferror/feof` 

```c
#include <stdio.h>
int ferror(FILE *fp);
int feof(FILE *fp);
//Both return: nonzer o(true) if condition is true, 0 (false) otherwise
void clearerr(FILE *fp);
```

```c
#include <stdio.h>
int ungetc(int c, FILE *fp);
//Returns: c if OK, EOF on error
```

回送的字符不必是上一次读到的字符，不能回送`EOF`，已到达文件尾端时，仍可以回送一字符，下次读将返回该字符，再次读则返回`EOF`

> `ungetc`并没将字符写到文件中设备中，只写回标准IO的流缓冲区中

### 输出函数
```c
#include <stdio.h>
int putc(int c, FILE *fp);
int fputc(int c, FILE *fp);
int putchar(int c);
//All three return:c if OK, EOF on error
```

同理，`putchar(c)`等价于`putc(c, stdout)`，`putc`可实现为宏，而`fputc`则为函数

## 每次一行IO
```c
#include <stdio.h>
char *fgets(char *restrict buf, int n, FILE *restrict fp);
char *gets(char *buf);
//Both return:buf if OK, NULL on end of file or error
```

对于`fgets`，必须指定缓冲区的长度`n`，一直读到下一个换行符为止，但不超过`n-1`个字符，缓冲区以`null`字符结尾，若该行（包括换行符）的字符数超过`n-1`，则返回一个不完整的行，下次调用继续读该行

`gets` __不推荐__ 使用，容易造成缓冲区溢出，与`fgets`另一个区别是，它不将换行符存入缓冲区中

```c
#include <stdio.h>
int fputs(const char *restrict str, FILE *restrict fp);
int puts(const char *str);
//Both return: non-negative value if OK, EOF on error
```

`fputs`将一个`null`符终止的字符串写到指定的流，尾端`null`不写出，通常`null`之前是一个换行符

`puts`会自动将换行符写到标准输出，也 __不推荐__ 使用，推荐总是使用`fgets/fputs`，手工处理换行符

## 标准IO的效率

如果`fgets/fputs`是用`getc/putc`实现的，那么，可以预期`fgets`版本的时间会和`getc`相接近，但`fgets`是用`memccpy`（使用汇编）实现的，会有较高的速度


## 二进制IO
```c
#include <stdio.h>
size_t fread(void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
size_t fwrite(const void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
//Both return: number of objects read or written
```

在一个系统上写的数据，要在另一个系统上进行处理，可能不能正常工作：

- 在一个结构中，同一成员的偏移量可能因编译器和系统而异：如紧密包装、内存对齐
- 用来存储多字节整数和浮点值的二进制格式在不同机器体系结构间也可能不同

## 定位流
- `ftell/fseek`，假定文件的位置可以存放在一个长整型中
- `ftello/fseeko`，使用`off_t`数据类型代替了长整型
- `fgetpos/fsetpos`，使用一个抽象数据类型`fpos_t`记录文件的位置，移植到非UNIX系统上应当使用它

```c
#include <stdio.h>
long ftell(FILE *fp );
//Returns: current file position indicator if OK, −1L on error
int fseek(FILE *fp, long offset,int whence );
//Returns: 0 if OK,−1 on error
void rewind(FILE * fp );
```

```c
#include <stdio.h>
off_t ftello(FILE *fp );
//Returns: current file position indicator if OK, (off_t)−1 on error
int fseeko(FILE *fp, off_t offset,int whence );
//Returns: 0 if OK,−1 on error
```

```c
#include <stdio.h>
int fgetpos(FILE *restrict fp, fpos_t *restrictpos);
int fsetpos(FILE * fp, const fpos_t * pos);
//Both return: 0 if OK, nonzero on error
```

## 格式化IO
### 格式化输出

```c
#include <stdio.h>
int printf(const char *restrict format, ...);
int fprintf(FILE *restrict fp, const char *restrict format, ...);
int dprintf(intfd, const char *restrict format, ...);
//All three return: number of characters output if OK, negative value if output error
int sprintf(char *restrict buf,const char *restrict format, ...);
//Returns: number of characters stored in array if OK, negative value if encoding error
int snprintf(char *restrict buf,size_tn, const char *restrictformat, ...);
//Returns: number of characters that would have been stored in array
//if buffer was large enough, negative value if encoding error
```

`sprintf`在数组的尾端自动加一个`null`字节，但该字节不包括在返回值中。该函数有可能造成`buf`指向的缓冲区的溢出，调用者有责任确保该缓冲区足够大

`snprintf`解决缓冲区溢出问题，缓冲区长度是一个显式参数，超过的任何字符都会被丢弃，返回的是写入缓冲区的字符数，同上，不包括`null`字节，若发生错误，则返回负值

`%[flags][fldwidth][precision][lenmodifier]convtype`

![img](../../imgs/apue_10.png)

- `fldwidth`说明转换的最小字段宽度，字符少时用空格填充它，是一个非负十进制数，或一个星号(`*`，占位符，具体值由后续的参数给出)
- `precision`说明整型转换后最小输出数字位数、浮点数转换后小数点后的最少位数、字符转换后的最大字符数。精度是一个句点(`.`)，后接一个可选的非负十进制整数或一个星号(`*`)
- 宽度和精度字段两者皆可为`*`，此时，一个整型参数指定宽度或精度的值。该整型参数正好位于被转换的参数之前
- `lenmodifier`说明参数长度

![img](../../imgs/apue_11.png)

- `convtype`是必选的，它控制如何解释参数

![img](../../imgs/apue_12.png)

下面是变体，可变参数代换成了`arg`

```c
#include <stdarg.h>
#include <stdio.h>
int vprintf(const char *restrict format, va_list arg);
int vfprintf(FILE *restrict fp, const char *restrict format, va_list arg);
int vdprintf(int fd, const char *restrict format, va_list arg);
//All three return: number of characters output if OK, negative value if output error
int vsprintf(char *restrict buf,const char *restrict format, va_list arg);
//Returns: number of characters stored in array if OK, negative value if encoding error
int vsnprintf(char *restrict buf,size_tn, const char *restrictformat, va_list arg);
//Returns: number of characters that would have been stored in array
//if buffer was large enough, negative value if encoding error
```

### 格式化输入

```c
#include <stdio.h>
int scanf(const char *restrictformat ,...);
int fscanf(FILE *restrictfp ,const char *restrict format ,...);
int sscanf(const char *restrict buf,const char *restrict format ,...);
//All three return: number of input items assigned,
//EOF if input error or end of file beforeany conversion
```

除转换说明和空白字符外，格式字符串中的其他字符必须与输入匹配，若有一个字符不匹配，则停止后续处理，不再读入输入的其余部分

`%[*][fldwidth][lenmodifier]convtype`

- `*`用于抑制转换，按照转换说明的其余部分对输入进行转换，但转换结果并不存放在参数中
- `fldwidth`说明最大宽度
- `lenmodifier`说明要用转换结果初始化的参数大小，同输出函数的长度修饰符
- `convtype`类似输出函数，但还有些差别，如存储在无符号类型中的结果可输入时带上符号，如-1可被转换成4294967295赋予无符号整型变量

![img](../../imgs/apue_13.png)

支持的可变参数

```c
#include <stdarg.h>
#include <stdio.h>
int vscanf(const char *restrict format ,va_list arg);
int vfscanf(FILE *restrict fp ,const char *restrict format ,va_list arg);
int vsscanf(const char *restrict buf,const char *restrict format ,va_list arg);
//All three return: number of input items assigned,
//EOF if input error or end of file beforeany conversion
```

## 实现细节
对一个流调用`fileno()`得到其描述符

```c
#include <stdio.h>
int fileno(FILE *fp);
//Returns: the file descriptor associated with the stream
```

在打印缓冲状态信息之前，需要对每个流执行IO操作，第一个IO操作通常就造成为该流分配缓冲。结构体成员`_IO_file_flags/_IO_buf_base/_IO_buf_end/_IO_UNBUFFERED/_IO_LINE_BUFFERED`由GUN标准IO库定义的

当标准输入、输出连至终端时，它们是行缓冲的，长度是1024，当重定向到普通文件时，则变成全缓冲的，长度是`stat.st_blksize`(4096)

## 临时文件

```c
#include <stdio.h>
char *tmpnam(char *ptr);
//Returns: pointer to unique pathname
FILE *tmpfile(void);
//Returns: file pointer if OK, NULL on error
```

若`ptr`是`NULL`，则所产生的路径名存放在一个静态区，并将指向该静态区的指针返回，下一次调用时，会 __重写__ 该静态区（想保存路径名，则应保存路径名的副本），若不是`NULL`，则认为它指向长度至少是`L_tmpnam`个字符的数组，所产生的路径名存放在该数组中，`ptr`也作为函数值返回。

`tmpfile`创建一个临时二进制文件（类型`wb+`），在关闭该文件或程序结束将自动删除这种文件

```c
#include <stdlib.h>
char *mkdtemp(char *template );
//Returns: pointer to directory name if OK,NULL on error
int mkstemp(char * template );
//Returns: file descriptor if OK,−1 on error
```

与`tempfile`不同，它们所创建的临时文件不会自动删除(需要手工`unlink`)，`tmpnam/tempnam`不足之处是，在返回唯一路径名和应用程序用该路径创建文件之间有一个时间窗口，不同进程可能创建同名文件，而`mkstemp`不会产生此问题

## 内存流
目前只有linux 3.2.0支持内存流

```c
#include <stdio.h>
FILE *fmemopen(void *restrict buf,size_t size, const char *restrict type);
//Returns: stream pointer if OK,NULLon error
```

如果`buf`为空，则函数分配`size`字节的缓冲区，当流关闭时，缓冲区被释放

```table
type             | Description
-----------------|------------
r or rb          | open for reading
w or wb          | open for writing
a or ab          | append; open for writing at first null byte
r+ or r+b or rb+ | open for reading and writing
w+ or w+b or wb+ | truncate to 0 length and open for reading and writing
a+ or a+b or ab+ | append; open for reading and writing at first null byte
```

`type`参数取值与标准IO类似，但有微小差别：

- 无论何时以追加写方式打开内存流时，当前文件位置为缓冲区中的第一个null字节，如果不存在null字节，则当前位置就设为缓冲区结尾的后一个字节
> 内存流不适合存储二进制数据（适合字符串），二进制数据在数据尾端之前就可以包含多个null字节
- 当流并不是以追加写方式打开时，当前位置为缓冲区的开始位置
- 如果`buf`参数是一个null指针，打开流进行读或写没有任何意义，无法找到缓冲区地址，只写打开流，意味着无法再读了，只读方式打开意味着只能读取那些无意义的数据
- 任何时候需要增加流缓冲区的数据量，以及调用`fclose/fflush/fseek/fseeko/fsetpos`时，都会在当前位置写入一个null字节

用于创建内存流的另外两个函数，一个是面向字节的，另一个是面向宽字节的

```c
#include <stdio.h>
FILE *open_memstream(char **bufp,size_t *sizep);
#include <wchar.h>
FILE *open_wmemstream(wchar_t **bufp,size_t *sizep);
//Both return: stream pointer if OK,NULLon error
```

- 创建的流只能写打开
- 不能指定自己的缓冲区，但可通过`bufp`和`sizep`参数访问缓冲区地址和大小
- 关闭流后需要自行释放缓冲区
- 对流添加字节会增加缓冲区大小
- 缓冲区地址和长度只有在调用`fclose/fflush`后才有效，这些值只有在下一次流写入或调用`fclose`前才有效（因为缓冲区可以增长，可能需要重新分配）


系统数据文件和信息
===============
## 口令文件
`<pwd.h>`中定义的`passwd`结构

![img](../../imgs/apue_14.png)

为了阻止一个特定用户登录系统:

- 使用`/dev/null`作为登录shell，如`squid`用户，它是一个设备（很多服务对帮助它们得以实现的不同守护进程使用不同的用户ID，`squid`项为实现squid代理缓冲服务而设置的），阻止任何人以该用户名义登录到系统
- 将`/bin/false`用作登录shell，将以不成功（非0）状态终止，该shell将此种终止状态判断为假
- 将`/bin/true`禁止一个账户，将以成功（0）状态终止
- 某此系统提供`nologin`命令，打印可自定义的出错信息，然后以非0状态终止

`nobody`用户名的目的是，使任何人都可登录至系统，但其用户和组不提供任何特权

POSIX.1定义了两个获取口令文件项的函数，在给出用户登录名或数值用户ID后，这两个函数就能查询相关项

```c
#include <pwd.h>
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
//Both return: pointer if OK, NULL on error
```

`getpwuid`由`ls(1)`使用，它将i节点中的数值用户ID映射为用户登录名；`getpwnam`由`login(1)`使用

如果需要查看整个口令文件，下面函数用于此目的

```c
#include <pwd.h>
struct passwd *getpwent(void);
//Returns: pointer if OK,NULL on error or end of file
void setpwent(void);
void endpwent(void);
```

`getpwent`返回口令文件中的下一个记录项，返回一个由它填写好的结构的指针，每次调用都重写该结构。在第一次调用它时，它打开所使用的各个文件。

`setpwent`rewind它所使用的文件（定位到文件开始处），`endpwent`则关闭这些文件

一个`getpwnam`实现

```c
#include <pwd.h>
#include <stddef.h>
#include <string.h>

struct passwd *
getpwnam(const char *name){
    struct passwd *ptr;
    setpwent();
    while ((ptr = getpwent()) != NULL)
        if (strcmp(name, ptr->pw_name) == 0)
            break;  /* found a match */
    endpwent();
    return(ptr);  /* ptr is NULL if no match found */
}
```

`getpwnam/getpwuid`调用完成后不应使有关文件仍处于打开状态，所以应调用`endpwent`关闭它们


## 阴影口令
`etc/shadow`文件中的字段

```table
Description                             | struct spwd member
----------------------------------------|------------------
user login name                         | char *sp_namp
encrypted password                      | char *sp_pwdp
days since Epoch of last password change| int  sp_lstchg
days until change allowed               | int  sp_min
days before change required             | int  sp_max
days warning for expiration             | int  sp_warn
days befor eaccount inactive            | int  sp_inact
days since Epoch when account expires   | int  sp_expire
reserved                                | unsigned int sp_flag
```

用于访问阴影口令文件的函数

```c
#include <shadow.h>
struct spwd *getspnam(const char *name);
struct spwd *getspent(void);
//Both return: pointer if OK, NULL on error
void setspent(void);
void endspent(void);
```

## 组文件
`/etc/group`中字段

```table
Description       |struct group member|POSIX.1|FreeBSD 8.0|Linux 3.2.0|Mac OS X 10.6.8|Solaris 10
------------------|-------------------|-------|----------|----------|--------------|---------
group name        | char  *gr_name    |•      |•         |•         |•             |•
encrypted password| char  *gr_passwd  |       |•         |•         |•             |•
numerical group ID| int  gr_gid       |•      |•         |•         |•             |•
array of pointers to individual user names| char **gr_mem |•|•|•    |•             |•
```

下面的函数来查看属于组名或数值组ID

```c
#include <grp.h>
struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *name);
//Both return: pointer if OK, NULL on error
```

如需搜索整个组文件，则需要另外几个函数

```c
#include <grp.h>
struct group *getgrent(void);
//Returns: pointer if OK,NULL on error or end of file
void setgrent(void);
void endgrent(void);
```

## 附加组ID
执行`newgrp(1)`，将组ID更改为新的组ID，不带参数则返回原来的组，创建的文件的gid则为切换后的

不仅可以属于口令文件记录项中组ID所对应的组，也可属于多达16个另外的组，文件访问权限也为：不仅将进程的有效组ID与文件的组ID相比较，而且也将所有附加组ID与文件的组ID进行比较

使用附加组则不必显式的经常更改组

```c
#include <unistd.h>
int getgroups(int gidsetsize ,gid_t grouplist []);
//Returns: number of supplementary group IDs if OK,−1 on error
#include <grp.h> /* on Linux */
#include <unistd.h> /* on FreeBSD, Mac OS X, and Solaris */
int setgroups(int ngroups,const gid_tgrouplist []);
#include <grp.h> /* on Linux and Solaris */
#include <unistd.h> /* on FreeBSD and Mac OS X */
int initgroups(const char *username ,gid_t basegid );
//Both return: 0 if OK, −1 on error
```

作为一个特例，若`gidsetsize`为0，则函数只返回附加组数，而对数组`grouplist`不作修改，使调用者可以确定`grouplist`的长度

后两个是特权操作，只有超级用户才能调用，`login(1)`在用户登录时调用该函数


## 实现的区别
```table
Information           | FreeBSD 8.0        | Linux 3.2.0 | Mac OS X 10.6.8   | Solaris 10
----------------------|--------------------|-------------|-------------------|------------
account information   | /etc/passwd        |/etc/passwd  |Directory Services |/etc/passwd
encrypted passwords   | /etc/master.passwd |/etc/shadow  |Directory Services |/etc/shadow
hashed passwordfiles? | yes                |no           |no                 |no
group information     | /etc/group         |/etc/group   |Directory Services |/etc/group
```

## 其他数据文件
```table
Description |Data file      |Header     |Structure |Additional keyed lookup functions
------------|---------------|-----------|----------|----------------------------------
passwords   |/etc/passwd    | pwd.h     |passwd    |getpwnam, getpwuid
groups      |/etc/group     | grp.h     |group     |getgrnam, getgrgid
shadow      |/etc/shadow    | shadow.h  |spwd      |getspnam
hosts       |/etc/hosts     | netdb.h   |hostent   |getnameinfo,  getaddrinfo
networks    |/etc/networks  | netdb.h   |netent    |getnetbyname, getnetbyaddr
protocols   |/etc/protocols | netdb.h   |protoent  |getprotobyname, getprotobynumber
services    |/etc/services  | netdb.h   |servent   |getservbyname, getservbyport
```

## 登录账户记录
`utmp`文件，记录当前登录进系统的各个用户；`wtmp`跟踪各个登录和注销事件

```c
struct utmp {
    char  ut_line[8]; /* tty line: "ttyh0", "ttyd0", "ttyp0", ... */
    char  ut_name[8]; /* login name */
    long  ut_time; /* seconds since Epoch */
};
```

登录时，`login`程序填写`utmp`结构，然后写入到`/var/run/utmp`文件中，同时也将其添写到`var/log/wtmp`文件中，注销时，`init`进程将`utmp`文件中相应的记录擦除（每个字节都填0），并将一个新记录添写到`wtmp`文件中，在`wtmp`文件注销记录中，将`ut_name`字段清0。

在系统重新启动时，以及更改系统时间和日期的前后，都在`wtmp`文件中添写特殊的记录项。

`who(1)`读`utmp`文件，并以可读格式打印其内容，`last(1)`，读`wtmp`文件并打印所选择的记录

## 系统标识
`uname`函数，返回与当前主机和操作系统有关的信息

```c
#include <sys/utsname.h>
int uname(struct utsname *name);
//Returns: non-negative value if OK,−1 on error

struct utsname {
    char  sysname[]; /* name of the operating system */
    char  nodename[]; /* name of this node */
    char  release[]; /* current release of operating system */
    char  version[]; /* current version of this release */
    char  machine[]; /* name of hardware type */
};
```

`utsname`结构中的信息，通常可用`uname(1)`打印

`gethostname`函数，返回主机名，通常是TCP/IP网络上主机的名字

```c
#include <unistd.h>
int gethostname(char *name,int namelen);
//Returns: 0 if OK,−1 on error
```

如果将主机连接到TCP/IP网络中，主机名通常是该主机的完全限定域名。

`hostname(1)`可以获取和设置主机名。主机名通常在系统自举时设置，由`/etc/rc`或`init`取自一个启动文件


## 时间和日期例程
UNIX内核提供的基本时间服务是计算国际标准时间公元19700101T00:00:00以来经过的秒数，以数据类型`time_t`表示

- 以国际标准时间而非本地时间计时
- 可自动进行转换，如变换到夏时制
- 将时间和日期作为一个量值保存

```c
#include <time.h>
time_t time(time_t *calptr);
//Returns: value of time if OK, −1 on error
```

时间值总是作为函数值，如果参数不为空，也存放在指针指向的单元内

POSXI.1扩展增加了对多个系统时钟的支持，时钟通过`clockid_t`类型进行标识：

```table
Identifier                 | Option                    | Description
---------------------------|---------------------------|-------------------
`CLOCK_REALTIME`           |                           | real system time
`CLOCK_MONOTONIC`          | `_POSIX_MONOTONIC_CLOCK`  | real system time with no negative jumps
`CLOCK_PROCESS_CPUTIME_ID` | `_POSIX_CPUTIME`          | CPU time for calling process
`CLOCK_THREAD_CPUTIME_ID`  | `_POSIX_THREAD_CPUTIME`   | CPU time for calling thread
```

`clock_gettime`用取获得指定时钟的时间，当`clock_id`为`CLOCK_REALTIME`时，与`time`函数功能相似，但精度更高

```c
#include <sys/time.h>
int clock_gettime(clockid_t clock_id,struct timespec *tsp);
//Returns: 0 if OK,−1 on error
```

`clock_getres`把参数`tsp`指向的结构初始化为与`clock_id`参数对应的时钟精度。

```c
#include <sys/time.h>
int clock_getres(clockid_t clock_id,struct timespec *tsp);
//Returns: 0 if OK,−1 on error
```

对特定时钟设置时间，需要适当的特权来更改时钟值，并且有些时钟是不能修改的

```c
#include <sys/time.h>
int clock_settime(clockid_t clock_id,const struct timespec *tsp);
//Returns: 0 if OK,−1 on error
```

与`time`相比，`gettimeofday`提供了微秒级，这对某些应用很重要，SUSv4中已弃用

```c
#include <sys/time.h>
int gettimeofday(struct timeval *restrict tp ,void *restrict tzp);
//Returns: 0 always
```

一旦取得这种以秒计的整型时间值后，通常要调用另一个时间函数将其转换为人可读的时间和日期

![img](../../imgs/apue_15.png)

虚线表示的函数`localtime/mktime/strftime`等受到环境变量`TZ`影响

```c
#include <time.h>
struct tm *gmtime(const time_t *calptr);
struct tm *localtime(const time_t *calptr);
//Both return: pointer to broken-down time, NULL on error
```

`localtime`和`gmtime`将日历时间转换成以下结构，两者区别是，前者转换成本地时间并考虑夏时制，后者为国际标准时间

> 秒可以超过59表示润秒

```c
struct tm { /* a broken-down time */
    int  tm_sec; /* seconds after the minute: [0 - 60] */
    int  tm_min; /* minutes after the hour: [0 - 59] */
    int  tm_hour;  /* hours after midnight: [0 - 23] */
    int  tm_mday;  /* day of the month: [1 - 31] */
    int  tm_mon; /* months since January: [0 - 11] */
    int  tm_year;  /* years since 1900 */
    int  tm_wday;  /* days since Sunday: [0 - 6] */
    int  tm_yday;  /* days since January 1: [0 - 365] */
    int  tm_isdst; /* daylight saving time flag: <0, 0, >0 */
};
```

`mktime`以 __本地时间__ 的年月日作参数，转换成`time_t`值

```c
#include <time.h>
time_t mktime(struct tm *tmptr);
//Returns: calendar time if OK, −1 on error
```

下面函数用于定制产生字符串（`asctime/ctime`已弃用，易受缓冲区溢出问题而影响）

```c
#include <time.h>
size_t strftime(char *restrict buf, size_t maxsize,
                const char *restrict format ,
                const struct tm *restrict tmptr);
size_t strftime_l(char *restrict buf, size_t maxsize,
                  const char *restrict format,
                  const struct tm *restrict tmptr, locale_t locale);
//Both return: number of characters stored in array if room, 0 otherwise
```

`strftime_l`允许指定区域

格式化的结果放在一个长度为`maxsize`个字符的`buf`数组中，`format`是格式

![img](../../imgs/apue_16.png)

`strptime`则反过来，将字符串转换成时间

```c
#include <time.h>
char *strptime(const char *restrict buf,const char *restrict format, struct tm *restrict tmptr);
//Returns: pointer to one character past last character parsed,NULLotherwise
```

```table
Format  |Description
--------|------------
%a      |abbreviated or full weekday name
%A      |same as%a
%b      |abbreviated or full month name
%B      |same as%b
%c      |date and time
%C      |all but the last two digits of the year
%d      |day of the month: [01–31]
%D      |date[MM/DD/YY]
%e      |same as%d
%h      |same as%b
%H      |hour of the day (24-hour format): [00–23]
%I      |hour of the day (12-hour format): [01–12]
%j      |day of the year: [001–366]
%m      |month: [01–12]
%M      |minute: [00–59]
%n      |any white space
%p      |AM/PM
%r      |locale’s time (12-hour format, AM/PM notation)
%R      |time as%H:%M
%S      |second: [00–60]
%t      |any white space
%T      |time as%H:%M:%S
%U      |Sunday week number: [00–53]
%w      |weekday: [0 = Sunday,0–6]
%W      |Monday week number: [00–53]
%x      |locale’s date
%X      |locale’s time
%y      |last two digits of year: [00–99]
%Y      |year
%%      |translates to a percent sign
```

进程环境
=============

## main函数
当内核执行C程序时，使用一个`exec`函数，在调用`main`前先调用一个特殊的启动例程，可执行程序文件将此启动例程指定为程序的起始地址（由链接器设置的，而链接器则由C编译器调用）。启动例程从内核取得命令行参数和环境变量值，然后为调用`main`做好安排

## 进程终止
有8种方式使进程终止，前5种为正常终止

- 从`main`返回
- 调用`exit`
- 调用`_exit`或`_Exit`
- 最后一个线程从其启动例程返回
- 最后一个线程调用`pthread_exit`
- 调用`abort`
- 接到一个信号并终止
- 最后一个线程对取消请求做出响应

如果将启动例程以C代码表示（实际上为汇编），形式如下

```c
exit(main(argc, argv));
```

### 退出函数
`_exit`和`_Exit`立即进入内核，`exit`则先执行一些清理处理（包括调用执行各终止处理程序，关闭所有标准IO流等），然后进入内核

```c
#include <stdlib.h>
void exit(int status );
void _Exit(int status );
#include <unistd.h>
void _exit(int status );
```

前两者是ISO C说明的，后一个是POSIX.1说明的

调用这些函数时不带终止状态；`main`执行了一个无返回值的`return`；`main`没声明返回类型为整型，则该进程的终止状态是未定义的。但若`main`返回类型为整型，并且`main`执行到最后一条语句时返回，则认为终止状态为0

### atexit函数
```c
#include <stdlib.h>
int atexit(void (*func)(void));
//Returns: 0 if OK, nonzero on error
```

`exit`调用这些注册函数（终止处理程序）的顺序与它们登记时顺序相反，同一函数若登记多次，就执行多次

![img](../../imgs/apue_17.png)

## 命令行参数
ISO C和POSIX.1均要求`argv[argc]`是一个空指针，可将参数处理循环写成：

```c
for (i = 0; argv[i] != NULL; i++)
```

## 环境表
全局变量`environ`

```c
extern char **environ;
```

![img](../../imgs/apue_18.png)

通常使用`getenv/putenv`来访问特定的环境变量，而不是用`environ`(除非遍历整个环境变量)

## C程序的存储空间布局
- Text segment，CPU执行的机器指指令部分，可共享，通常为只读
- Initialized data segment，也称数据段，程序中任何函数之外，需明确赋初值的变量
- Uninitialized data segment，也称bss段，函数外的变量声明，内核将此段中数据初始化为0或空指针
- Stack，自动变量以及每次函数调用时所需保存的信息
- Heap，通常在堆中进行动态存储分配，它位于bss和stack之间

![img](../../imgs/apue_19.png)

> 32位intel x86处理器上的linux，正文段从0x08048000单元开始，栈底则在0xC0000000之下开始

未初始化段内容并不存放在磁盘程序文件中，因为内核需要在程序开始运行前将它们都设置为0，需要存放在磁盘程序文件中的段只有正文段和初始化数据段

`size(1)`报告正文段、数据段和bss段的长度（以字节为单位）

## 共享库
动态链接，减少了执行文件的长度，但增加了一些运行时开销，这种开销发生在该程序第一次被执行，或每个共享库函数第一次被调用时。

共享库另一个优点是可以用库函数的新版本代替老版本，而无需对使用该库的程序重新链接编译

```shell
$gcc -static hello1.c    #prevent gcc from using shared libraries
$ls -l a.out
-rwxr-xr-x  1 sar  879443 Sep 2 10:39 a.out
$size a.out
text    data  bss    dec    hex    filename
787775  6128  11272  805175 c4937  a.out
```

```shell
$gcc hello1.c gcc       #defaults to use shared libraries
$ls -l a.out
-rwxr-xr-x  1 sar  8378 Sep 2 10:39 a.out
$size a.out
text  data  bss dec   hex  filename
1176  504   16  1696  6a0  a.out
```

## 存储空间分配
```c
#include <stdlib.h>
void *malloc(size_tsize);
void *calloc(size_tnobj,size_tsize);
void *realloc(void *ptr,size_tnewsize);
//All three return: non-null pointer if OK,NULL on error
void free(void *ptr);
```

这三个分配函数所返回的指针一定是适当对齐的，使其可用于任何数据对象，某些特定系统上，double必须8倍数地址单元处开始，那么这此函数返回的指针都应这样对齐

> 这些分配通常用`sbrk(2)`系统调用来实现
> 
> 标准的`malloc`算法是最佳适配或首次适配，现在许多分配程序基于快速适配`auick-fit`
> 
> `alloca`在当前函数的栈上分配存储空间，而不是堆中，当函数返回时，自动释放，缺点是增加了栈的长度

## 环境变量
```c
#include <stdlib.h>
char *getenv(const char *name);
//Returns: pointer to value associated with name,NULL if not found
```

![img](../../imgs/apue_20.png)

有时需要设置环境变量，虽然能影响的只是当前进程以及后生成和调用的任何子进程，不能影响父进程，但仍然很有用

![img](../../imgs/apue_21.png)

```c
#include <stdlib.h>
int putenv(char *str);
//Returns: 0 if OK, nonzero on error
int setenv(const char *name,const char *value,int rewrite);
int unsetenv(const char *name);
//Both return: 0 if OK,−1 on error
```

- `putenv`取形式`name=value`的字符串
- `setenv`会分配存储空间，但`putenv`自由的将传递给它的参数字符串直接放到环境中，因此 __将栈中字符串传递给它会出错__

## setjmp/longjmp函数
`setjmp/longjmp`对处理发生在很深层嵌套函数调用中的出错情况非常有用，在栈上跳过若干调用帧，返回到当前函数 __调用路径__ 上的某一个函数中

在希望返回到的位置调用`setjmp`，`longjmp`用于发生错误时调用，参数`val`为非零值，方便在`setjmp`时判断是谁发生了错误

当通过`longjmp`返回到的函数中，原先的变量是否恢复，大多数标准称它们的值为不确定。如果有一个自动变量，而 __不想使其回滚__，可定义为`volatile`（将不被优化而放到寄存器中），另外，声明为全局变量或静态变量的值，在执行`longjmp`时保持不变

```c
#include <setjmp.h>
int setjmp(jmp_buf env);
//Returns: 0 if called directly,nonzero if returning from a call to longjmp
void longjmp(jmp_buf env,int val);
```

## getrlimit/setrlimit函数
查询和更改进程的资源限制

```c
#include <sys/resource.h>
int getrlimit(int resource,struct rlimit *rlptr);
int setrlimit(int resource,const struct rlimit *rlptr);
//Both return: 0 if OK,−1 on error
```

每一次调用都指定一个资源以及一个指向下列结构的指针

```c
struct rlimit {
  rlim_t  rlim_cur;  /* soft limit: current limit */
  rlim_t  rlim_max;  /* hard limit: maximum value for rlim_cur */
};
```

![img](../../imgs/apue_22.png)

- `RLIMIT_AS` The maximum size in bytes of a process’s total available memory. This affects the `sbrk` function and the `mmap` function
- `RLIMIT_CORE` The maximum size in bytes of a corefile. A limit of 0 prevents the creation of a corefile.
- `RLIMIT_CPU` The maximum amount of CPU time in seconds. When the soft limit is exceeded, the `SIGXCPU` signal is sent to the process.
- `RLIMIT_DATA` The maximum size in bytes of the data segment: the sum of the initialized data, uninitialized data, and heap
- `RLIMIT_FSIZE` The maximum size in bytes of a file that may be created. When the soft limit is exceeded, the process is sent the `SIGXFSZ` signal.
- `RLIMIT_MEMLOCK` The maximum amount of memory in bytes that a process can lock into memory using `mlock(2)`.
- `RLIMIT_MSGQUEUE` The maximum amount of memory in bytes that a process can allocate for POSIX message queues.
- `RLIMIT_NICE` The limit to which a process’s nice value can be raised to affect its scheduling priority.
- `RLIMIT_NOFILE` The maximum number of open files per process. Changing this limit affects the value returned by the `sysconf` function for its `_SC_OPEN_MAX` argument
- `RLIMIT_NPROC` The maximum number of child processes per real user ID. Changing this limit affects the value returned for `_SC_CHILD_MAX` by the `sysconf` function
- `RLIMIT_NPTS` The maximum number of pseudo terminals that a user can have open at one time.
- `RLIMIT_RSS` Maximum resident set size(RSS) in bytes. If available physical memory is low, the kernel takes memory from processes that exceed their RSS.
- `RLIMIT_SBSIZE` The maximum size in bytes of socket buffers that a user can consume at any given time.
- `RLIMIT_SIGPENDING` The maximum number of signals that can be queued for a
process. This limit is enforced by the `sigqueue` function.
- `RLIMIT_STACK` The maximum size in bytes of the stack.
- `RLIMIT_SWAP` The maximum amount of swap space in bytes that a user can consume.
- `RLIMIT_VMEM` This is a synonym for `RLIMIT_AS`.

资源限制影响到调用进程并由其子进程继承。为了影响一个用户所有后续进程，需将资源限制在设置构造在shell之中


进程控制
===========
## 进程标识
- pid为0通常是调度进程，称为交换进程
- pid为1通常是`init`进程，在自举过程结束时 __由内核调用__。该进程决不会终止，它是普通进程（交换进程为内核进程），但以超级用户特权执行，是所有孤儿进程的父进程
> MacOSX 10.4中`init`被`launchd`进程所代替
- 某些unix的虚拟存储器实现中，pid为2的是页守护进程，负责支持虚拟存储器系统的分页操作

```c
#include <unistd.h>
pid_t getpid(void);
//Returns: process ID of calling process
pid_t getppid(void);
//Returns: parent process ID of calling process
uid_t getuid(void);
//Returns: real user ID of calling process
uid_t geteuid(void);
//Returns: effective user ID of calling process
gid_t getgid(void);
//Returns: real group ID of calling process
gid_t getegid(void);
//Returns: effective group ID of calling process
```

## fork函数
```c
#include <unistd.h>
pid_t fork(void);
//Returns: 0 in child, process ID of child in parent,−1 on error
```

子进程和父进程继续执行`fork`调用之后的指令，子进程是父进程的副本，__获得父进程的数据空间、堆和栈的副本（包括缓冲区数据）__，但父子进程 __并不共享这些存储空间__，子进程对变量做的改变不会影响父进程。父子进程共享正文段（Text Segment）

由于在`fork`之后经常跟随`exec`，现在许多实现并不执行一个父进程的数据段、栈和堆的完全副本，而使用 __写时复制__（COW）技术，这些区域由父子进程共享，并且内核将它们的访问权限改变为只读，如试图修改这些区域，则内核只为修改区域的那块内存制作一个副本，通常是虚拟存储系统中的一“页”

> linux 3.2提供了`clone(2)`系统调用，一种`fork`推广形式，允许调用者控制哪些部分由父进程和子进程共享

### 文件共享
在重定向父进程的标准输出时，子进程的标准输出也被重定向，`fork`的一个特性就是，父进程的所有打开文件描述符都被复制到子进程中，对于每个描述符，好像执行了`dup`

![img](../../imgs/apue_23.png)

父子进程共享同一个文件偏移量，子进程写到文件时，将更新与父进程共享的该文件的偏移量，如果两者都写到标准输出时，后写者将追加输出

父子进程写同一描述符指向的文件，如果不做同步，那么它们的输出就会相互混合

`fork`之后处理文件描述符：

- 父进程等待子进程完成，父进程无需对其描述符做任何处理，当子进程终止后，它曾进行过读、写操作的任一共享描述符的文件偏移量已做了相应更新
- 父进程和子进程各自执行不同的程序段，各自关闭它们不需使用的文件描述符，这是网络服务进程经常使用的

子进程继承的其它属性：

- Real user ID, real group ID, effective user ID, and effective group ID
- Supplementary group IDs
- Process group ID
- Session ID
- Controlling terminal
- The set-user-ID and set-group-ID flags
- Current working directory
- Root directory
- File mode creation mask
- Signal mask and dispositions
- The close-on-exec flag for any open file descriptors
- Environment
- Attached shared memory segments
- Memory mappings
- Resource limits

父进程和子进程之间的区别：

- The return values from fork are different.
- The process IDs are different.
- The two processes have different parent process IDs: the parent process ID of the child is the parent; the parent process ID of the parent doesn’t change.
- The child’s `tms_utime`, `tms_stime`, `tms_cutime`,and `tms_cstime` values are set to 0
- File locks set by the parent are not inherited by the child.
- Pending alarms are cleared for the child.
- The set of pending signals for the child is set to the empty set.

## vfork函数
> 可移植的应用程序不应该使用它

`vfork`也用于创建新进程，而该新进程的目的是`exec`一个新程序，但它并不将父进程地址空间完全复制到子进程，因为子进程会立即调用`exec`(或`exit`)，不过在子进程调用`exec/exit`之前，它在父进程的空间中运行，如果子进程修改数据（除用于存放`vfork`返回值的变量）、进行函数调用、或没有调用`exec/exit`，可能会带来未知结果

`vfork`保证子进程先运行，在它调用`exec/exit`后父进程才可能被调度运行，如果在调用这两个函数之前子进程依赖于父进程的进一步动作，则导致死锁

## exit函数
任意终止情形，终止进程通知其父进程是如何终止的。对于3个终止函数（`exit/_exit/_Exit`）将其 __退出状态(exit status)__ 作为参数传送给函数。异常终止时，内核（不是进程本身）产生一个指示其异常终止原因的 __终止状态(termination status)__，父进程能用`wait/waitpid`取得其终止状态

如果父进程在子进程之前终止，则子进程的父进程都改变为`init`进程，保证了每个进程都有父进程

一个已经终止、但父进程未对其处理（获取终止子进程的有关信息、释放它占用的资源）的进程称为 __僵尸进程__，`ps(1)`时的状态为`Z`

被`init`收养的进程终止时，并不会成为僵尸进程，只要一个子进程终止，`init`就会调用一个`wait`函数取得其终止状态

## wait/waitpid函数
当一个进程正常或异常终止时，内核就向其父进程发送`SIGCHLD`信号（一个异步通知），父进程可以忽略它，或提供一个信号处理程序

```c
#include <sys/wait.h>
pid_t wait(int *statloc);
pid_t waitpid(pid_t pid, int *statloc, int options);
//Both return: process ID if OK, 0 (see later), or −1 on error
```

调用`wait/waitpid`的进程：

- 如果其所有子进程都还在运行，则阻塞
- 如果一个子进程已终止，正等待父进程获取其终止状态，则取得该子进程的终止状态立即返回
- 如果调用者阻塞而且它有多个子进程，则在其某一子进程终止时，`wait`就立即返回
- 如果它没有任何子进程，则立即出错返回
- `waitpid`有一选项，可使调用者不阻塞（有时希望获取一个子进程状态，但不想阻塞）
- `waitpid`可以控制它所等待的进程
- `waitpid`通过`WUNTRACED/WCONTINUED`选项支持作业控制

检查`statloc`参数的宏

![img](../../imgs/apue_24.png)

```c
#include <sys/wait.h>
void pr_exit(int status){
    if (WIFEXITED(status))
        printf("normal termination, exit status = %d\n", 
            WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n", 
            WTERMSIG(status),
#ifdef  WCOREDUMP
            WCOREDUMP(status) ? " (core file generated)" : "");
#else
            "");
#endif
    else if (WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n",
            WSTOPSIG(status));
}
```

`waitpid`的`pid`参数

- -1，等待任一子进程，此时同`wait`
- >0，等待的进程ID与pid相等的子进程
- 0，等待组ID等于调用进程组ID的任一子进程
- <-1，等待组ID等于pid绝对值的任一子进程

`waitpid`的`options`参数

![img](../../imgs/apue_25.png)

如果一个进程`fork`一个子进程，不等待子进程终止，也不希望子进程处于僵死状态，诀窍是调用`fork`两次

```c
int
main(void)
{
    pid_t   pid;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {      /* first child */
        if ((pid = fork()) < 0)
            err_sys("fork error");
        else if (pid > 0)
            exit(0);    /* parent from second fork == first child */

        /*
         * We're the second child; our parent becomes init as soon
         * as our real parent calls exit() in the statement above.
         * Here's where we'd continue executing, knowing that when
         * we're done, init will reap our status.
         */
        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getppid());
        exit(0);
    }

    if (waitpid(pid, NULL, 0) != pid)   /* wait for first child */
        err_sys("waitpid error");

    /*
     * We're the parent (the original process); we continue executing,
     * knowing that we're not the parent of the second child.
     */
    exit(0);
}
```





## 函数
```c

```

![img](../../imgs/apue_00.png)






<!--
gcc -I../include/ ../lib/error.c seek.c -o seek

休闲食品
联合办宴
时时秒杀
意见奖励

-->
