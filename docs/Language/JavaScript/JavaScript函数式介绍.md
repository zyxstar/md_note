> 2014-11-01

- [函数式思维: 为什么函数式编程越来越受关注](http://www.ibm.com/developerworks/cn/java/j-ft20/)
- [coolshell上函数式编程](http://coolshell.cn/articles/10822.html)

高阶函数
=========

在数学和计算机科学中，__高阶函数__(high-order function, HOF)是至少满足下列一个条件的函数：

> - 接受一个或多个函数作为输入
> - 输出一个函数

js中函数第一公民，高阶函数自然支持，甚至下面的其他特性，都离不开它的支持。

不存在Haskell与Python中的列表解析，但`Array.protype`中的函数对集合处理还是很方便的

<!--language: !js-->

    alert([1, 2, 3].map(function(num){ return num*2; }));


js原生不支持lambda，但借助一些库（如[Functional Javascript](http://osteele.com/sources/javascript/functional/)，[lambda.js](http://www.javascriptoo.com/lambda-js)）可实现相关功能，或直接使用[CoffeScript](http://coffeescript.org/)

## map与reduce
以下概念对mongodb等分布式并发NoSql具有指导意义，对于js编程可作为参考

指定一个Map（映射）函数，用来把一组键值对映射成一组新的键值对，指定并发的Reduce（化简）函数，用来保证所有映射的键值对中的每一个共享相同的键组

简单来说，一个映射函数就是对一些独立元素组成的概念上的列表（例如，一个测试成绩的列表）的每一个元素进行指定的操作（比如，有人发现所有学生的成绩都被高估了一分，他可以定义一个“减一”的映射函数，用来修正这个错误。）。事实上，每个元素都是被独立操作的，而原始列表没有被更改，因为这里创建了一个新的列表来保存新的答案。这就是说，Map操作是可以高度并行的（js中并非并行），这对高性能要求的应用以及并行计算领域的需求非常有用。

而化简操作指的是对一个列表的元素进行适当的合并（继续看前面的例子，如果有人想知道班级的平均分该怎么做？他可以定义一个化简函数，通过让列表中的奇数（odd）或偶数（even）元素跟自己的相邻的元素相加的方式把列表减半，如此递归运算直到列表只剩下一个元素，然后用这个元素除以人数，就得到了平均分）。虽然他不如映射函数那么并行，但是因为化简总是有一个简单的答案，大规模的运算相对独立，所以化简函数在高度并行环境下也很有用。

## 自定义控制结构
在支持函数式编程的语言里，我们能自定义类似`if`、`while`、`for`的控制语句（高阶函数），函数作为第一类型的作用就是允许自定义控制语句、改造语言，让程序员能在更高的层次上做开发，而不是仅仅使用语言本身提供的低级的控制语句。比如Java语言通过不断地改进语法，引入了`foreach（for (xx : xx)）`、`with_open_file（try (resources)）`等语法，但如果Java最先引入`Lambda`，或许就不用添加这些语法糖了，因为程序员早就自己实现了！

比如下面的代码，需要将重复的地方给提炼出来：

```js
function is_collision(posx, posy, vec) {
    ret.push(vec);
    return !!me.board.getPiece(new Pos(posx,posy));
}

var topArr = utils.range(this.pos.y, Board.ROWS);
for (i = 0, len = topArr.length; i < len; i++) {
    if (is_collision(this.pos.x, topArr[i], [0, i]))
        break;
}

var rightArr = utils.range(this.pos.x, Board.COLS);
for (i = 0, len = rightArr.length; i < len; i++) {
    if (is_collision(rightArr[i], this.pos.y, [i, 0]))
        break;
}
```

第一个可以想到的就是把变化部分作为 __参数__ 给提炼出来，但在传递给`is_collision`时的参数，用到了数组和下标，所以参数还必须是回调函数：

```js
function is_collision(posx, posy, vec) {
    ret.push(vec);
    return !!me.board.getPiece(new Pos(posx,posy));
}

function process (rangestart, rangeend, fnPosx, fnPosy, fnVec) {
    var arr = utils.range(rangestart, rangeend);
    for (i = 0, len = arr.length; i < len; i++) {
        if (is_collision(fnPosx(arr,i), fnPosy(arr,i), fnVec(i)))
            break;
    }
}

process(this.pos.y, Board.ROWS,
    function (arr,i) {return this.pos.x;},
    function (arr,i) {return arr[i];},
    function (i) {return [0,i];}
});

process(this.pos.x, Board.COLS,
    function (arr,i) {return arr[i];},
    function (arr,i) {return this.pos.y},
    function (i) {return [i,0];}
});
```

但代码并未精简，反而带来了代码阅读上面的障碍，对提炼的函数以及它的参数如何命名也是个问题（只得命名为无意义的`process`）。

但如果再次审视原始的代码，代码的意图是需要将一个迭代对象进行迭代，遇到一个满足条件`predicate`时，就退出迭代，而可以将这种场景表达成一个控制结构`takeWhile`，这将是一个更通用且能被其他代码复用的结构，且重构后反而能增强代码表达：

```js
utils.takeWhile = function (list, predicate, context) {
    var ret = [];
    for(var i = 0, len = list.length; i < len; i++){
        if (predicate.call(context, list[i], i, list)) break;
        ret.push(list[i]);
    }
    return ret;
};

function is_collision(posx, posy, vec) {
    ret.push(vec);
    return !!me.board.getPiece(new Pos(posx,posy));
}

utils.takeWhile(utils.range(this.pos.y, Board.ROWS),
    function(item, idx, arr){
        return is_collision(this.pos.x, arr[idx], [0, idx]);
    }, this);

utils.takeWhile(utils.range(this.pos.x, Board.COLS),
    function(item, idx, arr){
        return is_collision(arr[idx], this.pos.y, [idx, 0]);
    }, this);
```

模式匹配
=========

原生的js不支持，但基于js的[LiveScript](http://livescript.net/)是支持的

闭包
=========

闭包是代码块和 __创建该代码块的上下文中数据__ 的结合。

## 面向堆栈动态作用域
在面向堆栈的编程语言中，函数的局部变量都是保存在栈上的，每当函数激活的时候，这些变量和函数参数都会压入到该堆栈上。

当函数返回的时候，这些参数又会从栈中移除。这种模型对将函数作为函数式值使用的时候有很大的限制（比方说，作为返回值从父函数中返回）。绝大部分情况下，问题会出现在当函数有自由变量的时候。

<!--language: !js-->

    function testFn() {
      var localVar = 10;

      function innerFn(innerParam) {
        alert(innerParam + localVar);
      }
      return innerFn;
    }

    var someFn = testFn();
    someFn(20); // 30

对于innerFn函数来说，localVar就属于自由变量。

对于采用面向栈模型来存储局部变量的系统而言，就意味着当testFn函数调用结束后，__其局部变量都会从堆栈中移除__。这样一来，当从外部对innerFn进行函数调用的时候，就会发生错误（因为localVar变量已经不存在了）。

而且，上述例子在面向栈实现模型中，要想将innerFn以返回值返回根本是不可能的。因为它也是testFn函数的局部变量，也会随着testFn的返回而 __移除__。

还有一个问题是当系统采用动态作用域，函数作为函数参数使用的时候有关。

<!--language: !js-->

    var z = 10;

    function foo() {
      alert(z);
    }

    foo(); // 10 – 使用静态和动态作用域的时候

    (function () {
      var z = 20;
      foo(); // 10 – 使用静态作用域, 20 – 使用动态作用域
    })();

    // 将foo作为参数的时候是一样的
    (function (funArg) {
      var z = 30;
      funArg(); // 10 – 静态作用域, 30 – 动态作用域
    })(foo);

采用动态作用域，变量（标识符）的系统是通过变量动态栈来管理的。因此，自由变量是在当前活跃的动态链中查询的，而不是在函数创建的时候保存起来的静态作用域链中查询的。这样就会产生冲突。比方说，即使Z仍然存在（与之前从栈中移除变量的例子相反），还是会有这样一个问题： 在不同的函数调用中，Z的值到底取哪个呢（从哪个上下文，哪个作用域中查询）？

上述描述的就是两类funarg问题 —— 取决于是否将函数以返回值返回（第一类问题）以及是否将函数当函数参数使用（第二类问题）。

js并未采用 面向堆栈动态作用域 的方式

## 静态词法作用域

<!--language: js-->

    var x = 20;

    function foo() {
      alert(x); // 自由变量"x" == 20
    }

    // 为foo闭包
    fooClosure = {
      call: foo // 引用到function
      lexicalEnvironment: {x: 20} // 搜索上下文的上下文
    };

“fooClosure”部分是伪代码。对应的，在ECMAScript中，“foo”函数已经有了一个内部属性——创建该函数上下文的作用域链。

“lexical”通常是省略的。上述例子中是为了强调在闭包创建的同时，上下文的数据就会保存起来。当下次调用该函数的时候，自由变量就可以在保存的（闭包）上下文中找到了，正如上述代码所示，变量“z”的值总是10。

定义中我们使用的比较广义的词 —— “代码块”，然而，通常（在ECMAScript中）会使用我们经常用到的函数。当然了，并不是所有对闭包的实现都会将闭包和函数绑在一起，比方说，在Ruby语言中，闭包就有可能是： 一个过程对象（procedure object）, 一个lambda表达式或者是代码块。

对于要实现将局部变量在上下文销毁后仍然保存下来，基于栈的实现显然是不适用的（因为与基于栈的结构相矛盾）。因此在这种情况下，上层作用域的闭包数据是 __通过 动态分配内存的方式来实现的（基于“堆”的实现）__，配合使用 __垃圾回收__ 器（garbage collector简称GC）和 引用计数（reference counting）。这种实现方式比基于栈的实现性能要低，然而，任何一种实现总是可以优化的： 可以分析函数是否使用了自由变量，函数式参数或者函数式值，然后根据情况来决定 —— 是将数据存放在堆栈中还是堆中。

ECMAScript只使用 __静态（词法）作用域__（而诸如Perl这样的语言，既可以使用静态作用域也可以使用动态作用域进行变量声明）。

<!--language: js-->

    var x = 10;

    function foo() {
      alert(x);
    }

    (function (funArg) {
      var x = 20;
      // 变量"x"在(lexical)上下文中静态保存的，在该函数创建的时候就保存了
      funArg(); // 10, 而不是20
    })(foo);

技术上说，创建该函数的父级上下文的数据是保存在函数的内部属性 [[Scope]]中的。根据函数创建的算法，我们看到 在ECMAScript中，所有的函数都是闭包，因为它们都是在创建的时候就保存了上层上下文的作用域链（除开异常的情况） （不管这个函数后续是否会激活 —— [[Scope]]在函数创建的时候就有了）

<!--language: js-->

    var x = 10;

    function foo() {
      alert(x);
    }

    // foo是闭包
    foo: <FunctionObject> = {
      [[Call]]: <code block of foo>,
      [[Scope]]: [
        global: {
          x: 10
        }
      ],
      ... // 其它属性
    };

在ECMAScript中，同一个父上下文中创建的闭包是共用一个[[Scope]]属性的。也就是说，某个闭包对其中[[Scope]]的变量做修改会影响到其他闭包对其变量的读取：

<!--language: !js-->

    var firstClosure;
    var secondClosure;

    function foo() {
      var x = 1;

      firstClosure = function () { return ++x; };
      secondClosure = function () { return --x; };

      x = 2; // 影响 AO["x"], 在2个闭包公有的[[Scope]]中

      alert(firstClosure()); // 3, 通过第一个闭包的[[Scope]]
    }

    foo();

    alert(firstClosure()); // 4
    alert(secondClosure()); // 3

同一个上下文中创建的闭包是共用一个[[Scope]]属性的，[如下面](#TOC3.6)

## 闭包中返回
在ECMAScript中，闭包中的返回语句会将控制流返回给调用上下文（调用者）。而在其他语言中，比如，Ruby，有很多中形式的闭包，相应的处理闭包返回也都不同，下面几种方式都是可能的：可能直接返回给调用者，或者在某些情况下——直接从上下文退出。

<!--language: !js-->

    function getElement() {
      [1, 2, 3, 4].forEach(function (element) {
        if (element % 2 == 0) {
          // 返回给函数"forEach"函数
          // 而不是返回给getElement函数
          alert('found: ' + element); // found: 2, 4
          return element;
        }
      });
      return null;
    }

    getElement();

然而，在ECMAScript中通过try catch可以实现如下效果：

<!--language: !js-->

    var $break = {};

    function getElement() {
      try {
        [1, 2, 3, 4].forEach(function (element) {
          if (element % 2 == 0) {
            // // 从getElement中"返回"
            alert('found: ' + element); // found: 2
            $break.data = element;
            throw $break;
          }
        });
      } catch (e) {
        if (e == $break) {
          return $break.data;
        }
      }
      return null;
    }

    alert(getElement()); // 2


## 生命周期与内存泄漏
高阶函数内部产生的函数引用外部作用域的变量之后，__该变量脱离帧栈（frame stack），将自身的生命期绑定到产生的函数__，给予其状态，即创建闭包的父环境即使被销毁了，但闭包仍然引用着父环境的变量对象，也就是说需要继续维护着这个变量对象的内存。即该变量的生命周期被延长了。


由于闭包会使得函数中的变量都被保存在内存中，内存消耗很大，所以不能滥用闭包，否则会造成网页的性能问题，在IE中可能导致内存泄露。解决方法是，在退出函数之前，将不使用的局部变量全部删除。

> 一个闭包在创建时会附有三个属性：VO、thisValue、scope chain，而其中scope chain是指向外层parent scope的引用。因此这个闭包实际上保存了外层函数中element的引用，而element本身又引用了闭包，循环引用因此而见。因为循环引用导致的泄露实际上是IE浏览器引擎的bug而导致的，而如果element不是dom对象，不产生循环引用也就不会leak了，早期的IE版本里（ie4-ie6），对宿主对象（也就是document对象）采用是计数的垃圾回收机制，闭包导致内存泄露的一个原因就是这个算法的一个缺陷。循环引用会导致没法回收，这个循环引用只限定于有宿主对象参与的循环引用，而js对象之间即时形成循环引用，也不会产生内存泄露，因为对js对象的回收算法不是计数的方式

<!--language: js-->

    window.onload = function(){
      var oDiv = document.getElementById("div1");
      oDiv.onclick = function(){
        alert(oDiv.id);
      }

      //方法一：
      window.unonload = function(){
        oDiv.onclick = null;
      }
    }

    //方法二：
    window.onload = function(){
      var oDiv = document.getElementById("div1");
      var id = oDiv.id;//然后在内部函数中使用id，而不是oDiv即可解决
      oDiv.onclick = function(){
        alert(id);
      }
      oDiv = null;
    }



## 信息隐藏与保持状态
函数 __内部的局部变量__ 可以被修改，而且当再次进入到函数内部的时候，上次被修改的状态仍然持续，此为 __信息隐藏/封装对象状态__ 的惯用法（无需使用全局变量来保持状态）。

计数器：

<!--language: !js-->

    function makeCounter() {
        var i = 0;
        return function () {
            return ++i;
        };
    }

    var counter = makeCounter();
    alert(counter()); //  1
    alert(counter()); //  2

    alert(typeof i); // undefined（存在于makeCounter内部，外部不可见）

无限add：

<!--language: !js-->

    function add(n){
        function _add(m){
            return add(n + m);
        }
        _add.toString = _add.valueOf = function(){return n;};
        return _add;
    }

    alert(add(1)(3)(5)(7));

通过递归，并利用 __参数与返回值__ 来保持中间结果，减少内部的局部变量的副作用


## 作用域与变量共享

作用域控制着变量的可见性及生命周期，js中 __没有块级作用域__（如for/if并不能创建一个局部的上下文，仅有函数能够创建新的作用域（但with声明和catch语句能修改作用域链））。它会导致在闭包中行为与有块级作用域的语言中表现不同：

<!--language: !js-->

    function fn() {
        var fnarr = [];
        for (var i = 0; i < 3; i++) {
            fnarr.push(function(){alert(i);});
        }
        return fnarr;
    }

    fn().forEach(function(_f){_f();});

在其它有块级作用域语言中，可以加入一个中间变量`j`，来解决这个问题：

<!--language: !csharp-->

    using System;
    using System.Collections.Generic;
    using System.Linq;

    class Program {
        static List<Action> Fn() {
            var fnarr = new List<Action>();
            for (int i = 0; i < 3; i++) {
                int j = i;
                fnarr.Add(new Action(() => Console.WriteLine(j)));
            }
            return fnarr;
        }

        static void Main(string[] args) {
            Fn().ForEach((action) => action());
        }
    }

但在js中，需要利用其它方案来解决，一种方式是利用函数再构造一个作用域出来

<!--language: !js-->

    function fn() {
        var fnarr = [];
        for (var i = 0; i < 3; i++) {
            fnarr.push(
                (function(j){
                    return function(){alert(j);}
                })(i));
        }
        return fnarr;
    }

    fn().forEach(function(_f){_f();});

一种方式是使用`with`语句，将对象中标识符`{idx:i}`的解析添加到作用域链的最前端：

<!--language: !js-->

    function fn() {
        var fnarr = [];
        for (var i = 0; i < 3; i++) {
           with({idx:i}){
              fnarr.push(function(){alert(idx);});
           }
        }
        return fnarr;
    }

    fn().forEach(function(_f){_f();});


一种方式是将中间结果作为函数对象的属性进行保持：

<!--language: !js-->

    function fn() {
        var fnarr = [];
        for (var i = 0; i < 3; i++) {
            (fnarr[i] = function(){alert(arguments.callee.x);}).x = i;
        }
        return fnarr;
    }

    fn().forEach(function(_f){_f();});


递归
=========

递归的精髓是描述问题，而这正是函数式编程的精髓。

学生在学习递归时，有时候是被鼓励在纸上追踪 (trace)递归程序调用 (invocation)的过程。可以看到一个递归函数的追踪过程)。但这种练习可能会误导你：一个程序员在定义一个递归函数时，通常不会特别地去想函数的调用顺序所导致的结果。如果一个人总是需要这样子思考程序，递归会是艰难的、没有帮助的。

递归的优点是它精确地让我们 __更抽象地来检视算法__。你不需要考虑真正函数时所有的调用过程，就可以判断一个递归函数是否是正确的。

要知道一个递归函数是否做它该做的事，你只需要问，__它包含了所有的情况吗__？举例来说，一个寻找列表长度的递归函数：

- 对长度为 0 的列表是有效的。
- 给定它对于长度为 n 的列表是有效的，它对长度是 n+1 的列表也是有效的。

如果这两点是成立的，我们知道这个函数对于所有可能的列表都是正确的。

```!js
function len(lst){
    if (Array.prototype.toString.call(lst) === '') return 0;
    return (1 + len(lst.slice(1)));
}

alert(len([1,2,3,4,5,6,7]));
```

能够判断一个递归函数是否正确只不过是理解递归的上半场，下半场是能够写出一个做你想做的事情的递归函数。

## reduce
一些语言提供了 __尾递归__(tail recursion/trai-end recursion) 优化，即如果一个函数的最后执行递归调用语句的特殊形式的递归，那么调用的过程会被替换为一个循环，可以提高速度。但js __并没有提供__ 该优化。

`reduce`或`flod`对尾递归情形的一种模式固定 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#关键字_fold)，不使用变量（无空间污染，无副作用），而通过参数与返回值来保持中间结果

> ES5中的`Array.prototype.reduce`已有支持

<!--language: !js-->

    // sum
    alert([1, 2, 3].reduce(function(memo, num){ return memo + num; }, 0));
    // join
    alert([1, 2, 3].reduce(function(memo, num){ return memo + num; }, ""));

偏函数应用
=========

计算一个数组里等于某个值的元素的个数。[参考](http://www.vaikan.com/learn-you-a-haskell-for-great-good)

<!--language: !js-->

    // 不灵活
    function countMatching(array, value) {
        var counted = 0;
        for (var i = 0; i < array.length; i++) {
            if (array[i] == value)
                counted++;
        }
        return counted;
    }

    // == 2
    alert(countMatching([1,3,3,4,5], 3));

它不灵活，因为它只能用来计算一个数组中精确匹配某个值的元素的个数。下面是一个灵活一些的版本，它能接受一个函数，而不是一个值，作为参数。我们可以用它来对任何数据、任何对象进行比较。

<!--language: !js-->

    // more flexible
    function count(array, matching) {
        var counted = 0
        for (var i = 0; i < array.length; i++) {
            if (matching(array[i]))
                counted++
        }
        return counted
    }

    // == 2, same as first example
    alert(count([1,3,3,4,5], function(num) {
        return (num == 3)
    }));

    // == 2, now we can use our functions for ANY kind of items or match test!
    alert(count([{name:"bob"}, {name:"henry"}, {name:"jon"}], function(obj) {
        return (obj.name.length < 4)
    }));

因为高阶函数更具灵活性，你就更少有机会去写它们，因为你一旦你写成一个，你可以它应用到各种不同的情况中。

但比较函数（称为`predicates`）却不可复用。如果是一些简单的情况，这就足够了，但经常，我们会需要更复杂的比较方法的函数。这样的函数不仅仅可用于计数，它们可以用于任何事情上，一但你写成或找到了这样的函数，从长期的角度看，它们会节省你大量的时间和调试功夫。

让我们来定义一个可复用的比较函数，达到我们的目的。`==`不是一个函数。我们是否可以定义一个eq函数来帮我们完成类似的事情呢？

<!--language: js-->

    function eq(a, b) {
        return (a == b)
    }

    count([1,3,3,4,5], function(num) {
        return eq(3, num)
    })

我们用了一个库函数来完成比较任务，而不是使用我们现写的代码。如果`eq`函数很复杂，我们可以测试它并可以在其它的地方复用它。

但这使代码变得冗长，因为`count`函数的参数是一个只需要一个参数——数组元素——的函数，而eq函数却需要两个参数。我还是要定义我们自己的匿名函数。让我们来简化一下这些代码。

<!--language: js-->

    function makeEq(a) {
        // countMatchingWith wants a function that takes
        // only 1 argument, just like the one we're returning
        return function(b) {
            return eq(a, b)
        }
    }

    // now it's only on one line!
    count([1,3,3,4,5], makeEq(3))

我们写了一个兼容`count`函数的函数(一个参数——数组元素——返回`true`或`false`)。看起来就像是`count`函数调用的是`eq(3, item)`。这叫做 __*偏函数应用*__(partial function application) ，__指创建一个调用另外一个部分参数已经预置的函数的函数的用法__。这样，它就能被别的地方，比如`count`函数，以更少的参数形式来调用。我们在`makeEq`函数里已经实现了这些，但是，我们并不想针对我们各种功能开发出各种版本的`makeX`(比如`makeEqt`，`makeGt`，`makeLt`等)函数。让我们来找一种方法能 __通用于各种形式的函数__。

<!--language: js-->

    function applyFirst(f, a) {
        return function(b) {
            return f.call(null, a, b)
        }
    }

    count([1,3,3,4,5], applyFirst(eq, 3))

现在我们不再需要一个`makeEq`函数了。任何2个参数的库函数，我们都可以按这种方式调用。通过偏函数用法，__使得定义即使是诸如`==`这样简单功能的各种函数都变得十分有意义__，我们可以在高阶函数中更容易的使用它们（更大程度的复用）。

对那些超过2个参数的函数如何办呢？下面的这一版本能让我们接受任意多的参数，高阶函数可以自己追加参数。

<!--language: js-->

    function apply(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, args.concat(x))
        }
    }

    function propertyEquals(propertyName, value, obj) {
        return (obj[propertyName] == value)
    }

    count([{name:"bob"},{name:"john"}], apply(propertyEquals, "name", "bob")) // == 1

我们预置了2个参数，“name” 和 “bob”，count函数补足了最后一个参数来完成整个调用。偏函数用法使我们能接受各样的函数为参数，例如eq，然后把它们用于各样的高阶函数，例如count，以此来解决我们特定的问题。

## Map和Filter中偏函数用法

<!--language: !js-->

    // this equals [1,3,3]
    [1,3,3,4,5].filter(function(num) {
        return (num < 4)
    })

让我们把它替换成一个可以复用的比较函数`lt` (less than)。

<!--language: !js-->

    function apply(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, args.concat(x))
        }
    }

    function lt(a, b) {
        return (b < a)
    }

    [1,3,3,4,5].filter(apply(lt, 4))

看上去添加这个lt函数的做法有点傻，但是，我们可以使用偏函数用法来创造一个很简练的比较函数，当这个比较函数变的很复杂的时候，我们就能从对它的复用过程中获得好处。

map函数能让你把数组里的一个东西变成另外一个东西。

<!--language: !js-->

    var usersById = {"u1":{name:"bob"}, "u2":{name:"john"}}
    var user = {name:"sean", friendIds: ["u1", "u2"]}

    // == ["bob", "john"]
    function friendsNames(usersById, user) {
        return user.friendIds.map(function(id) {
            return usersById[id].name
        })
    }

我们写一个可以复用的map变换函数，就像之前我们的可复用比较函数一样。让我们写一个叫做lookup的函数。

<!--language: !js-->

    var usersById = {"u1":{name:"bob"}, "u2":{name:"john"}}
    var user = {name:"sean", friendIds: ["u1", "u2"]}

    function lookup(obj, key) {
        return obj[key]
    }

    // == [{name:"bob"}, {name:"john"}]
    function friends(usersById, user) {
        return user.friendIds.map(apply(lookup, usersById))
    }

很接近要求，但我们需要的是名称，而不是friend对象本身。如果我们再写一个参数颠倒过来的 lookup函数，通过第二次的map可以把它们的名称取出来。

<!--language: !js-->

    function lookupFlipped(key, obj) {
        return lookup(obj, key)
    }

    // == ["bob", "john"]
    function friendsNames(usersById, user) {
        return friends(usersById, user)
                .map(apply(lookupFlipped, "name"))
    }

但是我不想定义这个lookupFlipped函数，这样干有点傻。这样，我们来定义一个函数，它接收参数的顺序是从右到左，而不是从左到右，于是我们就能够复用lookup了。

<!--language: !js-->

    function applyr(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, [x].concat(args))
        }
    }

    // == ["bob", "john"]
    function friendsNames(usersById, user) {
        return friends(usersById, user)
                .map(applyr(lookup, "name")) // we can use normal lookup!
    }

