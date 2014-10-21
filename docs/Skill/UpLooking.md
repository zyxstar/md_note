




mii-tool eth0

vncviewer ip
/etc/init.d/iptables stop
setenforce 0



目录结构FHS



安装
    rpm yum 
        .rpm结尾的软件 
        rpm不解决依赖关系，优先使用
        yum解决依赖关系，自动安装，需要配置源，在64位机器上默认不查找32位的软件，需要手工rpm安装
    编译安装
        .tar.gz .tar.bz2

.pl .py .sh .bundle .bin 可直接执行

rpm
    软件包名，软件名称
    挂载镜像
        #mount -o loop /path/rhel.iso /mnt
                                  iso9660
        /mnt/Packages
        软件名.版本.OS.CPU.rpm
            OS: el5 el6
            CPU: i386~i686 32bit
                x86_64 64bit
                noarch 通用
    安装
        #rpm -ivh 软件包名  
            -i install
            -vh 显示安装过程信息
            [--force] 强制安装
            不需要指定安装目录
                /usr 目录需要单独分出来，默认安装于此    

    查询
        查询已安装的软件包
            #rpm -qa | grep vnc 
            #rpm -q gcc 明确知道软件名
        查询软件安装位置
            #rpm -ql tigervnc
        查询文件所属软件，方便根据文件找到软件 再安装，或根据命令找到程序
            #rpm -qf /path/to/file
        查询已安装软件详细信息
            #rpm -qi 软件名（不是包）

    删除
        #rpm -e 软件名
            -e erase
            [--nodeps] 不查看依赖性关系删除，防止缺失文件不能删除
                实在卸载不了时，可强制安装，再删除
        编译安装的直接删除目录


#find / -size +2500M

-----

yum
    配置yum
        客户端
            #rm -rf /etc/yum.repos.d/* 清空目录 不带*删除目录
            #vim /etc/yum.repos.d/yum.repo 以.repo结尾
                [rhel6]
                name=rhel6
                baseurl=file:///mnt/Server
                enabled=1
                gpgcheck=0
                一个文件中可存在多个五行，从上往下查找安装
        服务端
    安装
        #yum install 软件名
        #yum groupinstall 软件组名字 如：通用桌面

    查询
        #yum repolist
        #yum list
           第三列是yum源名字，说明没装上
        #yum grouplist
        #yum search keyword
    卸载
        #yum erase
        #yum remove
    

归档压缩
    归档
        #tar cvf ab.tar a b c dfile
    解包
        tar xvf ab.tar [-C /path]

    压缩
        gzip a b c dfile 分开压缩
        bzip2

    解压
        gunzip a.gz
        bunzip2 a.bz2

    归档压缩
        tar cvzf ab.tar.gz a b c dfile
        tar cvjf ab.tar.bz2 a b c dfile
    解包
        tar xvzf ab.tar.gz
        tar xvjf ab.tar.bz2

-c create
-v verbose
-z gzip
-j bzip2
-f file指定文件
-x extract

编译安装
    ./configure --prefix=安装路径 (建议指定 删除时方便 否则分散在usr/locals各子目录) //产生makefile配置文件
    make //编译
    make install //安装

用户
    创建
        useradd <accountname>
    查看
        id <accountname>
            uid=502(jim) gid=502(jim) 组=502(jim),0(root)
                user_identify:number
                group_identify:number
                组 主属组+附属组 : array
                    usermod -G root jim 增加附属组



        cat /etc/passwd | grep <accountname> #存储用户信息
            jim:x:502:502::/home/jim:/bin/bash
                用户名
                密码占位 删除后用户就没密码，不能改密码
                uid
                gid
                描述字段
                家目录 
                登录shell


        cat /etc/shadow #真正存储密码的地方

    切换
        su - <accountname> 加载环境变量
        su <accountname> 不会加载环境变量，不推荐使用

    设置密码
        passwd <accountname> #不加<accountname>给自己重设
        #root用户能设置简单的，其他用户不行
    删除账户
        userdel -r <accountname>
            -r 家目录删除

