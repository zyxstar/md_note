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

参考 [Git分支管理策略](http://www.ruanyifeng.com/blog/2012/07/git.html)

分支可以为要发布的代码保留一份拷贝，无须停止正在开发的工作

```shell
git branch RB_1.0 master
```

该分支从主分支(master branch)上创建了一个RB_1.0的分支，分支名中"RB"代表"release branch"

现在对index.html做一些新的改动，用下面命令提交修改

```shell
git commit -a
```

将显示一个编辑器，让输入一些提交信息，保存退出后

```
[master b5e787e] add in a bio link
 1 file changed, 1 insertion(+)
```

`-a`代表提交全部修改过的文件
























































































































































































































































































































































[Git远程操作详解](http://www.ruanyifeng.com/blog/2014/06/git_remote.html)

