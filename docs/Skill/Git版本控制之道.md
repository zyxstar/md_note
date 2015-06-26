> 2014-10-29

Git版本控制之道
===============
## 版本库
分布式版本控制系统，每个人都会在本地有自己的版本库，而不是连接到服务器上的一个公共版本库，所有的历史记录都存储在本地版本库中，向版本库提交代码无须连接远程版本库，而是记录在本地在版本库中

## 工作目录树
工作目录树是版本库的一个“断面视图”，包括了开发该项目所需要的全部文件

git中，版本库不在服务器上，而存储在本地工作目录树的".git"目录中

最初的工作目录树的产生：

- 用git相关命令初始化版本库，也就生成了".git"目录，于是它的父目录就成了工作目录树
- `clone`一个已有的版本库，也就连带创建了相应的工作目录树

## 代码修改与文件同步
每次提交都使得版本库中新增一个版本，需要提交留言，方便将来查询为什么做这个改动

git除了把改动提交到本地版本库，还要通过某种方法将改动共享，以便其他程序员能看到，把改动`push`上游版本库

上游版本库是一个公共版本库，为了把别人完成的改动从公共版本库拿到本地版本库中

1. 把改动取来`fetch`，把远程版本库中的版本和分支复制到本地版本库中，
2. 把从远程版本库中取来的改动与自己本地改动合并`merge`
3. git提供一个命令完成两步操作，叫`pull`

git是分布式的，可以把本地版本库所做改动`push`到 __不止__ 一个远程版本库中，也可以把不止一个远程版本库中改动`pull`到本地版本库中

## 跟踪项目、目录和文件
git并 __不把__ 整个文件，作为不可分的整体来 __记录和跟踪__，而是记录和跟踪组成该文件的各部分内容

这样降低了版本库存储全部历史版本所需的磁盘空间，也使得“判断某个函数或类在文件间移动和拷贝的情况”的操作变得快捷容易

## 使用标签跟踪里程碑
使用标签能够记录下版本库在特定历史时刻的“断面视图”，以便日后查找和恢复

可以是一个正式对外发布的版本，也可以修复一个缺陷后打标签

## 使用分支来跟踪并行演进
打算为项目重写日历组件，从经验上看很难一次写对，分支可解决此问题

在版本库中创建分支的起点，自此，两路发展平行并进，每条分支记录这条分支上发生变更，而与其他分支隔离

主分支是研发的主线，分支可以长期存在，也可仅存数小时，分支可合并到别的分支，但并非所有分支都必须合并

有些情况下，分支不应该合并，比如用分支记录项目不同发布版本的

分支也可以在本地创建，并留作私用，完成试验性工作后，如果有价值，再分享不迟

## 合并
有几条分支是合理的，但它们的轨迹有时可能会重新聚集到一起，这时发生合并

当不同的变化发生文件的不同部分时，git能自动合并，但当不能自动合并时，就会提示冲突"conflict"

git有好几种解决冲突的方法

有许多的分支及其合并，想“如何记录和跟踪我所做的合并？”，git会自动记录和跟踪合并的功能，称为 __合并跟踪__

Git设置
=======
## 设置Git
git是分布式的，通过`git config`，用户可以把一些信息提供给本地版本库

全局的意思是，在这台计算机使用任何Git版本库时，这些全局变量的值都起作用，在相关命令中加上`--global`参数即可

```shell
git config --global user.name "zyxstar"
git config --global user.email "zyxstar@qq.com"
```

下面可检查设置是否成功

```shell
git config --global --list
```

想在命令行窗口中使用不同类型显示不同内容时

```shell
git config --global color.ui "always"
```

## 获取Git内置帮助
```shell
git help <command>
```

创建第一个项目
==============
## 创建版本库
```shell
mkdir mysite
cd mysite
git init
```

`git init`会创建一个".git"目录，这个目录用来存放版本库的全部元数据，"mysite"目录作为工作目录树，存放从版本库中检出的代码

## 代码修改
创建一个"index.html"文件，输入一些代码，想让git跟踪这个文件，须让它知道这个文件要分两步走

1. 使用`git add`把该文件添加到版本库的索引(index)
2. 使用`git commit`命令提交

```shell
git add index.html
git commit -m "add hello html"
```

```
[master (root-commit) 77105c0] add hello html
 1 file changed, 1 insertion(+)
 create mode 100644 index.html
```

`git commit`创建一个提交记录，存储在版本中的历史记录，每次提交（可能多个文件）创建一个记录，并标记出代码的演进

运行`git log`看到提交相关的信息

```
commit 77105c0e35c3922f17bfe8b0212778cb942f692d
Author: zyxstar <248340019@qq.com>
Date:   Wed Oct 29 15:33:50 2014 +0800

    add hello html
```

- 第一行显示提交名称，由git自动产生的 __SHA-1__ 码(40位)，其 __前七个字符__ 与`git commit`输出的相同
- 第二行是提交者的信息
- 第三行是提交日期
- 最后是提交留言

> 漂亮的log，`git config --global alias.lg "log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --"`


## 在项目中工作
在html文件中进行编辑，修改完毕，git可以检测到文件被修改，`git status`显示工作目录树的状态

```shell
git status
```

```
# On branch master
# Changes not staged for commit:
#   (use "git add <file>..." to update what will be committed)
#   (use "git checkout -- <file>..." to discard changes in working directory)
#
#       modified:   index.html
#
no changes added to commit (use "git add" and/or "git commit -a")
```

修改过文件在"Changes not staged for commit"下列出来，如果要提交，须要 __暂存(stage)__ 修改

git有三个地方可以存放代码

