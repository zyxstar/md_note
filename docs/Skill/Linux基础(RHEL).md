> 2014-09-27

回到索引
=======
- [嵌入式培训(Uplooking).md](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgitcafe.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FSkill%2F%25E5%25B5%258C%25E5%2585%25A5%25E5%25BC%258F%25E5%259F%25B9%25E8%25AE%25AD%25E7%25B4%25A2%25E5%25BC%2595%2528Uplooking%2529.md)

安装相关
========
## 安装事项
RHEL的经典版本：5.8和6.4

注意事项
> 从U盘安装时，GRUB引导程序默认在U盘上（`/dev/sdb`），需安装在`/dev/sda`上
>
> 新手安装时，防火墙、SElinux、kdump关闭

[Redhat网络系统安装说明](../../data/Redhat网络系统安装说明.pdf)

[老师的ule笔记.pdf](../../data/老师的ule笔记.pdf)

## Linux目录结构(FHS)
```
/           根目录
/boot       存放程序启动所必须的文件
            /boot/vmlinuz-3.2.0-61vm-generic-pae    内核文件
            uname -a                                显示系统信息
            /boot/grub/*                            引导程序
/var        系统中经常需要变化的一些文件
            /var/tmp 程序运行时使用的临时文件，删除可能会出错
            /var/lib;/var/lib64 库文件
            /var/log/messages   系统日志
/bin        存放系统基本用户命令，如ls,mv
/sbin       存放系统基本管理命令 s代表super，如fdisk,ip
/usr        存放系统大量的应用程序
            /usr/bin     用户命令
            /usr/sbin    管理命令
/etc        存放系统和各种程序的配置文件
            /etc/init.d  服务启动
/dev        设备文件
            ls /dev/sd*  查看所有硬盘及分区
/lib        库文件，驱动，由glibc提供
            /lib64       64位的库文件
/tmp        临时文件
/media      光盘的默认挂载点
            /media/cdrom
/mnt        其它的挂载点
/proc       进程文件(pid)，虚拟文件系统
            du -sh /proc 统计大小为0，表示数据在内存中，不占硬盘空间
/opt        主机额外安装软件所放的目录，也可以是个挂载点
/srv        一些服务启动后，这些服务所需访问的数据目录

```

## 分区设备
```
IDE硬盘
  hda   a表示第1个       a~d
  hda1  1表示第一个分区  1~63

SCSI硬盘
  sda   a表示第1个       a~p
  sda1  1表示第一个分区  1~15 最多4个主分区或3个主分区1个扩展分区
```
使用`ls /dev/hd*`，`ls /dev/sd*`查看

## 分区建议

```
/boot ext3 200M
      swap 4096M
/home ext3 看情况（个人资料）
/usr  ext3 看情况（程序数据）
/     ext3 看情况
```

> 要记得 `/lib` 不可以与 `/` 分别放在不同的 partition

使用`sudo fdisk -l`可查看硬盘分区(partition)情况，`df -h`查看文件系统使用情况，`du -sh /path/to` 统计指定目录的大小

## 快捷键
```
ctrl alt F1~F6     图形界面切换到字符界面
ctrl F7            字符界面切换到图形界面
ctrl shift +       放大窗口
ctrl -             缩小窗口
ctrl shift t       新建窗口
alt <num>          切换窗口
快捷复制            鼠标左键选中即复制 鼠标按中键即粘贴

ctrl l             清屏 或clear命令
ctrl c             中止任务执行
ctrl z             任务转至后面，挂起状态
ctrl a             shell中输入命令时，光标跳到最前
ctrl e             shell中输入命令时，光标跳到最后
ctrl u             shell中输入命令时，删除所有输入

ctrl s             暂停屏幕输入
ctrl q             恢复屏幕输入
ctrl d             退出终端

```

### history
```shell
!16                #运行history中第16行记录
!cd                #运行history中最近的cd命令
!!                 #运行history中上次命令
ctrl r             #进入history搜索
esc .              #使用上次命令的参数
alt .              #引用上次命令的参数，一直按. 继续向上查找参数
```

## 帮助
```shell
<command> --help
          #查找ls命令手册中有关时间与排序的条目
          ls --help|grep -E "time|sort"

man       #man是分章节的
           # 1 User Commands
           # 2 System Calls
           # 3 C Library Functions
           # 4 Devices and Special Files
           # 5 File Formats and Conventions
           # 6 Games et. Al.
           # 7 Miscellanea
           # 8 System Administration tools and Deamons
          #查找fopen的使用，在第3章节查找
          man 3 fopen
          #查询所有含有passwd的帮助文件
          man -k passwd

          #操作同less
          #space下一页；b上一页；enter下一行；q退出；G最后；g最前
          #?<keyword>查找光标之前；/<keyword>查找光标之后
          #n下一个关键字；shift n上一个关键字

          #将man手册转存为文本
          man ls |col -b >ls.txt  #col -b 过滤掉所有的控制字符

info <command>
          #将info手册转存为文本
          info make -o make.txt -s

```

## 启动过程
- 加载 BIOS 的硬件资讯与进行自我测试，并依据配置取得第一个可启动的装置；
- 读取并运行第一个启动装置内 MBR 的 boot Loader (亦即是 `grub`, `spfdisk` 等程序)；
- 依据 boot loader 的配置加载 Kernel ，Kernel 会开始侦测硬件与加载驱动程序；
- 在硬件驱动成功后，Kernel 会主动呼叫 `init` 程序，而 `init` 会取得 run-level 资讯；
- `init` 运行 `/etc/rc.d/rc.sysinit` 文件来准备软件运行的作业环境 (如网络、时区等)；
- `init` 运行 run-level 的各个服务之启动 (script 方式)；
- `init` 运行 `/etc/rc.d/rc.local` 文件；
- `init` 运行终端机模拟程序 `mingetty` 来启动 `login` 程序，最后就等待使用者登录啦；

runlevel:

- 0 - halt (系统直接关机)
- 1 - single user mode (单人维护模式，用在系统出问题时的维护)
- 2 - Multi-user, without NFS (类似底下的 runlevel 3，但无 NFS 服务)
- 3 - Full multi-user mode (完整含有网络功能的纯文字模式)
- 4 - unused (系统保留功能)
- 5 - X11 (与 runlevel 3 类似，但加载使用 X Window)
- 6 - reboot (重新启动)


## 关机
```shell
init 0            #关机
shutdown -h now

reboot            #重启
```

文件管理
========
## 路径
```shell
pwd       #查看当前路径
cd        #回到自己home
cd~       #回到自己home
cd ~jack  #去jack的home
cd-       #切到上次目录
```

