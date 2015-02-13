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
- `O_EXCL` 如果同时指定了`O_CREAT`，而文件存在，则会出错，可以测试一个文件是否存在，如果不存在则创建，测试和创建是一个原子操作
- `O_DSYNC` 使每次`write`等待物理IO操作完成，但是如果写操作并不影响读取刚写入的数据，则不等待文件属性被更新
- `O_SYNC` 使每次`write`等待物理IO操作完成，包括由`write`操作引起的文件属性更新所需的IO


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
- EAGAIN 提示没有数据可读，可以重试，也许下次能成功
- EBADF EISDIR 等属于程序错误，不应做错误处理（测试就能发现）
- EFAULT EIO　等错误，无法处理，只能出错返回
- EINTR 信号中断，需要重试

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

由`dup`返回的新文件描述符一定是当前可用文件描述符中的最小数值，用`dup2`则可以用参数`fd2`指定新描述符的数值，如果该数值已打开，则先将其关闭，如果`fd`等于`fd2`，则返回`fd2`而不关闭它

当两个描述符指向同一文件表项，它们 __共享__ 同一文件状态标志(file status flags)，以及同一当前文件偏移量(current file offset)，__但__ 却有自己的一套文件描述符标志(fd flags)，新描述符的执行时关闭(`close-on-exec`)标志总是由`dup`函数清除

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

该函数有5种功能

- 复制一个现有的描述符(cmd = `F_DUPFD`　or `F_DUPFD_CLOEXEC`)
- 获得、设置文件描述符标记(cmd = `F_GETFD`　or `F_SETFD`)
- 获得、设置文件状态标志(cmd = `F_GETFL`　or `F_SETFL`)
- 获得、设置异步IO所有权(cmd = `F_GETOWN` or `F_SETOWN`)
- 获得、设置记录锁(cmd = `F_GETLK`, `F_SETLK`,　or `F_SETLKW`)


## 函数
```c

```







<!--
gcc -I../include/ ../lib/error.c seek.c -o seek



-->
