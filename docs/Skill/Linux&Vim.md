Linux
=======
## 常用命令
```shell
clear                             #清屏 Ctrl+L
cd -                              #返回上次使用的目录
find /somedir -name "pattern"     #查找文件
find . -size +10000k
find . -name *.php|xargs ls -alt  #也可以查找指定类型的文件，然后指定按时间排序

grep -n "mail" /etc/*             #从/etc目录所有的文件中去找mail字符串

ls -hl                            #以k m为单位显示文件大小
ls -F                             #目录以/结尾显示
ls -d */                          #只显示目录
ls -i                             #显示inode
ls -R                             #递归显示
ls -ld /xxx                       #可以看到该目录的访问权限,不加d 有可能直接deny
ls -a | sort -i                   #排序显示

getfacl <filname>                 #文件权限

mkdir -p aaa/bbb/ccc              #创建多层次目录

mv;cp                             #最好加上 -i 否则可能覆盖

wc file                           #统计行数，字符，字节

head -5 /etc/passwd
tail -2 /etc/passwd
tail -f /var/log/messages         #不停地去读最新的内容，有实时监视的效果
sed -n '5,10p' /etc/passwd        #只查看文件的第5行到第10行。

uniq                              # -d 只显示重复的行 -u 只显示唯一的行
sort names | uniq -d              #显示在names文件中哪些行出现了多次

cat /dev/null > a.txt             #使文件清空，但所有者和权限不变
cat file1 file2 | grep word | wc -1   #.列出两个文件的内容.执行对输出的搜索.统计结果行的个数
cat -n -s                         #-n给文件所有行编号 -s把多个空白行合并为一个空白行

nl                                #向文件添加行号

tar -zxvf XX.tar.gz               #解压文件，-z因文件使用gzip压缩过

chmod 755 abc                     #组文件abc赋rwxr-xr-x
chmod u=rwx,g=rx,o=rx abc

chown user abc                    #改变abc拥有者
chgrp root abc                    #改变abc所属组为root

fdisk -l                          #查看分区情况
df somedir -lh                    #查看目录在哪个分区和使用情况

fmt                               #把文本格式化成指定宽度
pr                                #给文件分页 ,把文本文件转换为标有页码的版本
fold                              #把输入行换行，以适应指定的宽度

split                             #按行数或字节数拆分文件
csplit                            #由正则来分割文件

file                              #查看文件属性
strip                             #将编译链接的可执行文件进行剪切，去掉中间信息

ls --help|grep -E "time|sort"     #查找ls命令手册中有关时间与排序的条目

export PS1='\u@\h:\w\$'           #修改命令行提示符，在profile中修改

```

## sort
```
1) 对文件内容进行排序，缺省分割符为空格，如果自定义需要使用-t选择，如-t:
2) 使用分隔符分割后，第一个field为0，awk中为1
3) 具体用法如下：
sort -t: sort_test.txt(缺省基于第一个field进行排序，field之间的分隔符为":")
sort -t: -r sort_test.txt(缺省基于第一个field进行倒序排序，field之间的分隔符为":")
sort -t: +1 sort_test.txt(基于第二个field进行排序，field之间的分隔符为":")
sort +3n sort_test.txt(基于第三个field进行排序，其中n选项提示是进行"数值型"排序) 已丢弃，请使用 -k
sort -u  sort_test.txt(去除文件中重复的行，同时基于整行进行排序)
sort -o output_file -t: +1.2[n] sort_text.txt(基于第二个field,同时从该field的第二个字符开始，这里n的作用也是"数值型"排序,并将结果输出到output_file中)
sort -t: -m +0 filename1 filename2(合并两个文件之后在基于第一个field排序)

sort命令行选项：
  -t  字段之间的分隔符
  -f  基于字符排序时忽略大小写
  -k  定义排序的域字段，或者是基于域字段的部分数据进行排序
  -m  将已排序的输入文件，合并为一个排序后的输出数据流
  -n  以整数类型比较字段
  -o outfile  将输出写到指定的文件
  -r  倒置排序的顺序为由大到小，正常排序为由小到大
  -u  只有唯一的记录，丢弃所有具有相同键值的记录
  -b  忽略前面的空格


-t定义了冒号为域字段之间的分隔符，-k 2指定基于第二个字段正向排序(字段顺序从1开始)。
sed -n '1,5p' /etc/passwd | sort -t: -k 1

还是以冒号为分隔符，这次是基于第三个域字段进行倒置排序。
sed -n '1,5p' /etc/passwd | sort -t: -k 3r

先以第六个域的第2个字符到第4个字符进行正向排序，在基于第一个域进行反向排序。
sort -t':' -k 6.2,6.4 -k 1r

先以第六个域的第2个字符到第4个字符进行正向排序，在基于第一个域进行正向排序。和上一个例子比，第4和第5行交换了位置。
sort -t':' -k 6.2,6.4 -k 1

基于第一个域的第2个字符排序
sort -t':' -k 1.2,1.2

基于第六个域的第2个字符到第4个字符进行正向排序，-u命令要求在排序时删除 键值 重复的行。
sort -t':' -k 6.2,6.4 -u users

```