## 查看
```shell
ls
    -a    #显示所有文件，包含隐藏文件
          ls -a| sort -i     #名称排序显示
    -l    #显示详细信息，别名为ll
          #显示的 -rwx rw- --- 第一个是文件类型
          #普通- 目录d 块设备b 字符设备c 符号链接l 套接字s 管道p
    -t    #以时间顺序排列查询结果
    -h    #人性化显示
    -i    #显示inode号
    -d    #只显示目录信息
          ls -d */       #只显示目录
          ls -ld /<dir>  #可以看到该目录的访问权限,不加d可能直接deny
    -r    #逆向排序
    -R    #递归显示目录结构
    -F    #目录以/结尾显示

cat
    -n    #给文件所有行编号
    -A    #控制字符都显示
    -s    #把多个空白行合并为一个空白行
          #使文件清空，但所有者和权限不变
          cat /dev/null > a.txt
          #列出两个文件的内容.执行对输出的搜索.统计结果行的个数
          cat file1 file2 | grep word | wc -1

less      #分屏，可前翻
          #space下一页；b上一页；enter下一行；q退出；G最后；g最前
          #?<keyword>查找光标之前关键字；/<keyword>查找光标之后
          #n下一个关键字；shift n上一个关键字
more      #分屏，翻到最后直接退出，无法前翻

tail
    -f    #实时监控
head

tree      #树形显示目录
fdisk -l  #查看硬盘分区情况
du        #显示当前目录中对象的大小
    -s    #显示当前目录大小
    -h    #人性化显示
    --max-depth=1  #设置显示深度
df        #查看文件系统使用情况
    -h    #人性化显示
          #查看某目录所属硬盘
          df /<path> -h

nl        #向文件添加行号
wc        #统计行数，字符，字节
rev       #左右相反
tac       #cat相反
fmt       #把文本格式化成指定宽度
pr        #给文件分页 ,把文本文件转换为标有页码的版本
fold      #把输入行换行，以适应指定的宽度

stat      #文件信息查看
file      #查看文件属性
getfacl   #查看文件权限
```

## 查找
```shell
grep "<key>" <path/to/file>
    --color  #高亮
    -n       #显示行号

    -E, --extended-regexp     PATTERN is an extended regular expression (ERE)
    -F, --fixed-strings       PATTERN is a set of newline-separated fixed strings
    -G, --basic-regexp        PATTERN is a basic regular expression (BRE)
    -P, --perl-regexp         PATTERN is a Perl regular expression

    -v, --invert-match        select non-matching lines
    #将配置文件中注释行和空行去掉显示
    cat /etc/dhcpd/dhcpd.conf | grep -v "^#" | grep -v "^$"

find <path>
    -name "pattern"
    -size +1000M
    #查找指定类型的文件，然后指定按时间排序
    find . -name *.php |xargs ls -alt
    ls -alt $(find . -name *.php)
    #统计所有c文件的行数总和，不包括空行
    find . -name "*.c" |xargs cat|grep -v ^$|wc -l

    -i               #llignore
    -o               #or
    !                #not
```

## 创建
```shell
touch    #已创建的不会被清空，但会更新访问时间

mkdir
         #创建多个目录
         mkdir dir1 dir2
    -p   #创建层次目录
         mkdir -p aa/bbb/ccc

ln       #创建硬连接
    -s   #创建符号连接
```

## 复制
```shell
cp <src> <dest>
    -r   #recursive，实现同时拷贝文件和文件夹
    -f   #force，强制覆盖
    -v   #verbose
    -i   #ineractive
    -a   #权限也复制
```

## 移动
```shell
mv <src> <dest>
         #需要重命名，只需在同目录下执行就行
```

## 删除
```shell
rm       #安全的删除，建议先进入目录后再删除
    -r   #recursive
    -f   #force
rm -rf /etc/yum.repos.d/*    #清空目录
rm -rf /etc/yum.repos.d/     #删除目录
```

## 归档压缩
归档压缩其实是两个步骤，常用选项说明

```shell
-c   #create
-v   #verbose
-z   #gzip
-j   #bzip2
-f   #file指定文件
-x   #extract
```

- 归档

```shell
tar cvf ab.tar a b c dfile
```

- 解包

```shell
tar xvf ab.tar [-C /path]
```

- 压缩

```shell
gzip a b c dfile  #文件是分开压缩的
bzip2
```

- 解压

```shell
gunzip a.gz
bunzip2 a.bz2
```

- 归档压缩

```shell
tar cvzf ab.tar.gz a b c dfile
tar cvjf ab.tar.bz2 a b c dfile
```

- 解压解包

```shell
tar xvzf ab.tar.gz
tar xvjf ab.tar.bz2
```

软件安装
========

## RPM安装
软件包名，指安装时的文件名，而软件名，指系统中识别的名称
### 挂载镜像
```shell
mount -o loop /path/to/rhel.iso /mnt
```
> iso文件，即iso9660，这是光盘所使用的一种文件系统

```shell
ls /mnt/Packages
```
将看到许多`.rpm`的文件，通常格式如`<软件名>.<版本>.<OS>.<CPU>.rpm`，其中

- OS：el5,el6
- CPU：i386~i686(32位),x86_64(64位),noarch(通用)

### 安装
```shell
rpm -ivh <软件包名>
    -i      #install
    -vh     #显示安装过程信息
    --force #强制安装
```
不需要指定安装目录，默认安装于`/usr`（分区时最好单独分出来）

### 查询
- 查询已安装的软件包`-q`

```shell
rpm -qa | grep vnc   #模糊查询
rpm -q gcc           #明确知道软件名
```

- 查询软件安装位置`-l`

```shell
rpm -ql tigervnc
```

- 查询文件所属软件，方便根据文件找到软件 再安装，或根据命令找到程序`-f`

```shell
rpm -qf /path/to/file
```

- 查询已安装软件详细信息`-i`

```shell
rpm -qi <软件名>（不是包）
```

### 删除
```shell
rpm -e <软件名>（不是包）
    -e       #erase
    --nodeps #不查看依赖性关系删除，防止缺失文件不能删除
```
实在卸载不了时，可强制安装`--force`，再删除

## YUM安装
### 配置源
```shell
rm -rf /etc/yum.repos.d/*      #清空目录
vim /etc/yum.repos.d/yum.repo  #以.repo结尾
```

键入如下信息

```
[rhel6]
name=rhel6
baseurl=file:///mnt/Server
enabled=1
gpgcheck=0
```

一个文件中可存在多个五行，从上往下依次查找安装，`enabled＝0`则不查找该源

### 安装
```shell
yum -y install <软件名>  #可带通配符
yum -y groupinstall <软件组名> #如：通用桌面
```

### 查询
```shell
yum repolist
yum list        #第三列是yum源名字时，说明没装上
yum grouplist
yum search <keyword>
```

### 卸载
```shell
yum -y remove
yum -y erase
```

## RPM vs YUM
- 两者都安装`.rpm`结尾的软件
- rpm不解决依赖关系，优先使用
- yum解决依赖关系，自动安装，需要配置源，在64位机器上默认不查找32位的软件，特殊情况下可手工rpm安装需要的软件

## 编译安装
### 安装
利用命令将源代码的包（如`.tar.gz`,`.tar.bz2`）进行解压解包

