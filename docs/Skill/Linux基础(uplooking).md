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
/var        系统中经常需要变化的一些文件，如系统日志
            /var/tmp 程序运行时使用的临时文件，删除可能会出错
            /var/lib;/var/lib64 库文件
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
            du -sh /proc 统计大小为0，表示数据存在内存中，本身不占硬盘空间
/opt        主机额外安装软件所放的目录
/srv        一些服务启动后，这些服务所需访问的数据目录

```

## 分区设备
```
IDE硬盘
  hda     a表示第1个         a~d
  hda1    1表示第一个分区    1~63

SCSI硬盘
  sda     a表示第1个         a~p
  sda1    1表示第一个分区    1~15 最多4个主分区或3个主分区1个扩展分区
```
使用`ls /dev/hd*`，`ls /dev/sd*`

## 分区建议

```
/boot ext3 200M
      swap 4096M
/home ext3 看情况
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
```

## 帮助
```

```

ls --help|grep -E "time|sort"     #查找ls命令手册中有关时间与排序的条目

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
    -l    #显示详细信息，别名ll
    -t    #以时间顺序排列查询结果
    -h    #人性化显示
    -i    #显示inode号
    -d    #只显示目录信息
          ls -d */           #只显示目录
          ls -ld /<dir>      #可以看到该目录的访问权限,不加d 有可能直接deny
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
          #?<keyword>查找光标之前关键字；/<keyword>查找光标之后关键字
          #n下一个关键字；shift n上一个关键字
head
tail
    -f    #实时监控
rev       #左右相反
tac       #cat相反
tree      #树形显示目录
du

file      #查看文件属性
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
    {}   #创建目录集合 mkdir dir{5..8}; mkdir dir{a..d}; mkdir dir{a,d}

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
```
防止别名\


vim编辑
=======
命令模式
可视模式（可视、可视行、可视块）
插入模式
末行模式

软件安装
========
yum

配置yum源

yum repolist
yum -y install
yum list
yum -y remove
yum grouplist
yum -y groupinstall


rpm

用户管理
========


权限管理
========
getfacl   #查看文件权限


进程控制
========


服务配置
========

vnc