- 工作目录树，编辑文件时可以直接在这里操作
- 索引（index），也就是  __暂存区(staging area)__，它是工作目录树和版本库之间的缓冲区
- 版本库

现在看`git add`，它可以暂存对"index.html"刚刚做的修改，这与前面 __添加一个新文件时使用同一个命令__，只不过，这次告诉git __跟踪一个新的修改__，而非新的文件

```shell
git add index.html
git status
```

```
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#       modified:   index.html
#
```

标题变成"Changes to be committed"，"index.html"这一行由红色变成绿色

使用`git commit`，并提交留言

```shell
git commit -m "add body to index" \
 > -m "this allows for a more semantic doc"
```

```
[master 31181f4] add body to index
 1 file changed, 1 insertion(+)
```

可使用任意多次提交留言的输入，即多次使用`-m`，每次另起一段

使用`git log -1`使用数字限制log输出的提交条目的个数

```shell
git log -1
```

```
commit ea67a0a487ebfc1a03936f661b99217aa55fae65
Author: zyxstar <248340019@qq.com>
Date:   Wed Oct 29 16:59:30 2014 +0800

    add body to index

    this allow for a more semantic doc
```

## 理解并使用分支
有两种分支比较有用：

- 用来支持项目的不同发布版本的分支
- 用来支持一个特定功能的开发的分支（topic branch）

分支可以为要发布的代码保留一份拷贝，无须停止正在开发的工作

```shell
git branch RB_1.0 master
```

该分支从主分支(master branch)上创建了一个RB_1.0的分支，分支名中"RB"代表"release branch"

`git branch`不加参数，显示目前所有分支

现在对index.html做一些新的改动，用下面命令提交修改

```shell
git commit -a
```

将显示一个编辑器，让输入一些提交信息，保存退出后

```
[master b5e787e] add in a bio link
 1 file changed, 1 insertion(+)
```

`-a`代表提交全部修改过的文件(不包括新建的)

现在主分支上有最新的修改，而发布分支上 __还是原来的代码__，请切换到发布分支

```shell
git checkout RB_1.0
```

```
Switched to branch 'RB_1.0'
```

可以做一些新的改动，并进行提交(master不知道)

## 处理发布
要准备发由1.0版本了，要给它打个标签，意味着在版本库的历史中（用人类可以理解的名称而非SHA号）标记出特定的点

```shell
git tag 1.0 RB_1.0
```

可使用`git tag`显示版本库中标签列表

```shell
git tag
```

```
1.0
```

现有两条分支上有不同的提交，它们并不知道彼此所包含的代码，现在要把RB_1.0分支上所做的修改合并到主分支上来。变基命令`get rebase`(第一个参数就是成为base的)可完成这项工作，变基是把一条分支上修改在另一条分支的末梢重现

```shell
git checkout master
```

```
Switched to branch 'master'
```

```shell
git rebase RB_1.0
```

```
First, rewinding head to replay your work on top of it...
Applying: add in a bio link
Using index info to reconstruct a base tree...
Falling back to patching base and 3-way merge...
Auto-merging index.html
CONFLICT (content): Merge conflict in index.html
Failed to merge in the changes.
Patch failed at 0001 add in a bio link

When you have resolved this problem run "git rebase --continue".
If you would prefer to skip this patch, instead run "git rebase --skip".
To check out the original branch and stop rebasing run "git rebase --abort".
```

有合并冲突，需解决冲突后，再`git rebase --continue`

> 在master分支上进行rebase一般来说应该是 __不对__ 的。master分支默认是公共分支，当有多人协同时，master分支在多个地方都有副本。一旦修改了master的commit hash id，而如果其他人已经基于之前的commit对象做了工作，那么当他拉取master的新的对象时，会需要在合并一次，这样反复下去，会把master分支搞得一团乱
>
> 所以`git rebase master [<branch>]`可能更常用，或者在需要的分支上，直接使用`git merge --no-ff [<branch>]`
>
> 用`git merge`的时候，merge之后的commit id是按照时间先后排列的；如果用`git rebase`，应该是将B的修改全都放到A最新的commit的后面

最后，作为整理工作的一部分，删除发布分支RB_1.0，只要标签在，从标签到版本树起点一连串提交记录就都在，删除分支只是删除了分支的名字

```shell
git branch -d RB_1.0
```

```
Deleted branch RB_1.0 (was fa11273).
```

将来如何给"1.0.x"分支打丁呢，只须从打标签的地方再创建一条分支即可

```shell
git branch RB_1.0.1 1.0
git checkout RB_1.0.1
```

使用

```shell
git log --pretty=oneline
```

将看到该分支上只有3个提交（master后做的提交看不到）

将某个标签对应的版本内容打包

```shell
git archive --format=tar --prefix=mysite-1.0/ 1.0 | gzip > mysite-1.0.tar.gz
git archive --format=zip --prefix=mysite-1.0/ 1.0 > mysite-1.0.zip
git archive --format=zip --prefix=mysite-release/ HEAD > mysite-release.zip
```

Git日常用法
==========
## 添加与提交
### 添加文件到暂存区
`git add`将添加新文件和修改版本库中已有文件，暂存变更，暂存操作会更新git内部索引，该索引称为 暂存区

很多情况下，暂存区没什么作用，除非在提交版本库之前要选择提交哪些，不提交哪些

`git add -i`启动交互模式的添加

`git add -p`启动补丁模式

`git stash`: 备份当前的工作区的内容，从最近的一次提交中读取相关内容，让工作区保证和上次提交的内容一致(方便pull更新)。同时，将当前的工作区内容保存到Git栈中