```shell
./configure --prefix=<path/to/setup>  #建议指定，删除时方便
                                      #否则分散在usr/locals各子目录
                                      #此步骤将产生配置文件
make          #编译
make install  #安装
```

### 启动
手动安装的一般在`usr/locals`目录下，如httpd，此时应该在`/usr/local/apache2`，通过`/usr/local/apache2/bin/apachectl start`来启动

### 删除
直接删除由`--prefix`指定的目录

## 搜索程序
```shell
which          #定位一个命令，搜索磁盘上命令，不能找到内置命令
whereis        #定位一个命令的二进制、源文件、手册

type           #查看是否是内置命令

apropos        #查看相关名字命令的说明 同man -k
whatis         #查看相关名字命令的说明

locate         #搜索文件，需要配置/etc/updatedb.conf（运行updatedb）
slocate

ldd            #列出程序的动态依赖关系 ldd /bin/cp
```

## curl使用
```shell
curl http://www.linuxidc.com

#指定输出文件
curl -o page.html http://www.linuxidc.com
#指定代理
curl -x 123.45.67.89:1080 -o page.html http://www.linuxidc.com
#保存cookie
curl -x 123.45.67.89:1080 -o page.html -D cookie0001.txt http://www.linuxidc.com
#使用cookie
curl -x 123.45.67.89:1080 -o page1.html -D cookie0002.txt -b cookie0001.txt http://www.linuxidc.com
#指定referer
curl -A "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)" -x 123.45.67.89:1080 -e "mail.linuxidc.com" -o page.html -D cookie0001.txt http://www.linuxidc.com

#下载文件
curl -O http://cgi2.tky.3web.ne.jp/~zzh/screen1.JPG
#文件名匹配下载
curl -O http://cgi2.tky.3web.ne.jp/~zzh/screen[1-10].JPG
#下载多个目录下的文件
curl -O http://cgi2.tky.3web.ne.jp/~{zzh,nick}/[001-201].JPG

#断点续传，比如我们下载screen1.JPG中，突然掉线了，我们就可以这样开始续传
curl -c -O http://cgi2.tky.3wb.ne.jp/~zzh/screen1.JPG
#分块下载
curl -r 0-10240 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 10241-20480 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 20481-40960 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 40961- -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3

cat zhao.part* > zhao.mp3

#ftp下载
curl -u name:passwd ftp://ip:port/path/file
curl ftp://name:passwd@ip:port/path/file

#上传文件
curl -T localfile -u name:passwd ftp://upload_site:port/path/                #ftp
curl -T localfile http://cgi2.tky.3web.ne.jp/~zzh/abc.cgi                    #http put

#提交表单
curl -d "user=nickwolfe&password=12345" http://www.linuxidc.com/login.cgi    #http post

#表单中上传文件
curl -F upload=@localfile -F nick=go http://cgi2.tky.3web.ne.jp/~zzh/up_file.cgi

```

用户管理
========
## 创建
```shell
useradd <username>
useradd -u <uid> <username>
useradd -g <gid> <username>             #创建时指定主属组
useradd -G <gid>[,<gid>...] <username>  #创建时指定附属组
```

## 查看
```shell
id <username>
```

出现类似`uid=502(jim) gid=502(jim) 组=502(jim),0(root)`

- uid:user identify,数字，可能出现uid相同，但用户名不同的情况，以uid为准
- gid:group identify,数字
- 组:主属组+附属组,数组，通过`usermod -G root jim`给jim设置 __附属组__
> 但可能会使得以前执行过的`gpasswd -a`命令失效

```shell
cat /etc/passwd | grep <username>

```

出现类似`jim:x:502:502::/home/jim:/bin/bash`的信息，依次为

- 用户名
- 密码占位，删除后用户就没密码，虽然不能改密码，但能置空密码（可通过删除root的此信息，达到进入root账户）
- uid
- gid
- 描述字段
- 家目录
- 登录shell

> `/etc/shadow`才是真正存储密码的地方

## 修改
```shell
usermod -u <uid> <username>   #修改用户id
usermod -g <gid> <username>   #修改用户主属组
usermod -G <gid>[,<gid>...] <username>   #修改用户附属组
```

## 切换用户
```shell
su - <username>  #加载环境变量 如.profile
su <username>    #不会加载环境变量，不推荐使用
```
同理，在arm开发中，出现`-/bin/sh /path/to/script`中的`-`，也是用于加载环境设置的


## 设置密码
```shell
passwd [<username>]   #不加<username>给自己重设
                      #root用户能设置简单的密码，其他用户不行
```

## 删除账户
```shell
userdel -r <username>
        -r #家目录删除
```

## 查看登录信息
实时的登录

```shell
whoami
who
w
ps u
users

```

登录日志

```shell
last <username> <tty>
finger <username>
```

发信息

```shell
write <username> <tty>
wall
```

组管理
======
添加账户时，就会产生一个同名的组，这个组称为用户的 __主属组__

## 添加组
```shell
groupadd <groupname>
```

## 查看组
```shell
cat /etc/group
```
格式为`组名:组密码占位（目前无用）:gid:组员（数组）`

## 切换组
```shell
newgrp <groupname>  #临时切换到用户的某个附属组，exit退出
```

## 添加用户进组
```shell
gpasswd -a <username> <groupname>
```

## 删除用户出组
```shell
gpasswd -d <username> <groupname>
```

## 删除组
```shell
groupdel <groupname>
```

文件权限
=========
## 查看
```shell
ls -l <file>
```

得到`- rwx rw- ---`，第一个字符表示文件类型: 普通- 目录d 块设备b 字符设备c 符号链接文件l 套接字文件s 管道文件p；剩下的分成三组，简称`ugo`权限：第一组代表user权限；第二组代表group权限；第三组代表others权限

## 修改
```shell
chmod u-x a.txt
chmod g+x a.txt
chmod o+w a.txt
chmod u-x,g+x,o+w a.txt
chmod u+w-x a.txt
```
以数字方式修改 read:r=4 write:w=2 excute:x=1

```shell
chmod 744 a.txt
```
> 对于root用户，x权限同属主，rw的设置是无效的，依然能读能写

### suid
表示设置文件在 __执行阶段__(权限仅对二进位程序(binary program)有效，不能够用在 shell script 上面) 具有 __文件所有者__ 的权限

