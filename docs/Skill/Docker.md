# 安装

- https://github.com/turnbullpress
- https://github.com/zyxstar/dockerbook-code

- sudo sh -c "echo deb https://apt.dockerproject.org/repo ubuntu-trusty main ` /etc/apt/sources.list.d/docker.list"
- sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 58118E89F3A912897C070ADBF76221572C52609D  

- 使用安装脚本安装
- curl https://get.docker.com/ | sudo sh

- 检查docker守护进程状态
    - sudo status docker
    - sudo stop docker
    - sudo start docker

- sudo service docker stop/start

- 如果运行出错，查看/var/log/upstart/docker.log

- sudo docker info #查看docker是否正常

- sudo gpasswd -a zyx docker #加入docker组
- sudo service docker restart #并重启，以后可以不用sudo


# 运行

- 修改Docker配置文件/etc/default/docker如下：
    - DOCKER_OPTS="--registry-mirror=https://4pbxxsiu.mirror.aliyuncs.com"

```shell
或 

sudo mkdir -p /etc/docker
sudo tee /etc/docker/daemon.json <<-'EOF'
{
  "registry-mirrors": ["https://4pbxxsiu.mirror.aliyuncs.com"]
}
EOF
sudo systemctl daemon-reload
sudo systemctl restart docker
```

- sudo docker run -i -t ubuntu /bin/bash   #或imageid
- sudo docker ps -a #查看容器列表
- sudo docker run --name my_container -i -t ubuntu /bin/bash #容器命名

- 重启/附着
    - sudo docker start `CONTAINER ID`|`NAMES` #重启  -ia 进入交互模式
    - sudo docker attach `CONTAINER ID`|`NAMES` #附着

- 创建守护式容器
    - sudo docker run --name daemon_1 -d ubuntu /bin/sh -c "while true; do echo hello; sleep 1; done"

- 获取日志
    - sudo docker logs daemon_1 #获取日志
    - sudo docker logs -f daemon_1  #即时获取日志
    - sudo docker logs -ft daemon_1 #加上时间
    - --log-driver 日志输出选项，默认json-file，或syslog(需要syslogd守护进程)

- 查看各容器性能
    - sudo docker top daemon_1 #查看容器内进程
    - sudo docker stats daemon_1 daemon_2 #查看各容器 __性能指标__

- 执行命令
    - sudo docker exec -d daemon_1 touch /etc/new_cfg_file #-d指后台进程 可使用-u标志为新启进程指定一个用户属主
    - sudo docker exec -i -t daemon_1 /bin/bash #非附着，以另一个tty进入

- 停止守护式容器
    - sudo docker stop daemon_1

- 自动重启容器
    - sudo docker run --restart=always|on-failure:5  --name daemon_1 -d ubuntn /bin/sh ....

- 获取容器信息
    - sudo docker inspect daemon_1 #获取容器信息
        + --format='{{ .State.Running}}'  #有条件的获取信息


# 镜像和仓库
- sudo docker images #列出镜像
- sudo docker pull fedora:21 #拉取镜像
- sudo docker search keyword #查找公共可用镜像

- sudo docker commit -m"msg" `id` user/img:tag  #提交本地仓库（在tty执行了n个命令后），推荐dockerfile
- sudo docker inspect user/img:tag

- Dockerfile

```
FROM ubuntu:14.04
MAINTAINER James Turnbull "james@example.com"
RUN apt-get update               #RUN apt-get -qq update
RUN apt-get install -y nginx     #RUN ["apt-get", "install", "-y", "nginx"] 避免shell字符串篡改
RUN mkdir -p /var/www/html
RUN touch /var/www/html/index.html
RUN echo 'Hi, I am in your container' \
    >/var/www/html/index.html
EXPOSE 80       #向外部公开多个端口
```

- sudo docker build -t="zyxstar2013/my_2:v1" -f path/to/file  #执行dockerfile

- 如果执行失败，则docker run -t -i `id` /bin/bash 进入该容器，去排错，待问题解决后，修改dockerfile再build

- build时默认使用缓存，每步成功后均有缓存镜像层，如无需缓存，完全重新build，使用--no-cache

- sudo docker history `id` #查看该镜像如何build的

- sudo docker run -d -p 80 --name static_web zyxstar2013/my_2 nginx -g "daemon off;"
    + -d #docker以分离的方式在后台运行
    + nginx -g "daemon off;" #以前台方式启动nginx
    + -p 80 #宿主机随机选择32768～61000中某个端口来映射到容器中的80端口（也可以具体指定 -p 8088:80，8088映射到容器中的80）
        * 容器启动后，sudo docker port `id` 80 可查看容器端口映射（sudo docker ps -l 也可看到）
    + -P大写时，则不需要再写80，默认开放容器里声明的端口