> - `git stash pop`: 从Git栈中读取最近一次保存的内容，恢复工作区的相关内容。由于可能存在多个Stash的内容，所以用栈来管理，pop会从最近的一个stash中读取内容并恢复。
> - `git stash list`: 显示Git栈内的所有备份，可以利用这个列表来决定从那个地方恢复。
> - `git stash clear`: 清空Git栈。此时使用gitg等图形化工具会发现，原来stash的哪些节点都消失了。

`git clean`删除一些 没有 `git add` 的 文件，`-n` 显示 将要 删除的 文件 和  目录，`-f` 删除 文件，-`df` 删除 文件 和 目录

### 提交修改
它将变更添加到版本库的历史记录中，并为它们分配一个提交名称

> git不会单独记录和跟踪目录，确实需要跟踪空目录时，在该目录下建立一个以`.`开头的空文件

此处的提交不是发送到某个中央版本库中，而只是提交到本地版本库

git按以下顺序查找编辑器

- 环境变量`GIT_EDITOR`
- git的设置`core.editor`，如`git config --global core.editor vim`
- 环境变量`VISUAL`
- 环境变量`EDITOR`
- 尝试启动vi

三种提交方法

- `git add somefile`,`git commit -m "msg"`，__先暂存__，再提交
- `git commit -m "msg" -a`，__直接提交__  工作目录下所有文件，除未被跟踪的
- `git commit -m "msg" somefile`，__直接提交__  具体文件或文件夹

## 查看修改内容
### 查看当前状态
```shell
git status
```

```
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#       modified:   index.html
#
# Changes not staged for commit:
#   (use "git add <file>..." to update what will be committed)
#   (use "git checkout -- <file>..." to discard changes in working directory)
#
#       modified:   index.html
#
```

- Changes to be committed:  （绿色）列出了哪些文件待提交
- Changes not staged for commit:  （红色）未更新到索引的变更
- Untracked files:（红色）未被跟踪的文件

### 查看文件改动
```shell
git diff
```

```
diff --git a/index.html b/index.html
index e13d7c6..ca86894 100644
--- a/index.html
+++ b/index.html
@@ -6,7 +6,7 @@
 <body>
     <h1>Hello World!</h1>
     <ul>
-        <li><a href="bio.html">About</a></li>
+        <li><a href="about.html">About</a></li>
     </ul>
 </body>
 </html>
```


## 管理文件
### 文件重命名与移动
```shell
git mv index.html hello.html
git status
```

```
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#       renamed:    index.html -> hello.html
#
```

如果不使用该命令，使用系统的`mv`后，则需要再做`git add`和`git rm`达到跟踪文件移动

### 复制文件
不提供文件复制用的命令

### 忽略文件
忽略文件创建`.gitignore`文件，将相关信息写入(如*.swp)但该文件需要纳入管理（适合所有人都需要忽略的文件）

也可以把要忽略的文件添加到`.git/info/exclude`文件中（适合本人需要忽略的文件）

理解和使用分支
===============
## 什么叫分支
添加新功能，重构代码，修复BUG等，都会占用开发人员的时间和精力，如果只使用一条版本演进，难以支持上述情形。因此要创建和使用多条分支

事实上，在git里，__任何修改和提交都是 *在分支上* 完成的__

git的分支只记录和跟踪该分支末稍的那个提交，因为沿这个版本回溯，可以找到该分支完整的历史轨迹

### 创建新分支
```shell
git branch branch1
#基于当前分支创建新分支branch1
git checkout branch1
```

或者

```shell
git checkout -b branch1 master
# 基于master创建新分支branch1并签出
```

### 分支改名
```shell
git branch -m <old> <new>
```

### 合并分支间的修改
#### 直接合并
把两条分支上的历史轨迹合并，交汇到一起

```shell
#在其它分支上修改(比如branch1)
git checkout master
git merge branch1 #无须commit
```

#### 合并压缩
将一条分支上的若干个提交条目压合成一个提交条目，提交到另一条分支的末稍

当想开发一个试验性新功能或修复BUG时，这种合并很有用，此时所需要的并不是长期记录和跟踪每个试验性的提交，只是最后的成果

```shell
git checkout master
git merge --squash branch2
git commit -m "add some msg inclued all submit"
#还需commit
```

#### 拣选合并
拣选另一条分支上的某个提交条目的改动带到当前分支上，只须合并某个提交，而不是全部改动，因为该分支上可能含有未完成的新功能

```shell
git checkout master
git cherry-pick 321d76f
#321d76f为某一提交名,合并后会立即提交
```

```shell
git checkout master
git cherry-pick -n 321d76f #合并一个，不提交，方便下一个拣选，然后手工提交
git cherry-pick -n 321d76d
git commit #直接提交，不要-m，保留以前提交留言
```

## 冲突处理
```
<<<<<<< HEAD                 #当前分支 HEAD
<div>JS</div>
=======                      #分隔线
<div>EMCAScript</div>
>>>>>>> about2               #另一分支 about2
```

对于简单的合并，只须手工编辑并解决冲突即可，然后保存，暂存并提交

修改冲突后，提交可不用`-m`，将使用合并操作的信息来充当留言

`git config --global merge.tool kdiff3`可设置解决冲突的工具

`git mergetool`调出冲突解决工具

## 删除分支
```shell
git branch -d about2
```

如果未合并，是不能删除分支的，除非使用`-D`，适合已执行了压合合并和拣选合并后的情形

查询Git历史记录
==============
## 查看Git日志
```shell
git log                    #每个提交显示：提交名、提交人、日期、留言
    -p                     #显示版本间的代码差异，可组合其它
    -5                     #显示最近5条日志
    7b1558c                #指定版本(指定前4或5位即可)
```