[参考](http://vbird.dic.ksu.edu.tw/linux_basic/0220filemanager_4.php#suid)

如`ll /bin/ping`，得到`-rwsr-xr-x`，其中`s`就是`suid`，普通用户也是能使用`ping`的(此处为root)的权限

如普通用户无法通过`cat`查看`/ect/shadow`，但修改了`cat`的权限`chmod u+s /bin/cat`后，就可以了

### sgid
[参考](http://vbird.dic.ksu.edu.tw/linux_basic/0220filemanager_4.php#sgid)

如`ll /usr/bin/locate`，得到`-rwx--s--x`，其中`s`就是`sgid`

- 对二进制程序，运行者在运行过程中将会获得该程序群组的支持，上面普通用户将得到`slocate`群组支持
- 针对目录
  + 使用者若对於此目录具有 r 与 x 的权限时，该使用者能够进入此目录
  + 使用者在此目录下的有效群组(effective group)将会变成该目录的群组
  + 若使用者在此目录下具有 w 的权限(可以新建文件)，则使用者所创建的新文件，该新文件的群组与此目录的群组相同。

### skicky
[参考](http://vbird.dic.ksu.edu.tw/linux_basic/0220filemanager_4.php#sbit)

只针对 __目录__ 有效，当使用者在该目录下创建文件或目录时，仅有自己与 root 才有权力删除该文件

## 修改文件所属者/组
间接修改了权限

```shell
chown <username> <file>   #修改文件所属者
chown .<groupname> <file> #修改文件所组，前面加点
chown <username>:<groupname> <file> #同时修改文件所属者与组
chgrp <groupname> <file>  #修改文件所组
```


进程控制
========
## 查看进程
```shell
ps      #当前终端上进程
   a    #所有终端上进程，-a则为只显示非终端上进程
   ax   #所有进程，包括非终端上进程
   aux  #把进程启动的用户(及起始时间)包含进来
   e    #显示shell环境信息
   f    #格式化父子进程的显示
   l    #较长、较详细的将该 PID 的的资讯列出
   j    #工作的格式
   axjf #连同部分程序树状态

ps -l
   #F：代表这个程序旗标 (process flags)，说明这个程序的总结权限
       #4 表示此程序的权限为 root;
       #1 则表示此子程序仅进行复制(fork)而没有实际运行(exec)
   #S：代表这个程序的状态 (STAT)，主要的状态有
       #R (Running)：该程序正在运行中；
       #S (Sleep)：该程序目前正在睡眠状态(idle)，但可以被唤醒(signal)。
       #D ：不可被唤醒的睡眠状态，通常这支程序可能在等待 I/O 的情况(ex>列印)
       #T ：停止状态(stop)，可能是在工作控制(背景暂停)或除错 (traced) 状态；
       #Z (Zombie)：僵尸状态，程序已经终止但却无法被移除至内存外。
   #UID/PID/PPID：代表『此程序被该 UID 所拥有/程序的 PID 号码/此程序的父程序 PID 号码』
   #C：代表 CPU 使用率，单位为百分比；
   #PRI/NI：Priority/Nice 的缩写，数值越小代表该程序越快被 CPU 运行
   #ADDR/SZ/WCHAN：
       #ADDR 是 kernel function，指出该程序在内存的哪个部分，如果是个 running 的程序，一般就会显示『 - 』
       #SZ 代表此程序用掉多少内存
       #WCHAN 表示目前程序是否运行中，同样的， 若为 - 表示正在运行中
   #TTY：登陆者的终端机位置，若为远程登陆则使用动态终端介面 (pts/n)；
   #TIME：使用掉的 CPU 时间，注意，是此程序实际花费 CPU 运行的时间，而不是系统时间；
   #CMD：就是 command 的缩写，造成此程序的触发程序之命令为何

ps aux
   #USER：该 process 属於那个使用者帐号的？
   #PID ：该 process 的程序识别码。
   #%CPU：该 process 使用掉的 CPU 资源百分比；
   #%MEM：该 process 所占用的实体内存百分比；
   #VSZ ：该 process 使用掉的虚拟内存量 (Kbytes)
   #RSS ：该 process 占用的固定的内存量 (Kbytes)
   #TTY ：该 process 是在那个终端机上面运行，若与终端机无关则显示 ?，
          #tty1-tty6 是本机上面的登陆者程序，若为 pts/0 等等的，则表示为由网络连接进主机的程序。
   #STAT：该程序目前的状态，状态显示与 ps -l 的 S 旗标相同 (R/S/T/Z)
   #START：该 process 被触发启动的时间；
   #TIME ：该 process 实际使用 CPU 运行的时间。
   #COMMAND：该程序的实际命令为何？

pstree
   -p   #列出每个 process 的 PID
   -u   #列出每个 process 的所属帐号名称
```

## 查看系统状态
```shell
top
   -d   #后面可以接秒数，就是整个程序画面升级的秒数。默认是 5 秒；
   -n   #与 -b 搭配，意义是，需要进行几次 top 的输出结果
   -p   #指定某些个 PID 来进行观察监测而已

vmstat
   -a   #使用 inactive/active(活跃与否) 取代 buffer/cache 的内存输出资讯；
   -f   #启动到目前为止，系统复制 (fork) 的程序数；
   -s   #将一些事件 (启动至目前为止) 导致的内存变化情况列表说明；
   -S   #后面可以接单位，让显示的数据有单位。例如 K/M 取代 bytes 的容量；
   -d   #列出磁碟的读写总量统计表
   -p   #后面列出分割槽，可显示该分割槽的读写总量统计表
   #procs
        #r ：等待运行中的程序数量
        #b：不可被唤醒的程序数量
        #这两个项目越多，代表系统越忙碌 (因为系统太忙，所以很多程序就无法被运行或一直在等待而无法被唤醒之故)。
   #memory
        #swpd：虚拟内存被使用的容量；
        #free：未被使用的内存容量；
        #buff：用於缓冲内存；
        #cache：用於高速缓存。
        #这部份则与 free 是相同的。
   #swap
        #si：由磁碟中将程序取出的量；
        #so：由於内存不足而将没用到的程序写入到磁碟的 swap 的容量。
        #如果 si/so 的数值太大，表示内存内的数据常常得在磁碟与主内存之间传来传去，系统效能会很差！
   #io
        #bi：由磁碟写入的区块数量；
        #bo：写入到磁碟去的区块数量。
        #如果这部份的值越高，代表系统的 I/O 非常忙碌！
   #system
        #in：每秒被中断的程序次数；
        #cs：每秒钟进行的事件切换次数；
        #这两个数值越大，代表系统与周边设备的沟通非常频繁！ 这些周边设备当然包括磁碟、网络卡、时间钟等。
   #cup
        #us：非核心层的 CPU 使用状态；
        #sy：核心层所使用的 CPU 状态；
        #id：闲置的状态；
        #wa：等待 I/O 所耗费的 CPU 状态；
        #st：被虚拟机器 (virtual machine) 所盗用的 CPU 使用状态 (2.6.11 以后才支持)。

vmstat -a <延迟> <总计侦测次数> #CPU/内存等资讯
vmstat -fs                     #内存相关
vmstat -S <单位>               #配置显示数据的单位
vmstat -d                      #与磁碟有关
vmstat -p <分割槽>             #与磁碟有关

free    #内存空间
   -m   #字节单位为M
   -t   #在输出的最终结果，显示实体内存与 swap 的总量
```


## 杀死进程
```shell
ps aux | grep <keyword>         #查到进程pid
ps aux | egrep '(cron|syslog)'  #找出与 cron 与 syslog 这两个服务有关的PID
pidof sshd                      #查找sshd的进程pid

kill -9 <pid>
```

`kill -l` 列出所有信号：

- -9 强制
- -15 默认
- -19 挂起
- -18 唤醒

```shell
w               #看到所有登录系统的账户，查看是否异常
killall -9 sshd #杀死进程名称为sshd的进程，
                #即把所有通过ssh登录到系统的账户登出

pkill -9 httpd  #杀死进程名称为httpd的进程
```

## 后台任务
- 在命令尾处键入`&`把作业发送到后台
- 也可以把正在运行的命令发送到后台运行，首先键入`Ctrl+Z`挂起作业（此时是挂起状态），然后键入`bg`移动后台继续执行
- `bg %jobnumber` 或`bg %name`
- `fg %jobnumber` 把后台作业带到前台来
- `kill -18 pid` 也是唤醒
- `kill %jobnumber` 删除后台作业
- `jobs -l`将PID显示 `-r`运行中显示 `-s`显示停止
- `disown %jobnumber`从后台列表中移除任务，并没有终止
- `nohup command &` 如果你正在运行一个进程，而且你觉得在退出帐户时该进程还不会结束，那么可以使用`nohup`命令。该命令可以在你退出帐户之后继续运行相应的进程。


Linux其它
==========
## 重定向
```shell
>           #输出重定向 1> 的简写
            ls > /dev/null  #丢掉输出
            ls > /dev/pts/2 #输出到另一个终端
>>          #输出重定向追加
2>          #错误重定向
2>>         #错误重定向追加

&>          #标准与错误一起重新向
            ls > 0.txt 2>&1  #同上
>> 2>>      #标准与错误一起重新向追加

<           #输入重定向
            cat < /dev/zero -A

<<          #输入重定向，并定义结束符
            cat << EOF  #打开文件缓冲区，输入完后，最后输入'EOF'

            #下载ftp上内容，这类交互性的东西也可以写成脚本
            lftp 192.168.1.245 << EOF
             > get somefile
             > EOF

tee         #在管道过程中产生分支
            grep -v "a" file | tee file1 | grep -v "b" file

xargs
            find . -name *.php |xargs ls -alt
            #等同于
            ls -alt $(find . -name *.php)

```

> `except`可用于写交互性的脚本

## 环境变量
```shell
env     #查看当前用户的环境变量
```

一般情况下，环境变量由以下几个文件顺序进行加载

- `/etc/profile`
    - `/etc/profile.d/*.sh`
- `~/.bash_profile`
    - `~/.bashrc`
        - `/etc/bashrc`

非登录shell情况如 `su tom`，先

- `~/.bash_profile`
    - `~/.bashrc`
        - `/etc/bashrc`

然后`/etc/profile.d/*.sh`

没有`/etc/profile`

```shell
vim /etc/profile
export LC_ALL=zh_CN.GB18030       #设置语言信息
export PS1='\u@\h:\w\$'           #修改命令行提示符

vim /etc/sysconfig/i18n
LANG="zh_CN.GB18030"              #当前系统的语言环境变量设置
SUPPORTED="zh_CN.GB18030:zh_CN:zh:en_US.UTF-8:en_US:en"  #系统支持哪些字符集
SYSFONT="latarcyrheb-sun16"       #系统终端字符的字体
```

设置时区与同步时间

```shell
cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
ntpdate cn.pool.ntp.org
```

## Shell相关
```shell
alias                             #查看别名，别名的优先级高
    alias la="ls -a"              #设置别名

unalias la                        #取消别名
\<command>                        #不使用别名时，直接使用原始命令

<command1>&&<command2>            #顺序执行，前面的不出错，后面的才执行
<command1>||<command2>            #顺序执行，前面的出错，后面的才执行
<command1>;<command2>             #顺序执行，不管前面结果，后面照样执行

{}                                #命令的展开
                                  mkdir dir{5..8}
                                  mkdir dir{a..d}
                                  mkdir dir{a,d}
                                  #相当于括号外面 * 括号里现
                                  cp /abc{0,1}  #cp /abc0 /abc1

#保持不变，防止shell翻译
                                  touch "a b"
                                  touch a\ b
                                  touch a' 'b

echo -n                           #不换行，默认换行
echo -e "a\tb\tc"                 #翻译转义，否则原样输出

```

## Shell脚本
```shell
$1~$9                #位置变量
$*                   #具体参数数组，同$@
$#                   #共几个参
$$                   #PID
$?                   #上次命令执行的返回值，0为正常，非0为异常

test                 #if后面的条件都是 test 条件，可简写成 [ .. ]
    -f               #是否是文件
    -r               #是否可读
    -w               #是否可写
    -u               #是否具有suid
    -ot              #是否比某文件旧
    -gt              #数字比较 great than
    -a               #and

for((i=1;i<255;i++)); do
  ping -c1 192.168.1.$i > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    echo "192.168.1.$i is running"
  fi
done

#双重[[ .. ]] 解决单层 [ .. ] 不能逻辑运算和正则匹配的问题
[[ !cmd ]]
[[ cmd && cmd ]]
[[ cmd || cmd ]]
[[ $z =~ "o" ]]

#数学运算
expr

$(($x+$y))
$[$x+$y]
let x=$x+1
```

## 杂项
```shell
mount -l                          #查看目前所有挂载
mount /dev/sdb /media             #挂载U盘、光盘
      /dev/sr0 /mnt
umount /dev/sdb                   #取消挂载U盘
                                  #多取消几次 防止被多次挂载了
sudo fdisk -l | grep NTFS         #挂载ntfs的u盘
mount -t ntfs-3g <NTFS Partition> <Mount Point>

sed -n '5,10p' /etc/passwd        #只查看文件的第5行到第10行。
uniq                              # -d 只显示重复的行 -u 只显示唯一的行
sort names | uniq -d              #显示在names文件中哪些行出现了多次

tr                                #翻译
                                  #转换大小写
                                  cat /etc/passwd | tr [a-z] [A-Z]
                                  #计算
                                  echo "1 2 3 4 5" | tr " " "+" | bc

split                             #按行数或字节数拆分文件
csplit                            #由正则来分割文件

strip                             #将编译链接的可执行文件进行剪切，去掉中间信息

locale                            #查看系统当前locale环境变量

```


网络配置
========
## 查看网卡
```shell
ifconfig eth0
ip addr show      #或简写成 ip add sh
```

## 启动网卡
```shell
ifconfig eth0 up
ifconfig eth0 down
ifdown eth0
ifup eth0
```

## 临时设置网卡
```shell
ifconfig eth0 <ip> netmask <ip> up
ip addr add <ip> netmask 255.255.255.0 dev eth0
ip addr add <ip/24> dev eth0    #等同于上面，24个1（二进制）即255.255.255.0
```

## 查看网关
```shell
route -n
```

## 临时设置网关
```shell
route add default gw <ip>
```

## 临时设置DNS
```shell
vim /etc/resolv.conf

nameserver 202.106.0.46 #最多三个
```

## 永久的网络设置
```shell
vim /etc/sysconfig/network-scripts/ifcfg-eth{0,1..}

ONBOOT                      #是否启用网卡
NM_CONTROLLED               #是否受networkmanager(gui)控制
BOOTPROTO=dhcp|none|static  #获取ip方式

NETMASK=255.255.255.0
PREFIX=24

GETEWAY
DNS1
DNS2
```

## 重启网卡
```shell
service network restart
```

## 其它
```shell
lsof -i:80                    #查看80端口是否启动
mii-tool eth0                 #eth0网卡是否有效
ping <ip>
    -c1                       #ping一个包

netstat -ntlp
    -n                        #以网络IP地址代替名称，显示出网络连接情形
    -t                        #显示TCP协议的连接情况，-u是UDP
    -l                        #正在listening的
    -p                        #显示程序名字
    -a                        #将目前系统上所有的连线、监听、Socket 数据都列出来

service iptables stop         #关闭防火墙
iptables -F                   #关闭防火墙
setenforce 0                  #关闭SElinux

nslookup <domain>             #根据域名查看ip，附带检查dns效果

system-config-network         #GUI方式配置，RHEL6有问题
                              #可把它关闭`/etc/init.d/NetWorkManager stop`
```

> 常用服务端口号，可通过`grep 3306 /etc/services`查看某端口的服务

> - tcp
>>   - ftp 21
>>   - ssh 22
>>   - telnet 23
>>   - smtp 25
>>   - http 80
>>   - https 443
>>   - dns 53
>
> - udp
>>   - dhcp 67 68



> windows下网络配置查看
>
> `ipconfig [/release] [/renew] [/all]`

## 服务的防火墙
任何以 xinetd 管理的服务，都可以透过 `/etc/hosts.allow`, `/etc/hosts.deny`来配置防火墙，它们也是 `/usr/sbin/tcpd` 的配置文件，这两个文件的判断依据是allow为优先

基本上只要一个服务受到 xinetd 管理，或者是该服务的程序支持 TCP Wrappers 函式的功能时，那么该服务的防火墙方面的配置就能够以 `/etc/hosts.{allow,deny}` 来处

```shell
ldd $(which sshd httpd)
#有无支持tcp wrappers需要看是否依赖libwrap.so
#sshd 有支持，但是 httpd 则没有支持
```

例：只允许 140.116.0.0/255.255.0.0 与 203.71.39.0/255.255.255.0 这两个网域，及 203.71.38.123 这个主机可以进入我们的 rsync 服务器；此外，其他的 IP 全部都挡掉！

```shell
vim /etc/hosts.allow
rsync:  140.116.0.0/255.255.0.0
rsync:  203.71.39.0/255.255.255.0
rsync:  203.71.38.123
rsync:  LOCAL

vim /etc/hosts.deny
rsync: ALL  #利用 ALL 配置让所有其他来源不可登陆
```

服务配置
========
## daemon守护进程
相关目录

- `/etc/init.d/*`启动脚本放置处
- `/etc/sysconfig/*`各服务的初始化环境配置文件
- `/etc/xinetd.conf`, `/etc/xinetd.d/*`super daemon 配置文件
- `/etc/*`各服务各自的配置文件
- `/var/lib/*`各服务产生的数据库
- `/var/run/*`各服务的程序之 PID 记录处

## 查看所有服务
```shell
#找出目前系统开启的『网络服务』有哪些
netstat -tulp

#找出所有的有监听网络的服务 (包含 socket 状态）
netstat -lnp

#观察所有的服务状态
service --status-all
service --status-all | grep httpd
```

## 开机启动服务
`chkconfig`： 管理系统服务默认启动启动与否

```shell
#列出目前系统上面所有被 chkconfig 管理的服务
chkconfig --list |more

#显示出目前在 run level 3 为启动的服务
chkconfig --list | grep '3:on'

#让 atd 这个服务在 run level 为 3, 4, 5 时启动：
chkconfig --level 345 atd on
```


## SSH
代替了telnet，后者通信时使用明文，不安全，另外也不能使用root用户登录
### 安装
```shell
yum install -y *openssh*
```
### 启动
```shell
service sshd start      #redhat专有
/etc/init.d/sshd start  #linux通用，并可借助tab得到智能感知
```

### 登录
```shell
ssh <username>@<ip>        #如果不写<username>，默认是root用户

#设置rsa以后连接时，不需要再输入密码
ssh-keygen                 #产生密钥对
ssh-copy-id -i <sshhostip> #输入相应登录密码后，将放置对方 /.ssh/authorized_keys中

```

> 有关加密
>
> - 对称加密（机密性）：des 3des aes rc4/5；算法简单，适合大量加密
> - 单向加密（完整性）：md5 sha1 sha2；算法简单，不占用计算资源。又称哈希加密，具有不可逆，定长输出，雪崩效应，对应的shell命令有`md5sum` `sha1sum` `sha224sum`等
> - 非对称加密（身份认证，密钥传输）：rsa dsa；不适合大量加密，有公钥和私钥，公钥是私钥中的一部分
>> - 身份认证：自己用私钥加密，发送给他人，他人用公钥解密，可保证发送人的身份有效，又称数字签名
>> - 密钥传输：他人用公钥加密 对称加密用的密钥，传输给具有私钥的人，保证密钥不被窃取
>>> - 还可以使用密钥交换，又称IKE技术（dh算法）

### 远程拷贝
```shell
scp <file> <self_ip>:/path
```

## TFTP
arm板开发时会使用，平时意义不大
### 安装
```shell
yum install *tftp* -y
```
### 配置
```shell
vim /etc/xineted.d/tftp
```

- 其中`server_args = -s /var/lib/tftpboot`表示下载的目录，`-s`表示能下载，`-c`表示能上传
- 其中` disable = yes`改成`no`表示开启服务

为了防止权限问题，可执行

```shell
chmod 777 /var/lib/tftpboot -R
```

### 启动
```shell
chkconfig tftp on       #未修改配置的启动方式
service xineted restart #修改过配置文件后启动方式
```

如果怕安全机制影响，可执行

```shell
service iptables stop  #关闭防火墙
setenforce 0           #关闭SElinux
```

### 客户端连接
```shell
tftp <ip>       #进入命令提示符
 > get <file>   #指定名字，默认下载到当前目录
 > put <file>   #默认上传当前目录中文件
```

## VSFTPD
### 安装
```shell
yum install -y lftp    #客户端
yum install -y vsftpd  #服务器端
```

### 配置
修改文件夹权限

```shell
chmod 777 -R /var/ftp
```

修改配置文件

```shell
vim /etc/vstftpd/vstftpd.conf
```

找到`anon_upload_enable=yes`和`anon_mkdir_write_enable=yes`，前者表示可上传文件，后者表示可上传目录，需要时，可以去掉注释

### 启动
```shell
service vsftpd start
```
### 客户端连接
```shell
lftp <ip>  #进入命令提示符
 > ls                           #可浏览
 > get <file> -o /path/to/file  #下载到指定目录
 > put /path/to/file            #上传指定目录中文件
 > mirror <dir> /path/to/dir    #下载目录
 > mirror -R /path/to/dir       #上传目录
```

## NFS
全称为network file system
> ext3(linux) fat32(windows) ntfs(windows) 均是本地文件系统；cifs (samba协议）共享文件系统，用于windows与linux共享文件；nfs是linux间使用的共享文件系统

### 安装
```shell
yum install -y nfs-utils
yum install -y rpcbind
```

### 配置
```shell
vim /etc/exports
```

可以文件中指定共享的描述，如

`/tmp *(rw,async,no_root_squash)`，分别代表对于该目录，可读写，异步传输，取消root的权限压制

也可以描述为`/tmp <ip>[</网段>]`表示只对某ip网段才能访问

### 启动
```shell
service nfs restart
service rpcbind restart
```
### 客户端挂载
客户端也需要启动上述的服务，然后使用`mount`进行挂载

```shell
mount -t nfs <ip>:/tmp /opt
```

## DHCP
### 安装
```shell
yum install -y dhcp
```

### 配置
```shell
vim /etc/dhcp/dhcpd.conf

subnet 172.24.40.0 netmask 255.255.255.0{
    range 172.24.40.100 172.24.40.200;
    option routers 172.24.40.254;             #getway
    option domain-name "uplooking.com";
    option domain-name-servers 202.106.0.46;  #dns
}
```

### 启动
```shell
/etc/init.d/dhcpd start
```

## DNS
### 概述
FQDN是完全限定域名，准备的描述出主机所在的位置。DNS是倒置的树状结构的 分布式 数据库系统，存储着FQDN名和ip地址的对应关系，负责它们之间的解析

一个局域网内最好只有一个DNS

### 安装
```shell
yum install -y bind
```

### 配置
```shell
vim /etc/named.conf       #负责domain <-> zonefile
vim /var/named/xxx.zone   #负责subdomain <-> ip
```

### 启动
```shell
/etc/init.d/bind start
```

## SAMBA
samba的用户名必须与Linux系统的用户名一致，但密码可以不同，即必须先有linux的用户，然后使用`smbpasswd -a <username>`增加同名用户

### 配置
`/etc/samba/smb.conf`

```
[global]
  workgroup = WORKGROUP
  display charset = UTF-8
  unix charset = UTF-8
  dos charset = cp936
[Public]
  path = /home/zyx/Public
  available = yes
  browseable = yes
  public = yes
  writable = yes
  valida users = zyx
  create mask = 0664
  directory mask = 0775
  force user = zyx
  force group = zyx
```

<!-- ## gcc几个步骤
gcc -E hello.c -o hello.i          #预处理
gcc -S hello.i                     #编译，产行hello.s汇编文件
gcc -c hello.s                     #汇编，产生hello.o二进制目标文件
gcc hello.o -o hello.out           #链接
-->


Vim编辑器
=========
## 模式切换
任何模式，多按几次`ESC`即进入`命令模式`：

- `v`进入`可视模式`
- `Shift v`进入`可视行模式`
- `Ctrl v`进入`可视块模式`
- `： /`进入`提示符模式`
- `i o a`等进入`插入模式`


## 命令模式
多按几次`ESC`即进入命令模式，以下操作均在命令模式下进行

### 光标移动
```shell
h              光标向左移动一个字符
20h            光标向左移动20个字符
l              光标向右移动一个字符

j              光标向下移动一行
20j            光标向下移动20行
k              光标向上移动一行

Ctrl + f       屏幕『向下』移动一页，相当于 [Page Down]按键 #(常用)
Ctrl + b       屏幕『向上』移动一页，相当于 [Page Up] 按键 #(常用)

n[Space]       光标会向后面移动 n 个字符距离
n[Enter]       光标向下移动 n 行 #(常用)

G              移动到这个档案的最后一行 #(常用)
nG             移动到这个档案的第 n 行 #(常用)
gg             移动到这个档案的第一行，相当于 1G 啊！ #(常用)

```

### 定位单词
```
w              到下一个单词的开头  #(常用)
e              到下一个单词的结尾  #(常用)
b              到前一个单词的开头  #(常用)

%              匹配括号移动，包括 (, {, [    需要把光标先移到括号上 #(常用)
* 和 #         匹配光标当前所在的单词，移动光标到下一个（或上一个）匹配单词  #(常用，可用查找配合使用)

0              到行头
^              到本行的第一个非blank字符
$              到行尾
fa             到下一个为a的字符处，你也可以fs到下一个为s的字符。
t,             到逗号前的第一个字符。逗号可以变成其它字符。
3fa            在当前行查找第三个出现的a。
F 和 T         和 f 和 t 一样，只不过是相反方向。

dt"            删除所有的内容，直到遇到双引号(组合用法)

```

### 编辑操作
```shell
x              在一行字当中，x 为向后删除一个字符 (相当于 [del] 按键) #(常用)
X              为向前删除一个字符(相当于 [backspace] ) #(常用)

nx             n为数字，连续向后删除 n 个字符
nX             n为数字，连续删除光标前面的 n 个字符

dd             删除光标所在的那一整行 #(常用)
ndd            n为数字。删除光标所在行向下n行，例如 20dd 则是删除 20 行
d1G            删除光标所在行到第一行的所有数据(组合用法)
dG             删除光标所在行到最后一行的所有数据
d$             删除光标所在处，到该行的最后一个字符
d0             那个是数字的 0 ，删除光标所在处，到该行的最前面一个字符
:n1,n2 d       将 n1 行到 n2 行之间的内容删除 #(注意有冒号)

yy             复制光标所在的那一行 #(常用)
nyy            n为数字。复制光标所在行向下n行，例如 20yy 则是复制 20 行
y1G            复制光标所在行到第一行的所有数据(组合用法)
yG             复制光标所在行到最后一行的所有数据
y$             复制光标所在的那个字符到该行行尾的所有数据
y0             复制光标所在的那个字符到该行行首的所有数据
:n1,n2 d       将 n1 行到 n2 行之间的内容复制 #(注意有冒号)

p              将复制的数据，粘贴在光标的下一行 #(常用)
P              将复制的数据,粘贴到光标的上一行

ddp            上下两行的位置交换(组合用法)

J              将光标所在行与下一行的数据结合成同一行 #(常用)

u              撤销 #(常用)
Ctrl + r       撤销的撤销 #(常用)

```

## 提示符模式
### 查找替换
```shell
/string        向光标之下寻找一个名称为string字符串 #(常用)
?string        向光标之上寻找一个名称为string字符串 #(常用)

n              正向查找，搜索出的string,可以理解成next #(常用)
N              反向查找，搜索出的string,可以理解成Not next #(常用)

:g/str/s/str1/str2/g        第一个g表示对每一个包括s1的行都进行替换，第二个g表示对每一行的所有进行替换
包括str的行所有的str1都用str2替换

:n1,n2s/string1/string2/g   n1是查找的开始行数,n2是查找结束的行数,string1是要查找的字符串,string2是替换的字符串 #(常用)

:2,7s/ddd/fff/g             在第2行,第7行之间，将ddd替换成fff
:1,$s/string1/string2/gc    从第一行到最后一行寻找并替换，在替换前给用户确认 (confirm)

:%s/f $/for/g               将每一行尾部的“f ”（f键和空格键）替换为for，命令之前的"%"指定该命令将作用于所有行上. 不指定一个范围的话, ":s"将只作用于当前行.


:%s/^[^#]/#&                将不是#开头的加上#
```


### 文件操作
```shell
vi +n FileName  打开文件 FileName,并将光标置于第 n 行首。
vi + FileName   打开文件 FileName,并将光标置于最后一行
vi –r FileName  在上次正用 vi 编辑 FileName 发生系统崩溃后,恢复FileName。

:%!xxd          按十六进制查看当前文件
:%!xxd -r       从十六进制返回正常模式
```

### 保存退出
```shell
:w              将编辑的数据写入硬盘档案中 #(常用)
:w!             文件为『只读』时,强制写入,到底能否写入跟用户对档案的权限有关

:q              离开 vi  #(常用)
:q!             若曾修改过档案，又不想储存，使用 ! 为强制离开不储存档案。

:wq             储存后离开 #(常用)
:wq!            强制储存后离开

:ZZ             若档案没有更动，则不储存离开，若档案已经被更动过，则储存后离开!

:e!             重新编辑当前文件,忽略所有的修改

:w [filename]   另存为
:r [filename]   在编辑的数据中，读入另一个档案的数据。亦即将这个档案内容加到光标所在行后面

```

### 运行shell
```shell
:! command         暂时离开 vi 到指令列模式下执行 command 的显示结果！例如 『:! cat ./test』即可在 vi 当中察看当前文件夹中的test文件中的内容

:n1,n2 w! Command  将文件中n1行到n2行的内容作为 Command的输入并执行之，若不指定 n1、n2，则将整个文件内容作为 Command 的输入。

:r! Command        将命令 Command 的输出结果放到当前行。

```

### 设置环境
```
:set nu         显示行号
:set nonu
:syntax on
:set tabstop=4
```

也可在当前用户home目录下，新建.vimrc文件，将配置写入其中（无须set前面的冒号）

## 插入模式
在命令模式下，键入以下将会进入插入模式

```shell
i               在光标前插入 #(常用)
I               在光标行首字符前插入 #(常用)
a               在光标后插入
A               在光标行尾字符后插入 #(常用)
o               在当前行后插入一个新行 #(常用)
O               在当前行前插入一个新行
r               替换当前字符,接着键字的字符作为替换字符
R               替换当前字符及其后的字符，直至按 ESC 键

```

## 代码相关
- 格式化，可视模式，选择后，按`=`
- 代码自动补全，编辑模式，`ctrl p`，`ctrl n｜p`上下选择，`ctrl y`确认选择
- 查找变量或函数的声明，命令模式，`gd`
- 代码折叠，提示符模式`:set fdm=indent｜syntax`，以下是命令模式下
    - zc      折叠
    - zC      对所在范围内所有嵌套的折叠点进行折叠
    - zo      展开折叠
    - zO      对所在范围内所有嵌套的折叠点展开

<!-- http://blog.163.com/stu_shl/blog/static/599375092011639354090/ -->


Sublime快捷键
=============
## 编辑器
```shell
ctrl k b               #切换显示左侧文件夹
ctrl p                 #搜索项目中的文件
ctrl shift p           #控制面板
alt shift <num>        #分屏显示
f11                    #全屏
```

## 查找
```shell
ctrl f                 #当前文件查找
ctrl shift f           #项目或文件夹查找
ctrl h                 #替换
```

## 编辑
```shell
ctrl shift <up|down>   #选中行上下移动
ctrl ]                 #当前行缩进
ctrl [                 #去除当前行缩进
shift tab              #去除缩进

ctrl enter             #在当前行后 插入一行
ctrl shift enter       #在当前行前 插入一行
ctrl j                 #合并选择的多行
ctrl t                 #词互换(首先选择好两个词)

ctrl k k               #从光标处删除至行尾
ctrl k backspace       #从光标处删除至行首

ctrl k u               #大写
ctrl k l               #小写

f9                     #行排序（按a-z）

ctrl y                 #恢复撤销
```

## 选择复制
```shell
ctrl d                 #选择单词，重复可增加选择下一个相同的单词
                       #ctrl f调出查找，把'Aa'选中，则本快捷大小写敏感
ctrl k d               #选择单词，过滤当前，移到下一个相同单词
ctrl shift d           #复制上一行整行
ctrl x                 #剪切当前行 可当删除使用
alt f3                 #选择所有相同的词
ctrl l                 #选择行，重复可依次增加选择下一行
#按住ctrl 左键选择代码，可选不连续代码
#鼠标中键框选代码，可选矩形区代码
#直接在当前行任意位置 ctrl c，复制的是整行
```

## 代码相关
```shell
ctrl r                 #列出所有method
ctrl shift v           #粘贴并自动格式化

ctrl /                 #注释/取消注释 选中的行
ctrl shift /           #注释/取消注释 当前块

ctrl m                 #光标移动至括号内开始或结束的位置
ctrl shift m           #选择当前括号内内容（继续按则选择父括号）
ctrl shift j           #选择当前括号内内容，与上有差异（继续按则选择父括号）

ctrl shift [           #当前括号 折叠代码
ctrl shift ]           #当前括号 展开代码
ctrl k 0               #展开所有代码
ctrl k 1               #代码折叠至一层
ctrl k 2               #代码折叠至二层

alt .                  #闭合当前标签
alt f2                 #增删标记 按f2可来回切

ctrl b                 #编译
f7                     #运行

```

Xcode快捷键
=============
## 编辑
```shell
cmd [|]                #左右缩进
cmd opt [|]            #当前行上下移动

ctrl f                 #前移光标
ctrl b                 #后移光标
ctrl p                 #光标到上一行
ctrl n                 #光标到下一行
ctrl a                 #光标到行首
ctrl e                 #光标到行尾
opt left|right         #光标前进 后退一个单词
#以上加上shift就会选择

mouse three click      #选中当前行

ctrl d                 #删除光标右侧字符 同 fn delete
ctrl k                 #删除光标处到行尾
cmd delete             #删除光标处到行首

ctrl cmd space         #标准表情

```

## 代码相关
```shell
esc                    #当前单词重新补全
cmd r                  #运行
cmd /                  #注释
cmd shift j            #转到定义
cmd opt left|right     #折叠

ctrl i                 #当前格式化
cmd shift o            #显示open quikly
opt mouse dclick       #文档

```

<script>

(function(){
    if(typeof expand_toc !== 'function') setTimeout(arguments.callee,500);
    else expand_toc('md_toc',1);
})();

</script>