- sudo docker push zyxstar2013/static_web #推送
- sudo docker rmi zyxstar2013/static_web #删除本地
- sudo docker rmi `docker images -a -q` #删除本地所有

- sudo docker run -p 5000:5000 registry:2 #启动一个运行registry应用2.0版本的容器
    + sudo docker tag 53eaad3e2a54 localhost:5000/zyx/my_3
    + sudo docker push localhost:5000/zyx/my_3
    + sudo docker run -i -t localhost:5000/zyx/my_3 /bin/bash #从本地registry构建



## Dockerfile指令
- CMD #与docker run启动时指定运行命令类似, RUN用于构建，CMD用于容器启动时默认运行（当ENTRYPOINT未指定时），但更多时候它用来表示ENTRYPOINT默认参数
    + Dockerfile只能指定一条CMD,如需启动容器时运行多个进程或指令，可使用supervisor
    + docker run命令 __会__ 覆盖CMD指令
        * CMD [“/bin/echo”, “this is a echo test ”] 
        * docker run ec  #build后运行(假设镜像名为ec)
        * 就会输出: this is a echo test
        * docker run ec /bin/bash
        * 就不会输出：this is a echo test，因为CMD命令被”/bin/bash”覆盖了
    + 第一种用法：运行一个可执行的文件并提供参数。
    + 第二种用法：为ENTRYPOINT指定参数。
    + 第三种用法(shell form)：是以”/bin/sh -c”的方法执行的命令。


- ENTRYPOINT
    + docker run命令 __不会__ 覆盖CMD指令，它可以让你的容器功能表现得像一个可执行程序一样
        * ENTRYPOINT ["/bin/echo"] 
        * docker run -it imageecho “this is a test”  #build后运行(假设镜像名为imageecho)
        * 就会输出”this is a test”这串字符，而这个imageecho镜像对应的容器表现出来的功能就像一个echo程序一样
    + docker run命令行中指定的任何参数都被当做参数再次传递给ENTRYPOINT指令中指定的命令
    + CMD可以为ENTRYPOINT提供参数，ENTRYPOINT本身也可以包含参数，但是你可以把那些可能需要 __变动的参数__ 写到CMD里而把那些 __不需要变动的参数__ 写到ENTRYPOINT里面。然后你可以在docker run里指定参数，这样CMD里的参数(这里是-c)就会被覆盖掉而ENTRYPOINT里的不被覆盖
        * ENTRYPOINT ["/usr/sbin/nginx"] 
        * CMD ["-h"] 
        * docker run -it img -g "daemon off;"  #以前台方式运行
        * docker run -it img  #显示nginx帮助信息


- WORKDIR
    + `ENTRYPOING` `/` `CMD` `RUN`指定的程序会在这个目录下执行
        * WORKDIR /opt/webapp/db
        * RUN bundle install
        * WORKDIR /opt/webapp  #切换工作目录
        * ENTRYPOINT ["rackup"]
    + docker run -w 覆盖工作目录

- ENV
    + build的过程中设置环境变量，后续的RUN指令如同在命令前指定是环境变量前缀
    + 这些环境变量持久保持在该镜像创建的任何容器中

- USER
    + 指定镜像会以什么样的用户去运行
        * 可以指定用户或UID，以及组或GID
        * USER user:group
        * USER uid:group
        * USER user:gid
    + docker run -u 覆盖
    + 如果不指定则默认为root

- VOLUME
    + 卷功能可将数据（源代码）、数据库等内容添加到镜像中，而不是将这些内容提交到镜像中，并且允许在多个容器间共享
    + VOLUME ["/opt/project"] #创建挂载点
    + docker run -it -v /test:/soft centos /bin/bash  #宿主机的/test目录挂载到容器的/soft目录，容器内会自动创建/soft的目录
    + 如果要授权一个容器访问另一个容器的Volume，我们可以使用-volumes-from参数来执行docker run
        * 不管container-test是否运行，它都会起作用。只要有容器连接Volume，它就不会被删除
    + 常见的使用场景是使用纯数据容器来持久化数据库、__配置文件__(.yml .properties) 或者数据文件等
        * docker run --name dbdata postgres echo "Data-only container for postgres" #该命令将会创建一个已经包含在Dockerfile里定义过Volume的postgres镜像，运行echo命令然后退出。当我们运行docker ps命令时，echo可以帮助我们识别某镜像的用途。我们可以用-volumes-from命令来识别其它容器的Volume
        * 不要运行数据容器，这纯粹是在浪费资源。
        * 不要为了数据容器而使用“最小的镜像”，如busybox或scratch，只使用数据库镜像本身就可以了。你已经拥有该镜像，所以并不需要占用额外的空间。
    + 备份
        * docker run --rm --volumes-from dbdata -v $(pwd):/backup debian tar cvf /backup/backup.tar /var/lib/postgresql/data
        * 该示例应该会将Volume里所有的东西压缩为一个tar包（官方的postgres Dockerfile在/var/lib/postgresql/data目录下定义了一个Volume）

    + sudo docker inspect -f "{{ range .Mounts}}{{.}}{{end}}" `NAMES`  #查看挂载目录在宿主机位置
    
    + 权限