## 指定查找范围
```shell
git log
                           #时间可以为 24 hours, 1 minute, 2008-10.01
    --since="5 hours"      #最近5小时的提交
    --before="5 hours"     #5小时之前的提交
    --before="5 hours" -1  #5小时之前的最后一个提交

    18f822e..0bb3dfb       #不包括起点，包括终点
    18f822e..HEAD          #HEAD代表版本库当前分支末梢最新版本
    18f822e..              #默认HEAD

    Tag1.0..HEAD           #特定标签
```

- `^`一个脱字符，相当于回溯一个版本，`18f822e^`代表`18f822e`之前的那个版本，可使用多个`^`代表之前N个版本
- `~N`波浪符加数字，指回溯N个版本，`^`==`~1`

```shell
git log -1 HEAD^^^
git log -1 HEAD^~2
git log -1 HEAD~1^^
git log -1 HEAD~3
git log HEAD~10..HEAD
```

## 格式化显示
```shell
git log
    --pretty=oneline
    --pretty=format:"%h %s" 1.0..HEAD  #%h哈希值 %s消息第一行
```

## 查看版本之间的差异
- `git diff`显示工作目录树与暂存区的区别
- `git diff --cached`显示暂存区与版本库的区别
- `git diff HEAD`显示工作目录树与版本库的区别
- `git diff 18f822e`显示18f822e这个版本与当前工作目录树间差异

`git diff`中指定版本范围方法与`git log`一样，有一个差别是`git diff`输出最老版本与最新版本间差异，中间的并不显示

获取tag距现在之间代码量统计

```shell
git diff --stat 1.0 HEAD  #HEAD为可选
```

```
about.html   |   15 +++++++++++++++
contact.html |   23 +++++++++++++++++++++++
hello.html   |   13 +++++++++++++
index.html   |    9 ---------
4 files changed, 51 insertions(+), 9 deletions(-)
```

## 查明该向谁负责
```shell
git blame hello.html
```

```
^7b1558c index.html (Travis Swicegood 2008-09-21 14:20:21 -0500  1) <html>
a5dacabd index.html (Travis Swicegood 2008-09-21 20:37:47 -0500  2) <head>
a5dacabd index.html (Travis Swicegood 2008-09-21 20:37:47 -0500  3)     <title>Hello World in Git</title>
4b537794 index.html (Travis Swicegood 2008-09-22 08:04:05 -0500  4)     <meta name="description" content="hello world in Git" />
a5dacabd index.html (Travis Swicegood 2008-09-21 20:37:47 -0500  5) </head>
^7b1558c index.html (Travis Swicegood 2008-09-21 14:20:21 -0500  6) <body>
^7b1558c index.html (Travis Swicegood 2008-09-21 14:20:21 -0500  7)     <h1>Hello World!</h1>
4333289e index.html (Travis Swicegood 2008-09-22 07:54:28 -0500  8)     <ul>
6f1bf6ff index.html (Travis Swicegood 2008-10-03 23:43:06 -0500  9)         <li><a href="about.html">About</a></li>
6f1bf6ff index.html (Travis Swicegood 2008-10-03 23:43:06 -0500 10)         <li><a href="contact.html">Contact</a></li>
4333289e index.html (Travis Swicegood 2008-09-22 07:54:28 -0500 11)     </ul>
^7b1558c index.html (Travis Swicegood 2008-09-21 14:20:21 -0500 12) </body>
^7b1558c index.html (Travis Swicegood 2008-09-21 14:20:21 -0500 13) </html>
```

第一行有个脱字符，表示版本库中第一个提交，虽然参数输入`hello.html`，实际上输出的`index.html`，能跟踪重命名的文件

```shell
git blame -L 12,13 hello.html             #查看第12 13行
git blame -L 12,+2 hello.html             #查看第12 12+1行
git blame -L 12,-2 hello.html             #查看第12 12-1行
git blame -L "/<\/body>/",+2 hello.html   #正则匹配行
git blame -L "/<\/body>/",+2 4333289e^ -- index.html #该版本之间,如果重命名了，还要指定那时候的名字
```

## 跟踪内容
git能够跟踪文件里甚至文件间的内容移动，git认为至少要匹配三行才认为是复制和粘贴的结果

```shell
git blame -M original.txt  #检测同一文件内移动或复制的代码
git blame -C -C copy.txt   #检测不同文件复制的代码
git log -C -C -1 -p        #检测文件复制时代码具体变动
```

## 撤销修改
完全分布式开发好处之一就是可以只共享准备好的东西，在push变更之前确保变更是准备好的

一旦push了，就不能随意修改历史了，这会使版本库很难跟踪，如果在共享了代码，还修改历史，团队其他成员在同步这些代码改动时就会遇到麻烦

### 增补提交
小提交，比如在代码中增加个分号，修改后，直接

```shell
git commit -C HEAD -a --amend
```

`-C HEAD`复用HEAD留言 `-c`则使用编辑器；`--amend`增补上次提交（__与上次提交算一个提交__），只能针对最后一个提交

### 反转提交
增补提交只能针对最后一个提交，如果想更正好几个提交之前的某个错误，则须使用`git revert`

或者，有时候程序需要借助特定架构才能运行，而相关同事不在，不如先临时解决

通常git会立即提交反转结果，但通过`-n`可先不提交，运行多个`git revert -n`，git会暂存所有变更，然后做一次提交

做反转操作时候必须提供提交名称，以让git知道要反转什么

```shell
git revert -n HEAD  #该语句运行两次，则相当于没有修改
git revert -n 540ecb7
```

