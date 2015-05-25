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
[zyx@Uboot]set bootargs root=/dev/nfs nfsroot=192.168.4.118:/tomcat_root ip=192.168.4.119 console=ttySAC0 lcd=S70
[zyx@Uboot]save
```

> uboot中相关的网络也配置好（gatewayip,serverip,ipaddr等）

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

## 制作本地根
- 内核：zImage
- 根：rootfs.img

需要把 根目录 变成 镜像

```shell
./tools/linux_tools/make_ext4fs -s -l 512M -L linux rootfs.img /tomcat_root/

#  -s 按实际大小来算
#  -l 镜像最大大小 分区是512M
#  -L linux linux系统
#  -a root  android系统
```

产生的`rootfs.img`大约30M

将`linux_images.tar.bz2`解压到sd卡的`images\`中

把`zImage`和`rootfs.img`复制到sd卡`images\Linux\`中

```shell
cp linux-3.5_for_qt4/arch/arm/boot/zImage /media/88DE-4A63/images/Linux/
cp /rootfs.img /media/88DE-4A63/images/Linux/
```

修改刷机配置文件`FriendlyARM.ini`

```shell
vim FriendlyARM.ini
Linux-BootLoader = Superboot4412.bin
Linux-Kernel = Linux/zImage
Linux-CommandLine = root=/dev/mmcblk0p1 rootfstype=ext4 console=ttySAC0,115200 init=/linuxrc  ctp=2 skipcali=y lcd=S70
Linux-RamDisk = Linux/ramdisk-u.img
Linux-RootFs-InstallImage = Linux/rootfs.img

#Linux-CommandLine 就是 bootargs
#Linux-RootFs-InstallImage = Linux/rootfs.img 将根解压到 root=/dev/mmcblk0p1中
#加一个lcd=S70 | HD70   普通|高清屏
#Linux-RamDisk是一个临时根，是uboot作的
```

烧制sd卡

```shell
tools/write4412boot /media/88DE-4A63/images/Superboot4412.bin /dev/sdb
```

刷完切换为emmc启动

> 由于负载不大，所以启动时，内核会关闭CPU3,CPU2,CPU1，只保留CPU0

## 挂载nfs目录
开发板中

```shell
ifconfig eth0 192.168.4.119
mount -t nfs -o nolock,rw 192.168.4.118:/tomcat_root /mnt
chroot /mnt/
#切换mnt下目录为根
#缺任何命令时，ln /bin/busybox /bin/chroot -s
exit #退出刚才切入的根
```

ARM架构
=========
> 参照[ARM_Architecture_Reference_Manual.pdf](../../data/ARM_Architecture_Reference_Manual.pdf) p39

Byte 8 bits
Halfword 16 bits
Word 32 bits

- a8共有7种模式
- a9还有一个安全模式

- system模式存在，但没人用过


- arm中寄存器是32位 or 64位
- 32位机是数据总线，寄存器大小，与地址总线无关
- linux内核只使用了普通中断，未使用快速中断

## 寄存器

- r13 SP stack pointer

内存的布局，sp指向当前正在运行的进程的栈地址，意味着进程切换时，也需要cpu去修改sp指向

```plain
4G--------
   kernel
3G--------

        <-------sp 当前正在运行的进程的栈地址（进程切换时，也会修改sp）
        ----4   sp-1
        ----4   sp-1
        <-------sp

  -------heap
  -------xxx.so
  -------rodata
  -------data
  -------text
   application
0G--------
```

- r14 LR link register
- r15 PC process counter

```plain
a(){
  ------------    进入a函数时，将LR的值保存在SP中（保证嵌套时，LR不会被覆盖）
  ------------
  ------------    函数结束时，把LR值恢复，PC=LR的值，则执行函数返回后的代码
}