## find
```
find pathname -options [-print -exec -ok]
 -print find命令将匹配的文件输出到标准输出。
 -exec find命令对匹配的文件执行该参数所给出的shell命令。相应命令的形式为'command' {} \;，注意{}和\；之间的空格，同时两个{}之间没有空格,注意一定有分号结尾。
 -ok 和-exec的作用相同，只不过以一种更为安全的模式来执行该参数所给出的shell命令，在执行每一个命令之前，都会给出提示，让用户来是否执行

find . -name "datafile*" -ctime -1 -exec ls -l {} \; 找到文件名为datafile*, 同时创建实际为1天之内的文件, 然后显示他们的明细.
find . -name   基于文件名查找,但是文件名的大小写敏感.
find . -iname  基于文件名查找,但是文件名的大小写不敏感.
find . -maxdepth 2 -name fred 找出文件名为fred,其中find搜索的目录深度为2(距当前目录), 其中当前目录被视为第一层.
find . -perm 644 -maxdepth 3 -name "datafile*"  (表示权限为644的, 搜索的目录深度为3, 名字为datafile*的文件)
find . -path "./rw" -prune -o -name "datafile*" 列出所有不在./rw及其子目录下文件名为datafile*的文件。
find . -path "./dir*" 列出所有符合dir*的目录及其目录的文件.
find . \( -path "./d1" -o -path "./d2" \) -prune -o -name "datafile*" 列出所有不在./d1和d2及其子目录下文件名为datafile*的文件。

find . -user ydev 找出所有属主用户为ydev的文件。
find . ! -user ydev 找出所有属主用户不为ydev的文件， 注意!和-user之间的空格。
find . -nouser    找出所有没有属主用户的文件，换句话就是，主用户可能已经被删除。
find . -group ydev 找出所有属主用户组为ydev的文件。
find . -nogroup    找出所有没有属主用户组的文件，换句话就是，主用户组可能已经被删除。

find . -mtime -3[+3] 找出修改数据时间在3日之内[之外]的文件。
find . -mmin  -3[+3] 找出修改数据时间在3分钟之内[之外]的文件。
find . -atime -3[+3] 找出访问时间在3日之内[之外]的文件。
find . -amin  -3[+3] 找出访问时间在3分钟之内[之外]的文件。
find . -ctime -3[+3] 找出修改状态时间在3日之内[之外]的文件。
find . -cmin  -3[+3] 找出修改状态时间在3分钟之内[之外]的文件。

find . -newer file     找出所有比file的更改时间更新的文件
find . ! -newer file 找出所有比file的更改时间更老的文件
find . -newer eldest_file ! -newer newest_file 找出文件的更改时间 between eldest_file and newest_file。

find . -type d    找出文件类型为目录的文件。
find . ! -type d  找出文件类型为非目录的文件。
       b - 块设备文件。
       d - 目录。
       c - 字符设备文件。
       p - 管道文件。
       l - 符号链接文件。
       f - 普通文件。

find . -size [+/-]100[c/k/M/G] 表示文件的长度为等于[大于/小于]100块[字节/k/M/G]的文件。
find . -empty 查找所有的空文件或者空目录.
find . -type f | xargs grep "ABC"  使用xargs和-exec的区别是, -exec可能会为每个搜索出的file,启动一个新的进程执行-exec的操作, 而xargs都是在一个进程内完成, 效率更高.
```