反转提交，最好加上如“不能运行”，“速度太慢”等类似提示信息，反转提交本身作为一次提交，记录下来，__原先log不变__

### 复位
如在代码里包含了一个不该透露的密码，`git reset`以提交名作为参数，复位到一个特定版本，默认是`HEAD`，同样可以使用`^`和`~`作为提交名称的修饰符

`git reset`可以在复位版本库后，暂存工作目录树中因复位产生的与版本库的差异，以便提交，这对于在之前的提交中发现错误并须要更改时非常有用

```shell
gig reset --soft HEAD^  #要复位的两个提交
   --soft #表示暂存差异，
   --hard #表示版本库与工作目录同时删除提交（小心）
```

`git reset HEAD^ -- file.txt`只reset某个文件

`git reset --hard HEAD^`删除当前分支最后一个提交

复位与反转不同之处，回退某次提交，__同时回退修改log__，但是修改内容回退到本地暂存区，由用户确定丢弃（checkout）或者重新提交

### 重新改写历史记录
- 给历史记录重新排序，看起来更合理
- 将多个提交压合成一个提交
- 将一个大的提交分解成多个提交

`git rebase -i`交互式命令，完成改写历史的工作

#### 重新排序提交
```shell
git rebase -i HEAD~3
```

```
pick 2fc9c8c commit for original text file
pick 425f6d8 add link to twitter
pick b99c984 copy three lines

# Rebase 0bb3dfb..425f6d8 onto 0bb3dfb
#
# Commands:
#  p, pick = use commit
#  r, reword = use commit, but edit the commit message
#  e, edit = use commit, but stop for amending
#  s, squash = use commit, but meld into previous commit
#  f, fixup = like "squash", but discard this commit's log message
#  x, exec = run command (the rest of the line) using shell
#
# If you remove a line here THAT COMMIT WILL BE LOST.
# However, if you remove everything, the rebase will be aborted.
#
```

可通过移动`pick`开头的行，保存退出后就完成了变基操作

#### 将多个提交压合成一个提交
将上述命令出现的文本编辑内的`pick`开头，换成`squash`，将压合上下两条为一个提交

与远程版本库协作
================
## 网络协议
- ssh
- git
- http/https

## 克隆远程版本库
```shell
git clone git://github.com/tswicegood/mysite-chp6.git mysite-chp6
```

```
Cloning into 'mysite-chp6'...
remote: Counting objects: 53, done.
remote: Total 53 (delta 0), reused 0 (delta 0)
Receiving objects: 100% (53/53), 5.72 KiB, done.
Resolving deltas: 100% (19/19), done.
```

`git clone`有两个参数，远程版本库位置和存放该版本库的本地目录，第二个是可选的

## 版本库同步
`clone`可获得远程版本库中直到克隆时的全部历史，然而，在克隆远程版本库之后，还须用`fetch`远程版本库的改动到本地版本库

取来的操作能够更新本地版本库中的远程分支

```shell
git branch -r  #-r显示远程分支
```

```
  origin/HEAD -> origin/master
  origin/about
  origin/alternate
  origin/contacts
  origin/master
  origin/new
```

可以像检出普通分支一样检出远程分支，但不应该修改远程分支上的内容

```shell
git checkout origin/about
```

```
Note: checking out 'origin/about'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b new_branch_name

HEAD is now at f846762... Merge branch 'about2' into about
```

确实需要修改时，应该先从远程分支创建一个本地分支，再进行修改

```shell
git checkout contacts  #直接使用不带origin/的远程分支名，将依据该分支创建本地分支
```

```
Branch contacts set up to track remote branch contacts from origin.
Switched to a new branch 'contacts'
```

`git fetch`更新远程分支，但不会把远程分支上修改合并到本地分支上，使用`git pull`可顺序完成`fetch`和`merge`

`git pull`参数，一是远程版本库名称，另一个是须拖入的远程分支名(无须在分支前指定origin/)

前缀`origin/`表示远程版本库上的分支名称，用于区别本地分支名称，`origin`是默认的远程版本库 __别名__，即`clone`时指定的远程版本库

 ```shell
git fetch origin            #取得远程更新，这里可以看做是准备要取了
git fetch origin branch1    #取远程分支到FETCH_HEAD  (目前所在分支 .git/FETCH_HEAD中第一行)
git fetch origin branch1:branch2   #取远程分支branch2到本地分支branch1
git fetch origin :branch2   #等同git fetch origin master:branch2

git merge origin/master     #把更新的内容合并到本地分支/master
 ```


## 推入改动
把本地改动，也是就本地提交，推入到另一个版本库中

`git push`不带调用参数，会推入默认版本库的`origin`中，并把本地版本库中 __当前所在分支__ 变更推入远程版本库 __对应的分支__ 上

`git push --dry-run`查看推入哪些提交

如果需要指定推入的版本库`git push <repository> <refspec>`，`<repository>`可以是任意有效的版本库名称，`<refspec>`可以是标签、分支、或HEAD这样的关键字，如`git push origin mybranch:master`将本地分支mybranch上的提交推入远程版本的master分支上

```shell
git push -u origin master    #提交到主分支
git push -u origin 1.1       #提交到tag
git push origin RB_1_1       #提交到分支
git push origin test:master  #提交本地test分支作为远程的master分支
git push origin test:test    #提交本地test分支作为远程的test分支
git push origin :test        #刚提交到远程的test将被删除，但是本地还会保存的
```

## 添加新的远程版本库
只有相应权限，可以跟任意远程版本库打交道，进行推入和拖入操作。

在本地版本库中，远程版本库的别名默认是`origin`，它是克隆远程版本库时自动生成的。手动添加远程版本库别名：

