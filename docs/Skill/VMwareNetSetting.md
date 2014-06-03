# host-only(主机模式)
在host-only模式下，__虚拟系统和宿主机器系统是可以 *相互通信*__ 的，相当于这两台机器通过双绞线互连。但虚拟系统和  __真实的网络是被 *隔离* 开__ 的。

虚拟系统的TCP/IP配置信息(如IP地址、网关地址、DNS服务器等)，都是由 __*宿主VMnet1*__ (host-only)虚拟网络的DHCP服务器来动态分配的。如果你想利用VMWare创建一个与网内其他机器相隔离的虚拟系统，进行某些特殊的网络调试工作，可以选择host-only模式。

宿主中情况 (192.168.241.1):

```
以太网适配器 VMware Network Adapter VMnet1:

   连接特定的 DNS 后缀 . . . . . . . :
   本地链接 IPv6 地址. . . . . . . . : fe80::39c6:db69:154c:5670%15
   IPv4 地址 . . . . . . . . . . . . : 192.168.241.1
   子网掩码  . . . . . . . . . . . . : 255.255.255.0
   默认网关. . . . . . . . . . . . . :
```

虚拟机中情况 (192.168.241.128):

```
eth0      Link encap:Ethernet  HWaddr 00:0c:29:22:70:24
          inet addr:192.168.241.128  Bcast:192.168.241.255  Mask:255.255.255.0
          inet6 addr: fe80::20c:29ff:fe22:7024/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:345628 errors:265 dropped:0 overruns:0 frame:0
          TX packets:220480 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:514934028 (514.9 MB)  TX bytes:12177314 (12.1 MB)
          Interrupt:19 Base address:0x2000
```

ssh可连接到虚拟机，但虚拟机无法访问外部网络

# NAT(网络地址转换模式)
使用NAT模式，就是让虚拟系统借助NAT(网络地址转换)功能，通过宿主机器所在的网络来访问公网。也就是说，使用NAT模式可以 __实现在虚拟系统里访问互联网__ 。NAT模式下的虚拟系统的TCP/IP配置信息是由 __*VMnet8*__(NAT)虚拟网络的DHCP服务器提供的，__无法进行手工修改__，因此 __虚拟系统也就 *无法* 和本局域网中的其他真实主机进行通讯__。采用NAT模式最大的优势是虚拟系统接入互联网非常简单，你不需要进行任何其他的配置，只需要宿主机器能访问互联网即可。

如果你想利用VMWare安装一个新的虚拟系统，在虚拟系统中不用进行任何手工配置就能直接访问互联网，建议你采用NAT模式。

宿主中情况 (192.168.148.1):

```
以太网适配器 VMware Network Adapter VMnet8:

   连接特定的 DNS 后缀 . . . . . . . :
   本地链接 IPv6 地址. . . . . . . . : fe80::3500:9e87:a3be:8ff3%17
   IPv4 地址 . . . . . . . . . . . . : 192.168.148.1
   子网掩码  . . . . . . . . . . . . : 255.255.255.0
   默认网关. . . . . . . . . . . . . :
```

虚拟机中情况 (192.168.148.129):

```
eth0      Link encap:Ethernet  HWaddr 00:0c:29:22:70:24
          inet addr:192.168.148.129  Bcast:192.168.148.255  Mask:255.255.255.0
          inet6 addr: fe80::20c:29ff:fe22:7024/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:345628 errors:265 dropped:0 overruns:0 frame:0
          TX packets:220480 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:514934028 (514.9 MB)  TX bytes:12177314 (12.1 MB)
          Interrupt:19 Base address:0x2000
```

ssh可连接到虚拟机，虚拟机也可以访问外部网络，但局域网其他电脑无法连接到它

> VMnet8(NAT)和VMnet1(host-only)提供DHCP服务，VMnet0~9中其它虚拟网络则不提供。

# bridged(桥接模式)
VMWare虚拟出来的操作系统就像是局域网中的一台独立的主机，它可以访问网内任何一台机器。在桥接模式下，你需要手工为虚拟系统配置IP地址、子网掩码，而且还要和宿主机器处于同一网段，这样虚拟系统才能和宿主机器进行通信。同时，由于这个虚拟系统是局域网中的一个独立的主机系统，那么就可以手工配置它的TCP/IP配置信息，以实现通过局域网的网关或路由器访问互联网。

这是 __*最常用*__ 的模式，ssh可连接到虚拟机，虚拟机也可以访问外部网络，局域网其他电脑也可连接到它

> 单机多网卡 或者 笔记本和移动设备 使用虚拟机时,当在有线网络和无线网络切换时,勾选了`Replicate physical network connection state`,虚拟机网卡的ip地址会自动更新,不需要重新设置。一般用处不大。

网关是192.168.0.65时，宿主中情况 (192.168.0.82):

```
以太网适配器 以太网:

   连接特定的 DNS 后缀 . . . . . . . :
   本地链接 IPv6 地址. . . . . . . . : fe80::608b:26e8:d264:81fa%12
   IPv4 地址 . . . . . . . . . . . . : 192.168.0.82
   子网掩码  . . . . . . . . . . . . : 255.255.255.0
   默认网关. . . . . . . . . . . . . : 192.168.0.65
```

虚拟机中情况 (192.168.0.166):

```
eth0      Link encap:Ethernet  HWaddr 00:0c:29:22:70:24
          inet addr:192.168.0.166  Bcast:192.168.0.255  Mask:255.255.255.0
          inet6 addr: fe80::20c:29ff:fe22:7024/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:5048 errors:0 dropped:0 overruns:0 frame:0
          TX packets:99 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:339524 (339.5 KB)  TX bytes:13791 (13.7 KB)
          Interrupt:19 Base address:0x2000
```

> 虚拟机的MAC地址存，在虚拟机相应的配置文件（*.vmx），找到相应的MAC地址设置信息`ethernet0.generatedAddress = "00:0c:29:22:70:24"`  [VMware克隆Linux虚拟机网络不同（手动修改MAC地址）]（http://liuxun.org/blog/vmware-ke-long-linux-xu-ni-ji-wang-luo-bu-tong-shou-dong-xiu-gai-mac-di-zhi/）

[VMware虚拟机配置Ubuntu桥接方式](http://blog.chinaunix.net/uid-26498888-id-3185042.html)

[VMWare虚拟机Bridged类型网卡ping不通的原因和解决办法](http://www.2cto.com/os/201307/231390.html)

[Cannot change network to bridged: There are no un-bridged host network adapters](http://blog.csdn.net/dongfengkuayue/article/details/9068671)

[关于虚拟机选桥接时出现没有选项的问题](http://blog.sina.com.cn/s/blog_7035e7b00101ieb1.html)




rails rvm安装

```shell
curl -L get.rvm.io | bash -s stable
source ~/.bashrc
source ~/.bash_profile
sed -i -e 's/ftp\.ruby-lang\.org\/pub\/ruby/ruby\.taobao\.org\/mirrors\/ruby/g' ~/.rvm/config/db

rvm list known

rvm install 1.9.3
rvm use 1.9.3
rvm gemset create r3217
rvm use 1.9.3@r3217 --default

gem source -r https://rubygems.org/
gem source -a https://ruby.taobao.org

gem install rails -v='3.2.17' --no-rdoc --no-ri

```



sudo service smbd restart V
/etc/init.d/samba restart
