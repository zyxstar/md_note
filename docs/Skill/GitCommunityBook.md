Git对象模型
============

### SHA
所有用来表示项目历史信息的文件，通过一个40个字符的“对象名”来索引，每一个“对象名”是对“对象”内容做SHA1哈希计算得来的，意味着两个内容不同的对象不可能有相同的“对象名”

- Git只比较对象名，就可以很快的判断两个对象是否相同
- 因为每个仓库（repository）的“对象名”的计算方式都一样，如果同样内容存在两个不同的仓库中，就会存在相同的的“对象名”
- Git通过检查对象内容的SHA1与“对象名”是否相同，来判断对象内容是否正确（网络传输时内容检验）

### 对象
每个对象（object）包括三个部分：类型、大小、内容

四种类型:

- blob：存储文件数据，通常是一个文件
- tree：有点像一个目录，管理一些tree或blob（就像文件和子目录）
- commit：指向一个tree，标记项目某一个特定时间点的状态，包括一些关于时间点的元数据，如时间戳、最近一次提交的作者、指向上次提交（commit）的指针等
- tag：标记某一个提交（commit）的方法

### 与SVN区别
不是“增量文件系统”，它会把你每次提交的文件的全部内容都记录下来。

### Blob对象
一个blob通常用来存储文件的内容

![object-blob](../../imgs/object-blob.png)

可使用`git show`来查看一个blob对象里的内容

<!--language: shell-->

    $ git show 6ff87c4664

     Note that the only valid version of the GPL as far as this project
     is concerned is _this_ particular version of the license (ie v2, not
     v2.2 or v3.x or whatever), unless explicitly otherwise stated.
    ...

一个"blob对象"就是一块二进制数据，它没有指向任何东西或有任何其它属性，甚至连文件名都没有。

因为blob对象内容全部都是数据，如两个文件在一个目录树（或是一个版本仓库）中有同样的数据内容，那么它们将会共享同一个blob对象。Blob对象和其所对应的文件所在路径、文件名是否改被更改都完全没有关系。

### Tree对象
一个tree对象有一串(bunch)指向blob对象或是其它tree对象的指针，它一般用来表示内容之间的目录层次关系。

![object-tree](../../imgs/object-tree.png)

`git show`命令还可以用来查看tree对象，但是`git ls-tree`能让你看到更多的细节。如果我们有一个tree对象的SHA1哈希值，我们可以像下面一样来查看它：

<!--language: shell-->

    $ git ls-tree fb3a8bdd0ce

    100644 blob 63c918c667fa005ff12ad89437f2fdc80926e21c    .gitignore
    100644 blob 5529b198e8d14decbe4ad99db3f7fb632de0439d    .mailmap
    100644 blob 6ff87c4664981e4397625791c8ea3bbb5f2279a3    COPYING
    040000 tree 2fb783e477100ce076f6bf57e4a6f026013dc745    Documentation
    100755 blob 3c0032cec592a765692234f1cba47dfdcc3a9200    GIT-VERSION-GEN
    100644 blob 289b046a443c0647624607d471289b2c7dcd470b    INSTALL
    100644 blob 4eb463797adc693dc168b926b6932ff53f17d0b1    Makefile
    100644 blob 548142c327a6790ff8821d67c2ee1eff7a656b52    README
    ...


一个tree对象包括一串(list)条目，每一个条目包括：mode、对象类型、SHA1值 和名字(这串条目是按名字排序的)。它用来表示一个目录树的内容。

一个tree对象可以指向(reference): 一个包含文件内容的blob对象, 也可以是其它包含某个子目录内容的其它tree对象. Tree对象、blob对象和其它所有的对象一样，都用其内容的SHA1哈希值来命名的；只有当两个tree对象的内容完全相同（包括其所指向所有子对象）时，它的名字才会一样，反之亦然。这样就能让Git仅仅通过比较两个相关的tree对象的名字是否相同，来快速的判断其内容是否不同。

(注意：在submodules里，trees对象也可以指向commits对象. 请参见 Submodules 章节)

注意：所有的文件的mode位都是644 或 755，这意味着Git只关心文件的可执行位.

### Commit对象
"commit对象"指向一个"tree对象", 并且带有相关的描述信息.