## cut
```
cut [options] file1 file2  #从一个或多个文件中删除所选列或字段
  -b byte 指定剪切字节数
  -c list 指定剪切字符数。
  -f field 指定剪切域数。
  -d 指定与空格和tab键不同的域分隔符。
  -c 用来指定剪切范围，如下所示：
  -c1,5-7 剪切第1个字符，然后是第5到第7个字符。
  -c2- 剪切第2个到最后一个字符
  -c-5 剪切最开始的到第5个字符
  -c1-50 剪切前50个字符。
  -f 格式与-c相同。
  -f1,5 剪切第1域，第5域。
  -f1,10-12 剪切第1域，第10域到第12域。

cut -d: -f3 cut_test.txt (基于":"作为分隔符，同时返回field 3中的数据) *field从0开始计算。
cut -d: -f1,3 cut_test.txt (基于":"作为分隔符，同时返回field 1和3中的数据)
cut -d: -c1,5-10 cut_test.txt(返回第1个和第5-10个字符)
```

## grep
```
grep时应该把正则放到引号中(单引号优于双引号)，否则shell将它们作为文件名来解释
grep的可选项(以下只能3选1)
  -G默认动作，搜索模式作为基本正则来解释
  -E搜索模式作为扩展正则来解释
  -F搜索模式作为简单字符串来解释

fgrep == grep -F
egrep == grep -E
使用egrep不需要来转义花括号，加号等
gerp '[1-9]\{1,2\}' file
==
egrep '[1-9]{1,2}' file

grep其它可选项规定如何返回输出
  -c输出字符串行数，而不是行本身
  -h显示所有匹配的行（默认）
  -l只返回包含模式的文件的名称
  -L列出不包含匹配行文件的名称
  -n显示行及其行号
  -s避免返回目录和设备文件的错误消息
  -v指示输出不匹配模式的任何行
  -x显示那些模式匹配整行的行
  -d read(默认)|skip|resurse指示如何处理目录
  -f file从文件中取模式列表
  -i大小写不敏感
  -q找到第一个实例后停止扫描

grep中的file可以使用文件名扩展中任何通配符，因为它由shell解释
ps aux|gerp service
```

## xargs
```
xargs是给命令传递参数的一个过滤器，也是组合多个命令的一个工具。它把一个数据流分割为一些足够小的块，以方便过滤器和命令进行处理。通常情况下，xargs从管道或者stdin中读取数据，但是它也能够从文件的输出中读取数据。xargs的默认命令是echo，这意味着通过管道传递给xargs的输入将会包含换行和空白，不过通过xargs的处理，换行和空白将被空格取代。

xargs 是一个强有力的命令，它能够捕获一个命令的输出，然后传递给另外一个命令，下面是一些如何有效使用xargs 的实用例子。

1. 当你尝试用rm 删除太多的文件，你可能得到一个错误信息：/bin/rm Argument list too long. 用xargs 去避免这个问题
find ~ -name ‘*.log’ -print0 | xargs -0 rm -f

2. 获得/etc/ 下所有*.conf 结尾的文件列表，有几种不同的方法能得到相同的结果，下面的例子仅仅是示范怎么实用xargs ，在这个例子中实用 xargs将find 命令的输出传递给ls -l
# find /etc -name "*.conf" | xargs ls –l

3. 假如你有一个文件包含了很多你希望下载的URL, 你能够使用xargs 下载所有链接
# cat url-list.txt | xargs wget –c

4. 查找所有的jpg 文件，并且压缩它
# find / -name *.jpg -type f -print | xargs tar -cvzf images.tar.gz

5. 拷贝所有的图片文件到一个外部的硬盘驱动
# ls *.jpg | xargs -n1 -i cp {} /external-hard-drive/directory
```


