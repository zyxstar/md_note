> 2015-05-09

ARM版本
=======

```
内核版本   Soc版本号           芯片型号
  V4      arm7                S3C44B0
  V4T     arm920T             S3C2410/2440/2416
  V5      arm10
  V6      arm1176             S3C6410
          arm1136
          arm1156
  V6K     arm11MP

  V7      arm-cortex-a
              a8              S5PV210              平板2核
                              S5PC100              手机2核
              a9              Exynos4418           平板4核
                              Exynos4412           手机4核
                              Imax6Q               4核
                              OMAP4460             2核
              a7              Exynos5410(a7+a15)   低性能低功耗
                              全志A10/A20/A31
              a15             Exynos5410(a7+a15)   高性能高功耗
                              Exynos5250/5450
              a53/57          高能骁龙815/410       64位
                              iphone5S             64位
          arm-cortex-m
              m3              STM32f10x
              m0/m0+                               低功耗
              m4              STM32F40x            增加了浮点运算dsp
              m7              STM32F7xx            物联网 低功耗
          arm-cortex-r
              r4/r6
```

启动和Emmc启动
===============
## 查看sd卡
```shell
fdisk -l
```

将会出现下面的描述，表示位于/dev/sdb

```
Disk /dev/sdb: 7948 MB, 7948206080 bytes
```

## 对sd卡进行分区
exynos4412的bootloader必须在该sd卡起始偏移512字节

```
 /dev/sdb #整个sd卡是个线性空间
 [512bytes-bootloader---|-----分区-rom----------|-]
                        /dev/sdb1 #此部分才是可以挂载的
```

```shell
fdisk /dev/sdb
```

```
Command (m for help): d
Selected partition 1

Command (m for help): d
No partition is defined yet!

Command (m for help): p

Disk /dev/sdb: 7948 MB, 7948206080 bytes
81 heads, 10 sectors/track, 19165 cylinders
Units = cylinders of 810 * 512 = 414720 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disk identifier: 0x00000000

   Device Boot      Start         End      Blocks   Id  System

Command (m for help): n
Command action
   e   extended
   p   primary partition (1-4)
p
Partition number (1-4): 1
First cylinder (1-19165, default 1): 3
Last cylinder, +cylinders or +size{K,M,G} (3-19165, default 19165):
Using default value 19165

Command (m for help): p

Disk /dev/sdb: 7948 MB, 7948206080 bytes
81 heads, 10 sectors/track, 19165 cylinders
Units = cylinders of 810 * 512 = 414720 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disk identifier: 0x00000000

   Device Boot      Start         End      Blocks   Id  System
/dev/sdb1               3       19165     7761015   83  Linux

Command (m for help): t
Selected partition 1
Hex code (type L to list codes): b
Changed system type of partition 1 to b (W95 FAT32)

Command (m for help): w
The partition table has been altered!

Calling ioctl() to re-read partition table.

WARNING: Re-reading the partition table failed with error 16: 设备或资源忙.
The kernel still uses the old table. The new table will be used at
the next reboot or after you run partprobe(8) or kpartx(8)

WARNING: If you have created or modified any DOS 6.x
partitions, please see the fdisk manual page for additional
information.
Syncing disks.
```

- `d`是删除分区，`p`是打印信息，`n`是创建分区；
- First cylinder是表示分区起始柱面（一柱面大约8M），此处填3，前面部分用于存放bootloader（此处有可能是起始扇区，一扇区大约512字节，计算时需要注意）
- 此时默认文件格式是linux的，通过`t`，再输入`b`，得到fat32格式
- 输入`w`退出

接下来格式化该sd卡的sdb1分区

```shell
mkfs.vfat -F 32 /dev/sdb1
```

## 烧写sd卡
拷贝光盘B中images文件夹到sdb1

```
cp -rf images /media/F4C9-ED1D/
```

解压`tools.tar.bz2`，得到可执行文件`write4412boot`，执行它，本质是将boot烧写到指定位置

```
./write4412boot /media/F4C9-ED1D/images/Superboot4412.bin /dev/sdb
```

## 配置FriendlyARM.ini
配置FriendlyARM.ini，用于指定刷何种操作系统。然后将sd卡放入开发板，切换为非Emmc启动，重新启动，刷机完成后，切换回Emmc即可




Uboot烧制与调试
================
> Uboot中使用16进，即使不加0x，也是16进制

## 交叉编译器
得到`arm-linux-gcc.tar.tgz`文件，进行解压

配置环境变量，将其下的`bin`的绝对地址加入到`PATH`中