applyr(lookup, "name")函数返回的函数只接受一个参数——那个对象——返回对象的名称。我们不再需要反转任何东西：我们可以按任何顺序接受参数。

偏函数用法需要对一些常见的功能定义各种不同的函数，就像lt函数，但这正是我们的目的。你可以以偏函数用法把lt函数既用于count函数，也可用于Array.filter函数。它们可以复用，可以组合使用。

Curry化
=========

函数也是值，`Curry`允许我们将函数与传递它的参数相结合去产生一个新的函数 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#Curried_functions)

curried的函数固化第一个参数为固定参数,并返回另一个带n-1个参数的函数对象,分别类似于LISP的原始函数car和cdr的行为。currying能泛化为偏函数应用（partial function application, PFA）,p 这种函数将任意数量（顺序）的参数的函数转化为另一个带剩余参数的函数对象。

> ES5中的`Function.prototype.bind`已有支持

<!--language: !js-->

    Function.prototype.method = function(name, func){
        if(!this.prototype[name]){
            this.prototype[name] = func;
            return this;
        }
    }

    Function.method('curry', function () {
        var slice = Array.prototype.slice,
            args = slice.apply(arguments),
            that = this;
        return function () {
            return that.apply(null, args.concat(slice.apply(arguments)));
        };
    });

    function add2(x,y){return x+y;}
    var add2_x = add2.curry(1);
    // var add2_x = add2.bind(null,1);
    alert(add2_x(6));

    function add3(x,y,z){return x+y+z;}
    var add3_x = add3.curry(1);
    var add3_x_y = add3_x.curry(2);
    // var add3_x = add3.bind(null,1);
    // var add3_x_y = add3_x.curry(null,2);
    alert(add3_x_y(6));