## sed
```
sed复制原文件并修改新文件，默认发送给标准输出，也可以保存到其它文件中
可用于
  自动编辑文件
  简化对多个文件的重复编辑
  编写转换程序

sed是一次读取一行，每行在移到下行前都获得了对其应用的所有命令
两种语法
 sed options editing_instruction file(s)
  >filename 附加到命令结尾将让输出发送到这个文件
  options
    -e 告诉下个参数是一个编辑指令，除非有多个指令，否则不需要此选项
    -n 防止在应用指令后把输入行送到标准输出
  editing_instruction 格式为address,[adderss]!actions arguments
    address规定要操作的行，可以是行号、表示文件最后一行的$、括在正斜线中的正则，如何未指定则应用每行输入
    ,[adderss]地址结束，如果指定了它，要应用的行的范围是上面的开始到这里指定的结束
    actions arguments
      a\string把字串追加到行后
      c\string修改行并用字符串替换
      d删除行
      i\string插到行前
      p显示行
      s进行替换 s/oldstr/newstr/[g] g代表替换所有，默认只替换第一个

 sed options -f scriptfile file(s)




sed的命令和选项：
a\   在当前行的后面加入一行或者文本。
c\   用新的文本改变或者替代本行的文本。
d    从pattern space位置删除行。
i\   在当前行的上面插入文本。
h    拷贝pattern space的内容到holding buffer(特殊缓冲区)。
H    追加pattern space的内容到holding buffer。
g    获得holding buffer中的内容，并替代当前pattern space中的文本。
G    获得holding buffer中的内容，并追加到当前pattern space的后面。
n    读取下一个输入行，用下一个命令处理新的行而不是用第一个命令。
p    打印pattern space中的行。
P    打印pattern space中的第一行。
q    退出sed。
w file   写并追加pattern space到file的末尾。
!    表示后面的命令对所有没有被选定的行发生作用。
s/re/string  用string替换正则表达式re。
=    打印当前行号码。
替换标记
g    行内全面替换，如果没有g，只替换第一个匹配。
p    打印行。
x    互换pattern space和holding buffer中的文本。
y    把一个字符翻译为另一个字符(但是不能用于正则表达式)。
选项
-e   允许多点编辑。
-n   取消默认输出。


/> cat testfile
northwest       NW      Charles Main                3.0     .98     3       34
western         WE      Sharon Gray                 5.3     .97     5       23
southwest       SW      Lewis Dalsass               2.7     .8      2       18
southern        SO      Suan Chin                   5.1     .95     4       15
southeast       SE      Patricia Hemenway           4.0     .7      4       17
eastern         EA      TB Savage                   4.4     .84     5       20
northeast       NE      AM Main Jr.                 5.1     .94     3       13
north           NO      Margot Weber                4.5     .89     5       9
central         CT      Ann Stephens                5.7     .94     5       13

/> sed '/north/p' testfile #如果模板north被找到，sed除了打印所有行 之外，还有 打印匹配行。

#-n选项取消了sed的默认行为。在没有-n的时候，包含模板的行被打印两次，但是在使用-n的时候将 只 打印包含模板的行。
/> sed -n '/north/p' testfile

删除
/> sed '3d' testfile  #第三行被删除，其他行默认输出到屏幕。
/> sed '3,$d' testfile  #从第三行删除到最后一行，其他行被打印。$表示最后一行。
/> sed '$d' testfile    #删除最后一行，其他行打印。
/> sed '/north/d' testfile #删除所有包含north的行，其他行打印。

#s表示替换，g表示命令作用于整个当前行。如果该行存在多个west，都将被替换为north，如果没有g，则只是替换第一个匹配。没有p不打印
/> sed 's/west/north/g' testfile

/> sed -n 's/^west/north/p' testfile #-n表示只打印匹配行，如果某一行的开头是west，则替换为north。 注意p


#&符号表示替换字符串中被找到的部分。所有以两个数字结束的行，最后的数字都将被它们自己替换，同时追加.5。
/> sed 's/[0-9][0-9]$/&.5/' testfile            &是占位符

/> sed -n 's/Hemenway/Jones/gp' testfile  #所有的Hemenway被替换为Jones。-n选项加p命令则表示只打印匹配行。

#模板Mar被包含在一对括号中，并在特殊的寄存器中保存为tag 1，它将在后面作为\1替换字符串，Margot被替换为Marlianne。
/> sed -n 's/\(Mar\)got/\1lianne/p' testfile     1也是占位符


#s后面的字符一定是分隔搜索字符串和替换字符串的分隔符，默认为斜杠，但是在s命令使用的情况下可以改变。不论什么字符紧跟着s命令都认为是新的分隔符。这个技术在搜索含斜杠的模板时非常有用，例如搜索时间和路径的时候。
/> sed 's#3#88#g' testfile

#所有在模板west和east所确定的范围内的行都被打印，如果west出现在esst后面的行中，从west开始到下一个east，无论这个east出现在哪里，二者之间的行都被打印，即使从west开始到文件的末尾还没有出现east，那么从west到末尾的所有行都将打印。
/> sed -n '/west/,/east/p' testfile

#-e选项表示多点编辑。第一个编辑命令是删除第一到第三行。第二个编辑命令是用Jones替换Hemenway。
/> sed -e '1,3d' -e 's/Hemenway/Jones/' testfile

/> sed -n '/north/w newfile' testfile #将所有匹配含有north的行写入newfile中。

/> sed '/eastern/i\ NEW ENGLAND REGION' testfile #i是插入命令，在匹配模式行前插入文本。

#找到匹配模式eastern的行后，执行后面花括号中的一组命令，每个命令之间用逗号分隔，n表示定位到匹配行的下一行，s/AM/Archie/完成Archie到AM的替换，p和-n选项的合用，则只是打印作用到的行。
/> sed -n '/eastern/{n;s/AM/Archie/;p}' testfile

-e表示多点编辑，第一个编辑命令y将前三行中的所有小写字母替换为大写字母，-n表示不显示替换后的输出，第二个编辑命令将只是打印输出转换后的前三行。注意y不能用于正则。
/> sed -n -e '1,3y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' -e '1,3p' testfile

/> sed '2q' testfile  #打印完第二行后退出。

#当模板Lewis在某一行被匹配，替换命令首先将Lewis替换为Joseph，然后再用q退出sed。
/> sed '/Lewis/{s/Lewis/Joseph/;q;}' testfile

G命令告诉sed从holding buffer中取得该行，然后把它放回到pattern space中，且追加到现在已经存在于模式空间的行的末尾。
/> sed -e '/northeast/h' -e '$G' testfile

WE所在的行被移动并追加到包含CT行的后面。
/> sed -e '/WE/{h;d;}' -e '/CT/{G;}' testfile
```

