> 2014-11-01

依赖管理
==========
开发者需要自己解决“应用架构”的问题。构建巨型应用当然需要引入模块化和命名空间，但还不得不考虑另外一个问题——内置的 __依赖管理__ 系统。手动维护页面的中script 标签之间的依赖关系根本不可行，代码会变得混乱不堪。

依赖管理系统除了能解决实际的编程复杂度和可维护性的问题，还能解决性能方面的问题（异步延迟加载）。

## CommonJS
当大家开始关注如何将JavaScript应用于服务器端时，引入了很多解决依赖管理问题的建议方法。[SpiderMonkey](http://www.mozilla.org/js/spidermonkey/)和[Rhino](http://www.mozilla.org/rhino/)提供了`load()`函数，但并没有很好的解决命名空间的问题。[Node.js](http://nodejs.org/)提供了`require()`函数，用来加载外部资源文件，Node.js自有的模块系统也使用这种方式来管理。

为了让代码更具可移植性，则亟需引入一个标准解决方案，让所有的JavaScript都能遵照这个标准来实现统一的模块管理系统，这样JavaScript代码库就可以运行在所有的环境中了。KeviniDangoor按照这个思路提出了CommonJS规范。

### 模块的声明
声明CommonJS模块非常简单、直接。命名空间的概念也被融入在内。模块被分隔为不同的文件，通过给`exports`对象添加内容来对外暴露模块的变量和方法，`exports`变量是在解释器中定义好的：

<!--language: js-->

    // maths.js
    exports.per = function(value, total) {
      return( (value / total) * 100 );
    };

    // application.js
    var Maths = require("./maths");
    assertEqual( Maths.per(50, 100), 50 );

要想使用在模块中定义的函数，只需`require()`这个文件即可，同时将运行结果保存在本地变量中。在上面的例子中，maths.js 中暴露的方法都存在于`Maths`变量中，可以看到模块就是命名空间，并且这种代码可以在所有遵循CommonJS规范的JavaScript解释
器中运行，比如[Narwhal](http://narwhaljs.org/)和Node.js。

### 模块和浏览器
那么，如何在 __客户端__ JS中也实现CommonJS呢？因为很多开发者发现在客户端实现的模块加载器和服务器端有所不同，也就是说如果遵照现行版本的CommonJS规范，模块都是以异步的方式加载的。在服务器端可以完美地实现CommonJS规范，但在浏览器端实现CommonJS就不那么容易了，因为它需要阻塞UI并适时地执行刚加载的script脚本（在客户端则需要避免这种情况的出现）。CommonJS团队为此提出了一个规范：[“模块转换格式”](http://wiki.commonjs.org/wiki/Modules/Transport)。这种转换格式将CommonJS的模块包装在一个回调函数中，以便更好地处理客户端的异步加载。

>  在客户端里，为了处理模块依赖关系不得不将模块主逻辑包含在某个回调函数中，加载模块的过程实际是“保存回调”的过程，最后统一处理这些回调函数的执行顺序。作为模块加载器鼻祖的YUILoader便是遵循这种逻辑实现的，并在YUI3中形成了其独具特色的`use()` 和`add()` 模块化编程风格。为了便于理解客户端模块加载器的基本原理，可以参照一个小型的类库[Sandbox.js](https://github.com/jayli/sandbox)，文档中详细讲解了模块加载器的基本原理。

我们对上一个例子做一些改进。将它包装进一个“转换格式”里，以启用异步加载，这样就可以完美支持浏览器了：

<!--language: js-->

    // maths.js
    require.define("maths", function(require, exports){
      exports.per = function(value, total) {
        return( (value / total) * 100 );
      };
    });

    // application.js
    require.define("application", function(require, exports){
      var per = require("./maths").per;
      assertEqual( per(50, 100), 50 );
    }), ["./maths"]); // 给出它的依赖 (maths.js)


这样就可以通过在浏览器中引入模块加载器来管理并执行我们的模块了。这为我们管理代码带来很多便利，我们可以将代码分隔成模块组件，这是做良好的应用架构设计的秘诀之一，同时我们还获得了依赖管理的支持，包括独立的作用域和命名空间。没错，相同的模块可以运行在浏览器、服务器、桌面应用，以及任何支持CommonJS的环境中。


## 模块加载器
为了在客户端使用CommonJS模块，我们需要引入模块加载器类库。当然，可选的库还有很多，每个类库都各有其优缺点。

### AMD/RequireJS
[RequireJS](http://requirejs.org)（[中文API](http://makingmobile.org/docs/tools/requirejs-api-zh/) ，[阮一峰对它的介绍](http://www.ruanyifeng.com/blog/2012/11/require_js.html)，[requirejs2.0相关说明](http://www.cnblogs.com/snandy/archive/2012/06/04/2532997.html)）是Yabble的一个不错的替代品，它是现在最流行的加载器之一。RequireJS对模块加载的看法略有不同，它遵循“ __异步模块定义__ ”（Asynchronous Module Definition，简称[AMD](http://wiki.commonjs.org/wiki/Modules/AsynchronousDefinition)，[whyAMD](http://cyj.me/why-seajs/requirejs/#why-amd)）格式。主要的不同之处在于AMD的API是即时计算依赖关系，而不是延迟计算。实际上，RequireJS完全和CommonJS的模块相互兼容，只是包装转换的写法格式不同。

> 关于AMD规范到底是一种“模块书写格式”（Module Authoring Format）还是一种“转换格式”（Transport Format）一直存在争议

为了加载JavaScript文件，只需将它们的路径传入`require()`函数即可，并指定一个回调函数，当依赖都加载完成后执行这个回调函数：

<!--language: js-->

    <script>
      require(["lib/application", "lib/utils"], function(application, utils) {
        // 加载完成!
      });
    </script>

在这个例子中可以看出，`application` 和`utils`模块是以 __回调参数__ 的形式传入的，而不必使用`require()`函数来获取它们。

你能引用的不光是模块，RequireJS同样支持原始的JavaScript类库以依赖的形式载入，尤其是jQuery和Dojo。其他的类库可以正常工作，但它们不会以参数形式正确地传入所需的回调函数中。然而，任何以依赖形式载入的类库都需要使用模块格式写法：

<!--language: js-->

    require(["lib/jquery.js"], function($) {
      // jQuery 加载完毕
      $("#el").show();
    });

传入`require()`的路径是 __相对于__ 当前文件或模块的路径，除非路径的前缀是`/` 。出于最优化的考虑，RequireJS推荐你将初始脚本加载器放入一个单独的文件中，这个迷你的类库甚至提供了一种快捷的引入方式：`data-main`属性：

<!--language: html-->

    <script data-main="lib/application" src="lib/require.js"></script>

设置script标签的`data-main`属性是告知RequireJS直接调用`require()`，将这个属性值以参数传入`require()`中（来加载主JS模块），在这个例子中，它将会调用`lib/application.js`脚本，它还会加载我们应用所需的其他脚本：

<!--language: js-->

    // lib/application.js
    require(["jquery", "models/asset", "models/user"], function($, Asset, User) {
      //...
    });

我们刚刚讲了加载模块的方式，那么如何定义模块呢？上文已经提到，RequireJS使用一种完全不同的语法来定义模块——不是使用`require.define()`，而是直接使用`define()`函数。因为模块都在不同的文件中，因此无须使用显式的命名。第一个参数是它的依赖，是一个字符串组成的数组，随后的参数是一个回调函数，回调函数中包含实际的模块逻辑，和RequireJS的`require()`函数类似，依赖的内容以参数的形式传给回调函数：

<!--language: js-->

    define(["underscore", "./utils"], function(_, Utils) {
      return({
        size: 10
      })
    });

默认情况下是没有`exports`变量的。如果要从模块中暴露一些变量，只需将数据从函数中返回即可。RequireJS的模块有一个优势，就是它们已经是被包装好的，因此你不必担心为了兼容浏览器还要再去写转换格式。

> 参考todomvc中的[backbone_require版本](http://todomvc.com/dependency-examples/backbone_require/)，[代码打包](../../../data/backbone_require.zip)，更多[require+mvc示例](http://todomvc.com)

> 如果你是脚本库开发者，[条件调用 define()](https://github.com/umdjs/umd)。妙处在于不依靠 AMD 你仍然可以编写你的库，只要可用的时候参与一下就可以了

### Modules/Wrappings
然而，需要注意的是 ，上述API并不兼容CommonJS的模块，即 __不能__ 写一个同时运行在Node.js和浏览器中的模块代码。其实是可以做一些简单的hack来让CommonJS的模块代码运行在客户端，RequireJS为  __CommonJS提供了一个“容错层（Modules/Wrappings）”__ ——只需将现有的模块代码外部用`define()`函数包装一层即可：

<!--language: js-->

    define(function(require, exports) {
      var mod = require("./relative/name");
      exports.value = "exposed";
    });

回调函数的参数必须和这段示例代码中所示的一模一样，用`require`和`exports`。现在你的模块就可以照常使用这些变量了，而不用作任何改动。当 `define(id?, dependencies?, factory)` 只有 `factory` 参数时，`dependencies` 无需开发者提前指定，`define` 会调用 `factory.toString` 方法，通过正则匹配，自动找出需要依赖的模块。

如果在node中使用，需`npm install amdefine`，再引入`define`定义，实现代码在前后端环境中都能正常使用

```js
if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}
```

如果，想编写通用环境下的库，如underscore.js，可模仿如下写法：


> - underscore 1.6.0

<!--language: js-->

    // Export the Underscore object for **Node.js**, with
    // backwards-compatibility for the old `require()` API. If we are in
    // the browser, add `_` as a global object via a string identifier,
    // for Closure Compiler "advanced" mode.
    if (typeof exports !== 'undefined') {
      if (typeof module !== 'undefined' && module.exports) {
        exports = module.exports = _;
      }
      exports._ = _;
    } else {
      root._ = _;
    }

    // AMD registration happens at the end for compatibility with AMD loaders
    // that may not enforce next-turn semantics on modules. Even though general
    // practice for AMD registration is to be anonymous, underscore registers
    // as a named module because, like jQuery, it is a base library that is
    // popular enough to be bundled in a third party lib, but not be part of
    // an AMD load request. Those cases could generate an error when an
    // anonymous define() is called outside of a loader request.
    if (typeof define === 'function' && define.amd) {
      define('underscore', [], function() {
        return _;
      });
    }

> - jquery 2.0.0

<!--language: js-->

    if ( typeof module === "object" && module && typeof module.exports === "object" ) {
      // Expose jQuery as module.exports in loaders that implement the Node
      // module pattern (including browserify). Do not create the global, since
      // the user will be storing it themselves locally, and globals are frowned
      // upon in the Node module world.
      module.exports = jQuery;
    } else {
      // Register as a named AMD module, since jQuery can be concatenated with other
      // files that may use define, but not via a proper concatenation script that
      // understands anonymous AMD modules. A named AMD is safest and most robust
      // way to register. Lowercase jquery is used because AMD module names are
      // derived from file names, and jQuery is normally delivered in a lowercase
      // file name. Do this after creating the global so that if an AMD module wants
      // to call noConflict to hide this version of jQuery, it will work.
      if ( typeof define === "function" && define.amd ) {
        define( "jquery", [], function () { return jQuery; } );
      }
    }

其它参考：

> [CommonJS 的模块系统，AMD 和 Wrappings, 以及 RequireJS](http://www.udpwork.com/item/3978.html)

> [AMD终极揭秘](http://blog.csdn.net/dojotoolkit/article/details/7820321)

> [Browserify：浏览器加载Node.js模块](http://javascript.ruanyifeng.com/tool/browserify.html)

> [volojs](https://github.com/volojs/volo)

### SeaJs
[SeaJs](https://github.com/seajs/seajs)，[Dosc](http://seajs.org/docs/)

[使用SeaJS实现模块化JavaScript开发](http://blog.codinglabs.org/articles/modularized-javascript-with-seajs.html)


## 服务端的配合

### 包装模块
现在我们有了管理模块依赖和命名空间的方法，但仍然有一个问题自始至终都没有解决——每个文件都独占一个HTTP请求。我们依赖的所有模块都从远程加载，尽管是以异步的形式，还是会造成很严重的性能问题——将应用的启动时间推后。

对于需要异步加载的模块，我们手动将模块包装成一种"转换格式"，这种做法看起来也很冗余。可以在服务器端将小文件合并为一个文件输出，这种做法一举两得。这样浏览器只需发起一个HTTP 请求来抓取一个资源文件，就能将所有的模块都载入进来，显然这种做法更高效。使用打包工具也是一种明智的做法，这样就不必随意、无组织地打包模块了，而是静态分析这些文件，然后递归地计算它们的依赖关系。打包工具同样会将不符合要求的模块包装成转换格式，这样就不必手动输入代码了。

很多模块打包工具也支持代码的压缩（minify），以进一步减少请求的体积。实际上，一些工具——比如[rack-modulr](https://github.com/maccman/rack-modulr)和[Transporter](https://github.com/kriszyp/transporter)——已经整合进了Web服务器，当首次处理某个请求时会自动处理模块操作。

<!--language: js-->

    // app/javascripts/utils.js
    exports.sum = function(val1, val2){
      return(val1 + val2);
    };

    // app/javascripts/application.js
    var utils = require("./utils");
    console.log(utils.sum(1, 2));

When the browser requests a module, all its dependencies will be recursively resolved.

<!--language: shell-->

    $ curl "http://localhost:5001/javascripts/application.js"

<!--language: js-->

    (function() {
      require.define({
        'utils': function(require, exports, module) {
          exports.sum = function(val1, val2){
            return(val1 + val2);
          };
        }
      });

      require.ensure(['utils'], function(require) {
        var utils = require("./utils");
        console.log(utils.sum(1, 2));
      });
    })();

[FlyScript](http://www.flyscript.org/)是用PHP编写的一个CommonJS模块包装器，Transporter是基于[JSGI](http://jackjs.org/)服务器的转换器，[Stitch](http://github.com/sstephenson/stitch)则整合了Node.js服务器。

### 模块的按需加载
你可能不想用模块化的方式来写代码，或许因为你现有的代码和库改成用模块化的方法来管理需要作太多改动。幸运的是，还有很多其他的替代方案，比如[Sprockets](http://getsprockets.org)。Sprockets给你的JavaScript代码添加了同步`require()`支持。以`//=`形式书写的注释指令都会被Sprockets作 __预处理__，通知Sprockets来检查类库的加载路径，加载它并以行内形式包含进来：

<!--language: js-->

    //= require <jquery>
    //= require "./states"

在这个例子中，`jquery.js` 是Sprockets所需的加载地址，`states.js`是以相对路径的形式给
出的。Sprockets非常聪明，它会保证只加载库文件一次，自动忽略之后的加载需求。相
比于CommonJS模块，Sprockets支持缓存和压缩（minify）。在开发过程中，你的服务
器会根据页面的需要动态解析并合并文件。当站点的访问非常频繁时，JavaScript文件可以被预合并，这样就可以作为静态文件来处理，从而提高了性能。

尽管Sprockets是一个基于命令行的工具，但也有一些工具集成了Rack和Rails，比如[rack-sprockets](https://github.com/sstephenson/sprockets)，甚至还有一些[PHP的实现](https://github.com/stuartloxton/php-sprockets)。Sprockets（包括所有的模块包装器）的中心思想是，所有的JavaScript文件都需要预处理，不管是在服务器端用程序作处理，还是使用命令行工具作处理。

> 参考rails [assets静态文件](http://ihower.tw/rails3/assets-and-ajax.html)


项目构建
========
## 代码质量
### 代码规范
[Google JavaScript Style Guide](http://google-styleguide.googlecode.com/svn/trunk/javascriptguide.xml)，[中文](../../../data/Google-JavaScript-Style-Guide.htm)

### JSLint
[JSLint](www.jslint.com)，JavaScript 作为一门年轻、语法灵活多变且对格式要求相对松散的语言，代码格式的混乱和某些语言特性的不正确使用，往往使得最终交付的产品中包含许多因编码风格约定造成的未预见的行为或错误，这种习惯性的问题如果不及时指出并修改，往往会在项目的迭代过程中不断的重现，严重影响 Web 产品的稳定性与安全性。JSLint 正是为解决此类问题创建的工具，JSLint 除了能指出这些不合理的约定，还能标出结构方面的问题。虽然 JSLint 不能保证代码逻辑一定正确，但却有助于发现错误并教会开发人员一些好的编码实践，更加注重静态代码格式的检测。

### 其它工具
[JSHint](http://www.jshint.com/docs/)

[SublimeLinter](https://github.com/SublimeLinter/SublimeLinter-for-ST2‎)

## 测试
### 单元测试
手工测试更像集成测试，从更高层次上保证应用的正常运行。单元测试则是更低层次的测试，确保特定的后台代码片段能正常运行。

单元测试的另一个优势是为 __自动化测试__ 铺平道路。将很多单元测试整合起来就可以做到连续的集成测试了——每次代码有更新时都重新执行一遍所有的单元测试。这要比对应用做手动 __回归测试__ 省时省力得多，并可确保每一处代码的小改动都不会影响到应用中其他的功能。

#### Qunit
[Qunit](http://docs.jquery.com/Qunit)是现在最流行且维护良好的测试类库，这个库最初是用来测试jQuery用的。

#### Jasmine
[Jasmine](http://jasmine.github.io/)是另一个非常流行的测试类库，和QUnit不同，Jasmine定义了用以描述应用中特定对象的行为的测试片段（spec）。实际上这些片段和单元测试非常类似，只不过换了一种表述方式。

### 驱动测试
尽管使用测试框架可以做到一定程度的自动化测试，但在各式各样的浏览器中进行测试依然是个问题。每次测试时都要开发者手动在五个浏览器中执行刷新，这种做法显然很低效。

驱动实际上是一个守护进程，它整合了不同的浏览器，可以自动运行JavaScript测试代码，测试不通过时会给出提示。有一个单独的持续集成服务器，利用post-commit的hook功能（SVN的一个事件）来自动运行JavaScript测试代码，确保每次提交的代码都是正确无误的。

[Watir](http://watir.com)，[Selenium](http://seleniumhq.org)等工具可做UI方面的测试，并可用于驱动测试

### 无界面测试
现在越来越多的人开始在服务器端（比如基于Node.js或Rhino）编写JavaScript程序，这时就需要在脱离浏览器环境的命令行中运行你的测试代码。这种做法的优势是命令行环境速度快而且易于安装，同时不用涉及多浏览器及持续集成服务器环境。它的不足也很明显，就是测试代码无法在真实环境中运行。

这听起来不像是太严重的问题，因为你会发现你写的大多数JavaScript代码都是应用逻辑，是 __不依赖于浏览器__ 的（而jQuery才更多的是处理DOM和Event的浏览器兼容性问题）。


#### PhantomJS
[PhantomJS](http://phantomjs.org/)是一个基于WebKit的服务器端 JavaScript API。提供一个浏览器环境的命令行接口。它全面支持web而不需浏览器支持，其快速，原生支持各种Web标准： DOM 处理，CSS 选择器，JSON，Canvas，和 SVG。PhantomJS可以用于页面自动化，网络监测，网页截屏，以及无界面测试等。[Quick-Start](https://github.com/ariya/phantomjs/wiki/Quick-Start)，[Examples](https://github.com/ariya/phantomjs/wiki/Examples)

#### 其它工具
[Envjs](http://www.envjs.com)是JohnResig开发的一个类库，这个类库在Rhino环境中实现了浏览器DOM API，Rhino是Mozilla用Java实现的JavaScript引擎。你可以使用Rhino和env.js在命令行下执行JavaScript测试。

[Zombie.js](http://zombie.labnotes.org)是一个无界面的JavaScript类库，专门为Node.js设计，充分利用了它的高性能和异步特性。主要特点是速度快，花在等待测试执行上的时间越少，用在实现新功能和修复bug的时间就越多。

当需要一个异步测试框架，可使用[Vows.js](http://vowsjs.org/)，这是一个非常优秀的框架，它可以识别出哪些测试需要并行执行，哪些测试需要串行执行。

[Ichabod](http://github.com/maccman/ichabod)是另一个用于无界面测试的类库，除了能模拟DOM和解析引擎之外，Ichabod的优势还在于它使用了Webkit解析引擎，这也是Safari和Chrome浏览器所使用的解析引擎。但它的缺点是只能运行在OS X中，因为它需要MacRuby和OSX WebView API的支持。

### 分布式测试
跨浏览器测试的一个解决方案是利用外包的专用服务器集群，这些集群都安装有不同的浏览器。这正是[TestSwarm](http://swarm.jquery.org)的做法，它简化在多个浏览器中执行繁琐、耗时的JavaScript测试用例的工作。它为你的JavaScript项目提供了一个持续集成的工作流，并带有必要的测试工具。

TestSwarm并不是通过向浏览器集成一些插件和扩展来实现，这种做法非常低端，而是选择了另外一种思路。浏览器在TestSwarm的终端里运行，并自动执行推送给它们的测试。它们可以部署在任意机器、任意操作系统中，TestSwarm会自动将得到的测试地址传递给一个新打开的浏览器。

## 性能
### JSLitmus
[JSLitmus](http://www.broofa.com/Tools/JSLitmus/)是一个轻量级的工具，用于创建针对性的JavaScript基准测试工具，这个性能基准测试工具可以看到

- 使用内存的情况
- 多长时间内执行了多少次

### console.profile
Web Inspector和Firebug都包含了检查程序运行效率和时间的工具，它们可以帮助你更精确地把控程序的性能。只要在你想统计的代码段两端加上`console.profile()`和`console.profileEnd()`即可当调用到`profileEnd()`时，控制台就会创建一个报表，将期间所有的函数调用都统计出来，包括每次调用花费的时间及调用次数

你也可以使用调试器Profile的record特性，它的功能和直接嵌入console语句是一样的。通过查看哪些函数被调用了及哪些函数耗费了更长的时间，可以发现代码中的性能瓶颈。

也可以使用Profile的快照（snapshot）功能生成页面当前的堆（heap）的快照，显示了当前使用了多少对象，占用了多少内存。这是查找内存泄漏的好方法，因为你可以看到哪些对象被无意间存储在内存中，应当被回收而未被回收。

### console.time
需给要统计时间的代码前后加上`console.time(name)`和`console.timeEnd(name)`即可，当执行到`timeEnd()`时，它们之间的代码执行时间就会以毫秒为单位发送给控制台，以log的形式输出。使用控制台的时间统计API，可以将性能测试也加入到你的测试代码中，以保证你的代码不会出现性能瓶颈，从而从整体上保证应用的良好用户体验。

## 部署
### 源码压缩
JavaScript源码压缩是从脚本文件中删除不必要的字符，它不改变功能。删除的字符包括空白、换行和注释。更好的压缩工具应该能够翻译JavaScript，因此能安全地缩短变量和函数的名字，这样就进一步减少了字符。文件越小越好，因为在网络上传输的数据越少越好。

主要工具有：
[YUI Compressor](http://developer.yahoo.com/yui/compressor)，
[Closure Compiler](https://developers.google.com/closure/compiler)，
[UglifyJS](https://github.com/mishoo/UglifyJS)

但压缩后的代码不容易被debug，你看着报错信息，感到毫无头绪，根本不知道它所对应的原始位置。

这就是[Source map](http://www.ruanyifeng.com/blog/2013/01/javascript_source_map.html)想要解决的问题，简单说，Source map就是一个信息文件，里面储存着位置信息。也就是说，转换后的代码的每一个位置，所对应的转换前的位置。有了它，出错的时候，除错工具将直接显示原始代码，而不是转换后的代码。这无疑给开发者带来了很大方便。

上述工具后两者均支持Source map。

### 构建工具
合并压缩js/css文件、单元测试、包的依赖管理、版本控制、文档化等工作如果都用手工来完成，将是一件烦杂的事情，而构建工具是将这些任务交由相应的工具来完成

####Rake
与其说[Rake](http://rake.rubyforge.org/)是一个代码构建工具，不如说Rake是一个任务管理工具，通过Rake我们可以得到两个好处：

- 以任务的方式创建和运行脚本
- 追踪和管理任务之间的依赖

如underscore.js 1.4.0中的rakefile：

```rb
require 'rubygems'
require 'uglifier'

desc "Use the Closure Compiler to compress Underscore.js"
task :build do
  source  = File.read('underscore.js')
  min     = Uglifier.compile(source)
  File.open('underscore-min.js', 'w') {|f| f.write min }
end

desc "Build the docco documentation"
task :doc do
  sh "docco underscore.js"
end

desc 'Run tests'
task :test do
  pid = spawn('bundle exec serve', err: '/dev/null')
  sleep 2

  puts "Running Underscore test suite."
  result2 = system %{phantomjs ./test/run-qunit.js "http://localhost:4000/test/test_underscore/test.html"}

  Process.kill 'INT', pid
  exit(result1 && result2 ? 0 : 1)
end

```

#### Grunt
[Grunt](http://www.gruntjs.org/)，是基于Node.js的自动化任务运行器。Grunt.js结合NPM的包依赖管理，完全可以媲美Maven。Grunt.js天然适合前端应用程序的构建——不仅限于JavaScript项目，同样可以用于其他语言的应用程序构建。越来越多的JavaScript项目已经在使用Grunt，其中最大的使用者包括著名的jQuery项目。

Grunt没有像Maven那样强调构建的生命周期，各种任务的执行顺序可以随意配置。Grunt本身仅是一个执行器，大量的功能都存在于NPM管理的插件中。特别是以grunt-contrib-开头的核心插件，覆盖了大部分的核心功能，比如`less`，`compass`，`jshint`，`concat`，`minify`，`uglify`等。

通过提供通用的接口以进行代码规范检验（Lint）、合并、压缩、测试及版本控制等任务，Grunt使入门门槛大大降低了。

>  使用Grunt时，需要用到npm，并需要注意package.json的书写，[nmp参考](http://www.cnblogs.com/chyingp/p/npm.html)

一个示例：

package.json编写

```js
{
  "name": "Bejs",
  "version": "0.1.0",
  "devDependencies": {
    "grunt": "~0.4.0",
    "grunt-contrib-uglify": "~0.1.2",
    "grunt-contrib-concat": "~0.1.1",
    "grunt-css":   ">0.0.0"
  }
}
```

shell中运行`npm install`

Gruntfile.js编写

```js
module.exports = function(grunt) {
  grunt.initConfig({
    pkg : grunt.file.readJSON('package.json'),
    concat : {
      domop : {
        src: ['src/ajax.js', 'src/selector.js'],
        dest: 'dest/domop.js'
      },
      css :{
        src: ['src/asset/*.css'],
        dest: 'dest/asset/all.css'
      }
    },
    uglify : {
      options : {
        banner : '/*! <%= pkg.name %> <%= grunt.template.today("yyyy-mm-dd") %> */\n',
        sourceMap:'dest/domop.min.js.map',
        sourceMappingURL: 'domop.min.js.map'
      },
      build : {
        src : 'dest/domop.js',
        dest : 'dest/domop.min.js'
      }
    },
    cssmin: {
        css: {
            src: 'dest/asset/all.css',
            dest: 'dest/asset/all-min.css'
        }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-css');

  grunt.registerTask('default', ['concat', 'uglify', 'cssmin']);
};
```

shell中运行`grunt`

#### package.json中scripts
通过package.json中的scripts来运行简单的shell脚本，达到轻量级的自动构建

underscore.js 1.6.0中的package.json

```js
{
  "name"          : "underscore",
  "description"   : "JavaScript's functional programming helper library.",
  "homepage"      : "http://underscorejs.org",
  "keywords"      : ["util", "functional", "server", "client", "browser"],
  "author"        : "Jeremy Ashkenas <jeremy@documentcloud.org>",
  "repository"    : {"type": "git", "url": "git://github.com/jashkenas/underscore.git"},
  "main"          : "underscore.js",
  "version"       : "1.6.0",
  "devDependencies": {
    "docco": "0.6.x",
    "phantomjs": "1.9.0-1",
    "uglify-js": "2.4.x"
  },
  "scripts": {
    "test": "phantomjs test/vendor/runner.js test/index.html?noglobals=true",
    "build": "uglifyjs underscore.js -c \"evaluate=false\" --comments \"/    .*/\" -m --source-map underscore-min.map -o underscore-min.js",
    "doc": "docco underscore.js"
  },
  "licenses": [
    {
      "type": "MIT",
      "url": "https://raw.github.com/jashkenas/underscore/master/LICENSE"
    }
  ],
  "files"         : ["underscore.js", "underscore-min.js", "LICENSE"]
}
```

shell中运行`npm install`，再运行`npm test`，`npm run-script build`，`npm run-script doc`