以上实现方式是通过给函数对象加上`curry`属性来实现的，下面将`curry`定义成一个函数，支持更直接的连续调用 [参考](http://www.cnblogs.com/rubylouvre/archive/2009/11/09/1598761.html)：

版本1：

<!--language: !js-->

    function curry(fun) {
        if (typeof fun != 'function') {
          throw new Error("The argument must be a function.");
        }
        if (fun.arity == 0) {
          throw new Error("The function must have more than one argument.");
        }

        var funText = fun.toString();
        var args = /function .*\((.*)\)(.*)/.exec(funText)[1].split(', ');
        var firstArg = args.shift();
        var restArgs = args.join(', ');
        var body = funText.replace(/function .*\(.*\) /, "");

        var curriedText =
          "function (" + firstArg + ") {" +
          "return function (" + restArgs + ")" + body +
          "}";

        eval("var curried =" + curriedText);
        return curried;
    }

    function sum(x, y) {
      return x + y;
    }
    function mean3(a, b, c) {
      return (a + b + c)/3;
    }

    var a = curry(sum)(10)(15)
    alert(a)//25
    var b = curry(mean3)(10)(20, 30);
    alert(b)//20
    var c =  curry(curry(sum))(10)()(20);
    alert(c);//error
    var d = curry(curry(mean3)(10))(20)(30);
    alert(d);//error

版本2：

<!--language: !js-->

    var curry= function(fn){
      return function(args){
        //内部函数的参数为数组，由于立即执行，因此直接到第三重去
        //args是相对于第三重内部函数可是全局变量
        var self= arguments.callee;//把自身保存起来（就是那个数组为参数的第二重函数）
        return function(){ //这才是第二次调用的函数
          if(arguments.length){//如果还有要添加的参数
            [].push.apply(args,arguments);//apply把当前传入的所有参数放进args中
            return self(args);
          }else{
            return fn.apply(this,args);//apply的第二参数为数组
          }
        }
      }([]);
    };

    function sum(){
        var result=0;
        for(var i=0, n=arguments.length; i<n; i++){
            result += arguments[i];
        }
        return result;
    }

    var sum2= curry(sum);
        sum2= sum2(1)(2)(3)(4)(5);

    alert(sum2()); // 15

    var sum3= curry(sum);
        sum3= sum3(1,2,3);
        sum3= sum3(4,5,6);
        sum3= sum3(7,8,9);
    alert(sum3()); // 45

上面的函数针对不固定参数的，最后怎么也要放个括号，如果原函数的参数是固定的，想只要参数足够就返回结果，多出的参数忽略。改进如下：

<!--language: !js-->

    function curry(f) {
      if (f.length == 0) return f;
      function iterate(args) {
        if (args.length >= f.length)
          return f.apply(null, args);
        return function () {
          return iterate(args.concat(Array.prototype.slice.call(arguments)));
        };
      }
      return iterate([]);
    }

    function mean3(a, b, c) { return (a + b + c) / 3; }

    var curriedMean3 = curry(mean3);
    alert(curriedMean3(1)(2, 3)); // => 2
    alert(curriedMean3(1)(2)(3));//空括号无效
    alert(curriedMean3()(1)()(2)()(3)); // => 2
    alert(curriedMean3(1, 2)(3, 4)); // => 2 (第四个参数无效)


## 偏函数 vs Curry化
偏函数解决这样的问题：__如果我们有函数是多个参数的，我们希望能固定其中某几个参数的值__。

在Python语言中，我们可以这样做：

<!--language: !python-->

    from functools import partial

    def foo(a,b,c):
      return a + b + c

    foo23 = partial(foo, b=23)
    print foo23(a = 1, c = 3)


函数Curry化明显解决的是一个完全不同的问题：__如果我们有几个单参数函数，并且这是一种支持一等函数(first-class)的语言，如何去实现一个多参数函数__？函数Curry化是一种实现多参数函数的方法。

下面是一个单参数的Javascript函数:

<!--language: !js-->

    var foo = function(a) {
      return a * a;
    }

如果我们 __受限只能写单参数函数__，可以像下面这样模拟出一个多参数函数：

<!--language: !js-->

    var foo = function(a) {
      return function(b) {
        return a * a + b * b;
      }
    }

通过这样调用它：`(foo(3))(4)`，或直接 `foo(3)(4)`。

函数Curry化提供了一种非常自然的方式来实现某些偏函数应用。如果你希望函数`foo`的第一个参数值被固定成5，你需要做的就是`var foo5 = foo(5)`。这就OK了。函数`foo5`就是`foo`函数的偏函数。注意，尽管如此，我们没有很简单的方法对`foo`函数的第二个参数偏函数化(除非先偏函数化第一个参数)。

当然，Javascript是支持多参数函数的：

<!--language: !js-->

    var bar = function(a, b) {
      return a * a + b * b;
    }

我们定义的`bar`函数并不是一个Curry化的函数。调用`bar(5)`并不会返回一个可以输入`12`的函数（而实际上相当于`b`赋值为`undefined`）。我们只能像`bar(5,12)`这样调用这个函数。

在一些其它语言里，比如 Haskell 和 OCaml，所有的 __多参数函数__ 都是通过Curry化实现的。

下面是一个把上面的foo函数用OCaml语言写成的例子：

<!--language: ocaml-->

    let foo = fun a ->
      fun b ->
        a * a + b * b

下面是把上面的bar函数用OCaml语言写成的例子：

<!--language: ocaml-->

    let bar = fun a b ->
      a * a + b * b

头一个函数我们叫做“显式Curry化”，第二个叫做“隐式Curry化”。跟Javascript不一样，在OCaml语言里，`foo`函数和`bar`函数是完全一样的。我们用完全一样的方式调用它们。

<!--language: ocaml-->

    # foo 3 4;;
    - : int = 25
    # bar 3 4;;
    - : int = 25

两个函数都能够通过提供一个参数值来创造一个偏函数：

<!--language: ocaml-->

    # let foo5 = foo 5;;
    val foo5 : int -> int = <fun>

    # let bar5 = bar 5;;
    val bar5 : int -> int = <fun>
    # foo5 12;;
    - : int = 169
    # bar5 12;;
    - : int = 169

小结：

> - 偏函数应用是找一个函数，固定其中的几个参数值，从而得到一个新的函数。
> - 函数Curry化是一种 __使用匿名单参数函数来实现多参数函数__ 的方法。
> - 函数Curry化能够让你轻松的实现某些偏函数应用。
> - 函数Curry化的重要意义在于可以把函数完全变成「接受一个参数；返回一个值」的固定形式，这样对于讨论和优化会更加方便。
> - 有些语言(例如 Haskell, OCaml)所有的多参函数都是在内部通过函数Curry化实现的。



函数组合
=========

组合函数三个函数`f()`, `g()`与`h()`，执行效果与`f(g(h()))`等价，能减少括号的使用，减少参数赋值，能对函数对象组合后再调用，增加流程的灵活性 [参考1](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#Function_composition)，[参考2](http://javascriptweblog.wordpress.com/2010/04/14/compose-functions-as-building-blocks/)，函数组合是右结合的

以下代码摘自underscore.js

<!--language: !js-->

    function compose() {
        var funcs = arguments;
        return function() {
            var args = arguments;
            for (var i = funcs.length - 1; i >= 0; i--) {
                args = [funcs[i].apply(this, args)];
            }
            return args[0];
        };
    }

    var greet    = function(name){ return "hi: " + name; };
    var exclaim  = function(statement){ return statement.toUpperCase() + "!"; };
    var welcome = compose(greet, exclaim);
    alert(welcome('moe'));

在[前面map](#TOC5.1)例子中，我们遍历了数组两次，一次用来获取users，一次为了获取names。如果能在一次map映射操作中同时做这两件事情，效率会高很多。

<!--language: !js-->

    function apply(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, args.concat(x))
        }
    }

    function applyr(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, [x].concat(args))
        }
    }

    function lookup(obj, key) {
        return obj[key]
    }

    function friendsNames(usersById, user) {
        return user.friendIds.map(function(id) {
            var friend = lookup(usersById, id)
            return lookup(friend, "name")
        })
    }

    var usersById = {"u1":{name:"bob"}, "u2":{name:"john"}}
    var user = {name:"sean", friendIds: ["u1", "u2"]}

    alert(friendsNames(usersById, user));