## awk
```
 awk的基本格式：
    /> awk 'pattern' filename
    /> awk '{action}' filename
    /> awk 'pattern {action}' filename

   /> awk '/Mary/' employees   #打印所有包含模板Mary的行。

 #打印文件中的第一个字段，这个域在每一行的开始，缺省由空格或其它分隔符。
    /> awk '{print $1}' employees
 /> awk '/Sally/{print $1, $2}' employees #打印包含模板Sally的行的第一、第二个域字段。

 2.  awk的格式输出：
    awk中同时提供了print和printf两种打印输出的函数，其中print函数的参数可以是变量、数值或者字符串。字符串必须用双引号引用，参数用逗号分隔。如果没有逗号，参数就串联在一起而无法区分。这里，逗号的作用与输出文件的分隔符的作用是一样的，只是后者是空格而已。下面给出基本的转码序列：
转码  含义
\n  换行
\r  回车
\t  制表符

    /> date | awk '{print "Month: " $2 "\nYear: ", $6}'
    Month: Oct
    Year:  2011

    /> awk '/Sally/{print "\t\tHave a nice day, " $1,$2 "\!"}' employees
                    Have a nice day, Sally Chang!

   在打印数字的时候你也许想控制数字的格式，我们通常用printf来完成这个功能。awk的特殊变量OFMT也可以在使用print函数的时候，控制数字的打印格式。它的默认值是"%.6g"----小数点后面6位将被打印。
    /> awk 'BEGIN { OFMT="%.2f"; print 1.2456789, 12E-2}'
    1.25  0.12


  现在我们介绍一下功能更为强大的printf函数，其用法和c语言中printf基本相似。下面我们给出awk中printf的格式化说明符列表：
格式化说明符  功能  示例  结果
%c  打印单个ASCII字符。    printf("The character is %c.\n",x)  The character is A.
%d  打印十进制数。 printf("The boy is %d years old.\n",y)  The boy is 15 years old.
%e  打印用科学记数法表示的数。   printf("z is %e.\n",z)  z is 2.3e+01.
%f  打印浮点数。  printf("z is %f.\n",z)  z is 2.300000
%o  打印八进制数。 printf("y is %o.\n",y)  y is 17.
%s  打印字符串。  printf("The name of the culprit is %s.\n",$1);  The name of the culprit is Bob Smith.
%x  打印十六进制数。    printf("y is %x.\n",y)  y is f.
    注：假设列表中的变脸值为x = A, y = 15, z = 2.3, $1 = "Bob Smith"

    /> echo "Linux" | awk '{printf "|%-15s|\n", $1}'  # %-15s表示保留15个字符的空间，同时左对齐。
    |Linux          |

    /> echo "Linux" | awk '{printf "|%15s|\n", $1}'   # %-15s表示保留15个字符的空间，同时右对齐。
    |          Linux|

#%8d表示数字右对齐，保留8个字符的空间。
    /> awk '{printf "The name is %-15s ID is %8d\n", $1,$3}' employees

awk中默认的记录分隔符是回车，保存在其内建变量ORS和RS中。$0变量是指整条记录。
    /> awk '{print $0}' employees #这等同于print的默认行为。


  变量NR(Number of Record)，记录每条记录的编号。
    /> awk '{print NR, $0}' employees

 变量NF(Number of Field)，记录当前记录有多少域。
    /> awk '{print $0,NF}' employees


  /> awk -F: '/root/{print $1,$2}' /etc/passwd  #这里-F选项后面的字符表示分隔符。如果分隔符不是空白字符时)了


 对于awk而言，其模式部分将控制这动作部分的输入，只有符合模式条件的记录才可以交由动作部分基础处理，而模式部分不仅可以写成正则表达式(如上面的例子)，awk还支持条件表达式，如：
    /> awk '$3 < 4000 {print}' employees


 模式和动作一般是捆绑在一起的。需要注意的是，动作是花括号内的语句。模式控制的动作是从第一个左花括号开始到第一个右花括号结束，如下：
    /> awk '$3 < 4000 && /Sally/ {print}' employees


" ~ " 用来在记录或者域内匹配正则表达式。
    /> awk '$1 ~ /[Bb]ill/' employees      #显示所有第一个域匹配Bill或bill的行。


 /> awk '$1 !~ /[Bb]ill/' employees     #显示所有第一个域不匹配Bill或bill的行，其中!~表示不匹配的意思。
   /> awk '/^north/' testfile            #打印所有以north开头的行。

 /> awk '/^(no|so)/' testfile          #打印所有以so和no开头的行。

 /> awk '$5 ~ /\.[7-9]+/' testfile     #第五个域字段匹配包含.(点)，后面是7-9的数字。
/> awk '$8 ~ /[0-9][0-9]$/{print $8}' testfile  #第八个域以两个数字结束的打印。
```