![object-commit](../../imgs/object-commit.png)

你可以用 `--pretty=raw` 参数来配合 `git show` 或 `git log` 去查看某个提交(commit):

<!--language: shell-->

    $ git show -s --pretty=raw 2be7fcb476

    commit 2be7fcb4764f2dbcee52635b91fedb1b3dcf7ab4
    tree fb3a8bdd0ceddd019615af4d57a53f43d8cee2bf
    parent 257a84d9d02e90447b149af58b271c19405edb6a
    author Dave Watson <dwatson@mimvista.com> 1187576872 -0400
    committer Junio C Hamano <gitster@pobox.com> 1187591163 -0700

        Fix misspelling of 'suppress' in docs

        Signed-off-by: Junio C Hamano <gitster@pobox.com>


你可以看到, 一个提交(commit)由以下的部分组成:

- 一个 tree 对象: tree对象的SHA1签名, 代表着目录在某一时间点的内容.
- 父对象 (parent(s)): 提交(commit)的SHA1签名代表着当前提交前一步的项目历史. 上面的那个例子就只有一个父对象; 合并的提交(merge commits)可能会有不只一个父对象. 如果一个提交没有父对象, 那么我们就叫它“根提交"(root commit), 它就代表着项目最初的一个版本(revision). 每个项目必须有至少有一个“根提交"(root commit). 一个项目可能有多个"根提交“，虽然这并不常见(这不是好的作法).
- 作者 : 做了此次修改的人的名字,　还有修改日期.
- 提交者（committer): 实际创建提交(commit)的人的名字, 同时也带有提交日期. TA可能会和作者不是同一个人; 例如作者写一个补丁(patch)并把它用邮件发给提交者, 由他来创建提交(commit).
- 注释 用来描述此次提交.


一个提交(commit)本身并没有包括任何信息来说明其做了哪些修改; 所有的修改(changes)都是通过与父提交(parents)的内容比较而得出的. 值得一提的是, 尽管git可以检测到文件内容不变而路径改变的情况, 但是它不会去显式(explicitly)的记录文件的更名操作.　(你可以看一下 `git diff` 的 `-M`参数的用法)

一般用 git commit 来创建一个提交(commit), 这个提交(commit)的父对象一般是当前分支(current HEAD),　同时把存储在当前索引(index)的内容全部提交.

### 标签对象
![object-tag](../../imgs/object-tag.png)

一个标签对象包括一个对象名(译者注:就是SHA1签名), 对象类型, 标签名, 标签创建人的名字("tagger"), 还有一条可能包含有签名(signature)的消息. 你可以用 `git cat-file` 命令来查看这些信息:

<!--language: shell-->

    $ git cat-file tag v1.5.0

    object 437b1b20df4b356c9342dac8d38849f24ef44f27
    type commit
    tag v1.5.0
    tagger Junio C Hamano <junkio@cox.net> 1171411200 +0000

    GIT 1.5.0
    -----BEGIN PGP SIGNATURE-----
    Version: GnuPG v1.4.6 (GNU/Linux)

    iD8DBQBF0lGqwMbZpPMRm5oRAuRiAJ9ohBLd7s2kqjkKlq1qqC57SbnmzQCdG4ui
    nLE/L9aUXdWeTFPron96DLA=
    =2E+0
    -----END PGP SIGNATURE-----

`git tag` 同样也可以用来创建 "轻量级的标签"(lightweight tags), 但它们并不是标签对象, 而只一些以 "refs/tags/" 开头的引用罢了


Git目录 与 工作目录
===================

## Git目录
是为你的项目存储所有历史和元信息的目录,包括所有的对象(commits,trees,blobs,tags), 这些对象指向不同的分支.

每一个项目只能有一个'Git目录'(这和SVN,CVS的每个子目录中都有此类目录相反),　这个叫'.git'的目录在你项目的根目录下(这是默认设置,但并不是必须的). 如果你查看这个目录的内容, 你可以看所有的重要文件:

