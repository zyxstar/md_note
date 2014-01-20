概述
====

本工具主要是为了方便程序开发人员作笔记，能在文档中 __直接运行代码__ 小片断，从而提高笔记的效果。又因为文档的原始文件为纯文本格式，非常适合做 __版本管理__。

快速使用
========
## 环境搭建
### python(必选)
- 建议使用版本2.7.x，windows环境下可使用[activepython](http://www.activestate.com/activepython/downloads)
- 在cmd中进入Python27\Scripts\目录，运行`easy_install markdown`安装python的Markdown模块

### java(可选)
- 如果仅需要制作javascript相关的笔记，则不需要安装java环境，但 __如果需要运行其它语言时，则java环境是必装的__，因为运行其它语言的机制是，通过java编写的applet来调用本地的语言解析器，并将运行结果返回至applet，再显示到页面上。
- 安装完了java后，还需要在浏览器上启用java，如chrome，在地址栏输入`chrome://plugins/`，查看`Java(TM)`是否启动
- 打开 控制面板 - Java (32bit) - Security，设置为Medium
- 在Java\jre7\lib\security\java.policy文件(请确定该java版本是浏览器使用的版本)里最后加上`permission java.security.AllPermission;`，修改该文件时，修改者首先需要具备可修改的权限(以上权限的修改，由可能引起安全隐患，使用者可在运行完文档后，酌情还原)
- 如需要重新生成applet，[查看](../applet/ReadMe.html)，这种情况并不多见

### 环境变量(可选)
- 语言的编译器(解析器)工具需要在环境变量中配置，如python.exe、ruby.exe、java.exe、javac.exe、csc.exe等 __所在的目录__ 需要添加到环境变量PATH中

## 创建文档
以创建xx文件为例，在`md_note/doc`中创建`xx.md`文件

## 编写文档
主要使用[Markdown](http://zh.wikipedia.org/wiki/Markdown)来编写文档

## 查看文档

- __本地模式__：运行`md_note/runshell.bat`进入shell命令行，输入`.\rundoc.bat .\doc\xx.md`即打开 __本地file:///请求__ 的`.doc\xx.html`文件<br/>
- __Http模式__：运行`md_note/runhttp.bat`，启动python简易http的server，再在运行`md_note/runshell.bat`进入shell命令行，输入`.\rundoc.bat .\doc\xx.md http`即打开 __基于http://请求的__ `http://localhost:8000/doc/xx.html`　
- 以上述方式查看文档，默认是滚动到文件尾部，因为一般情况下文件尾部是作者刚编写的部分，如果想回到文件首部，需点击目录右下侧`Top`按钮。

扩展特性
========
## 可缩放的层级目录
生成的html文件，会自动根据Markdown中标题(h1~h6)生成层级目录，并支持目录的展开与收缩

![生成的目录](../imgs/manual_toc.png)

## 支持table编写

原生的Markdown需要描述table时，只能使用html来编写，十分不便，于是遵循
[Adam Pritchard的Markdown-Cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#wiki-tables)中的table规则：

- 支持列的对齐方式
- table中支持内联的Markdown语法

并在编写时，在首行(不缩进)写上`<!-- language: table -->`，接着空一行，并需要将整个table缩进4个字符

编写规则如下：

![table编写规则](../imgs/manual_table_rule.png)

生成效果如下(你见到的效果就是生成出来的)：

<!-- language: table -->

    |              | 类型        | 直接量声明       | 包装对象 |
    |-------------:|-------------|:-----------------|:--------:|
    | **基本类型** | *undefined* | `v=undefined`    | ~~无~~   |
    | **基本类型** | *string*    | `v='..';v=".."`  | String   |
    | **基本类型** | _number_    | `v=1234 `        | Number   |
    | **基本类型** | _boolean_   | `v=true;v=false` | Boolean  |
    | **基本类型** | _function_  | `v=function(){}` | Function |
    | __对象__     | _object_    | `v={..};v=null`  | Object   |
    | __对象__     | _regex_     | `v=/.../..`      | RegExp   |
    | __对象__     | _array_     | `v=[...]`        | [__Array__](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)    |

## 代码语法高亮
依赖[syntaxhighlighter](http://alexgorbatchev.com/SyntaxHighlighter/)，支持绝大多数的语法高亮，使用时，首行(不缩进)加`<!-- language: «brush» -->`，其中`«brush»`为[syntaxhighlighter brush](http://alexgorbatchev.com/SyntaxHighlighter/manual/brushes/)中所定义的brushes，接着空一行，再编写代码，所有代码缩进4个字符。

> ps: `<!-- language: «brush» -->`的想法[参考](http://stackoverflow.com/editing-help#syntax-highlighting)，通过html的comment来标识语法，对于Markdown的解析无侵入性。<br/>
已知的BUG，在代码注释中，不要使用`'`与`"`，否则容易将注释中引号与代码中的引号相匹配；不要在注释中使用html标签，否则生成的高亮代码块中会产生该标签，影响代码阅读

如css需高亮，则在首行加`<!-- language: css -->`(Manual.md中为编写规则，Manual.html中为展示效果)：

<!-- language: css -->

    ul.TOCEntry {
        font-family: sans-serif;
        margin: 0;
    }

    ul.TOCEntry li {
        list-style: none;
        display: block;
        margin-left: -20px;
        font-size: 1.1em;
    }

## 代码即时运行
使用时，与代码高亮一样，只是首行为`<!-- language: !«brush» -->`，其中的`!`代表是可执行的。目前支持以下几种语言，以下示例需先将环境配置好([参考](#TOC2.1.2))，所有示例中，Manual.md中为编写规则，Manual.html中为展示效果：

> ps: 如果以firefox打开时，请在http模式下运行，否则会因权限受限不能正常运行

### javascript
首行加`<!-- language: !js -->`或`<!-- language: !javascript -->`，

<!-- language: !js -->

    function MyClass(name) {
        this.name = name;
        this.say = function() {
            return "hello, " + name;
        };
    }
    var m = new MyClass("javascript");
    alert(m.say());

使用js库，在代码的首部使用`//import «lib.version»`的方式引入，库的代码存放在`/vendor`文件夹中，如`//import jquery.1.9.0`，将引用`/vendor/jquery/1.9.0/jquery.js`文件，如果没有版本时，如`//import json2`，将引用`/vendor/json2/json2.js`文件

<!-- language: !js -->

    //import jquery.1.9.0
    $(function(){
        alert(typeof jQuery);
    });

### web
首行加`<!-- language: web -->`，并且在每部分(html,css,js)代码起始前加相应的`comment`，如图：

![web编写规则](../imgs/manual_web_rule.png web编写规则)

<!-- language: web -->

    <!-- language: html -->
    <div>
        <input id='txt_say' type='text'/>
        <input type='button' value='Say'/>
    </div>

    <!-- language: css -->
    @import url(../css/normalize.css);
    body{
        padding:20px;
    }
    input[type='button']{
        padding:3px 5px;
        font-size:14px;
        font-weight:bold;
        background-color:black;
        color:white;
    }

    <!-- language: js -->
    //import jquery.1.9.0
    $(function(){
        $("#txt_say").val("hello, web")
        $("input[type='button']").click(function(){
            alert($("#txt_say").val())
        });
    })


### python
首行加`<!-- language: !py -->`或`<!-- language: !python -->`

<!-- language: !py -->

    #coding:utf-8
    class MyClass(object):
        def __init__(self, name):
            self.name = name

        def say(self):
            return "hello, %s" % self.name

    m = MyClass("python")
    print m.say()

### ruby
首行加`<!-- language: !rb -->`或`<!-- language: !ruby -->`

<!-- language: !rb -->

    #coding:utf-8
    class MyClass
        def initialize(name)
            @name = name
        end

        def say
            return "hello, #{@name}"
        end
    end

    m = MyClass.new "ruby"
    puts m.say

### csharp
首行加`<!-- language: !c# -->`或`<!-- language: !csharp -->`

<!-- language: !csharp -->

    using System;
    using System.Collections.Generic;
    using System.Text;

    class Program
    {
        static void Main(string[] args)
        {
            Console.Write(new MyClass("csharp").say());
        }

        public class MyClass
        {
            private string name;
            public MyClass(string name)
            {
                this.name = name;
            }
            public string say()
            {
                return string.Format("hello, {0}", name);
            }
        }
    }

### java
首行加`<!-- language: !java -->`

<!-- language: !java -->

    import java.io.*;
    import java.text.*;
    public class Program {
        public static void main(String[] args) {
            try {
                System.out.print(new Program().new MyClass("java").say());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public class MyClass {
            private String name;
            public MyClass(String name){
                this.name=name;
            }
            public String say(){
                return MessageFormat.format("hello, {0}", name);
            }
        }
    }

## 外链的样式与打开
根据链接的host，判断是否是外链，如果是，则以特殊样式 ([示例](http://www.baidu.com))表示，并在点击链接时，在新窗口打开

## 特别强调的样式
Markdown中`特别强调`的表示方式如下：

<!-- language: plain -->

    ***特别强调*** 或者 ___特别强调___

默认情况下样式是<i><b>粗斜体</b></i>，现在表现为 ___特别强调___

> ps: 只是修改一个样式而已，不值一提，只为提醒本人

## 浏览器支持
- Chrome
- Firefox
- IE9+

文件结构
=========
- applet文件夹：applet的源代码及applet的配置说明
- css文件夹：存放样式
- doc文件夹：文档存放位置，一般情况情况下，用户只需使用该目录即可
- font文件夹：字体存放位置
- imgs文件夹：文档中图片存放位置
- js文件夹：存放本工具所使用的javascript文件
- popup文件夹：运行代码所需的弹出页面
- scripts文件夹：该工具使用的相关脚本，用以生成html
- vendor文件夹：第三方插件存放位置，如syntaxhighlighter，以及存放js常用库，如jquery
- rundoc.bat脚本：用来生成文档
- runhttp.bat脚本：用来启动http服务
- runshell.bat脚本：用来启动powershell，并使md_note为当前目录

后续计划
=========
- <del>支持在线编辑器，并有语法高亮与即时运行</del>
- <del>提供整个web项目demo的制作，例如[jsfiddle](http://jsfiddle.net/)或[dabblet](http://dabblet.com/)，但侧重点不同，以文档为主，代码为辅，主要应用于教程的制作</del>
- <del>支持更多语言的运行</del>
- 代码运行的实现，使用[compileonline](http://www.compileonline.com)代替applet
- markdown生成使用js代码代替python脚本
- 将项目构建在GAE与Github上，去掉本地相关脚本
- web运行页面支持coffeescript,sass,less等


知者行之始，行者知之成
行之明觉精察处便是知，知之真切笃实处便是行

史笔是非空自许，世情真伪后谁知
世事洞明皆学问，人情练达即文章

罗织经
败经
观人经
AngularJS


时间管理
思维　思考　
见识
行动 不要再拖延
心理　人性

知名网站、名人微博
发散思维　洞察力　批判性
规划
阿里巴巴　黄页

zepto senchatouch tianium phonegap
webkit

头撇上位千，百万二撇前，三撇前面是十亿，兆在4撇前


易中天先生谈逻辑
　　韩寒说，世界上有两种逻辑。一种叫逻辑，另一种叫“中国逻辑”。确实如此。比如南方周末发表王彬彬的文章，质疑汪晖涉嫌剽窃，马上就有人说王彬彬态度不好。其实，汪晖抄没抄，是一个“事实判断”。跟王彬彬的态度，又有什么关系？如果抄了，王彬彬态度再好，他也是抄了。如果没抄，王彬彬态度再不好，也没抄，怎么连这个都搞不清楚呢？这就是典型的中国逻辑：问态度，不问事实。

　　第二，问动机，不问是非。动机是第一重要的，事实和是非则可以不顾。比如一个贪官为了保命，揭发另一个贪官。难道因为他“动机不纯”，纪检委就不查了？当然，一个人的错误，如果是“无心之过”，应该原谅。但这总得在认定事实之后嘛。

　　第三，问亲疏，不问道理。只要是自家人，说什么都是对的。如果是自己仇恨、反对的人，说啥都不对。他不是反对你的意见，是反对你这个人。哪怕你跟他意见一致，他也要从动机、态度等方面，找你的茬。

　　现在最喜欢讲“中国逻辑”的，主要是三种人。

第 一种是“揣着明白装糊涂”，故意把水搅混。这种人，我称之为“乌贼”；

第二种是“疯狗”，逮谁咬谁；

第三就是“脑残”，人数最多。

乌贼是使坏的，疯狗是咬人的，脑残是起哄的。共同特点，是不讲事实，不讲逻辑，不讲道理。





建筑的结构决定功能
流程解耦才能模块解耦



任何选项和规则都是矛盾的，因为规则趋于动态，而选项就是静态，他们必然矛盾，可贵而神奇的是，规则最终的选择还是源自这些选项之一


如何让客户理解软件开发过程中的辛酸以及承认软件的价值。

我们经常能看到谷歌发布一些文章或者是视频，介绍他们储存多大的数据量，涉及到全球多么广泛的人群，应对多么密集的请求响应，搞定了多么精准的广告投放等等等等。后来百度也这么干，说自己中文搜索多么地道，顺带也说自己存储量多么多么大，百度贴吧多么话题涉及范围广等等等等。后来阿里巴巴也这么干，说自己每秒处理多少笔业务，据说还在自己的呼叫中心安排一个墙体大屏幕，显示一张中国地图，哪个地方发生一笔交易就在哪个地方弹出一个彩色气球，随着气球的沸腾，显示交易多么的热闹等等等等。

你需要不断的刷存在感，在用户界面和交互的范畴内，尽可能详实的展现自己的工作，不要以为这是作秀，你要知道，用户就吃这一套。然后，当真的这么做的时候，你会惊奇的发现，利用数据来概括这个存在感是最完美的！如果你处理的量很大，你就可以适当的夸大和模糊的炫耀一下！如果你处理的信息来源繁杂，就用快速闪动的方式，让用户用眼睛感受那种飞驰的快感！

如果满世界发小样，那么这个企业也就倒闭了！所以，一个迫在眉睫的问题是：向谁发放，发放哪种产品，多少剂量，什么时间去发放！　　不兜圈子了，这是数据挖掘！并需要辅以小范围的统计试验！采集大量的数据，积累大量的经验，才能有效的搞定这个尺寸的把握！这涉及很繁缛的细节，各位自行百度！让我们继续讨论。