## crontab
```
usage:  crontab [-u user] file
        crontab [-u user] [ -e | -l | -r ]
                (default operation is replace, per 1003.2)
        -e      (edit user's crontab)
        -l      (list user's crontab)
        -r      (delete user's crontab)
        -i      (prompt before deleting user's crontab)

文件格式如下(每个列之间是使用空格分开的):
       第1列分钟1～59
       第2列小时1～23（0表示子夜）
       第3列日1～31
       第4列月1～12
       第5列星期0～6（0表示星期天）
       第6列要运行的命令

       分 时 日 月 星期 要运行的命令

       30 21* * * /apps/bin/cleanup.sh
       上面的例子表示每晚的21:30运行/apps/bin目录下的cleanup.sh。
       45 4 1,10,22 * * /apps/bin/backup.sh
       上面的例子表示每月1、10、22日的4:45运行/apps/bin目录下的backup.sh。
       10 1 * * 6,0 /bin/find -name "core" -exec rm {} \;
       上面的例子表示每周六、周日的1:10运行一个find命令。
       0,30 18-23 * * * /apps/bin/dbcheck.sh
       上面的例子表示在每天18:00至23:00之间每隔30分钟运行/apps/bin目录下的dbcheck.sh。
       0 23 * * 6 /apps/bin/qtrend.sh
       上面的例子表示每星期六的11:00pm运行/apps/bin目录下的qtrend.sh。

       -u 用户名。
       -e 编辑crontab文件。
       -l 列出crontab文件中的内容。
       -r 删除crontab文件。
       系统将在/var/spool/cron/目录下自动保存名为<username>的cron执行脚本.
       cron是定时完成的任务, 在任务启动时,一般来讲都是重新启动一个新的SHELL, 因此当需要使用登录配置文件的信息,特别是环境变量时,是非常麻烦的.
       一般这种问题的使用方法如下:
       0 2 * * * ( su - USERNAME -c "export LANG=en_US; /home/oracle/yb2.5.1/apps/admin/1.sh"; ) > /tmp/1.log 2>&1
       如果打算执行多条语句, 他们之间应使用分号进行分割. 注: 以上语句必须在root的帐户下执行.
```

