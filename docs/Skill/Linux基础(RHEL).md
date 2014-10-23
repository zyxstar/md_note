> 2014-09-27

安装相关
========
## 安装事项
RHEL的经典版本：5.8和6.4

注意事项
> 从U盘安装时，GRUB引导程序默认在U盘上（`/dev/sdb`），需安装在`/dev/sda`上
>
> 新手安装时，防火墙、SElinux、kdump关闭

[Redhat网络系统安装说明](../../data/Redhat网络系统安装说明.pdf)

## Linux目录结构(FHS)
```
/           根目录
/boot       存放程序启动所必须的文件
            /boot/vmlinuz-3.2.0-61vm-generic-pae    内核文件
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
/opt        主机额外安装软件所放的目录，也是个挂载点
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
使用`sudo fdisk -l`可查看分区情况，`df -h`查看文件系统使用情况，`du -sh /path/to` 统计指定目录的大小

## 快捷键
```
ctrl alt F1~F6     图形界面切换到字符界面
ctrl F7            字符界面切换到图形界面
ctrl shift +       放大窗口
ctrl -             缩小窗口
ctrl shift t       新建窗口
alt <num>          切换窗口
ctrl l             清屏 或clear命令
esc .              使用上次命令的参数
快捷复制           鼠标左键选中即复制 鼠标按中键即粘贴
```

## 帮助
```shell
<command> --help
          #查找ls命令手册中有关时间与排序的条目
          #ls --help|grep -E "time|sort"     

man       #man是分章节的
            1 User Commands
            2 System Calls
            3 C Library Functions
            4 Devices and Special Files
            5 File Formats and Conventions
            6 Games et. Al.
            7 Miscellanea
            8 System Administration tools and Deamons
          #查找fopen的使用，在第3章节查找
          #man 3 fopen

          #操作同less
          #space下一页；b上一页；enter下一行；q退出；G最后；g最前
          #?<keyword>查找光标之前；/<keyword>查找光标之后
          #n下一个关键字；shift n上一个关键字

          #将man手册转存为文本
          #man ls |col -b >ls.txt  #col -b 过滤掉所有的控制字符
          #info make -o make.txt -s
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
          #cat /dev/null > a.txt 使文件清空，但所有者和权限不变
          #列出两个文件的内容.执行对输出的搜索.统计结果行的个数
          #cat file1 file2 | grep word | wc -1

nl        #向文件添加行号

wc        #统计行数，字符，字节

more      #分屏，翻到最后直接退出，无法前翻

less      #分屏，可前翻
          #space下一页；b上一页；enter下一行；q退出；G最后；g最前
          #?<keyword>查找光标之前关键字；/<keyword>查找光标之后
          #n下一个关键字；shift n上一个关键字
head
tail
    -f    #实时监控
rev       #左右相反
tac       #cat相反
tree      #树形显示目录
du

file      #查看文件属性
getfacl   #查看文件权限
```

## 查找
```shell
grep "<key>" <path/to/file>
    --color  #高亮
    -n       #显示行号

find <path>
    -name "pattern"
    -size +1000M
    #查找指定类型的文件，然后指定按时间排序
    -name *.php |xargs ls -alt
    #统计所有c文件的行数总和，不包括空行
    -name "*.c" |xargs cat|grep -v ^$|wc -l

```

## 创建
```shell
touch    #已创建的不会被清空

mkdir    #创建多个目录 mkdir dir1 dir2
    -p   #创建层次目录 mkdir -p aa/bbb/ccc
    {}   #创建目录集合
         #mkdir dir{5..8}; mkdir dir{a..d}; mkdir dir{a,d}

```

## 复制
```shell
cp <src> <dest>
    -r   #recursive，实现同时拷贝文件和文件夹
    -f   #force，强制覆盖
    -v   #verbose
    -i   #ineractive
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

防止别名\


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









其它命令
========

```shell
&&顺序执行


mii-tool eth0

vncviewer ip
/etc/init.d/iptables stop
setenforce 0


lftp 

eject
eject -t


pkill -9 httpd
isof -i:80


/etc/ iptables stop
lftp <ip>

cherrytree


fdisk -l
mount /dev/sdb /media
      /dev/sr0 /mnt
umount /dev/sdb      
df -h


/usr/local/apache2/bin/apachectl start

lsof -i:80  80端口是否启动

netstat -ntlp

mii-tool eth0 哪块网卡有效
ifconfig eth0 <ip> 临时ip
service dhcpd stop


q 276267003


ntfs u盘
vnc
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

### 删除
直接删除由`--prefix`指定的目录

用户管理
========
## 创建
```shell
useradd <username>
```

## 查看
```shell
id <username>
```

出现类似`uid=502(jim) gid=502(jim) 组=502(jim),0(root)`

- uid:user identify,数字，可能出现uid相同，但用户名不同的情况，以uid为准
- gid:group identify,数字
- 组:主属组+附属组,数组，通过`usermod -G root jim`给jim增加 __附属组__
            
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

## 切换
```shell
su - <username>  #加载环境变量
su <username>    #不会加载环境变量，不推荐使用
```

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

文件权限管理
============
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

以数字方式修改 r4 w2 x1

```shell         
chmod 744 a.txt
```

## 修改文件所属者/组
间接修改了权限

```shell
chown <username> file   #修改文件所属者
chown .<groupname> file #修改文件所组，前面加点
chown <username> .<groupname> file #同时修改文件所属者与组
```

## 其它
对于root用户，x权限同属主，rw的设置是无效的，依然能读能写

其它权限：  suid sgid skicky


进程控制
========
## 查看进程
```shell
ps      #当前终端上进程
ps a    #所有终端上进程
ps ax   #所有进程，包括非终端上进程
ps aux  #把进程启动的用户(及起始时间)包含进来
ps e    #显示shell环境信息
ps f    #格式化父子进程的显示

top
free -k

```

## 杀死进程
```shell
ps aux | grep <keyword>   #查到进程pid
kill -9 <pid>
```

`kill -l` 列出所有信号：

- -9 强制 
- -15 默认
- - 19 挂起
- - 18 唤醒

```shell
w               #看到所有登录系统的账户，查看是否异常
killall -9 sshd #杀死进程名称为sshd的进程，
                #即把所有通过ssh登录到系统的账户登出
```


服务配置
========
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
ssh <username>@<ip>     #如果不写<username>，默认是root用户
```
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
> ext4(linux) fat32(windows) ntfs(windows) 均是本地文件系统；cifs (samba协议）共享文件系统，用于windows与linux共享文件；nfs是linux间使用的共享文件系统

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




vim编辑器
==========
命令模式
可视模式（可视、可视行、可视块）
插入模式
末行模式


vim
    可视模式 V
        选择到
        y 复制
        p 
        d
    可视行 S+V

    可视块 C+V
        I esc

    c格式化  ＝

    u
    C+r