```shell
vim ~/.bashrc
    PATH=/path/to/arm-linux-gcc/4.5.1/bin:$PATH

source ~/.bashrc #或 . ~/.bashrc 以确保当前终端就可用
```

取得当前版本

```shell
arm-linux-gcc -v
```

> 缺库`ld-linux.so.2`时，进行`yum install ld-linux.so.2`

> 如果ubuntu系统出现`/bin/bash` 命令找不到错误，`sudo apt-get install ia32-libs`

## 配置minicom
板子开发需要用到串口调试，原理是开发板与PC通过串口连接，双方通过`/dev/ttyS0`(PC上真实串口)或`/dev/ttyUSB0`(PC机上使用USB转串口时)文件进行读写。使用`minicom`能方便这一过程

```shell
minicom -s
```

- 选择`Serial port setup`
- 在弹出窗口中，选择`A`，输入`/dev/ttyS0`或`/dev/ttyUSB0`
- 选择`E`，确保为`115200 8N1`
- 选择`F/G`，关闭硬件/软件控制
- 回车，退出弹窗，然后选择`Save setup as dfl`
- 最后，选择`Exit from Minicom`

## Uboot流程
得到`uboot201210.tar.tgz`文件，进行解压

- `./arch`是所支持的架构，如x86,arm等
- `./arch/arm/cpu/armv7/exynos/`本开发板的cpu
- `./board/samsung/tiny4412/`本开发板的board BSP

uboot的第一阶段执行流程：

1. `./arch/arm/cpu/Start.S` #它与具体cpu型号无关
2. `./board/samsung/tiny4412/lowlevel_init.S` #开发中对板子的修改，需要编写它
3. `./board/samsung/tiny4412/clock_init_tiny4412.S`
4. `./board/samsung/tiny4412/mem_init_tiny4412.S`
5. `./board/samsung/tiny4412/tiny4412.c`

第二阶段则执行`./arch/arm/cpu/armv7/exynos/`下的相关代码，如`clock.c,nand.c`等

## Make相关
在解压的当前目录

```shell
vim Makefile
    160 CROSS_COMPILE ?=arm-linux-
```

会在`config.mk`中被使用到

```shell
vim config.mk
    104 AS      = $(CROSS_COMPILE)as
    105 LD      = $(CROSS_COMPILE)ld
    106 CC      = $(CROSS_COMPILE)gcc
    107 CPP     = $(CC) -E
    108 AR      = $(CROSS_COMPILE)ar
    109 NM      = $(CROSS_COMPILE)nm
    110 LDR     = $(CROSS_COMPILE)ldr
    111 STRIP   = $(CROSS_COMPILE)strip
    112 OBJCOPY = $(CROSS_COMPILE)objcopy
    113 OBJDUMP = $(CROSS_COMPILE)objdump
    114 RANLIB  = $(CROSS_COMPILE)RANLIB
```

执行make配置，`vim boards.cfg`查看开发板型号、cpu、board之间的一些对应表

```shell
make tiny4412_config
```

关闭MMU功能，并修改提示符

```shell
vim include/configs/tiny4412.h
    311 //#define CONFIG_ENABLE_MMU
    312 #undef CONFIG_ENABLE_MMU
    255 #define CONFIG_SYS_PROMPT  "[zyx@Uboot]# "
```

修改链接地址，本开发板中DDR地址位于`0x40000000`-`0x80000000`，默认情况下是开启MMU的，所以链接地址是`0xc3e00000`

```shell
vim board/samsung/tiny4412/config.mk
    CONFIG_SYS_TEXT_BASE = 0x43e00000
```

接下来make文件，使用4个任务同时进行

```shell
make -j4
```

> 缺库`libz.so.1`时，`yum install zlib.i686`

## 烧制Uboot
上述make完成后，会产生`u-boot`一个可执行文件

```shell
file u-boot
```

```
u-boot: ELF 32-bit LSB shared object, ARM, version 1 (SYSV), statically linked, not stripped
```

可以看到是ARM架构的ELF文件，它的组成示意如下

```
u-boot=[ELF head][u-boot.bin]
```

`[ELF head]`用于记录它的依赖包等情况，后面`[u-boot.bin]`，而烧制uboot时，还没有操作系统，无法认识`[ELF head]`，只能使用`[u-boot.bin]`部分，其实刚才make后，就生成了一个无`[ELF head]`的一个文件`u-boot.bin`

> 可以使用`hexdump -C`去看一下，两者确实只差一个`[ELF head]`

开发板不能直接使用刚才的`u-boot.bin`，必须在它之前加上三星的安全与签名等模块，切换到`./sd_fuse/tiny4412`，执行`sd_fusing.sh`将自动加上（该脚本需要分析一下，大量使用了`dd`）