```shell
git remote add erin git://..git
git pull erin HEAD
```

> 先在本地`git init`创建的版本库，后来又必须推送到远程版本库里，就可以用这个方法
>
> 也可以`.git/config`文件中直接添加


```shell
git remote               #查看所有远程版本库
git remote -v            #查看所有远程版本库对应的fetch与push源
git remote show <name>   #查看指定远程版本库详细信息
```

`git remote rm <name>`删除别名


管理本地版本库
===============
## 使用标签标记里程碑
版本库里标签就像书签，常用于给项目代码的发布版本做标识，以便以后在需要修正或功能变更时，可以通过标签回到该发布代码上

git中 __不能__ 像修改分支一样修改标签内容，确保在任意时刻取出标签对应的代码，都与创建标签时一样

```shell
git tag                        #查看标签
git tag <tagname> [<commit>]   #创建标签，不写第二个参数，默认使用检出分支的末端版本
```

> `git constacts/1.1 contact`在contact分支上创建标签

```shell
git checkout 1.0               #检出标签，但不能修改，没法提交
```

> 可在当前tag下创建分支`git checkout -b from-1.0 1.0`，用于修改与提交

## 发布分支的处理
发布分支即将要发布代码的地方，开发团队一般用它来隔离即将要发布的代码，发布分支指当一个项目的所有功能都 __已__ 开发完成，__但尚未__ 达到发布的质量标准时创建的分支(侧重BUG与逻辑修正，无新功能添加)

有了发布分支，在主分支上继续开发新功能就很方便，发布分支上不会将并新开发的代码包含进来

发布分支持续时间不长，通常只用于发布代码的最终测试期间，一旦该版本发布，应该使用标签 __标记__ 项目，然后就可以 __删除__ 该发布分支了

如何修正发布版本中的BUG，如果当前发布后做了标签1.0

```shell
git branch RB_1.0.1 1.0
git checkout RB_1.0.1
```

进行bug修复后：

```shell
git tag 1.0.1
git checkout master
git branch -D RB_1.0.1
```

## 使用子模块跟踪外部版本库
git的sub-module可以跟踪外部版本库，和svn:externals差不多

> 现在推荐使用subtree

### 添加新子模块
```shell
git submodule add git://github.com/tswicegood/hocus.git hocus  #创建
git submodule  #查询
```

```
 20cc9ddc65b5f3ea3b871480c1e6d8085db48457 hocus (heads/master)
```

> 在`.git/config`中增加了`[submodule "hocus"]`

```shell
git submodule init hocus  #子模块初始化
```

```
Submodule 'hocus' () registered for path 'hocus'
```

```shell
git status
```

```
# On branch master
#
# Initial commit
#
# Changes to be committed:
#   (use "git rm --cached <file>..." to unstage)
#
#       new file:   .gitmodules
#       new file:   hocus
#
```

`.gitmodules`是一个存储用户所有模块信息的纯文本文件，当别人共享你的版本库时，git就根据它来部署子模块

接下来`git commit`它

### 克隆含子模块的版本库
在克隆版本库后，还需要一些额外步骤，来设置好子模块

```
git submodule
```

```
-20cc9ddc65b5f3ea3b871480c1e6d8085db48457 hocus
```

前面`-`代表还未初始化，需要

```shell
git submodule init hocus
```

```
Submodule 'hocus' (git://github.com/tswicegood/hocus.git) registered for path 'hocus'
```

运行完，hocus目录还是空的，需要

```shell
git submodule update hocus
```

```
Cloning into 'hocus'...
remote: Counting objects: 7, done.
remote: Total 7 (delta 0), reused 0 (delta 0)
Receiving objects: 100% (7/7), done.
Submodule path 'hocus': checked out '20cc9ddc65b5f3ea3b871480c1e6d8085db48457'
```

hocus目录包含了提交名称为`20cc9dd`版本对应的全部文件

### 改变子模块的版本
子模块不会自动引用版本库中最新提交，__利于保持版本的稳定__，它只反映某个提交对应的版本，这一点刚好与subversion相反

当用户第一次创建子模块时，git记录所引用的提交，之后，它与该版本库间，不会有"偷偷的"通信与同步

事实上，子模块了对检出某个特定提交的版本库的完整克隆，可以看`git branch`,`git log`,甚至`git checkout`

```shell
git checkout HEAD^
```

```
cd ..
git submodule
```

```
+7901f67feaadeeef755734a92febbc7214fb7871 hocus (7901f67)
```

`+`表示不是容器版本库所记录的该子模块应该在的版本，使用`git status`也可看出hocus目录被改动了

可以添加到暂存区，并提交，表示希望引用子模块的这个版本

```shell
git add hocus
git commit -m "update commit to track in submoduel"
```

再使用`git submodule`将看到引用了这个版本

### 使用子模块时注意
`git add`时不要加`\`，否则将把源版本库中全部文件添加到当前版本库，而不是更新子模块引用

`git submodule update`有可能 __会覆盖__ 本地子模块中所有没有提交的内容，在使用前请确认必要的提交

子模块和容器模块里的分支是分开的，需要分别使用`git checkout`来切换

一旦子模块中提交修改，必须确保改动push到子模块的远程版本库中


局域网简易Git
============
确保服务器与客户机均已安装好git和ssh

- 服务端设置

```shell
sudo useradd gituser                           #设置用户与密码,ubuntu下使用adduser

su - gituser
mkdir repo                                     #第一次需要创建目录,以后不需要