<!--language: shell-->

    $>tree -L 1
    .
    |-- HEAD         # 这个git项目当前处在哪个分支里
    |-- config       # 项目的配置信息，git config命令会改动它
    |-- description  # 项目的描述信息
    |-- hooks/       # 系统默认钩子脚本目录
    |-- index        # 索引文件
    |-- logs/        # 各个refs的历史信息
    |-- objects/     # Git本地仓库的所有对象 (commits, trees, blobs, tags)
    `-- refs/        # 标识你项目里的每个分支指向了哪个提交(commit)。

## 工作目录

Git的 '工作目录' 存储着你现在签出(checkout)来用来编辑的文件. 当你在项目的不同分支间切换时, 工作目录里的文件 __经常会被替换和删除__. 所有历史信息都保存在 'Git目录'中 ;　工作目录只用来临时保存签出(checkout) 文件的地方, 你可以编辑工作目录的文件直到下次提交(commit)为止.

'Git目录' 一般就是指项目根目录下的'.git'目录.


Git索引
=======
Git索引是一个在你的工作目录和项目仓库间的暂存区(staging area). 有了它, 你可以把许多内容的修改一起提交(commit). 如果你创建了一个提交(commit), 那么提交的是当前索引(index)里的内容, 而不是工作目录中的内容.

使用 `git status` 命令是查看索引内容的最简单办法. 你运行 git status命令, 就可以看到: 哪些文件被暂存了(就是在你的Git索引中), 哪些文件被修改了但是没有暂存, 还有哪些文件没有被跟踪(untracked).

<!--language: shell-->

    $>git status

    # On branch master
    # Your branch is behind 'origin/master' by 11 commits, and can be fast-forwarded.
    #
    # Changes to be committed:
    #   (use "git reset HEAD <file>..." to unstage)
    #
    #   modified:   daemon.c
    #
    # Changed but not updated:
    #   (use "git add <file>..." to update what will be committed)
    #
    #   modified:   grep.c
    #   modified:   grep.h
    #
    # Untracked files:
    #   (use "git add <file>..." to include in what will be committed)
    #
    #   blametree
    #   blametree-init
    #   git-gui/git-citool


Git配置
========

使用Git的第一件事就是设置你的名字和email,这些就是你在提交commit时的签名。

<!--language: shell-->

    $ git config --global user.name "Scott Chacon"
    $ git config --global user.email "schacon@gmail.com"

执行了上面的命令后,会在你的主目录(home directory)建立一个叫 ~/.gitconfig 的文件. 内容一般像下面这样:

<!--language: plain-->

    [user]
            name = Scott Chacon
            email = schacon@gmail.com


这样的设置是全局设置,会影响此用户建立的每个项目.

如果你想使项目里的某个值与前面的全局设置有区别(例如把私人邮箱地址改为工作邮箱);你可以在项目中使用git config 命令不带 --global 选项来设置. 这会在你项目目录下的 .git/config 文件增加一节[user]内容


获得一个Git仓库
===============

既然我们现在把一切都设置好了，那么我们需要一个Git仓库。有两种方法可以得到它：一种是从已有的Git仓库中　clone (克隆，复制)；还有一种是新建一个仓库，把未进行版本控制的文件进行版本控制。

## Clone一个仓库

为了得一个项目的拷贝(copy),我们需要知道这个项目仓库的地址(Git URL). Git能在许多协议下使用，所以Git URL可能以ssh://, http(s)://, git://,或是只是以一个用户名（git 会认为这是一个ssh 地址）为前辍. 有些仓库可以通过不只一种协议来访问，例如，Git本身的源代码你既可以用 git:// 协议来访问：

<!--language: shell-->

    git clone git://git.kernel.org/pub/scm/git/git.git

也可以通过http 协议来访问:

<!--language: shell-->

    git clone http://www.kernel.org/pub/scm/git/git.git

git://协议较为快速和有效,但是有时必须使用http协议,比如你公司的防火墙阻止了你的非http访问请求.如果你执行了上面两行命令中的任意一个,你会看到一个新目录: 'git',它包含所有的Git源代码和历史记录.

## 初始化一个新的仓库

现在假设有一个叫”project.tar.gz”的压缩文件里包含了你的一些文件，你可以用下面的命令让它置于Git的版本控制管理之下.

<!--language: shell-->

    $ tar xzf project.tar.gz
    $ cd project
    $ git init

Git会输出:

<!--language: plain-->

    Initialized empty Git repository in .git/

如果你仔细观查会发现project目录下会有一个名叫”.git” 的目录被创建，这意味着一个仓库被初始化了。

正常的工作流程
=================

修改文件，将它们更新的内容添加到索引中.

<!--language: shell-->

    $ git add file1 file2 file3

你现在为commit做好了准备，你可以使用 `git diff` 命令再加上 `--cached` 参数 ,看看哪些文件将被提交(commit)。

<!--language: shell-->

    $ git diff --cached

(如果没有--cached参数，git diff 会显示当前你所有已做的但没有加入到索引里的修改.) 你也可以用`git status`命令来获得当前项目的一个状况:

<!--language: shell-->

    $ git status

    # On branch master
    # Changes to be committed:
    #   (use "git reset HEAD <file>..." to unstage)
    #
    #   modified:   file1
    #   modified:   file2
    #   modified:   file3
    #

如果你要做进一步的修改, 那就继续做, 做完后就把新修改的文件加入到索引中. 最后把他们提交：

<!--language: shell-->

    $ git commit

这会提示你输入本次修改的注释，完成后就会记录一个新的项目版本.

除了用git add 命令，我还可以用

<!--language: shell-->

    $ git commit -a

这会自动把所有内容被修改的文件( __不包括新创建__ 的文件)都添加到索引中，并且同时把它们提交。

这里有一个关于写commit注释的技巧和大家分享:commit注释最好以一行短句子作为开头，来简要描述一下这次commit所作的修改(最好不要超过50个字符)；然后空一行再把详细的注释写清楚。这样就可以很方便的用工具把commit注释变成email通知，第一行作为标题，剩下的部分就作email的正文.

__Git跟踪的是内容不是文件__

很多版本控制系统都提供了一个 "add" 命令：告诉系统开始去跟踪某一个文件的改动。但是Git里的 ”add” 命令从某种程度上讲更为简单和强大. git add __不但是用来添加不在版本控制中的新文件，也用于添加已在版本控制中但是刚修改过的文件__; 在这两种情况下, Git都会获得当前文件的快照并且把内容暂存(stage)到索引中，为下一次commit做好准备。

分支与合并@基础
===============

一个Git仓库可以维护很多开发分支。现在我们来创建一个新的叫”experimental”的分支：

<!--language: shell-->

    $ git branch experimental

如果你运行下面这条命令：

<!--language: shell-->

    $ git branch

你会得到当前仓库中存在的所有分支列表：

<!--language: plain-->

      experimental
    * master

“experimental” 分支是你刚才创建的，“master”分支是Git系统默认创建的主分支。星号(“*”)标识了你当工作在哪个分支下，输入：

<!--language: shell-->

    $ git checkout experimental

切换到”experimental”分支，先编辑里面的一个文件，再提交(commit)改动，最后切换回 “master”分支。

<!--language: shell-->

    (edit file)
    $ git commit -a
    $ git checkout master

你现在可以看一下你原来在“experimental”分支下所作的修改还在不在；因为你现在切换回了“master”分支，所以原来那些修改就不存在了。

你现在可以在“master”分支下再作一些不同的修改:

<!--language: shell-->

    (edit file)
    $ git commit -a

这时，两个分支就有了各自不同的修改(diverged)；我们可以通过下面的命令来合并“experimental”和“master”两个分支:

<!--language: shell-->

    $ git merge experimental

如果这个两个分支间的修改没有冲突(conflict), 那么合并就完成了。如有有冲突，输入下面的命令就可以查看当前有哪些文件产生了冲突:

<!--language: shell-->

    $ git diff

当你编辑了有冲突的文件，解决了冲突后就可以提交了：

<!--language: shell-->

    $ git commit -a

提交(commit)了合并的内容后就可查看一下:

<!--language: shell-->

    $ gitk

执行了gitk后会有一个很漂亮的图形的显示项目的历史。

这时你就可以删除掉你的 “experimental” 分支了(如果愿意)：

<!--language: shell-->

    $ git branch -d experimental

git branch -d只能删除那些已经被当前分支的合并的分支. 如果你要强制删除某个分支的话就用git branch –D；下面假设你要强制删除一个叫”crazy-idea”的分支：

<!--language: shell-->

    $ git branch -D crazy-idea

分支是很轻量级且容易的，这样就很容易来尝试它。