```shell
cd ./sd_fuse/tiny4412
./sd_fusing.sh /dev/sdb
sync
```

> 如果提示`Block device size ??? is too large`，去修改`sd_fusing.sh`第二段，将`if [ ${BDEV_SIZE} -gt 32000000 ]; then`数字部分调大点

## 调试Uboot
1. 把sd卡插入开发板
2. 开发板切换为sd卡启动
3. 用串口把开发板和pc链接
4. 检查`minicom`的配置
5. 给开发板上电，在`minicom`中观察启动信息

```shell
minicom
```

- `ctrl+a`进入菜单项，`w`切换是否自动换行，`q`退出

常用命令如下

```
?       - alias for 'help'
base    - print or set address offset
bdinfo  - print Board Info structure
boot    - boot default, i.e., run 'bootcmd'
bootd   - boot default, i.e., run 'bootcmd'
bootelf - Boot from an ELF image in memory
bootm   - boot application image from memory
bootp   - boot image via network using BOOTP/TFTP protocol
bootvx  - Boot vxWorks from an ELF image
chpart  - change active partition
cmp     - memory compare
coninfo - print console devices and information
cp      - memory copy
crc32   - checksum calculation
dcache  - enable or disable data cache
dnw     - dnw     - initialize USB device and ready to receive for Windows serv)

echo    - echo args to console
editenv - edit environment variable
emmc    - Open/Close eMMC boot Partition
env     - environment handling commands
exit    - exit script
ext2format- ext2format - disk format by ext2

ext2load- load binary file from a Ext2 filesystem
ext2ls  - list files in a directory (default /)
ext3format- ext3format - disk format by ext3

false   - do nothing, unsuccessfully
fastboot- fastboot- use USB Fastboot protocol

fatformat- fatformat - disk format by FAT32

fatinfo - fatinfo - print information about filesystem
fatload - fatload - load binary file from a dos filesystem

fatls   - list files in a directory (default /)
fdisk   - fdisk for sd/mmc.

go      - start application at address 'addr'
help    - print command description/usage
icache  - enable or disable instruction cache
iminfo  - print header information for application image
imxtract- extract a part of a multi-image
itest   - return true/false on integer compare
loadb   - load binary file over serial line (kermit mode)
loads   - load S-Record file over serial line
loady   - load binary file over serial line (ymodem mode)
loop    - infinite loop on address range
md      - memory display
mm      - memory modify (auto-incrementing address)
mmc     - MMC sub system
mmcinfo - mmcinfo <dev num>-- display MMC info
movi    - movi  - sd/mmc r/w sub system for SMDK board
mtdparts- define flash/nand partitions
mtest   - simple RAM read/write test
mw      - memory write (fill)
nfs     - boot image via network using NFS protocol
nm      - memory modify (constant address)
ping    - send ICMP ECHO_REQUEST to network host
printenv- print environment variables
reginfo - print register information
reset   - Perform RESET of the CPU
run     - run commands in an environment variable
saveenv - save environment variables to persistent storage
setenv  - set environment variables
showvar - print local hushshell variables
sleep   - delay execution for some time
source  - run script from memory
test    - minimal test like /bin/sh
tftpboot- boot image via network using TFTP protocol
true    - do nothing, successfully
usb     - USB sub-system
version - print monitor version
```

- `md`查看内存，`.b`以8位查看(char,2个16进制)，`.w`以16位查看(short,4个16进制)，`.l`以32位查看(int,8个16进制)
- `fdisk`针对sd卡与Emmc查看创建分，`-p`查看，`-c`创建，<device_num>通过启动画面可看到sd与Emmc对应的设备号，如`MMC Device 0: 30608 MB`
- >创建分区是为Android定制的，它有四个分区，`system:data:cache:storage`，前三个是linux文件格式是ext2的(0x83)，而`storage`是fat32的(0x0c)，`kernel/bootloader/ramdisk`存在于 __未划__ 的分区中(分区是会按柱面对齐)
- 格式化不同文件格式的分区，需要不同的`format`，如`fatformat mmc 1:1`或`ext3format mmc 1:4`
- 不同的linux提供的VFS，在这里，针对不同的文件格式分区，需要不同的`ls`，如`fatls mmc 0 /images`或`ext2ls mmc 1:2 /`
- `fatload/ext2load`将rom中的文件load指定的内存地址，方便被`bootm`引导执行
> `fatload mmc 0 0x50000000 /images/linux/zimage`，`bootm 0x50000000`

- `bdinfo`显示板子参数

