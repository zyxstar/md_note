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
int open(const char *path ,int oflag ,... /* mode_t mode*/ );
int openat(int fd ,const char *path ,int oflag ,... /* mode_t mode*/ );
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
int creat(const char *path ,mode_tmode);
```

此函数等效于

```c
open(path ,O_WRONLY | O_CREAT | O_TRUNC, mode);
```

`creat`不足之外它以 __只写__ 方式打开所创建的文件，如果需要创建一个临时文件，并先写，然后再读，推荐以下方式

```c
open(path ,O_RDWR | O_CREAT | O_TRUNC, mode);
```

## close函数
```c
#include <unistd.h>
int close(int fd );
```

关闭一个文件时还会释放该进程加在该文件上的所有记录锁。当一个进程终止时，内核自动关闭它所有打开的文件。

## lseek函数
```c
#include <unistd.h>
off_t lseek(intfd ,off_t offset,int whence );
```

当打开一个文件时，除非指定`O_APPEND`选项，否则该偏移量被设置为0

若`lseek`成功，则返回新的文件偏移量，为此可以用以下方式确定打开文件的当前偏移量(相当于标准库中的`ftell`?)

```c
off_t  currpos;
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
ssize_t read(int fd ,void *buf,size_tnbytes );
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
ssize_t write(int fd ,const void *buf,size_tnbytes );
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
ssize_t pread(int fd ,void *buf,size_tnbytes ,off_t offset);
ssize_t pwrite(int fd ,const void *buf,size_tnbytes ,off_t offset);
```

相当于顺序调用`lseek`和`read`/`write`，但它是 __原子操作，且不更新文件偏移量__

### 创建一个文件
对`open`函数同时使用`O_CREAT`和`O_EXCL`，否则如果先`open`再`creat`间，另一个进程创建了该文件，就会引起问题

## dup和dup2函数
```c
#include <unistd.h>
int dup(int fd );
int dup2(int fd ,int fd2);
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
void sync(void);
```

`sync`函数将所有修改过的缓冲区排入写队列，然后就返回，所以它　__并不等于__ 实际写硬盘操作结束

通常称为update的系统守护进程会周期性的调用`sync`，保证了定期冲洗内核的块缓冲区

`fsync`只对由文件描述符指定的单一文件起作用，并且 __等待写磁盘操作结束__，然后返回，可用于数据库这样的应用程序

`fdatasync`类似于`fsync`，但它只影响文件的数据部分，而`fsync`还会同步更新文件的属性

## fcntl函数
```c
#include <fcntl.h>
int fcntl(int fd ,int cmd ,... /* int arg */ );
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
```

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

当执行一个程序文件时，进程的有效用户ID等于实际用户ID，有效组ID等于实际组ID，但是可以在文件模式字`st_mode`中设置一个特殊标志，其含义是，当执行此文件时，将进程的有效用户ID设置为`st_uid`，同理，文件模式中可以设置另一位，将执行此文件的进程的有效组ID为文件的组所有者ID`st_gid`

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
```



## 函数
```c

```







<!--
gcc -I../include/ ../lib/error.c seek.c -o seek



-->