```
FROM debian:wheezy
RUN useradd foo
VOLUME /data
RUN touch /data/x
RUN chown -R foo:foo /data  #不起作用
```

```
FROM debian:wheezy
RUN useradd foo
RUN mkdir /data && touch /data/x
RUN chown -R foo:foo /data
VOLUME /data   #最后挂载才起作用
```

    

- ADD `src`... `dest`
    + 将构建环境下的文件和目录复制到镜像中
    + 相比COPY,可以指定远程文件 URLS，如果是归档文件，还能自动解压
    + 通过目的地址是否/结尾，来判断文件源是目录还是文件
    + 如果目的位置不存在的话，docker会创建这个全路径，模式为0755，并且uid:gid都为0

- COPY
    + 类似ADD，但不会去归档解压，源路径必须是当前构建环境相对的文件或目录，本地文件都放到和Dockerfile同一个目录下
    + 目的位置则必须是容器内一个绝对路径，创建的文件或目录uid:gid都为0

- LABEL
    + 为镜像添加元数据，以k-v形式出现
    + 推荐所有元数据放到一条LABEL中
    + 可通过docker inspect查看

- STOPSIGNAL
    + 容器终止时发送什么系统调用信号给容器，如SIGKILL

- ARG
    + 定义build时变量
    + --build_arg可覆盖
    + ARG指令定义了用户可以在编译时或者运行时传递的变量，如使用如下命令：--build-arg `varname`=`value`
    + ENV指令是在dockerfile里面设置环境变量，不能在编译时或运行时传递。
    + 以下是ARG和ENV的有效结合:

```
ARG var
ENV var=${var}
```

- ONBUILD
    + 当一个镜像被用做其他镜像的基础镜像时，被触发执行
    + ONBUILD RUN cd /app/src && make
    + 可通过docker inspect查看
    + ONBUILD ADD . /var/www/ #在构建时，将所在目录的内容添加到镜像中的/var/www目录下，方便用这个dockerfile作为一个通用web应用程序模板（最好还是挂载）
    + ONBUILD只有新镜像被执行，但基于新镜像再做基础镜像时，则无效




# 阿里云docker镜像服务
```
登录阿里云docker registry:
  $ sudo docker login --username=zyxstar2013@163.com registry.cn-beijing.aliyuncs.com
登录registry的用户名是您的阿里云账号全名，密码是您开通服务时设置的密码。

你可以在镜像管理首页点击右上角按钮修改docker login密码。

从registry中拉取镜像：
  $ sudo docker pull registry.cn-beijing.aliyuncs.com/zyx_ns1/zyx_repo:[镜像版本号]
将镜像推送到registry：
  $ sudo docker login --username=zyxstar2013@163.com registry.cn-beijing.aliyuncs.com
  $ sudo docker tag [ImageId] registry.cn-beijing.aliyuncs.com/zyx_ns1/zyx_repo:[镜像版本号]
  $ sudo docker push registry.cn-beijing.aliyuncs.com/zyx_ns1/zyx_repo:[镜像版本号]
其中[ImageId],[镜像版本号]请你根据自己的镜像信息进行填写。
```


# 一个测试

- sudo docker run -d -p 80 --name website -v $PWD/website:/var/www/html/website:ro zyx/nginx
    - -v将宿主机的目录$PWD/website作为卷，挂载到容器var/www/html/website
        + 卷为docker提供持久数据或共享数据，提交或创建镜像时，卷不被包含在镜像里
        + 可以用rw或ro来指定容器目录的读写状态
        + -v $PWD/website:/var/www/html/website:ro  #只读
    - nginx.conf中daemon off; 让nginx以前台方式运行，防止docker容器退出


- sinatra例子