```
arch_number = 0x00001200    开发板id，bootloader中内核id必须与此一致
boot_params = 0x40000100    开发板启动参数的地址，bootloader传给内核的，(/proc/cmdline 是x86的启动参数)
DRAM bank   = 0x00000000    内存开始地址与大小
-> start    = 0x40000000
-> size     = 0x10000000
DRAM bank   = 0x00000001
-> start    = 0x50000000
-> size     = 0x10000000
DRAM bank   = 0x00000002
-> start    = 0x60000000
-> size     = 0x10000000
DRAM bank   = 0x00000003
-> start    = 0x70000000
-> size     = 0x0FF00000
ethaddr     = 00:40:5c:26:0a:5b
ip_addr     = 192.168.0.20
baudrate    = 0 bps
TLB addr    = 0x3FFF0000
relocaddr   = 0x43E00000    uboot的链接地址，也是uboot的运行地址
reloc off   = 0x00000000
irq_sp      = 0x43CFBF58
sp start    = 0x43CFBF50
FB base     = 0x00000000
```

## 下载文件到开发板
- uart 串口方式
- fatload

```
[zyx@Uboot] # fatload mmc 0:1 0x50000000 /doc/arm.bin
Partition1: Start Address(0x7d82), Size(0x3bbc57b)
reading /doc/arm.bin

688 bytes read
[zyx@Uboot] # go 50000000
## Starting application at 0x50000000 ...
## Application terminated, rc = 0x1
```

- tftp 网络方式
    + pc端安装tftp服务，并存在arm.bin裸机程序
    + 开发板执行`tftp 0x50000000 arm.bin`写入指定地址0x50000000
    + 开发板需要`setenv serverip 192.168.4.254`,`setenv ipaddr 192.168.4.20`,`saveenv`
- dnw  USB方式
    + pc端安装dnw发送端（切换到`tools/dnw-linux`下进行`make`，出现警告要修复，`vim src/driver/secbulk.c`,`to_write = min((int)len, BULKOUT_BUFFER_SIZE);`，再运行`make install`）
    + 如果想卸载:.....[看一下Makefile]
    + pc与开发板通过usb连接
    + 开发板`dnw 0x50000000`，pc执行`dnw arm.bin`，开发板`go 50000000`

```
[zyx@Uboot] # dnw 50000000
Insert a OTG cable into the connector!
OTG cable Connected!
Now, Waiting for DNW to transmit data
Download Done!! Download Address: 0x50000000, Download Filesize:0x2b0
Checksum is being calculated.
Checksum O.K.
```

```
[root@localhost class]# dnw arm.bin
load address: 0x57E00000
Writing data...
100%  0x000002BA bytes (0 K)
speed: 0.004160M/S
```

```
[zyx@Uboot] # go 50000000
## Starting application at 0x50000000 ...
## Application terminated, rc = 0x1
```

## Uboot环境变量
```shell
printenv           #简写pr
echo $bootdelay
```

```
baudrate=115200
bootcmd=movi read kernel 0 40008000;movi read rootfs 0 41000000 100000;bootm 400
bootdelay=3
ethaddr=00:40:5c:26:0a:5b
gatewayip=192.168.0.1
ipaddr=192.168.0.20
netmask=255.255.255.0
serverip=192.168.0.10
```

```shell
setenv bootdelay 10   #临时有效
setenv bootdelay      #删除
saveenv               #保存在sd卡中
```

- `bootargs`内核参数 uboot值将放到`bdinfo`的`boot_params = 0x40000100`地址中，供内核使用，包含 文件系统位置，驱动选择参...
- `bootcmd`自动操作(倒计时结束后执行的操作)，如`set bootcmd "echo $bootdelay;fatls mmc 0:1;fdisk -p 1;" `

## shell脚本
pc上编写脚本test.sh，需要额外加一个头，方便uboot识别

```shell
uboot_tiny4412/tools/mkimage -A arm -O linux -T script -C none -n "uboot_script" -d test.sh test.img
```

下载到开发板

```shell
dnw 50000000
source 50000000
```

编译内核
========
## 给x86下的换内核

```shell
[root@pc]xz -d linux-3.10.5.tar.xz
[root@pc]tar -xvf linux-3.10.5.tar
[root@pc]cd linux-3.10.5
[root@pc]make menuconfig
[root@pc]vim .config
  124 CONFIG_SYSFS_DEPRECATED_V2=y
[root@pc]make dep
[root@pc]make clean
[root@pc]make bzImage
[root@pc]make modules
[root@pc]make modules_install
[root@pc]make install
[root@pc]reboot
```