组
    添加账户时，就会产生一个同名的组，这个组称为用户的主属组

    添加组
        groupadd <groupname>
    查看组
        cat /etc/group
            组名:组密码占位（目前无用）:gid:组员（数组）
    把用户添加进组
        gpasswd -a <useraccount> <groupname>
    把用户删除出组
        gpasswd -d <useraccount> <groupname>
    删除组
        groupdel <groupname>

权限
    查看 ls -l (ll)
        -rwx rw- ---
        文件类型: 普通- 目录d 块设备b 字符设备c 符号链接文件l 套接字文件s 管道文件p
        ugo权限

    修改
        chmod u-x a.txt
        chmod g+x a.txt
        chmod o+w a.txt
        chmod u-x,g+x,o+w a.txt
        chmod u+w-x a.txt

        数字方式修改
            r4 w2 x1
        chmod 744 a.txt

    修改文件所属者/组(间接修改了权限)
        chown <username> file
        chown .<groupname> file #组前加点
        chown <username> .<groupname> file

    对于root用户，x权限同属主，rw的设置是无效的，能读能写

    其它权限：  suid sgid skicky

进程控制
    查看进程
        ps 当前终端上进程
        ps a 所有终端上进程
        ps ax 所有进程，包括非终端上进程
        ps aux 把进程启动的用户包含进来

        user pid cpu mem vsz rss tty stat start time command

    杀死进程
        ps aux | grep <keyword> 查到进程pid
        kill -9 pid 
            -9 强制    
            -15 默认
            - 19 挂起
            - 18 唤醒
            -l 列出所有信号

            w 看到所有登录系统的账户
            killall -9 sshd （进程名称）


服务
  ssh tftp vsftpd nfs 日志
           ftp服务器


  ssh远程登录管理工具
    安装
        yum install -y *openssh*
    开启
        service sshd start #redhat专有
        /etc/init.d/sshd start #linux通用
    登录
        ssh <user>@<ip>
        ssh <ip> 不写账户默认为root

        scp file <myip>:/path 远程拷贝
            相对于现在登录位置，那个是远程就加ip


  tftp
    安装
        yum install *ftfp* -y
    启动
        chkconfig tftp on
        service xineted restart 修改过配置文件后
            vim /etc/xineted.d/tftp
               server_args             = -s /var/lib/tftpboot
                                          下载的目录
                                         -c 打开上传功能
               disable                 = yes
                                         no 开启服务

        chmod 777 /var/lib/tftpboot -R
        service iptables stop  关闭防火墙
            cat /etc/services  看服务用的端口
        setenforce 0           关闭SElinux

    客户端连接
        tftp <ip>
            > get a 默认下载到当前目录
            > put c 默认上传当前目录中文件



  vsftpd
    安装
        lftp 客户端
        vsftpd 服务器端

    配置
        chmod 755 -R /var/ftp

        vim /etc/vstftpd/vstftpd.conf

        27 anon_upload_enable=yes 上传文件
        31 anon_mkdir_write_enable=yes 上传目录
        去掉注释

    重启服务
        service vsftpd restart

    连接
        lftp <ip>
          > ls
          > get
          > get file -o /path/to/file
          > put /path/to/file
          > mirror <dir> /path/to/dir 下载目录
          > mirror -R /path/to/dir 上传目录


  nfs
      network file system
        ext4 fat32 ntfs 本地文件系统
        cifs (samba协议）共享文件系统
        nfs linux中共享文件系统

    安装
        nfs-utils
        rpcbind
    配置
        vim /etc/exports
          /tmp *(rw,async,no_root_squash)
                 读写 异步传输（先写内存/ window默认同步） 默认使用某用户 改成 取消root的权限压制
          /tmp <ip></网段> 
    启动服务
        service nfs restart [status]
        service rpcbind restart
    客户端挂载
        客户端也需要开 nfs-utils rpc
        mount -t nfs <ip>:/tmp /opt



查看日志
  /var/log/messages





shell
    local variable



172.16.20.250



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

lsof -i:80

netstat -ntlp


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