```
FROM ubuntu:14.04
MAINTAINER James Turnbull "james@example.com"
ENV REFRESHED_AT 2014-06-01

RUN apt-get update && apt-get -y install ruby ruby-dev build-essential redis-tools
RUN gem install --no-rdoc --no-ri sinatra -v 1.4.0
RUN gem install --no-rdoc --no-ri json
RUN gem install --no-rdoc --no-ri redis -v 3.0.0

RUN mkdir -p /opt/webapp

EXPOSE 4567

CMD [ "/opt/webapp/bin/webapp" ]
```

- 因apt-get ruby只能安装1.9版本，所以gem install时，需要指定sinatra与redis版本
- sudo docker build -t zyx/sinatra .
- cd sinatra
- wget --cut-dirs=3 -nH -r --reject Dockerfile,index.html --no-parent https://dockerbook.com/code/5/sinatra/webapp/
    + --cut-dirs=3  #'code/5/sinatra' 路径不要
    + -nH #'dockerbook.com' 域名不需要
    + --no-parent #webapp同级的目录 不下载
- chmod +x webapp/bin/webapp
- sudo docker run -d -p 4567 --name webapp -v $PWD/webapp:/opt/webapp zyx/sinatra

- curl -i -H 'Accept: application/json' -d 'name=Foo&status=Bar' http://localhost:32774/json
- curl -i http://localhost:32774/json

- redis-cli -h 127.0.0.1 -p 32775 #测试redis

# docker网络连接
## 内部连网(了解)

- 宿主机`ip a show docker0`，会有一个新的网络接口，如`172.17.0.1/16`，172.17.0.1本身是docker网络的网关地址，也是所有docker容器的网关地址

- 宿主机还会有其他网络接口，以`veth`开头，一端作为容器里的eth0的接口，另一端则在宿主里以`veth`表示

- `iptables -t nat -L -n` #查看宿主机上的NAT配置

```
Chain PREROUTING (policy ACCEPT)
target     prot opt source               destination
DOCKER     all  --  0.0.0.0/0            0.0.0.0/0            ADDRTYPE match dst-type LOCAL

Chain INPUT (policy ACCEPT)
target     prot opt source               destination

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination
DOCKER     all  --  0.0.0.0/0           !127.0.0.0/8          ADDRTYPE match dst-type LOCAL

Chain POSTROUTING (policy ACCEPT)
target     prot opt source               destination
MASQUERADE  all  --  172.17.0.0/16        0.0.0.0/0
MASQUERADE  tcp  --  172.17.0.2           172.17.0.2           tcp dpt:80
MASQUERADE  tcp  --  172.17.0.3           172.17.0.3           tcp dpt:4567
MASQUERADE  tcp  --  172.17.0.4           172.17.0.4           tcp dpt:6379

Chain DOCKER (2 references)
target     prot opt source               destination
RETURN     all  --  0.0.0.0/0            0.0.0.0/0
DNAT       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:32773 to:172.17.0.2:80
DNAT       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:32774 to:172.17.0.3:4567
DNAT       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:32775 to:172.17.0.4:6379
```

- sudo docker inspect -f '{{.NetworkSettings.IPAddress}}' redis #查看容器地址
- redis-cli -h `ip`

- 这种方式互联，容易IP地址硬编码，重启dokcer，地址会变

## Docker Networking(使用)
- sudo docker network create app #创建
- sudo docker network inspect app #查看

- ip a show #宿主机多了一个

```
42: br-5de7ee98fdd0: `NO-CARRIER,BROADCAST,MULTICAST,UP` mtu 1500 qdisc noqueue state DOWN group default
    link/ether 02:42:cd:db:fa:77 brd ff:ff:ff:ff:ff:ff
    inet 172.18.0.1/16 scope global br-5de7ee98fdd0
       valid_lft forever preferred_lft forever
```

- overlay网络，允许 __跨多台宿主机__ 进行通信

- sudo docker network ls #列出当前系统中所有网络
- sudo docker run -d --net=app --name db -h db zyx/redis #在该网络上创建一个新容器, -h 创建hostname
- sudo docker network inspect app #再次查看，多一个在上面运行的容器

- sudo docker run -p 4567 --net=app --name webapp_redis2 -ti -v $PWD/webapp_redis:/opt/webapp zyx/sinatra /bin/bash #在该网络下启动新容器

- 进入容器，ping db.app可以ping通

- sudo docker network connect app db2 #正在运行的容器db2添加到网络中
- sudo docker network disconnect app db2 #断开网络

- 一个容器可以隶属多个docker网络