b(){
  ------------
  ------------
  a();            先将下一条指令放在LR中，然后改变PC，使PC=a，则执行a函数
  ------------
```

> - arm在函数传参、返回值、保存返回地址，均使用寄存器，只有局部变量使用栈
> - 而x86中上述的值均使用栈来保存

- cpsr
- spsr

### ARM寄存器的别名

```table
Reg#|  APCS|  意义
----|------|------
R0  |a1    | 工作寄存器
R1  |a2    | "
R2  |a3    | "
R3  |a4    | "
R4  |v1    | 必须保护
R5  |v2    | "
R6  |v3    | "
R7  |v4    | "
R8  |v5    | "
R9  |v6    | "
R10 |sl    | 栈限制
R11 |fp    | 桢指针
R12 |ip    | 内部过程调用寄存器
R13 |sp    | 栈指针
R14 |lr    | 连接寄存器
R15 |pc    | 程序计数器
```


## 流水线
- arm9   5级流水线
- arm11  8级流水线
- a8     13级流水线
- a9     12级流水线

流水线多了，最怕指令跳转导致的流水线重新构建，arm对此增加了预取指判断

## 指令集
- arm指令集     32位
- thumb指令集   16位     armV7 m系列
- thumb2指令集  32/16位
- java字节码

## 哈佛与冯诺依曼

- 哈佛（安全性高，性能高）

```
  cpu_core  --->  I-cache(一级 指令cache)
                                      ------> cache(二级统一cache)----->  ddr
            --->  D-cache(一级 数据cache)
```

- 冯诺依曼

```
  cpu_core  --->  cache(一级统一cache) ------> cache(二级统一cache)----->  ddr
```

- a8单核 自带二级cache

```
   a8_core-->D-cache         |
                 ---->cache--|->ddr
          -->I-cache         |
```

- a9多核 不带二级cache，方便多个核共享一个二级cache

```
   a9_core-->D-cache |
          -->I-cache |
   a9_core-->D-cache |
          -->I-cache |
                 ----|->cache---->ddr
   a9_core-->D-cache |
          -->I-cache |
   a9_core-->D-cache |
          -->I-cache |
```

- cpu = cpu_core + mmu + 协处理器 + cache + tcm...
    + cache 通过行，组等方式访问
    + tcm 紧耦合性内存 通过地址访问（指针） 与访问寄存器一样快
    + 协处理器: cp11(浮点VFP) cp10(图形NEON) cp14(DEBUG) cp15(控制CTRL)....




ARM汇编
=========
> - 内嵌汇编编译器：armcc(keil内部集成)， gcc
> - 编译过程
>     - gcc   x.c  x.s  优化在此发生，为了防止优化，请使用`volatile`
>     - as    x.s  x.o
>     - ld    x.o  x

## 汇编指令
> [arm常用指令.pdf](../../data/arm常用指令.pdf)



##　绑定寄存器
- 立即数，被编译到指令当中的数据:
    + 本身小于等于255 (一条指令32位，所以指令中的数据就最大可以存8位)
    + 经过循环右移偶数位之后小于等于255
    + 按位取反之后符合上面

```c
#include <stdio.h>

int main(void){
  register unsigned int a asm("r0");
  asm volatile(
    "mov r0, #4\n"
    :
    :
    :"r0"
  );
  printf("r0 = %x\n", a);
  return 0;
}
```

## 输入输出变量
- 输出变量，对于汇编而言的，汇编对这个变量只能写不能读
- 输入变量，对于汇编而言的，汇编对这个变量只能读不能写
- 输入变量，输出变量，输入输出变量的具体寄存器名由编译时指定，可通过`arm-linux-gcc -S`来生成汇编查看
- `=`:代表输出变量
- `+`:代表输入输出变量
- `r`:操作方式是寄存器
- `m`:操作方式是内存
- `&`:代表输出变量和输入变量　__不共用__ 寄存器
- 三个冒号，第一行用于声明　`输出`　或　`输入输出`　变量；第二行用于声明　`输入`　变量；第三行用于　__保护__ 上面代码块中显示使用过的寄存器，使用内存的需要标识`memory`

- 声明输入输出变量，使用 `下标` 来访问

```c
#include <stdio.h>

int main(void){
  int in = 40;
  int out = 23;

  asm volatile(
    "mov %0, %1\n"
    :"=&r"(out) //声明输出变量
    :"r"(in) //声明输入变量
    :
  );
  printf("in = %d\n", in);
  printf("out = %d\n", out);
  return 0;
}
```

对应汇编如下，将下标转换为寄存器

```assembly
#APP
@ 7 "03in_out.c" 1
  mov r4, r3

@ 0 "" 2
  str r4, [fp, #-20]
```

- 声明输入输出变量，使用 `名称` 来访问

```c
#include <stdio.h>

int main(void){
  int a = 5;
  int b = 6;
  int c; //temp

  asm volatile(
    "mov %[c], %[a]\n"
    "mov %[a], %[b]\n"
    "mov %[b], %[c]\n"

    :[a]"+r"(a),[b]"+r"(b),[c]"+r"(c)
    :
    :
  );
  printf("a = %d\n", a);
  printf("b = %d\n", b);
  return 0;
}
```

## 64位加法
- 除了`比较指令`和直接操作`cpsr`的指令，其他指令要影响`cpsr`标志位就必须加`s`
- 利用`adds`和`adc`，`adds`后有进位，`cpsr_C`的标志位就为1
- 如果要实现更大位的相加，`adc`也可以使用`s`后缀

```c
#include <stdio.h>

int main(void){
  int a, b;

  //0x0000000400000005
  //0x00000008ffffffff
  //0x0000000d00000004
  asm volatile(
    "mov r0, #5\n"
    "mvn r1, #0\n"       //r1=~0;
    "adds %0, r0, r1\n"  //cpsr_C=1
    "mov r0, #4\n"
    "mov r1, #8\n"
    "adc %1, r0, r1\n"   //%1=r0+r1+C
    :"=&r"(a), "=&r"(b)
    :
    :"r0", "r1"
  );

  printf("%#010x%08x\n", b, a);
  return 0;
}
```

## 64位减法
- arm本身没有减法，减法是通过加法和补码（取反+1）配合得到的
- 利用`subs`和`sbc`，`subs`有借位后，`cpsr_C`的标志位就为0

```c
#include <stdio.h>

int main(void){
  int a, b;

  //0x0000000900000004
  //0x0000000400000005
  //0x00000004ffffffff

  __asm__ __volatile__(
    "mov r0, #4\n"
    "mov r1, #5\n"
    "subs %0, r0, r1\n"   //clear C
    "mov r0, #9\n"
    "mov r1, #4\n"
    "sbc %1, r0, r1\n"    //%1=r0-r1-!C
    :"=&r"(a), "=&r"(b)
    :
    :"r0", "r1"
  );

  printf("%#010x%08x\n", b, a);
  return 0;
}
```

## 乘加/乘减
- 方便数学上的西格玛计算

```c
#include <stdio.h>

int main(void){
#if 0
       i = 0;
  c =  ∑    i * i
       i<100

  i = 0;
  while(i < 100)｛
  　　　mla r2, r0, r0, r2
  　　　i++;
  ｝
#endif
  int a;

  asm volatile(
    "mov r0, #3\n"
    "mov r1, #4\n"
    "mov r2, #5\n"
    //"mla %0, r0, r1, r2\n"  //%0=r2+r0*r1
    "mls %0, r0, r1, r2\n"    //%0=r2-r0*r1
    :"=&r"(a)
    :
    :"r0", "r1", "r2"
  );

  printf("a = %d\n", a);
  return 0;
}
```

## 位操作
- 清零操作`bic`

比如有个控制LED的寄存器，第`n`位为`1`则该位对应的灯亮

```
LENCON |= 1 << n;              #c语言
orr LENCON, LENCON, 1 << n     #汇编
```

如果想第`n`与`n+1`位为零，即相邻两位要清零，相邻两位表达为`bin11`，即十进制的`3`

```
LEDCON &= ~(3 << n);          #c语言

mvn r0, 3 << n                #汇编1
and LEDCON, LEDCON, r0

bic LEDCON, LEDCON, 3 << n    #汇编2
```

- bit反转`rbit`

```c
int main(void){
  int bic, rbit = 0x12345678;

  asm volatile(
    "mov r0, #0xff\n"
    "mov r1, #(3 << 2)\n"  //1100  3 << 2
    "bic %0, r0, r1\n"     //%3=r0&(~r1)
    "rbit %1, %1\n"
    :"=r"(bic), "+r"(rbit)
    :
    :"r0", "r1"
  );

  printf("bic = %x\n", bic);
  printf("rbit = %x\n", rbit);

  return 0;
}
```

- 左移右移
    + `lsl` 逻辑左移
    + `lsr` 逻辑右移
    + `asr` 算术右移，空出位以符号位补充，不存在算术左移，`asl`将翻译成`lsl`
    + `ror` 循环右移


## 大小端转换
```c
#include <stdio.h>

int main(void){
  int a = 0x12345678;

  //htonl  ntohl
  asm volatile(
    "rev %0, %0\n"     //32位
    :"+r"(a)
  );

  printf("a = %x\n", a);

  a = 0x1234;
  //htons ntohs
  asm volatile(
    "rev16 %0, %0\n"   //16位
    :"+r"(a)
  );

  printf("a = %x\n", a);
  return 0;
}

```

## 读写cpsr
- `mrs`读`cpsr`
- `msr`写`cpsr`，user模式只能修改`NZCVQ`位

```c
#include <stdio.h>

int main(void){
  int c;
  asm volatile(
    "mrs %0, cpsr\n"  //%0=cpsr
    :"=&r"(c)
  );

  printf("c = %x\n", c);

  //N=0 Z=0 C=0 V=0 Q
  asm volatile(
    "mov r0, #3\n"
    "adds r0, r0, #0\n"
    :::"r0"
  );

  asm volatile(
    "mrs %0, cpsr\n"  //%0=cpsr
    :"=&r"(c)
  );

  printf("c = %x\n", c);

  asm volatile(
    "mrs r0, cpsr\n"
    "mvn r1, #0\n"
    "orr r0, r0, r1\n"
    "msr cpsr, r0\n"  //cpsr=r0
    :::"r0", "r1"
  );

  asm volatile(
    "msr cpsr, #0xff\n"
  );

  asm volatile(
    "mrs %0, cpsr\n"  //%0=cpsr
    :"=&r"(c)
  );

  printf("c = %x\n", c);
  return 0;
}
```

## 内存操作
### 普通变量
- `m`:操作方式是内存，`"m"(a)`表示`a`的地址作为输入变量
- 第三行冒号，使用内存的需要标识`memory`

```c
  int a = 4;
  asm volatile(
    "ldr r0, %0\n"   //r0=*addr  %0是a的地址
    "add r0, r0, #1\n"
    "str r0, %0\n"   //*addr=r0
    :
    :"m"(a)
    :"r0", "memory"
  );
  printf("a = %d\n", a);
```

### 数组操作1
```c
  int b[3] = {0, 1, 2};

  asm volatile(
    "add %0, %0, #1\n"
    "add %1, %1, #1\n"
    "add %2, %2, #1\n"
    :"+r"(b[0]), "+r"(b[1]), "+r"(b[2])
    :
    :
  );

  int i;
  for(i = 0; i < 3; i++)
    printf("b[%d] = %d\n", i, b[i]);
```

### 数组操作2
```c
  int b[3] = {0, 1, 2};

  asm volatile(
    "mov r0, %0\n"
    "ldr r1, [r0]\n"   //r1=*r0
    "add r1, r1, #4\n"
    "str r1, [r0]\n"   //*r0=r1

    "add r0, r0, #4\n" //地址移4位
    "ldr r1, [r0]\n"
    "add r1, r1, #4\n"
    "str r1, [r0]\n"

    "add r0, r0, #4\n"
    "ldr r1, [r0]\n"
    "add r1, r1, #4\n"
    "str r1, [r0]\n"
    :
    :"r"(b)
    :"r0", "r1"
  );

  int i;
  for(i = 0; i < 3; i++)
    printf("b[%d] = %d\n", i, b[i]);
```

### 数组操作3
```c
  int b[3] = {0, 1, 2};

  asm volatile(
    "mov r0, %0\n"

    "ldr r1, [r0]\n"     //r1=*r0
    "add r1, r1, #3\n"
    "str r1, [r0], #4\n" //*r0=r1 r0+=4 修改值后 直接地址移动　上面两条指令合并成一条

    "ldr r1, [r0]\n"     //r1=*r0
    "add r1, r1, #3\n"
    "str r1, [r0], #4\n" //*r0=r1 r0+=4

    "ldr r1, [r0]\n"     //r1=*r0
    "add r1, r1, #3\n"
    "str r1, [r0], #4\n" //*r0=r1 r0+=4
    :
    :"r"(b)
    :"r0", "r1"
  );

  int i;
  for(i = 0; i < 3; i++)
    printf("b[%d] = %d\n", i, b[i]);
```

### 数组操作4
```c
  int b[3] = {0, 1, 2};

  asm volatile(
    "mov r0, %0\n"
    "ldr r1, [r0]\n"
    "add r1, r1, #2\n"
    "str r1, [r0]\n"

    "ldr r1, [r0, #4]\n"   //r1=*(r0+4) 不改变数据基地址,通过位移来访问
    "add r1, r1, #2\n"
    "str r1, [r0, #4]\n"

    "ldr r1, [r0, #8]\n"   //r1=*(r0+8)
    "add r1, r1, #2\n"
    "str r1, [r0, #8]\n"
    :
    :"r"(b)
    :"r0", "r1"
  );

  int i;
  for(i = 0; i < 3; i++)
    printf("b[%d] = %d\n", i, b[i]);
```

### 数组操作5
```c
  int b[3] = {0, 1, 2};

  asm volatile(
    "mov r0, %0\n"
    "ldr r1, [r0]\n"
    "add r1, r1, #5\n"
    "str r1, [r0]\n"

    "ldr r1, [r0, #4]!\n"    //r0+=4 r1=*r0 直接地址移动　再取值　也是两条指令合并成一条
    "add r1, r1, #5\n"
    "str r1, [r0]\n"

    "ldr r1, [r0, #4]!\n"    //r0+=4 r1=*r0
    "add r1, r1, #5\n"
    "str r1, [r0]\n"
    :
    :"r"(b)
    :"r0", "r1"
  );

  int i;
  for(i = 0; i < 3; i++)
    printf("b[%d] = %d\n", i, b[i]);
```

## 栈操作
- c语言中入栈采用FD(满递减模式)，即`[sp]`的指向是有效空间，当需要入栈时，先`sp=sp-1`，然后再把值放到`[sp]`中；而出栈，则先出栈，后`sp=sp+1`

### 内存方式读写栈
- 因为`[sp]`指向的内容是有效空间，这种方式容易造成程序错误

```c
  int a;
  
  asm volatile(
    "ldr %0, [sp]\n"
    "mov r0, #10\n"
    "str r0, [sp]\n"
    :"=&r"(a)
    :
    :"r0"
  );

  printf("a = %x\n", a);
```

### push/pop操作栈
- 入栈和出栈时寄存器需要保持从小到大的次序（否则会有警告），__低地址__ 空间的值对应 __小号__ 的寄存器（低地址空间的先出栈）

```c
  int a, b, c;
  
  asm volatile(
    "mov r0, #1\n"
    "mov r1, #2\n"
    "mov r2, #3\n"
    "push {r0-r2}\n"
    "pop {r3-r4, r5}\n"
    "mov %0, r3\n"
    "mov %1, r4\n"
    "mov %2, r5\n"
    :"=&r"(a), "=&r"(b), "=&r"(c)
    :
    :"r0", "r1", "r2", "r3", "r4", "r5"
  );

  printf("a = %x\n", a);
  printf("b = %x\n", b);
  printf("c = %x\n", c);
```

### stmfd/ldmfd操作栈
明确指出使用满递减的方式操作栈

- stmfd (ST: Store, M: Multiple, F: FULL, D: Descending)
- ldmfd (LD: Load, M: Multiple, F: FULL, D: Descending)

```c
  int a, b, c;
  
  asm volatile(
    "mov r0, #1\n"
    "mov r1, #2\n"
    "mov r2, #3\n"
    "stmfd sp!, {r0-r2}\n"
    "ldmfd sp!, {r3-r4, r5}\n"
    "mov %0, r3\n"
    "mov %1, r4\n"
    "mov %2, r5\n"
    :"=&r"(a), "=&r"(b), "=&r"(c)
    :
    :"r0", "r1", "r2", "r3", "r4", "r5"
  );

  printf("a = %x\n", a);
  printf("b = %x\n", b);
  printf("c = %x\n", c);
```

## 原子操作
- c语言中类型修饰符为`atomic_t`
- 汇编中使用`ldrex`和`strex`，并依据`cmp`来判断是否修改成功，不成功则重新执行
- 内核调度的是线程（不是进程），类似锁、互斥量都是由汇编中原子操作来支持

```c
atomic_t a;

atomic_add(a, 1);

int atomic_add(int a, int b){
  asm volatile(
    "again:\n"
    "ldrex r0, %0\n"
    "add r0, r0, %1\n"
    "strex r1, r0, %0\n"
    "cmp r1, #0\n"
    "bne again\n"
    :
    :"m"(a), "r"(b)
    :
  );
}
```

## 比较指令与条件执行
- 比较指令是不需要加`s`就能影响`cpsr`位的指令
- 比较指令完成后，一般就需要条件执行
- 一般语句都可加条件执行的后缀`[ne|eq|lt|gt|le|ge]`，如`mov`可以为`movne`

```c
if(a > b)     //cmp a, b       @转化为 a - b,将修改cpsr 中的 NZ 位
  c = 7       //movgt c, #7    @N==0 Z==0
else if(a == b)  
  c = 9       //moveq c, #9    @Z==1
else  
  c = 10      //movlt c, #10   @N==1
```

```c
  int a = 5, b = 6, c;

  asm volatile(
    "mov r0, #1\n"
    "mov r1, #2\n"
    "cmp r0, r1\n"
    "movgt %0, #7\n"
    "moveq %0, #9\n"
    "movlt %0, #10\n"
    :"=&r"(c)
    :"r"(a), "r"(b)
    :"r0", "r1"
  );

  printf("c = %d\n", c);
```

## 跳转指令
```c
  int a;
  asm volatile(
    "mov %0, #3\n"
    "b end\n"
    "mov %0, #4\n"  //不会被执行到
    "end:\n"
    :"=&r"(a)
  );
```

- 跳转到指定label
- `b/bl/bx`最多只能跳转32M字节

```c
int main(void){
  int a;
  asm volatile(
    "mov r0, #1\n"
    "mov r1, #2\n"
    "bl nihao\n"         //先把下一条指令的地址保存在lr中,然后再跳转
    "mov %0, r0\n"
    :"=&r"(a)::"r0", "r1"
  );

  printf("a = %d\n", a);
  return 0;
}

asm (                   //可以位于函数外部,此处不需要加volatile
  "nihao:\n"
  "add r0, r0, r1\n"
  //"mov pc, lr\n"      //把lr的值放入pc,实现了跳转
  "bx lr\n"             //等价于上面的语句 
);
```

## 汇编中调用C函数
- 利用上面的跳转指令来实现函数调用
- 根据APCS（arm过程调用标准），用来约束汇编和C/C++相互调用
    + 传参数：`r0`, `r1`, `r2`, `r3` 如果参数个数大于4个，那么超过4个的参数放到栈（越靠后的参数越先入栈）
    + 要求必须用满递减栈
    + 返回值：如果返回值是32位则放到`r0`；如果64位则高32位放到`r1`，低32位放到`r0`

```c
#include <stdio.h>
int main(void){
  int a;
  asm volatile(
    "mov r0, #5\n"
    "mov r1, #6\n"
    "push {r0, r1}\n"    //超过4个参数的,做入栈处理
    "mov r0, #1\n"       //该句以及下面三句,即将参数放在相应寄存器上
    "mov r1, #2\n"
    "mov r2, #3\n"
    "mov r3, #4\n"
    "bl nihao\n"         //先把下一条指令的地址保存在lr中,然后在跳转
    //pop {r0, r1}       //函数执行完成后,需清理参数,做出栈处理,此处有bug,会覆盖寄存器值
    "add sp, sp, #8\n"   //正确做法是,将sp+8
    "mov %0, r0\n"
    :"=&r"(a)::"r0", "r1"
  );

  printf("a = %d\n", a);
  return 0;
}

int nihao(int a, int b, int c, int d, int e, int f){
  printf("a = %d\n", a);
  printf("b = %d\n", b);
  printf("c = %d\n", c);
  printf("d = %d\n", d);
  printf("e = %d\n", e);
  printf("f = %d\n", f);
  return a + b + c + d + e + f;
}
```

## 未定义指令
将产生一条`SIGILL        4       Core    Illegal Instruction`信号

```c
#include <stdio.h>

int main(void){
  //内嵌的指令是处在代码段
  asm volatile(
    ".word 0x77777777\n"
  );

  return 0;
}
```

<script>

(function(){
    if(typeof expand_toc !== 'function') setTimeout(arguments.callee,500);
    else expand_toc('md_toc',1);
})();

</script>


<!--
ifconfig eth0 192.168.4.119
mount -t nfs -o nolock,rw 192.168.4.118:/tomcat_root /mnt
-->