cd repo
git --bare init proj1.git                      #创建项目
```

- 客户端设置

```shell
ssh-keygen -C "user1@a.com"                    #生成ssh key,该过程中一直回车
ssh-copy-id -i gituser@192.168.0.3             #假设服务器192.168.0.3,该过程需输入gituser密码
                                               #如不设置,将每次push/pull时要求输入gituser密码

git config --global user.name "user1"          #设置git配置,不同用户使用不同的名字
git config --global user.email "user1@a.com"

cd work_space                                  #进入某个工作目录
git clone gituser@192.168.0.3:repo/proj1.git   #克隆项目到本地

cd proj1                                       #将产生proj1的目录,下面进行测试
echo "a">a.txt
git add .
git commit -a
git push origin master
```

> 出现ssh连接"The authenticity of host can't be established"

```shell
sudo vim /etc/ssh/ssh_config
  StrictHostKeyChecking no
  UserKnownHostsFile /dev/null
```

Git subtree试用
===============
有多个android项目，一个web项目。android项目依赖于某个ide，通过编译在其`bin`目录下产生一个`.apk`文件；而该文件需要放在互联网上供用户下载，下载目录位于web项目的某个`download`的文件夹下，因为存在多个android项目，即最终会有多个`.apk`文件存放在该目录下。

项目设置如下：

```shell
#前置条件
#git@gitcafe.com:zyxstar/web.git         #web项目
#git@gitcafe.com:zyxstar/android1.git    #android1项目
#git@gitcafe.com:zyxstar/android2.git    #android2项目
#git@gitcafe.com:zyxstar/resource.git    #所有需要下载的资源，也设置一个git（它可以只是一个在远程的一个库）
#resource.git中，设置好.gitignore，将无关的文件与目录过滤掉


#切到android1项目目录下，增加一个resource.git作为远程版本库
git remote add resource git@gitcafe.com:zyxstar/resource.git
#将resource作为本项目的bin子目录存在（如果已经存在，则不必执行）
git subtree add --prefix=Music/bin --squash resource master
#假设编译生成了一个.apk文件，并已commit(可选择android1本身是否需要push)
git subtree pull --prefix=Music/bin --squash resource master #如果不想pull,以后只能强推了
git subtree push --prefix=Music/bin resource master
#如果push不成功，尝试强推
git push resource `git subtree split --prefix=Music/bin`:master --force


#切到web项目下，增加一个resource.git作为远程版本库
git remote add resource git@gitcafe.com:zyxstar/resource.git
#将resource作为本项目的download子目录存在（如果已经存在，则不必执行）
git subtree add --prefix=public/download --squash resource master
#将资源pull到下载目录
git subtree pull --prefix=public/download --squash resource master
#提交web项目
git push origin master
```

团队协作建议
===================
## 创建个人的项目
- 在gitlab中去`fork`团队的proj.git项目（该项目更新后需通知所有人fetch)
- proj.git项目的master分支被保护，不允许普通开发人员push
- `clone`刚才的`fork`项目到指定的项目目录
- 并将proj.git源加到remote(该源需要对该开发人员开放fetch/push权限)

```shell
git clone git@114.242.131.210:zyx/proj.git zyxproj
cd zyxproj
git remote add build git@114.242.131.210:root/proj.git
```

## 开发新功能时
- 当某开发人员需要同时进行多个新功能时，需要创建多个分支，分别进行checkout后工作
- 以下以需要开发一个wechat的功能为示例

```shell
git branch wechat master          #基于master(或其它)分支，创建新工作分支
git branch
git checkout wechat               #进入分支开始工作
```

## 保存开发任务
```shell
git add .
git commit -a
git push -u origin wechat:wechat  #push当前分支到origin的同名分支
git branch -r
```

## [篇外]fetch项目源
> 当自己开发中，需要fetch proj.git项目时(项目有更新)

```shell
git fetch build master:tmp        #将build fetch到临时分支
git diff master tmp               #与build的更新进行比较
git checkout master               #进入需要合并的分支
git merge --no-ff tmp             #确认无问题后进行合并
git push -u origin master:master  #解决完冲突，并git commit后提交
git diff wechat tmp
git checkout wechat               #对其它需要合并的分支进行同样处理
git merge --no-ff tmp
git push -u origin wechat:wechat
git branch -d tmp                 #合并完成后，删除临时分支
```

## 开发任务自测完成时
```shell
git push -u origin wechat:wechat
git push -u build wechat:wechat   #将功能分支提交到build的同名分支，用于测试
```

## 首次配置某功能测试环境
> 配置功能测试环境，需要测试不同功能分支时，需要先`git checkout`
> 也可映射到本地多个目录，方便针对不同功能去做测试

```shell
git clone git@114.242.131.210:root/proj.git feature_xxx
```

## 测试员对新功能进行测试
```shell
cd feature_xxx

git fetch origin wechat:wechat    #首次测试将proj.git中的wechat分支fetch下来
git checkout wechat               #签出wechat分支

    #再次测试时直接fetch可能会被挂住，先fetch到临时分支再合并
    git fetch origin wechat:tmp
    git checkout wechat
    git diff wechat tmp --
    git merge tmp
    git branch -d tmp

...                               #进行必要的环境配置，如bundle install
rails s                           #将单功能测试环境运行起来(或将feature_xxx目录配置到nginx中)
```

## 开发人员修复bug
```shell
git checkout wechat
...                               #修复bug并自测完
git push -u origin wechat:wechat
git push -u build wechat:wechat
```

## 首次部署集成测试环境
> 测试人员测试与开发人员修复是一个反复过程，直至所有bug修复完，才进入本环节

```shell
git clone git@114.242.131.210:root/proj.git proj
```

## 部署功能到集成测试环境
> 确保单个功能测试没问题后，才能合并到build的master分支，接下来进行的是集成测试，如果还存在bug的，则修复过程同上

```shell
cd proj
git checkout master
git fetch origin wechat:wechat
git diff master wechat                  #做一下codeReview(gitlab中进行compare)
                                        #本处的冲突有可能是最多的
                                        #有问题时需要开发人员修复后重新提交