利用函数组合可将`friendsNames`中的代码优化，函数组合意思是串联多个函数，组成一个新的函数，每一次串联都是把前一个函数的输出当作下一个函数的输入

<!--language: !js-->

    function apply(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, args.concat(x))
        }
    }

    function applyr(f) {
        var args = Array.prototype.slice.call(arguments, 1)
        return function(x) {
            return f.apply(null, [x].concat(args))
        }
    }

    function lookup(obj, key) {
        return obj[key]
    }

    function compose() {
        var funcs = arguments;
        return function() {
            var args = arguments;
            for (var i = funcs.length - 1; i >= 0; i--) {
                args = [funcs[i].apply(this, args)];
            }
            return args[0];
        };
    }

    function friendsNames(usersById, user) {
        return user.friendIds.map(compose(applyr(lookup, "name"), apply(lookup, usersById)))
    }

    var usersById = {"u1":{name:"bob"}, "u2":{name:"john"}}
    var user = {name:"sean", friendIds: ["u1", "u2"]}

    alert(friendsNames(usersById, user));

对数组的遍历只进行了一次，只使用一次map操作，下面是可读性更好的写法：

<!--language: !js-->

    var friend = lookup // lookup 恰巧能干我们想要的事情。
    var name = applyr(lookup, "name")

    function friendsNames(usersById, user) {
        // this line is now more semantic.
        return user.friends.map(compose(name, apply(friend, usersById)))
    }


惰性求值
=========

表达式不在它被绑定到变量之后就立即求值，而是在该值被取用的时候求值。

假设我们对包含1000个整数的数组进行一些操作，underscore.js版本，会生成许多巨大的中间数组：

<!--language: js-->

    var result = _.chain(array).map(square).map(inc).filter(isEven).take(5).value();

[Lazy.js](http://danieltao.com/lazy.js)版本，不会生成中间数组。

<!--language: !js-->

    //= require lazy.0.3.2
    var array = Lazy.range(1000).toArray();
    function square(x) { return x * x; }
    function inc(x) { return x + 1; }
    function isEven(x) { return x % 2 === 0; }

    var result = Lazy(array).map(square).map(inc).filter(isEven).take(5);
    alert(result.toArray());

它返回的不是数组，而是带`each`方法的序列对象（生成器）。它不会生成中间数组，而且只有在调用`each`方法的时候才产生迭代

基于Lazy.js，还能生成无穷序列、实现异步迭代

[Lo-Dash](http://lodash.com/)也将加入惰性求值

## 实现yield