## Docker链接连接容器(1.9之前使用)
- 需要引用容器的名字
- 只能在同一台宿主机中工作 
- sudo docker run -p 4567 --name webapp_redis3 --link redis:db -t -i -v $PWD/webapp_redis:/opt/webapp zyx/sinatra /bin/bash
    + --link，创建了两个容器间的链接，一个是要链接的容器名字redis，另一个是链接的别名db
    + 启动redis容器时，不需要指定-p，宿主机也不需要知道端口
    + 多个web容器可以和同一个redis容器链接在一起，也可以多次--link来连接多个容器（服务端）
    + --add-host=docker:10.0.0.1 #在容器内添加/etc/hosts记录

- 在容器中`env`，会自动记录，以别名db开头的服务所使用协议与IP port

```
HOSTNAME=f8fc5edc2b0a
DB_NAME=/webapp_redis4/db
DB_PORT_6379_TCP_PORT=6379
TERM=xterm
DB_PORT=tcp://172.17.0.4:6379
DB_PORT_6379_TCP=tcp://172.17.0.4:6379
DB_PORT_6379_TCP_ADDR=172.17.0.4
DB_PORT_6379_TCP_PROTO=tcp
```

- 代码中，可以使用`ENV['DB_PORT']`来引用容器内部的环境变量


# 使用Docker构建服务

- jekyll镜像创建容器，存放通过卷挂载网站的源代码
- apache镜像创建容器，利用包含编译后的网站的卷，并为其服务

## jekyll镜像

```
FROM ubuntu:16.04
MAINTAINER James Turnbull `james@example.com`
ENV REFRESHED_AT 2016-06-01

RUN apt-get -qq update
RUN apt-get -qq install ruby ruby-dev libffi-dev build-essential nodejs
RUN gem install --no-rdoc --no-ri jekyll -v 2.5.3

VOLUME /data
VOLUME /var/www/html
WORKDIR /data

ENTRYPOINT [ "jekyll", "build", "--destination=/var/www/html" ]
```

- /data用来存放网站的源代码
- /var/www/html存放编译后的jekyll网站代码
- sudo docker build -t zyx/jekyll .

## apache镜像

```
FROM ubuntu:16.04
MAINTAINER James Turnbull `james@example.com`

RUN apt-get -qq update
RUN apt-get -qq install apache2

VOLUME [ "/var/www/html" ]
WORKDIR /var/www/html

ENV APACHE_RUN_USER www-data
ENV APACHE_RUN_GROUP www-data
ENV APACHE_LOG_DIR /var/log/apache2
ENV APACHE_PID_FILE /var/run/apache2.pid
ENV APACHE_RUN_DIR /var/run/apache2
ENV APACHE_LOCK_DIR /var/lock/apache2

RUN mkdir -p $APACHE_RUN_DIR $APACHE_LOCK_DIR $APACHE_LOG_DIR

EXPOSE 80

ENTRYPOINT [ "/usr/sbin/apache2" ]
CMD ["-D", "FOREGROUND"]
```

- /var/www/html作为挂载点
- sudo docker build -t zyx/apache .

## 启动网站
- git clone https://github.com/turnbullpress/james_blog.git #将博客clone下来
- sudo docker run -v /home/zyx/james_blog:/data/ --name james_blog zyx/jekyll
    + 将下载博客的目录作为/data挂载到容器里
    + 运行完即退出，但不影响它作为数据卷（/var/www/html）存在
- sudo docker run -d -P --volumes-from james_blog zyx/apache
    + --volumes-from，把指定容器里的所有卷都加入到新创建的容器里

## 更新网站
- 修改james_blog下任意文件
- sudo docker start james_blog #将重新编译

## 备份jekyll
- 创建一个用完就删除(--rm)的镜像来备份/var/www/html

```
sudo docker run --rm --volumes-from james_blog \
-v $(pwd):/backup ubuntu \
tar cvf /backup/james_blog_backup.tar /var/www/html
```

## 扩展
- 进一步构建一个镜像，把用户git clone到卷里，再把这个卷挂载到jekyll容器里


# 不使用shh管理docker容器
- 大部分容器只运行一个进程，所以不能使用这种方法访问
- 使用卷或链接完成大部分同样的管理工作
- 使用unix套接字来管理
- 需要给容器发送信息
    + sudo docker kill -s `signal` `container`
- 使用nsenter工具
    + 或docker exec







- $ docker stop $(docker ps -a -q) //  stop停止所有容器
- $ docker rm $(docker ps -a -q) //   remove删除所有容器




# 从PPA库安装软件

```
sudo apt-get -qq update
sudo apt-get install -qq software-properties-common python-software-properties
sudo add-apt-repository ppa:chris-lea/redis-server
sudo apt-get -qq update
sudo apt-get -qq install redis-server redis-tools
```