## 编译内核
```shell
tar -xvf linux3.5.tar.bz2
cd linux-3.5_for_qt4
cp exynos4412_config .config
make -j4
#编译完成的内核在<linux_src>/arch/arm/boot/zImage
```

由于ddr位于`0x40000000`-`0x80000000`，内核必须下载至少`0x40008000`位置

```
zImage
---------------              至少0x40008000
16k translation table
16k bootargs 位于0x40000100
---------------              内存起始0x40000000
```

```shell
[zyx@Uboot]# dnw 40008000
[root@pc]#dnw <linux_src>/arch/arm/boot/zImage
[zyx@Uboot]# bootm 40008000
```

会运行内核，但由于找不到根目录，将会异常

busybox
==========
## 编译busybox
使用busybox来制作文件系统，挂载根目录

> android使用toolbox

```shell
[root@pc]tar -xvf busybox-1.21.1.tar.bz2
[root@pc]cd busybox-1.21.1
[root@pc]make defconfig
[root@pc]make menuconfig
#如果出现ncurse相关错误  yum install ncurse*
```

具体配置如下，需将busybox编译成静态库，并设置好arm编译器前缀，并勾选必要的内核使用的命令

![img](../../imgs/busybox_config_01.png)

![img](../../imgs/busybox_config_02.png)

![img](../../imgs/busybox_config_03.png)

![img](../../imgs/busybox_config_04.png)

![img](../../imgs/busybox_config_05.png)

```shell
[root@pc]make -j4
[root@pc]make install
[root@pc]cd _install/
[root@pc]mkdir home root var mnt tmp sys dev proc #创建必要目录
[root@pc]cp ../examples/bootfloppy/etc/ . -rf     #复制etc
[root@pc]cp ~/class_t/4.5.1/arm-none-linux-gnueabi/lib/ . -rf  #复制lib
```

分析`busybox-1.21.1/init/`目录下文件，这是`init`进程的源代码

嵌入式系统由`linuxrc`启动，调用`init.d/rcS`

## 搭建nfs服务
将busybox中的`_install`目录下文件复制到nfs的指定目录

```shell
[root@pc _install]mkdir /tomcat_root
[root@pc _install]chmod 777 /tomcat_root
[root@pc _install]cp * /tomcat_root -rf
[root@pc _install]cd /tomcat_root
[root@pc]vim etc/profile #登录用户时执行
    echo "#######################"
    echo "#######################"
    echo "#######################"
    echo "#######################"
    echo "Welcom To My FileSystem"
    echo "#######################"
    echo "#######################"
    echo "#######################"
    echo "#######################"
    #在/dev/下创建设备节点
    mdev -s
    #export是确保子进程也能使用
    export PATH=/bin:/sbin:/usr/bin:/usr/sbin
    #\W会被替换为当前工作路径
    export PS1="[root@uplooking \W]# "
    export HOME=/root
[root@pc]vim etc/init.d/rcS     #由init进程执行，不修改
[root@pc]vim etc/fstab          #mount -a时使用，在etc/init.d/rcS中
    proc   /proc proc   defaults  0 0
    sysfs  /sys  sysfs  defaults  0 0
    tmpfs  /tmp  tmpfs  defaults  0 0
```

配置nfs服务器

```shell
[root@pc]vim /etc/exports
    #no_root_squash表示root用户进入后，还是root权限，sync表示每次创建更新就会同步
    /tomcat_root *(rw,no_root_squash,sync)
```

关闭防火墙，启动服务

```shell
[root@pc]service iptables stop
[root@pc]setup #永久关闭
   --->防火墙设置
    [ ]开启防火墙
[root@pc]setenforce 0 #关闭selinux
[root@pc]vim /etc/selinux/config #永久关闭
    SELINUX=disabled
[root@pc]service nfs restart
```

## 网络文件系统作根
本着先使用网络系统，再使用本地系统的原则进行

```shell
[zyx@Uboot]set bootargs root=/dev/nfs nfsroot=192.168.4.252:/tomcat_root ip=192.168.4.20 console=ttySAC0 lcd=S70
[zyx@Uboot]save
```

用网线把开发板和pc链接，并配置pc与开发板处于同一网段（pc如果是虚拟机时，需要使用桥接模式）

```
[zyx@Uboot]dnw 40008000
[root@pc]dnw <linux_src>/arch/arm/boot/zImage

[zyx@Uboot]dnw 41000000
[root@pc]dnw ramdisk-u.img

[zyx@Uboot]bootm 40008000 41000000

#如果成功minicom会出现如下shell，可运行busybox命令
[root@uplooking \]#
```



朱老师
=============