## 网络配置
- root登录 运行setup 设置网络
    - `/etc/rc.d/init.d/network restart`
    - `/etc/rc.d/init.d/xinetd restart`
- ifconfig eth0 x.x.x.x 对IP设置
    - `ifconfig eth0 network x.x.x.x` 对子网掩码设置
    - `ifconfig eth0 down|up` 设置网卡是否有效
    - 立即生效，重启失效
- 修改`/etc/sysconfig/network-scripts/ifcfg-eth0` restart生效

## 启动服务
```shell
sudo service smbd restart
/etc/init.d/samba restart
```

## 后台任务
- 在命令尾处键入&把作业发送到后台
- 也可以把正在运行的命令发送到后台运行，首先键入Ctrl+Z挂起作业，然后键入bg移动后台继续执行
- bg %jobnumber 或bg %name
- fg %jobnumber 把后台作业带到前台来
- kill %jobnumber 删除后台作业
- jobs -l将PID显示 -r运行中显示 -s显示停止
- nohup command & #如果你正在运行一个进程，而且你觉得在退出帐户时该进程还不会结束，那么可以使用nohup命令。该命令可以在你退出帐户之后继续运行相应的进程。

## 搜索程序
```shell
which                              #定位一个命令，搜索磁盘上命令，不能找到内置命令
whereis                            #定位一个命令的二进制、源文件、手册

type                               #查看是否是内置命令

apropos                            #查看相关名字命令的说明 同man -k
whatis                             #查看相关名字命令的说明

slocate                            #搜索文件，需要配置/etc/updatedb.conf（运行updatedb）
```

## 查看用户
```shell
who                                #列出登录用户列表
w
ps u

finger {usr}                       #用户详细情况
```

## 查看进程
```shell
top
free -k

ps aux | grep pattern
ps                                 #快速浏览自己的进程与PID
ps e                               #显示shell环境信息
ps f                               #格式化父子进程的显示
ps a                               #显示所有终端上进程
ps l                               #显示详细列表
ps u                               #显示用户及起始时间
ps x                               #不带终端的显示

```

## 将man手册转存为文本
```shell
man ls |col -b >ls.txt             # col -b  过滤掉所有的控制字符
info make -o make.txt -s
```

## 语言设置
```shell
/etc/sysconfig/i18n
LANG="zh_CN.GB18030"

/etc/profile
export LC_ALL=zh_CN.GB18030
```

## 加载.profile后运行shell
```shell
-/bin/sh
```

<!-- ## gcc几个步骤
gcc -E hello.c -o hello.i          #预处理
gcc -S hello.i                     #编译，产行hello.s汇编文件
gcc -c hello.s                     #汇编，产生hello.o二进制目标文件
gcc hello.o -o hello.out           #链接
-->


Vim
=====
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


## 编辑模式
在命令模式下，键入以下将会进入编辑模式

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


<!-- http://blog.163.com/stu_shl/blog/static/599375092011639354090/ -->
