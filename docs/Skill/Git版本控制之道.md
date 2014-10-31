> 2014-10-29

[Git分支管理策略](http://www.ruanyifeng.com/blog/2012/07/git.html)

[Git远程操作详解](http://www.ruanyifeng.com/blog/2014/06/git_remote.html)

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
> 所以`git rebase master [<branch>]`可成更常用
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
```

Git日常用法
==========
## 添加与提交
### 添加文件到暂存区
`git add`将添加新文件和修改版本库中已有文件，暂存变更，暂存操作会更新git内部索引，该索引称为 暂存区

很多情况下，暂存区没什么作用，除非在提交版本库之前要选择提交哪些，不提交哪些

`git add -i`启动交互模式的添加

`git add -p`启动补丁模式

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

- `git diff`显示工作目录树与暂存区的区别
- `git diff --cached`显示暂存区与版本库的区别
- `git diff HEAD`显示工作目录树与版本库的区别

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

`^`一个脱字符，相当于回溯一个版本


## 格式化显示
```shell
git log
    --pretty=oneline
    --pretty=format:"%h %s" 1.0..HEAD  #%h哈希值 %s消息第一行
```













## 克隆远程版本库
```shell
apt-get good
git clone git://github.com/tswicegood/mysite.git mysite-remote
```

```
Cloning into 'mysite-remote'...
remote: Counting objects: 12, done.
remote: Total 12 (delta 0), reused 0 (delta 0)
Receiving objects: 100% (12/12), done.
Resolving deltas: 100% (2/2), done.
```

`git clone`有两个参数，远程版本库位置和存放该版本库的本地目录，第二个是可选的





































































































































































































































































































