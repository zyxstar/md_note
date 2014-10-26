


桥接
  把本地真实网卡 当交换机使用

NAT 网络地址转换
  产生一个v8虚拟网卡 也认为交换机 可以设置IP地址

  虚拟机需要设置网关 即上面设置的ip地址 方可访问外网
   
  v8网卡转换到本地真实网卡

  虚拟机可ping外网，外不可ping内

host
  v1网卡


channel
  除了v1 v8之外的
  可以仅虚拟机间通讯


看
ifconfig eth0
ip addr show
ip add sh

启
ifconfig eth0 down|up
ifdown eth0
ifup eth0


设  一个机器可设多个IP
ifconfig eth0 <ip> netmask <ip> up

ip addr add <ip> netmask 255.255.255.0 dev eth0
ip addr add <ip/24> dev eth0

网关
看
route -n

设
route add default gw <ip>

dns
vim /etc/resolv.conf
  nameserver 202.106.0.46 最多三个


以上全部了临时的

修改它，才会永久生效
/etc/sysconfig/network-scripts/ifcfg-eth0

ONBOOT 是否启用网卡
NM_CONTROLLED 是否受networkmanager gui控制
BOOTPROTO=dhcp|none|static  获取ip方式

PREFIX=24
or
NETMASK=255.255.255.0

GETEWAY=
DNS1=
DNS2=




service network restart 重启网卡


DNS 202.106.46.151

hosts文件
ip name

mail.163.com
FQDN 完全限定域名
 准备的描述出主机所在的位置

倒置的树状结构的 分布式 数据库系统，存储着FQDN名和ip地址的对应关系，负责它们之间的解析








domain <-> zonefile
/etc/named.conf

subdomain <-> ip
dns zonefile
/var/named/


yum -y install bind

nslookup

iptables -F






history
 !16 第16行记录

 !cd 最近的cd命令

 !! 最后一个命令


 Ctrl r 进入搜索
 
 alt . 引用上次参数  一直按. 多向上找几次


alias  la = "ls -a"
unalias la

alias grep = "grep --color"

别名优先级最高

^ c z   s q
  中止
  传后台
  a跳到最前
  e最后
  u删除
  l清屏
  d退出shell



<< 定义结束符


grep -v "a" file | tee file1 | grep -v "b" file


大括号外*内
    {}   #创建目录集合
         mkdir dir{5..8}
         mkdir dir{a..d}
         mkdir dir{a,d}

cp /etc/eth{0,1}
  前面改成名后面


保持不变，防止shell翻译
\ 
``

touch "a b"
touch a\ b
touch a' 'b

echo -n 不换行
echo -e "a\tb\tc" 翻译转义


grep root /etc/passwd | tr [a-z] [A-Z]

echo "1 2 3 4 5" | tr " " "+" | bc



sh中
位置变量
$1 第一个参数

$* 或 $@  都是什么
$#  一共几个

$$ PID
$? last retval


test -f 
     -r
     -u
     -s
     -S


if 后面的条件都是 test 条件 

可简写成 `[ ]`


可将执行的后的$? 进行  []


for((i=1;i<255;i++)); do
  ping -c1 192.168.1.$i > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    echo "192.168.1.$i is running"
  else

  fi
done


双重[[ ]]
[[ !cmd ]]
[[ cmd && cmd ]]
[[ cmd || cmd ]]
[[ $z =~ "o" ]] 包含  正则



expr 运算
echo $(($x+$y))
     $[$x+$y]

let x=$x+1



grep -P "\d"  grex     

















命令合并
；；执行，不管逻辑，即使前面出错，后面也执行

&& 执行具有逻辑

|| 前面不成功，执行后面




echo $? 查看上条命令是否成功






ping -c1 一个包






init 0