git merge --no-ff wechat
grep -rn "<<<<<<" ./*                   #查找合并后冲突，并解决
                                        #必要时，需要开发人员修复后重新提交

git commit -a                           #提交合并后的分支
git branch build master                 #创建其于master的build的分支(该分支永远不会提交到远程)
git checkout build                      #签出其分支
...                                     #进行必要的环境配置
bundle install
RAILS_ENV=production rake assets:precompile
nginx -s reload                         #将集成测试环境运行起来
```

## 制作生产环境可用版本
> 确保上面的集成测试无问题后，由测试环境制作新tag

```shell
cd proj
git checkout master
git push -u origin master:master        #提交稳定版本
git tag R1.0.1 master                   #以后依据它来编写特征列表
git push -u origin R1.0.1:R1.0.1        #将发布tag也push到orgin上同名tag
git branch -d build_x                   #删除之前用于运行集成环境的build分支
```

## 部署生产版本
> 进入生产环境，生产环境中最好配置一个preview环境

```shell
cd production
git pull origin master:master
...                                     #进行必要的环境配置
bundle install
RAILS_ENV=production rake assets:precompile
nginx -s reload                         #将集成测试环境运行起来
```

## 开发人员后续处理
```shell
...                                     #重新fetch与merge项目源到master与wechat上

git tag wechat1.0 master                #对当前分支打上功能tag
git tag
git push -u origin wechat1.0:wechat1.0  #将tag也push到orgin上同名tag

git checkout master                     #切到主分支
git branch -d wechat                    #删除本地工作分支
git branch -d -r origin/wechat          #删除本地映射的远程分支
git push --delete origin wechat         #真实的删除orign上的同名分支
git push --delete build wechat          #真实的删除build上的同名分支
```


## [篇外]临时接替开发
> 其他开发人员临时接替开发功能wechat

```shell
git clone git@114.242.131.210:zyx/proj.git xxxproj
cd xxxproj
git fetch origin wechat:wechat
...                                     #开发新功能
...                                     #接下同正常开发
git push -u origin wechat:wechat
git push -u build wechat:wechat         #提交到build的同名分支，用于测试
...                                     #bug修复过程
```

## [篇外]基于某tag再进行开发时
> 发现之前的方案其实还不错，所以记得将有意义的尝试打上tag

```shell
git branch wechat_old wechat1.0
git branch
git checkout wechat_old
...                                #工作进行时，单独提交分支
...                                #完成后打tag，合并(可能合并多个分支)，删除分支
```

## [篇外]当发现master存在bug时
```shell
git branch bugfix master
git branch
git checkout bugfix
...                                #工作进行时，单独提交分支
...                                #合并(可能合并多个分支)，删除分支
```

## 提示
- 除非有充分理由，否则不要使用`git pull`，以`git fetch`,`git diff`,`git merge`取代
- 在需要工作时，首先确认当前分支是什么，切换到合适分支下再工作
- 在build目录下做测试时，只会产生`git fetch`等相关操作，不应产生`git push`操作
- 开发人员的`master`分支，只会从`proj.git`上`git fetch/git merge`，该`master`只作新功能/bug修复时的被branch，开发人员不应将功能/bug修复分支合并到自己的`master`分支
- `Gemfile`书写时，必须将版本号写上，防止将来`bundle install`时引用了新的gem包

## 数据库脚本??
- 时间序的创建更新脚本
- 配置数据录入
- 整个schema.rb版本

## 拆分项目??
- gem
- subtree
- 基础模块 与 view ？


GitHub
======
- [Generating SSH keys](https://help.github.com/articles/generating-ssh-keys/)
- [Github Flow](http://scottchacon.com/2011/08/31/github-flow.html)是Github内部使用git的原则：
>    - master分支上的代码保证是deployable。
>    - 每个功能创建一个特性分支，起一个描述性的分支名（比如：new-oauth2-scopes），并且push到远程。
>    - 本地commit到这个分支，并且经常push到远程同名分支。
>    - 需要反馈或功能完成时，发pull request。相关人员review之后合并回master。新merge之后deploy master代码。
>    - master分支随时可以deploy这是Github Flow的核心。就是说所有merge到master分支的代码都需要测试和构建通过。
>    - 从master分支分出功能分支。描述性的分支名的好处是大家都可以很容易通过branch list知道其他人在做什么。
>    - 经常把本地代码push到远程的好处是不但可以起到备份作用，也可以让其他了解你的工作进展状况。并且不会影响其他同事。
>    - 在Github，Pull Request被当成issue系统和code review系统，基本上都是用pull request来沟通的。
>    - Pull Request之后所有人看过之后并且签名并且通过CI测试，你的代码就可以merge回master分支，然后部署到线上。


参考
====
- [Git分支管理策略](http://www.ruanyifeng.com/blog/2012/07/git.html)
- [Git远程操作详解](http://www.ruanyifeng.com/blog/2014/06/git_remote.html)
- [理解Git的工作流程](http://kb.cnblogs.com/page/152176/)
- [GIT分支管理是一门艺术](http://kb.cnblogs.com/page/132209/)



<!--
$ git rm --cached FILENAME
git clone gituser@123.57.253.111:/var/data/repo/sdjfood.git

-->
