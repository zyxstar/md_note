> 2013-06-18

语言核心
=========
本部分主要摘自[Stoyan Stefanov](http://hub.tutsplus.com/authors/stoyan-stefanov)，[zhangxinxu](http://www.zhangxinxu.com/)，[TomXu](http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html)相关内容

## 执行上下文堆栈
每次当控制器转到ECMAScript可执行代码的时候，即会进入到一个执行上下文。执行上下文(简称-EC)是ECMA-262标准里的一个抽象概念，用于同可执行代码(executable code)概念进行区分。

我们可以定义执行上下文堆栈是一个数组：

<!--language: js-->

    ECStack = [];

每次进入function (即使function被递归调用或作为构造函数) 的时候或者内置的eval函数工作的时候，这个堆栈都会被压入。

### 全局代码
这种类型的代码是在"程序"级处理的：例如加载外部的js文件或者本地`<script></script>`标签内的代码。全局代码不包括任何function体内的代码。

在初始化（程序启动）阶段，ECStack是这样的：

<!--language: js-->

    ECStack = [
      globalContext
    ];

### 函数代码
当进入funtion函数代码(所有类型的funtions)的时候，ECStack被压入新元素。需要注意的是，具体的函数代码不包括内部函数(inner functions)代码。如下所示，我们使函数自己调自己的方式递归一次：

<!--language: js-->

    (function  foo(bar) {
      if (bar) {
        return;
      }
      foo(true);
    })();

那么，ECStack以如下方式被改变：

<!--language: js-->

    // 第一次foo的激活调用
    ECStack = [
      <foo> functionContext
      globalContext
    ];

    // foo的递归激活调用
    ECStack = [
      <foo> functionContext – recursively
      <foo> functionContext
      globalContext
    ];

每次return的时候，都会退出当前执行上下文的，相应地ECStack就会弹出，栈指针会自动移动位置，这是一个典型的堆栈实现方式。一个抛出的异常如果没被截获的话也有可能从一个或多个执行上下文退出。相关代码执行完以后，ECStack只会包含全局上下文(global context)，一直到整个应用程序结束。

### Eval代码
eval 代码有点儿意思。它有一个概念：__调用上下文__(calling context),例如，eval函数调用的时候产生的上下文。eval(变量或函数声明)活动 __会影响__ 调用上下文。

<!--language: !js-->

    eval('var x = 10');

    (function foo() {
      eval('var y = 20');
    })();

    alert(x); // 10
    alert(y); // "y" 提示没有声明

ECStack的变化过程：

<!--language: js-->

    ECStack = [
      globalContext
    ];

    // eval('var x = 10');
    ECStack.push(
      evalContext,
      callingContext: globalContext
    );

    // eval exited context
    ECStack.pop();

    // foo funciton call
    ECStack.push(<foo> functionContext);

    // eval('var y = 20');
    ECStack.push(
      evalContext,
      callingContext: <foo> functionContext
    );

    // return from eval
    ECStack.pop();

    // return from foo
    ECStack.pop();


## 变量对象VO
声明函数和变量，以成功构建我们的系统，解释器是如何并且在什么地方去查找这些函数和变量呢？任何function在执行的时候都会创建一个 __执行上下文__，因为为function声明的变量和function有可能只在该function内部，这个上下文，在调用function的时候，提供了一种简单的方式来创建自由变量或私有子function。

__变量对象__（variable object）是一个与执行上下文相关的特殊对象，它存储着上下文中声明的以下内容：

> - 变量(VariableDeclaration, var);
> - 函数声明(Function Declaration, FD)
> - 函数的形参(Function arguments)

<!--language: js-->

    var a = 10;

    function test(x) {
      var b = 20;
    };

    test(30);

则对应的变量对象是：

<!--language: js-->

    // 全局上下文的变量对象
    VO(globalContext) = {
      a: 10,
      test: <reference to function>
    };

    // test函数上下文的变量对象
    VO(test functionContext) = {
      x: 30,
      b: 20
    };

### 全局上下文中的变量对象
全局对象是进入任何执行上下文之前就已经创建了的对象，这个对象只存在一份，它的属性在程序中任何地方都可以访问，全局对象的生命周期终止于程序退出那一刻。

全局对象初始创建阶段将Math、String、Date、parseInt作为自身属性，等属性初始化，同样也可以有额外创建的其它对象作为属性（其可以指向到全局对象自身）。例如，在DOM中，全局对象的window属性就可以引用全局对象自身

<!--language: js-->

    global = {
      Math: <...>,
      String: <...>
      ...
      ...
      window: global //引用自身
    };

全局上下文中的变量对象就是全局对象自己，即`VO(globalContext) === global`

### 函数上下文中的变量对象
在函数执行上下文中，变量对象VO是不能直接访问的，此时由 __活动对象__(activation object, AO)来扮演VO角色，即`VO(functionContext) === AO`

活动对象是进入函数上下文时刻被创建的，通过函数的`arguments`属性初始化，arguments属性的值是Arguments对象：

<!--language: js-->

    AO = {
      arguments: <ArgO>
    };


Arguments对象是活动对象的一个属性，它包括如下属性：

> - callee: 指向当前函数的引用
> - length: 真正传递的参数个数
> - properties-indexes: (字符串类型的整数) 属性的值就是函数的参数值(按参数列表从左到右排列)。 properties-indexes内部元素的个数等于arguments.length. properties-indexes 的值和实际传递进来的参数之间是共享的。

## 处理代码两阶段
### 进入执行上下文
当进入执行上下文(代码执行之前)时，__VO里已经包含了下列属性__：

- 函数的所有形参(如果我们是在函数执行上下文中)
    - 由名称和对应值组成的一个变量对象的属性被创建；__没有传递__ 对应参数的话，那么由名称和undefined值组成的一种变量对象的属性也将 __被创建__。

- 所有函数声明(FunctionDeclaration, FD)
    - 由名称和对应值（函数对象(function-object)）组成一个变量对象的属性被创建；如果变量对象 __已经存在__ 相同名称的属性，则完全 __替换__ 这个属性。

- 所有变量声明(var, VariableDeclaration)
    - 由名称和对应值（undefined）组成一个变量对象的属性被创建；如果变量名称跟已经声明的形式参数或函数相同，则变量声明 __不会干扰__ 已经存在的这类属性。

<!--language: js-->

    function test(a, b) {
      var c = 10;
      function d() {}
      var e = function _e() {};
      (function x() {});
    }

    test(10); // call

当进入带有参数10的test函数上下文时，AO表现为如下：

<!--language: js-->

    AO(test) = {
      a: 10,
      b: undefined,
      c: undefined,
      d: <reference to FunctionDeclaration "d">
      e: undefined
    };

AO里并不包含函数“x”。这是因为“x” 是一个函数表达式(FunctionExpression, 缩写为 FE) 而不是函数声明，函数表达式不会影响VO。 不管怎样，函数“_e” 同样也是函数表达式，但是就像我们下面将看到的那样，因为它分配给了变量 “e”，所以它可以通过名称“e”来访问。

### 执行代码
这个周期内，AO/VO已经拥有了属性(不过，并不是所有的属性都有值，大部分属性的值还是系统默认的初始值undefined )。

还是前面那个例子, __AO/VO在代码解释期间被修改__ 如下：

<!--language: js-->

    AO['c'] = 10;
    AO['e'] = <reference to FunctionExpression "_e">;

再次注意，因为FunctionExpression“_e”保存到了已声明的变量“e”上，所以它仍然存在于内存中。而FunctionExpression “x”却不存在于AO/VO中，也就是说如果我们想尝试调用“x”函数，不管在函数定义之前还是之后，都会出现一个错误“x is not defined”，未保存的函数表达式只有在它自己的定义或递归中才能被调用。

另一个经典例子：

<!--language: !js-->

    console.log(x); // function

    var x = 10;
    console.log(x); // 10

    x = 20;

    function x() {};

    console.log(x); // 20

为什么第一个alert “x” 的返回值是function，而且它还是在“x” 声明之前访问的“x” 的？为什么不是10或20呢？因为，根据规范函数声明是在当 __进入上下文__ 时填入的； 同一周期，在进入上下文的时候还有一个变量声明“x”，那么正如我们在上一个阶段所说，变量声明在顺序上跟在函数声明和形式参数声明之后，而且在这个 __进入上下文阶段，变量声明不会干扰VO中已经存在的同名函数声明或形式参数声明__，因此，在进入上下文时，VO的结构如下：

<!--language: js-->

    VO = {};

    VO['x'] = <reference to FunctionDeclaration "x">

    // 找到var x = 10;
    // 如果function "x"没有已经声明的话
    // 这时候"x"的值应该是undefined
    // 但是这个case里变量声明没有影响同名的function的值

    VO['x'] = <the value is not disturbed, still function>

紧接着，在执行代码阶段，VO做如下修改：

<!--language: js-->

    VO['x'] = 10;
    VO['x'] = 20;

我们可以在第二、三个console.log中看到这个效果。

在下面的例子里我们可以再次看到，变量是在进入上下文阶段放入VO中的。(因为，虽然else部分代码永远不会执行，但是不管怎样，变量“b”仍然存在于VO中。)

<!--language: !js-->

    if (true) {
      var a = 1;
    } else {
      var b = 2;
    }

    alert(a); // 1
    alert(b); // undefined,不是b没有声明，而是b的值是undefined

## this关键字
this是执行上下文中的一个属性：

<!--language: js-->

    activeExecutionContext = {
      VO: {...},
      this: thisValue
    };

这里VO是我们前一章讨论的变量对象。

this与上下文中可执行代码的类型有直接关系，this值在进入上下文时确定，并且在上下文运行期间永久不变。

### 全局代码中this
在这里一切都简单。在全局代码中，this始终是全局对象本身，这样就有可能间接的引用到它了。

### 函数代码中this
它不是静态的绑定到一个函数，this是 __进入上下文时确定__，在一个函数代码中，这个值在每一次完全不同。不管怎样，在代码运行时的this值是不变的，也就是说，因为它不是一个变量，就不可能为其分配一个新值（相反，在Python编程语言中，它明确的定义为对象本身，在运行期间可以不断改变）。

<!--language: !js-->

    var foo = {x: 10};

    var bar = {
      x: 20,
      test: function () {
        alert(this === bar);
        alert(this.x);

        //this = foo; // 错误，任何时候不能改变this的值
      }
     };

    // 在进入上下文的时候
    // this被当成bar对象
    bar.test(); // true, 20

    foo.test = bar.test;

    // 不过，这里this不会是foo
    // 尽管调用的是相同的function
    foo.test(); // false, 10

影响了函数代码中this值的变化有几个因素：

首先，在通常的函数调用中，__this是由激活上下文代码的调用者来提供的__，即调用函数的父上下文(parent context )。this取决于调用函数的方式。为了在任何情况下准确无误的确定this值，有必要理解和记住这重要的一点。正是调用函数的方式影响了调用的上下文中的this值，没有别的什么

即使是正常的全局函数也会被调用方式的不同形式激活，这些不同的调用方式导致了不同的this值。

<!--language: !js-->

    function foo() {
      alert(this);
    }

    foo(); // global

    alert(foo === foo.prototype.constructor); // true

    // 但是同一个function的不同的调用表达式，this是不同的
    foo.prototype.constructor(); // foo.prototype

有可能作为一些对象定义的方法来调用函数，但是this将不会设置为这个对象。

<!--language: !js-->

    var foo = {
      bar: function () {
        alert(this);
        alert(this === foo);
      }
    };

    foo.bar(); // foo, true

    var exampleFunc = foo.bar;
    alert(exampleFunc === foo.bar); // true

    // 再一次，同一个function的不同的调用表达式，this是不同的
    exampleFunc(); // global, false

那么，调用函数的方式如何影响this值？为了充分理解this值的确定，需要详细分析其内部类型之一—— __引用类型__（Reference type）。

### 引用类型
使用伪代码我们可以将引用类型的值可以表示为拥有两个属性的对象——base（即拥有属性的那个对象），和base中的propertyName 。

<!--language: js-->

    var valueOfReferenceType = {
      base: <base object>,
      propertyName: <property name>
    };

引用类型的值只有两种情况：

> - 当我们处理一个标示符时
> - 或一个属性访问器

#### 标识符

标识符是变量名，函数名，函数参数名和全局对象中未识别的属性名。例如，下面标识符的值：

<!--language: js-->

    var foo = 10;
    function bar() {}

在操作的中间结果中，引用类型对应的值如下：

<!--language: js-->

    var fooReference = {
      base: global,
      propertyName: 'foo'
    };

    var barReference = {
      base: global,
      propertyName: 'bar'
    };

为了从引用类型中得到一个对象真正的值，伪代码中的GetValue方法可以做如下描述：

<!--language: js-->

    function GetValue(value) {
      if (Type(value) != Reference) {
        return value;
      }

      var base = GetBase(value);

      if (base === null) {
        throw new ReferenceError;
      }

      return base.[[Get]](GetPropertyName(value));
    }

内部的[[Get]]方法返回对象属性真正的值，包括对原型链中继承的属性分析。

<!--language: js-->

    GetValue(fooReference); // 10
    GetValue(barReference); // function object "bar"

#### 属性访问器
它有两种变体：点（.）语法（此时属性名是正确的标示符，且事先知道），或括号语法（[]）。

<!--language: js-->

    foo.bar();
    foo['bar']();

在中间计算的返回值中，我们有了引用类型的值。

<!--language: js-->

    var fooBarReference = {
      base: foo,
      propertyName: 'bar'
    };

    GetValue(fooBarReference); // function object "bar"

### 决定this的值

在一个函数上下文中，this由调用者提供，由调用函数的方式来决定。如果调用括号()的左边是引用类型的值，this将设为引用类型值的base对象（base object），在其他情况下（与引用类型不同的任何其它属性），这个值为null。不过，实际不存在this的值为null的情况，因为当this的值为null的时候，其值会被隐式转换为全局对象。注：第5版的ECMAScript中，已经不强迫转换成全局变量了，而是赋值为undefined。

<!--language: !js-->

    function foo() {
      return this;
    }

    foo(); // global

我们看到在调用括号的左边是一个引用类型值（因为foo是一个 __标示符__）。

<!--language: js-->

    var fooReference = {
      base: global,
      propertyName: 'foo'
    };

相应地，this也设置为引用类型的base对象。即全局对象。

同样，使用 __属性访问器__：

<!--language: !js-->

    var foo = {
      bar: function () {
        return this;
      }
    };

    foo.bar(); // foo

我们再次拥有一个引用类型，其base是foo对象，在函数bar激活时用作this。

<!--language: js-->

    var fooBarReference = {
      base: foo,
      propertyName: 'bar'
    };

但是，用另外一种形式激活相同的函数，我们得到其它的this值。

<!--language: !js-->

    var test = foo.bar;
    test(); // global

因为test作为 __标示符__，__生成了引用类型的其他值__，其base（全局对象）用作this 值。

<!--language: js-->

    var testReference = {
      base: global,
      propertyName: 'test'
    };

为什么用表达式的不同形式激活同一个函数会不同的this值，__答案在于引用类型（type Reference）不同的中间值__。

<!--language: js-->

    function foo() {
      alert(this);
    }

    foo(); // global, because

    var fooReference = {
      base: global,
      propertyName: 'foo'
    };

    alert(foo === foo.prototype.constructor); // true

    // 另外一种形式的调用表达式

    foo.prototype.constructor(); // foo.prototype, because

    var fooPrototypeConstructorReference = {
      base: foo.prototype,
      propertyName: 'constructor'
    };

### 函数调用和非引用类型
因此，正如我们已经指出，当调用括号的左边不是引用类型而是其它类型，这个值自动设置为null，结果为全局对象。让我们再思考这种表达式：

<!--language: !js-->

    (function () {
      alert(this); // null => global
    })();

在这个例子中，我们有一个函数对象但不是引用类型的对象（它 __不是标示符，也不是属性访问器__），相应地，this值最终设为全局对象。

更多复杂的例子：

<!--language: !js-->

    var foo = {
      bar: function () {
        alert(this);
      }
    };

    foo.bar(); // Reference, OK => foo
    (foo.bar)(); // Reference, OK => foo

    (foo.bar = foo.bar)(); // global?
    (false || foo.bar)(); // global?
    (foo.bar, foo.bar)(); // global?

- 第一个例子很明显——明显的引用类型，结果是，this为base对象，即foo。
- 在第二个例子中，组运算符并不适用，想想上面提到的，从引用类型中获得一个对象真正的值的方法，如GetValue。相应的，在组运算的返回中——我们得到仍是一个引用类型。这就是this值为什么再次设为base对象，即foo。
- 第三个例子中，与组运算符不同，赋值运算符调用了GetValue方法。返回的结果是函数对象（但不是引用类型），这意味着this设为null，结果是global对象。
- 第四个和第五个也是一样——逗号运算符和逻辑运算符（OR）调用了GetValue 方法，相应地，我们失去了引用而得到了函数。并再次设为global。

### 引用类型和this为null
当调用表达式限定了call括号左边的引用类型的值，尽管this被设定为null，但结果被隐式转化成global。当引用类型值的base对象是被 __活动对象__ 时，这种情况就会出现。

下面的实例中，内部函数被父函数调用，此时我们就能够看到上面说的那种特殊情况，局部变量、内部函数、形式参数储存在给定函数的激活对象中。

<!--language: !js-->

    function foo() {
      function bar() {
        alert(this); // global
      }
      bar(); // the same as AO.bar()
    }

活动对象总是作为this返回，值为null——（即伪代码的 __AO.bar()相当于null.bar()__）。这里我们再次回到上面描述的例子，this设置为全局对象。

有一种情况除外：如果with对象包含一个函数名属性，在with语句的内部块中调用函数。With语句添加到该对象作用域的最前端，即在活动对象的前面。相应地，也就有了引用类型（通过标示符或属性访问器）， 其base对象不再是活动对象，__而是with语句的对象__。顺便提一句，它不仅与内部函数相关，也与全局函数相关，因为with对象比作用域链里的 __最前端的对象__ (全局对象或一个活动对象)还要靠前。

<!--language: !js-->

    var x = 10;

    with ({
      foo: function () {
        alert(this.x);
      },
      x: 20
    }) {
      foo(); // 20
    }

对应的结构如下：

<!--language: js-->

    var  fooReference = {
      base: __withObject,
      propertyName: 'foo'
    };

同样的情况出现在catch语句的实际参数中函数调用：在这种情况下，catch对象添加到作用域的最前端，即在活动对象或全局对象的前面。但是，这个特定的行为被确认为ECMA-262-3的一个bug，这个在新版的ECMA-262-5中修复了。这样，在特定的活动对象中，this指向全局对象。而不是catch对象

### 构造器调用的函数中this

<!--language: !js-->

    function A() {
      alert(this); // "a"对象下创建一个新属性
      this.x = 10;
    }

    var a = new A();
    alert(a.x); // 10

在这个例子中，new运算符调用“A”函数的内部的[[Construct]] 方法，接着，在对象创建后，调用内部的[[Call]] 方法。 函数“A”将this的值设置为新创建的对象。

### 函数调用中手动设置this
它们是.apply和.call方法

## 作用域链
作用域链大多数与内部函数相关，ECMAScript 允许创建内部函数，我们甚至能从父函数中返回这些函数。

<!--language: !js-->

    var x = 10;

    function foo() {
      var y = 20;
      function bar() {
        alert(x + y);
      }
      return bar;
    }

    foo()(); // 30

很明显每个上下文拥有自己的变量对象：对于全局上下文，它是全局对象自身；对于函数，它是活动对象。__作用域链__ 正是内部上下文 __所有变量对象（包括父变量对象）的列表__。此链用来变量查询。即在上面的例子中，“bar”上下文的作用域链包括AO(bar)、AO(foo)和VO(global)。

作用域链与一个执行上下文相关，变量对象的链用于在标识符解析中变量查找。

函数上下文的作用域链在函数调用时创建的，包含活动对象和这个函数内部的[[scope]]属性。下面我们将更详细的讨论一个函数的[[scope]]属性。在上下文中示意如下：

<!--language: js-->

    activeExecutionContext = {
        VO: {...}, // or AO
        this: thisValue,
        Scope: [ // Scope chain
          // 所有变量对象的列表
          // for identifiers lookup
        ]
    };


其scope定义为`Scope = AO + [[Scope]]`，这种联合和标识符解析过程，我们将在下面讨论，这与 __函数的生命周期__ 相关。

### 函数的生命周期
函数的的生命周期分为 __创建阶段__ 和 __激活阶段__（调用时）

#### 函数创建
在进入上下文时函数声明放到 __变量/活动（VO/AO）对象__ 中

<!--language: !js-->

    var x = 10;

    function foo() {
      var y = 20;
      alert(x + y);
    }

    foo(); // 30

变量“y”在函数“foo”中定义（意味着它在foo上下文的AO中），但是变量“x”并未在“foo”上下文中定义，相应地，它也不会添加到“foo”的AO中。乍一看，变量“x”相对于函数“foo”根本就不存在

<!--language: js-->

    fooContext.AO = {
      y: undefined // undefined – 进入上下文的时候是20 – at activation
    };

函数“foo”如何访问到变量“x”？理论上函数应该能访问一个更高一层上下文的变量对象。实际上它正是这样，这种机制是通过函数内部的[[scope]]属性来实现的。

[[scope]]是所有父变量对象的层级链，处于当前函数上下文之上，在函数创建时存于其中。[[scope]]在函数创建时被存储－－静态（不变的），直至函数销毁。

与作用域链对比，[[scope]]是函数的一个属性而不是上下文。考虑到上面的例子，函数“foo”的[[scope]]如下：

<!--language: js-->

    foo.[[Scope]] = [
      globalContext.VO // === Global
    ];

#### 函数激活
函数调用时进入上下文，这时候活动对象被创建，this和作用域（作用域链）被确定。进入上下文创建AO/VO之后，上下文的Scope属性（变量查找的一个作用域链）作如下定义：

<!--language: js-->

    Scope = AO|VO + [[Scope]]

上面代码的意思是：活动对象是作用域数组的第一个对象，即添加到作用域的 __前端__。

<!--language: js-->

    Scope = [AO].concat([[Scope]]);

这个特点对于标示符解析的处理来说很重要。

标示符解析是一个处理过程，用来确定一个变量（或函数声明）属于哪个变量对象。

这个算法的返回值中，我们总有一个引用类型，它的base组件是相应的变量对象（或若未找到则为null），属性名组件是向上查找的标示符的名称。引用类型的详细信息在this中已讨论。

标识符解析过程包含与变量名对应属性的查找，即作用域中变量对象的连续查找，从最深的上下文开始，绕过作用域链直到最上层。

这样一来，在向上查找中，一个上下文中的局部变量较之于父作用域的变量拥有较高的优先级。万一两个变量有相同的名称但来自不同的作用域，那么第一个被发现的是在 __最深作用域__ 中。

我们用一个稍微复杂的例子描述上面讲到的这些。

<!--language: !js-->

    var x = 10;

    function foo() {
      var y = 20;

      function bar() {
        var z = 30;
        alert(x +  y + z);
      }

      bar();
    }

    foo(); // 60

对此，我们有如下的变量/活动对象，函数的的[[scope]]属性以及上下文的作用域链：

- 全局上下文的变量对象是：

<!--language: js-->

    globalContext.VO === Global = {
      x: 10
      foo: <reference to function>
    };

- 在“foo”创建时，“foo”的[[scope]]属性是：

<!--language: js-->

    foo.[[Scope]] = [
      globalContext.VO
    ];

- 在“foo”激活时（进入上下文），“foo”上下文的活动对象是：

<!--language: js-->

    fooContext.AO = {
      y: 20,
      bar: <reference to function>
    };

- “foo”上下文的作用域链为：

<!--language: js-->

    fooContext.Scope = fooContext.AO + foo.[[Scope]]

    fooContext.Scope = [
      fooContext.AO,
      globalContext.VO
    ];

- 内部函数“bar”创建时，其[[scope]]为：

<!--language: js-->

    bar.[[Scope]] = [
      fooContext.AO,
      globalContext.VO
    ];

- 在“bar”激活时，“bar”上下文的活动对象为：

<!--language: js-->

    barContext.AO = {
      z: 30
    };

- “bar”上下文的作用域链为：

<!--language: js-->

    barContext.Scope = barContext.AO + bar.[[Scope]]

    barContext.Scope = [
      barContext.AO,
      fooContext.AO,
      globalContext.VO
    ];

- 对“x”、“y”、“z”的标识符解析如下：

<!--language: js-->

    - "x"
    -- barContext.AO // not found
    -- fooContext.AO // not found
    -- globalContext.VO // found - 10

    - "y"
    -- barContext.AO // not found
    -- fooContext.AO // found - 20

    - "z"
    -- barContext.AO // found - 30

### 作用域特征

#### 闭包
在ECMAScript中，闭包与函数的[[scope]]直接相关，正如我们提到的那样，[[scope]]在函数创建时被存储，与函数共存亡。实际上，闭包是函数代码和其[[scope]]的结合。因此，作为其对象之一，[[Scope]]包括在函数内创建的词法作用域（父变量对象）。当函数进一步激活时，在变量对象的这个词法链（静态的存储于创建时）中，来自较高作用域的变量将被搜寻。

<!--language: !js-->

    var x = 10;

    function foo() {
      alert(x);
    }

    (function () {
      var x = 20;
      foo(); // 10, but not 20
    })();

在标识符解析过程中，使用函数创建时定义的 __词法作用域__－－变量解析为10，而不是30。此外，这个例子也清晰的表明，一个函数（这个例子中为从函数“foo”返回的匿名函数）的[[scope]] __持续存在__，即使是 __在函数创建的作用域已经完成__ 之后。


#### Function创建的函数
在函数创建时获得函数的[[scope]]属性，通过该属性访问到所有父上下文的变量。但是，这个规则有一个重要的例外，它涉及到通过函数构造函数Function创建的函数。

<!--language: !js-->

    var x = 10;

    function foo() {
      var y = 20;

      function barFD() { // 函数声明
        alert(x);
        alert(y);
      }

      var barFE = function () { // 函数表达式
        alert(x);
        alert(y);
      };

      var barFn = Function('alert(x); alert(y);');

      barFD(); // 10, 20
      barFE(); // 10, 20
      barFn(); // 10, "y" is not defined
    }

    foo();

通过函数构造函数（Function constructor）创建的函数“bar”，是不能访问变量“y”的。但这并不意味着函数“barFn”没有[[scope]]属性（否则它不能访问到变量“x”）。问题在于通过函构造函数创建的函数的[[scope]]属性总是唯一的全局对象。考虑到这一点，如通过这种函数创建除全局之外的最上层的上下文闭包是不可能的。

#### 二维作用域链查找
在作用域链中查找最重要的一点是变量对象的属性（如果有的话）须考虑其中－－源于ECMAScript 的 __原型__ 特性。如果一个属性在对象中没有直接找到，查询将在原型链中继续。即常说的二维链查找：

> 1. 作用域链环节；
> 1. 每个作用域链－－深入到原型链环节。如果在Object.prototype 中定义了属性，我们能看到这种效果。

<!--language: !js-->

    function foo() {
      alert(x);
    }

    Object.prototype.x = 10;

    foo(); // 10

通过作用域链找到golbal对象，但不存在“x”，就尝试查找golbal对象的原型链，它从Object.prototype继承而来，相应地，“x”解析为10。

#### 全局和eval中的作用域链
全局上下文的作用域链仅包含全局对象。代码eval的上下文与当前的调用上下文（calling context）拥有同样的作用域链。

<!--language: js-->

    globalContext.Scope = [
      Global
    ];

    evalContext.Scope === callingContext.Scope;

#### 代码执行时对作用域链影响
在代码执行阶段有两个声明能修改作用域链。这就是with声明和catch语句。它们添加到作用域链的最前端，对象须在这些声明中出现的标识符中查找。如果发生其中的一个，作用域链简要的作如下修改：

<!--language: js-->

    Scope = withObject|catchObject + AO|VO + [[Scope]]

在这个例子中添加对象，对象是它的参数（这样，没有前缀，这个对象的属性变得可以访问）。

<!--language: !js-->

    var foo = {x: 10, y: 20};

    with (foo) {
      alert(x); // 10
      alert(y); // 20
    }

作用域链修改成这样：

<!--language: js-->

    Scope = foo + AO|VO + [[Scope]]

我们再次看到，通过with语句，对象中标识符的解析添加到作用域链的最前端：

<!--language: !js-->

    var x = 10, y = 10;

    with ({x: 20}) {

      var x = 30, y = 30;

      alert(x); // 30
      alert(y); // 30
    }

    alert(x); // 10
    alert(y); // 30

在进入上下文时发生了什么？标识符“x”和“y”已被添加到变量对象中。此外，在代码运行阶段作如下修改：

> 1. x = 10, y = 10;
> 1. 对象{x:20}添加到作用域的前端;
> 1. 在with内部，遇到了var声明，当然什么也没创建，因为在进入上下文时，所有变量已被解析添加;
> 1. 在第二步中，仅修改变量“x”，实际上对象中的“x”现在被解析，并添加到作用域链的最前端，“x”为20，变为30;
> 1. 同样也有变量对象“y”的修改，被解析后其值也相应的由10变为30;
> 1. 此外，在with声明完成后，它的特定对象从作用域链中移除（已改变的变量“x”－－30也从那个对象中移除），即作用域链的结构恢复到with得到加强以前的状态。
> 1. 在最后两个alert中，当前变量对象的“x”保持同一，“y”的值现在等于30，在with声明运行中已发生改变。

## 函数

### 函数类型
在ECMAScript 中有三种函数类型：函数声明，函数表达式和函数构造器创建的函数。每一种都有自己的特点。

#### 函数声明
函数声明（缩写为FD）是这样一种函数：

> 1. 有一个特定的名称
> 1. 在源码中的位置：要么处于程序级（Program level），要么处于其它函数的主体（FunctionBody）中
> 1. 在进入上下文阶段创建
> 1. 影响变量对象
以下面的方式声明

<!--language: js-->

    function exampleFunc() {
      ...
    }

这种函数类型的主要特点在于它们仅仅影响变量对象（即存储在上下文的VO中的变量对象）。该特点也解释了第二个重要点（它是变量对象特性的结果）—— __在代码执行阶段它们已经可用__（因为FD在进入上下文阶段已经存在于VO中——代码执行之前）。

函数声明在源码中的位置：

<!--language: js-->

    // 函数可以在如下地方声明：
    // 1) 直接在全局上下文中
    function globalFD() {
      // 2) 或者在一个函数的函数体内
      function innerFD() {}
    }

只有这两个位置可以声明函数，也就是说，__不能在表达式位置或一个代码块中定义__(如if,while,for等)它。

#### 函数表达式
函数表达式（缩写为FE）是这样一种函数：

> 1. 在源码中须出现在表达式的位置
> 1. 有可选的名称
> 1. 不会影响变量对象
> 1. 在代码执行阶段创建

这种函数类型的主要特点在于它在源码中总是处在表达式的位置。最简单的一个例子就是一个赋值声明：

<!--language: js-->

    var foo = function () {
      ...
    };

该例演示是让一个匿名函数表达式赋值给变量foo，然后该函数可以用foo这个名称进行访问——foo()。

同时和定义里描述的一样，函数表达式也可以拥有可选的名称：

<!--language: js-->

    var foo = function _foo() {
      ...
    };

需要注意的是，在外部FE通过变量“foo”来访问——foo()，而在函数内部（如递归调用），有可能使用名称“_foo”。

如果FE有一个名称，就很难与FD区分。但是，如果你明白定义，区分起来就简单明了：FE总是处在表达式的位置。在下面的例子中我们可以看到各种ECMAScript 表达式：

<!--language: js-->

    // 圆括号（分组操作符）内只能是表达式
    (function foo() {});

    // 在数组初始化器内只能是表达式
    [function bar() {}];

    // 逗号也只能操作表达式
    1, function baz() {};

表达式定义里说明：__FE只能在代码 *执行阶段* 创建而且 *不存在于变量对象* 中__，让我们来看一个示例行为：

<!--language: js-->

    // FE在定义阶段之前不可用（因为它是在代码执行阶段创建）
    alert(foo); // "foo" 未定义

    (function foo() {});

    // 定义阶段之后也不可用，因为他不在变量对象VO中
    alert(foo);  // "foo" 未定义

在表达式中使用它们，__不会污染__ 变量对象。最简单的例子是将一个函数作为参数传递给其它函数。

<!--language: !js-->

    function foo(callback) {
      callback();
    }

    foo(function bar() {
      alert('foo.bar');
    });

在上述例子里，FE赋值给了一个变量（也就是参数），函数将该表达式保存在内存中，并通过 __变量名__ 来访问（因为 __变量影响变量对象__ [参考](#TOC1.3.1))，如下：

<!--language: !js-->

    var foo = function () {
      alert('foo');
    };

    foo();

另外一个例子是创建封装的闭包从外部上下文中隐藏辅助性数据（在下面的例子中我们使用FE，它在创建后立即调用）：

<!--language: !js-->

    var foo = {};

    (function initialize() {
      var x = 10;

      foo.bar = function () {
        alert(x);
      };
    })();

    foo.bar(); // 10;
    alert(x); // "x" 未定义

我们看到函数foo.bar（通过[[Scope]]属性）访问到函数initialize的内部变量“x”。同时，“x”在外部不能直接访问。在许多库中，__这种策略常用来创建”私有”数据和隐藏辅助实体__。在这种模式中，初始化的FE的名称(即initialize)通常被忽略：

还有一个例子是：在代码执行阶段通过条件语句进行创建FE，不会污染变量对象VO。

<!--language: !js-->

    var foo = 10;

    var bar = (foo % 2 == 0
      ? function () { alert(0); }
      : function () { alert(1); }
    );

    bar(); // 0

#### 函数构造器创建的函数
既然这种函数对象也有自己的特色，我们将它与FD和FE区分开来。其主要特点在于这种函数的[[Scope]]属性仅包含全局对象：

<!--language: !js-->

    var x = 10;

    function foo() {
      var x = 20;
      var y = 30;

      var bar = new Function('alert(x); alert(y);');

      bar(); // 10, "y" 未定义
    }

我们看到，函数bar的[[Scope]]属性不包含foo上下文的Ao——变量”y”不能访问，变量”x”从全局对象中取得。顺便提醒一句，Function构造器既可使用new 关键字，也可以没有，这样说来，这些变体是等价的。

### 立即调用
“为何在函数创建后的立即调用中必须用圆括号来包围它？”，答案就是：表达式句子的限制就是这样的。

按照标准，表达式语句不能以一个大括号{开始是因为他很难与代码块区分，同样，他也不能以函数关键字开始，因为很难与函数声明进行区分。即，所以，如果我们定义一个立即执行的函数，在其创建后立即按以下方式调用：

<!--language: !js-->

    function () {
      ...
    }();

    // 即便有名称

    function foo() {
      ...
    }();

我们使用了函数声明，上述2个定义，解释器在解释的时候都会 __报错__，但是可能有多种原因。

如果在全局代码里定义（也就是程序级别），解释器会将它看做是函数声明，因为他是以function关键字开头，第一个例子，我们会得到SyntaxError错误，是因为函数声明没有名字（我们前面提到了函数声明必须有名字）。

第二个例子，我们有一个名称为foo的一个函数声明正常创建，但是我们依然得到了一个语法错误——没有任何表达式的分组操作符错误。在函数声明后面他确实是一个分组操作符，而不是一个函数调用所使用的圆括号。所以如果我们声明如下代码：

<!--language: !js-->

    // "foo" 是一个函数声明，在进入上下文的时候创建
    alert(foo); // 函数

    function foo(x) {
      alert(x);
    }(1); // 这只是一个分组操作符，不是函数调用！

    foo(10); // 这才是一个真正的函数调用，结果是10

上述代码是没有问题的，因为声明的时候产生了2个对象：一个函数声明，一个带有1的分组操作，上面的例子可以理解为如下代码：

<!--language: !js-->

    // 函数声明
    function foo(x) {
      alert(x);
    }

    // 一个分组操作符，包含一个表达式1
    (1);

    // 另外一个操作符，包含一个function表达式
    (function () {});

    // 这个操作符里，包含的也是一个表达式"foo"
    ("foo");

我们如果来告诉解释器：我就像在函数声明之后立即调用，答案是很明确的，你得声明函数表达式function expression，而不是函数声明function declaration，并且创建表达式最简单的方式就是用分组操作符括号，里边放入的永远是表达式，所以解释器在解释的时候就不会出现歧义。在代码执行阶段这个的function就会被创建，并且立即执行，然后自动销毁（如果没有引用的话）。

<!--language: !js-->

    (function foo(x) {
      alert(x);
    })(1); // 这才是调用，不是分组操作符

上述代码就是我们所说的在用括号括住一个表达式，然后通过（1）去调用。

注意，下面一个立即执行的函数，周围的括号不是必须的，因为函数已经处在表达式的位置，解析器知道它处理的是在函数执行阶段应该被创建的FE，这样在函数创建后立即调用了函数。

<!--language: !js-->

    var foo = {
      bar: function (x) {
        return x % 2 != 0 ? 'yes' : 'no';
      }(1)
    };

    alert(foo.bar); // 'yes'

就像我们看到的，foo.bar是一个字符串而不是一个函数，这里的函数仅仅用来根据条件参数初始化这个属性——它创建后并立即调用。

因此，“关于圆括号”问题完整的答案如下：__当函数不在表达式的位置的时候，分组操作符圆括号是必须的——也就是手工将函数转化成FE。如果解析器知道它处理的是FE，就没必要用圆括号。__

除了大括号以外，如下形式也可以将函数转化为FE类型，例如：

<!--language: !js-->

    // 注意是1,后面的声明
    1, function () {
      alert('anonymous function is called');
    }();

    // 或者这个
    !function () {
      alert('ECMAScript');
    }();

    // 或者这个
    void function(){
      alert('ECMAScript');
    }();


### 命名函数表达式的特性
当函数表达式FE有一个名称（称为命名函数表达式，缩写为NFE）时，将会出现一个重要的特点。从定义（正如我们从上面示例中看到的那样）中我们知道函数表达式不会影响一个上下文的变量对象（那样意味着既不可能通过名称在函数声明之前调用它，也不可能在声明之后调用它）。但是，FE在递归调用中可以通过名称调用自身。

<!--language: !js-->

    (function foo(bar) {
      if (bar) {
        return;
      }
      foo(true); // "foo" 是可用的
    })();

    // 在外部，是不可用的
    foo(); // "foo" 未定义

“foo”储存在什么地方？__在foo的活动对象中？不是__，因为在foo中没有定义任何”foo”。在上下文的 __父变量对象中创建foo？也不是__，因为按照定义——FE不会影响VO(变量对象)——从外部调用foo我们可以实实在在的看到。那么在哪里呢？

以下是关键点。当解释器在代码执行阶段遇到命名的FE时，在FE创建之前，它创建了 __辅助的特定对象，并添加到当前作用域链的最前端__。然后它创建了FE，此时（正如作用域链知道的那样）函数获取了[[Scope]] 属性——创建这个函数上下文的作用域链）。此后，FE的名称添加到特定对象上作为唯一的属性；这个属性的值是引用到FE上。最后一步是从父作用域链中移除那个特定的对象。让我们在伪码中看看这个算法：

<!--language: js-->

    specialObject = {};

    Scope = specialObject + Scope;

    foo = new FunctionExpression;
    foo.[[Scope]] = Scope;
    specialObject.foo = foo; // {DontDelete}, {ReadOnly}

    delete Scope[0]; // 从作用域链中删除定义的特殊对象specialObject

因此，在函数外部这个名称不可用的（因为它不在父作用域链中），但是，特定对象已经存储在函数的[[scope]]中，在那里名称是可用的。

但是需要注意的是一些实现（如Rhino）不是在特定对象中而是在FE的激活对象中存储这个可选的名称。Microsoft 中的执行完全打破了FE规则，它在父变量对象中保持了这个名称，这样函数在外部变得可以访问。

### 创建函数的算法
下面的伪码描述了函数创建的算法（与联合对象相关的步骤除外）。这些描述有助于你理解ECMAScript中函数对象的更多细节。这种算法适合所有的函数类型。

<!--language: js-->

    F = new NativeObject();

    // 属性[[Class]]是"Function"
    F.[[Class]] = "Function"

    // 函数对象的原型是Function的原型
    F.[[Prototype]] = Function.prototype

    // 医用到函数自身
    // 调用表达式F的时候激活[[Call]]
    // 并且创建新的执行上下文
    F.[[Call]] = <reference to function>

    // 在对象的普通构造器里编译
    // [[Construct]] 通过new关键字激活
    // 并且给新对象分配内存
    // 然后调用F.[[Call]]初始化作为this传递的新创建的对象
    F.[[Construct]] = internalConstructor

    // 当前执行上下文的作用域链
    // 例如，创建F的上下文
    F.[[Scope]] = activeContext.Scope
    // 如果函数通过new Function(...)来创建，
    // 那么
    F.[[Scope]] = globalContext.Scope

    // 传入参数的个数
    F.length = countParameters

    // F对象创建的原型
    __objectPrototype = new Object();
    __objectPrototype.constructor = F // {DontEnum}, 在循环里不可枚举x
    F.prototype = __objectPrototype

    return F


注意，F.[[Prototype]]是函数（构造器）的一个原型，F.prototype是通过这个函数创建的对象的原型（因为术语常常混乱，一些文章中F.prototype被称之为“构造器的原型”，这是不正确的）。



## 原型与原型链


## 执行上下文


基于对象
===========
基于类的语言（如java）中，类型的继承提供了 __代码重用__，与 __类型多态__

1. 对于代码重用，js是基于原型的，意味着对象直接从其他对象继承；
1. 对于类型多态，js是弱类型语言，不需要类型转换，能做什么比它是什么更重要(duck typing)


## 伪类
__不推荐__ 使用`new`去创建对象，因为它不让对象直接从其他对象继承，反而插入了一个多余的间接层，使构造器产生对象

每个一函数被创建时，`Function`构造器产生的函数对象会运行类似这样的代码`this.prototype = {constructor: this};`，该`prototype`对象是存放继承特征的地方。

因为js没提供（标识/关键字）哪些函数是用来作构造器，所以每一个函数都会得到一个`prototype`对象，而且`constructor`属性几乎没什么用

以下代码摘自《JavaScript Good Parts》

<!--language: !js-->

    var Mammal = function (name) {
        this.name = name;
    };
    Mammal.prototype.get_name = function () {
        return this.name;
    };
    Mammal.prototype.says = function () {
        return this.saying || '';
    };

    var Cat = function (name) {
        this.name = name;
        this.saying = 'meow';
    };

    // Replace Cat.prototype with a new instance of Mammal
    Cat.prototype = new Mammal();

    Cat.prototype.get_name = function () {
        return this.says() + ' ' + this.name + ' ' + this.says();
    };
    var myCat = new Cat('Henrietta');
    alert(myCat.says()); // 'meow'
    alert(myCat.get_name()); // 'meow Henrietta meow'

伪类模式本意是向面向对象靠拢，但看起来格格不入，无法访问super类的方法。伪类给不熟悉js的程序员提供便利，但也隐藏了语言的本质。可能误导去编写过于深入与复杂的层次结构（__许多复杂的类层次结构产生的原因是因为静态类型检查的，而js完全没有类型约束__）

### 实现继承机制

以下代码摘自《Pro JavaScript Design Patterns》

<!--language: !js-->

    function extend(subClass, superClass) {
        var F = function() {};
        F.prototype = superClass.prototype;
        subClass.prototype = new F();
        subClass.prototype.constructor = subClass;

        subClass.superclass = superClass.prototype;
        if(superClass.prototype.constructor == Object.prototype.constructor) {
            superClass.prototype.constructor = superClass;
        }
    }

    function BasePiece(name) {
        this.name = name;
    }

    BasePiece.prototype = {
        say: function () {
            return this.name;
        }
    };

    extend(SubPiece, BasePiece);

    function SubPiece(camp, pos) {
        SubPiece.superclass.constructor.call(this, "sub");
    }

    alert((new SubPiece).say());


### new的过程

如果`new`不是运算符，而只是一个方法（类似ruby中的new），它可能是这样执行的

<!--language: !js-->

    Function.prototype.method = function(name, func){
        if(!this.prototype[name]){
            this.prototype[name] = func;
            return this;
        }
    }

    Function.method('new', function () {
        // 创建一个新对象，它继承自构造器的原型对象
        var that = Object.create(this.prototype);

        // 调用构造器，绑定this到新对象上
        var other = this.apply(that, arguments);

        // 如果它的返回值不是一个对象，就返回该新对象
        return (typeof other === 'object' && other) || that;
    });

    function Person(name){
        this.name =name;
    }
    var p = Person.new("zyx");
    debugger;


或者可以把`var p = new Person();`的过程拆分成以下三步：

1. `var p = {};` 初始化一个对象`p`
1. `p.__proto__ = Person.prototype;` 实例的`__proto__`正巧指向构造器的`prototype`上，通过此找到原型（ES6中即`Object.setPrototypeOf(p, Person.prototype)`）
1. `Person.apply(p, arguments);` 构造`p`，修正`this`指向，使实例得到构造器定义的相关属性/方法

## 原型
每个对象都连接到一个原型对象，并且它可以从中继承属性，所有通过对象字面量创建的对象都连接到`Object.prototype`这个标准对象

### 创建新对象
当你创建一个新对象时，可以选择某个对象作为它的原型，Js提供的实现机制杂乱而复杂，但其实可以被简化。我们将给`Object`增加一个`beget`方法，这个`beget`方法创建一个使用原对象作为其原型的新对象：

<!--language: !js-->

    if(typeof Object.beget != 'function'){
        Object.beget = function(o){
            var F = function(){};
            F.prototype = o;
            return new F();
        }
    }

    var obj1 = new (function(){this.name='obj1';});
    var obj2 = Object.beget(obj1);
    alert(obj2.name);
    alert(obj2.constructor);

    var obj3 = Object.create(obj1);
    alert(obj3.name);
    alert(obj3.constructor);

    obj1.age = 10;
    alert(obj2.age);

> ES5中的`Object.create`已有支持，缺点是新对象的`constructor`指向原型对象的构造器

原型连接在更新时是不起作用的，当我们对某个对象做出改变时，不会触及到该对象的原型，原型连接只有在检索值的时候才被用到。

在一个纯粹的原型模式中，__摒弃类，转而专注于对象__。基于原型的继承相比基于类的继承在概念上更为简单：一个新对象可以继承一个旧对象的属性。

__通过构造一个有用的对象开始，接着可以构造（`Object.create`）更多的和那个对象类似的对象，完全避免把一个应用拆解成一系统嵌套抽象类的分类过程__

<!--language: !js-->

    var myMammal = {
        name : 'Herb the Mammal',
        get_name : function () {
            return this.name;
        },
        says : function () {
            return this.saying || '';
        }
    };

    var myCat = Object.create(myMammal);

    myCat.name = 'Henrietta';
    myCat.saying = 'meow';
    myCat.get_name = function () {
        return this.says() + ' ' + this.name + ' ' + this.says();
    };

    alert(myCat.get_name()); // 'meow Henrietta meow'

这是一种 __差异化继承__，通过定制一个新的对象，指名了它与所基于的基本对象的区别。

## 信息隐藏
上面的继承模式没法 __保护隐私__，可选择方式的有 __模块化__ 和 __闭包__

以下通过把`constructor`返回对象的私有属性/方法，封装到单独模块`spec`中

<!--language: js-->

    var constructor = function (spec, my) {
        var that, other private instance variables;
        my = my || {};
        Add shared variables and functions to my
        that =a new object;
        Add privileged methods to that
        return that;
    };

示例：

<!--language: !js-->

    var mammal = function (spec) {
        var that = {};
        that.get_name = function () {
            return spec.name;
        };
        return that;
    };
    var myMammal = mammal({name: 'Herb'});
    alert(myMammal.get_name());

保护了`mammal`的`name`

<!--language: !js-->

    var mammal = function (spec) {
        var that = {};
        that.get_name = function () {
            return spec.name;
        };
        that.says = function () {
            return spec.saying || '';
        };
        return that;
    };

    var cat = function (spec) {
        spec.saying = spec.saying || 'meow';
        var that = mammal(spec);  // 继承
        that.get_name = function () {
            return that.says() + ' ' + spec.name + ' ' + that.says();
        };
        return that;
    };
    var myCat = cat({name: 'Henrietta'});
    alert(myCat.get_name());

这种模式还提供了一个处理父类方法的方法

<!--language: !js-->

    Function.prototype.method = function(name, func){
        if(!this.prototype[name]){
            this.prototype[name] = func;
            return this;
        }
    };

    Object.method('superior', function (name) {
        var that = this,
            method = that[name];
        return function () {
            return method.apply(that, arguments);
        };
    });

    var mammal = function (spec) {
        var that = {};
        that.get_name = function () {
            return spec.name;
        };
        return that;
    };

    var coolcat = function (spec) {
        var that = mammal(spec),
            super_get_name = that.superior('get_name');
        that.get_name = function (n) {
            return 'like ' + super_get_name() + ' baby';
        };
        return that;
    };
    var myCoolCat = coolcat({name: 'Bix'});
    alert(myCoolCat.get_name());


## 类型判断

### typeof运算符

js是弱类型的，具有6种基本数据类型，任何一个变量或值的类型都可以使用 __`typeof`__ 运算符来得到，以字符串形式返回此6种类型值之一：

> - undefined
> - number
> - string
> - boolean
> - function 具有多种含义：函数、方法、构造器、类、函数对象等
> - object 基于原型继承的面向对象

通过typeof运算考察变量时，要么是对象(`object`)，要么是非对象(`number`,`undefined`,`string`等)

其中`function`,`object`为引用类型，其它均为值类型，包括`undefined`，但`string`在赋值运算中会按引用类型方式来处理

以下是特殊几个值的`typeof`的结果

<!--language: !js-->

    [null, undefined, NaN, String(""), Number(0),
     Boolean(false), function(){}, new Function()].forEach(function(item){
        console.log(typeof item);
        //object,undefined,number,string,number,boolean,function
    });

### instanceof运算符
对象是不是另一个类的实例，使用`instanceof`运算符，会检测类的继承关系，因此一个子类的实例，在对祖先类做`instanceof`运算时，仍然得到`true`

以下是特殊几个值的`instanceof`的结果

<!--language: !js-->

    // all false
    [null, undefined, "", String(""),
     0, Number(0), false, Boolean(false)].forEach(function(item){
        console.log(item instanceof Object);
    });

    console.log("----");

    console.log(NaN instanceof Number); //false
    console.log(new String instanceof String); //true
    console.log([] instanceof Array); //true

    console.log("----");

    // all true
    [new String, new Number, new Boolean,
     [], new Array, /\s/i, new RegExp].forEach(function(item){
        console.log(item instanceof Object);
    });

### is[Type]的判断
摘自underscore.js

<!--language: js-->

    // Is a given array, string, or object empty?
    // An "empty" object has no enumerable own-properties.
    _.isEmpty = function(obj) {
      if (obj == null) return true;
      if (_.isArray(obj) || _.isString(obj)) return obj.length === 0;
      for (var key in obj) if (_.has(obj, key)) return false;
      return true;
    };

    // Is a given value a DOM element?
    _.isElement = function(obj) {
      return !!(obj && obj.nodeType === 1);
    };

    // Is a given value an array?
    // Delegates to ECMA5 s native Array.isArray
    _.isArray = nativeIsArray || function(obj) {
      return toString.call(obj) == '[object Array]';
    };

    // Is a given variable an object?
    _.isObject = function(obj) {
      return obj === Object(obj);
    };

    // Add some isType methods: isArguments, isFunction, isString, isNumber, isDate, isRegExp.
    each(['Arguments', 'Function', 'String', 'Number', 'Date', 'RegExp'], function(name) {
      _['is' + name] = function(obj) {
        //避免对象重新定义toString情况下命名冲突，仍然使用原始的定义
        return toString.call(obj) == '[object ' + name + ']';
      };
    });

    // Define a fallback version of the method in browsers (ahem, IE), where
    // there isn t any inspectable "Arguments" type.
    if (!_.isArguments(arguments)) {
      _.isArguments = function(obj) {
        return !!(obj && _.has(obj, 'callee'));
      };
    }

    // Optimize `isFunction` if appropriate.
    if (typeof (/./) !== 'function') {
      _.isFunction = function(obj) {
        return typeof obj === 'function';
      };
    }

    // Is a given object a finite number?
    _.isFinite = function(obj) {
      return isFinite(obj) && !isNaN(parseFloat(obj));
    };

    // Is the given value `NaN`? (NaN is the only number which does not equal itself).
    _.isNaN = function(obj) {
      return _.isNumber(obj) && obj != +obj;
    };

    // Is a given value a boolean?
    _.isBoolean = function(obj) {
      return obj === true || obj === false || toString.call(obj) == '[object Boolean]';
    };

    // Is a given value equal to null?
    _.isNull = function(obj) {
      return obj === null;
    };

    // Is a given variable undefined?
    _.isUndefined = function(obj) {
      return obj === void 0;
    };


动态性
==========

对象的字面量产生的对象连接到`Object.prototype`，函数对象连接到`Function.prototype`（该原型对象本身连接到`Object.prototype`），每个函数在创建时附有两个附加的隐藏属性：函数的上下文和实现函数行为的代码

<!--language: !js-->

    function f(){}
    alert(f.constructor === Function);
    alert(f.__proto__ === Function.prototype);

每个函数对象在创建时也随带一个`prototype`属性，它是值是一个拥有`constructor`属性且值即为该函数的对象

<!--language: !js-->

    function f(){}
    alert(f.prototype); // like {constructor: f}
    alert(f.prototype.constructor === f);


函数可以被定义在其他函数中，一个内部函数也能方便地访问它被嵌套在其中的那个函数的 __参数__ 与 __变量__，但不能访问`this`和`arguments`，通过函数字面量创建的函数对象包含一个连接到外部上下文的连接，称为 __闭包__，它是js强大表现力的根基。


## 函数调用方式
js中共有多种调用模式：方法调用模式、函数调用模式、构造器调用模式、`apply`调用模式，这些模式在如何初始化关键参数`this`上存在差异

### 方法调用模式
当一个函数被保存为对象的一个属性时，我们称为它为一个 __方法__，当一个方法被调用时，`this`被绑定到该对象。如果一个调用表达式包含一个属性存取表达式(即`.`或`[]`)，那么它被当作一个方法来调用。

<!--language: !js-->

    var myObject = {
        value: 0;
        increment: function(inc){
            this.value += typeof inc === 'number' ? inc : 1;
        }
    };
    myObject.increment();
    alert(myObject.value); // 1

    myObject.increment(2);
    alert(myObject.value); // 3

方法可以使用`this`去访问对象，所以它能从对象中取值或修改该对象，`this`到对象的 __绑定发生在调用__ 的时候，这个“超级”迟绑定(very late binding)使得函数可以对`this`高度复用。通过`this`可以取得它们所属对象的上下文的方法称为公共方法

### 函数调用模式
当一个函数并非一个对象的属性时，那么它被当作一个函数来调用，当函数以此模式调用时，__`this`被绑定到全局对象__，如果想让`this`绑定外部函数的`this`变量，有一个容易的解决方案：

<!--language: !js-->

    myObject = {value: 3};
    myObject.double = function () {
        var that = this;
        var helper = function () {
            that.value *= 2;
        };
        helper();
    };

    myObject.double();
    alert(myObject.value); // 6

### 构造器调用模式
如果在一个函数前面带上`new`来调用，那么将创建一个隐藏连接到该函数的`prototype`成员的新对象，同时`this`将会被绑定到那个新对象上

<!--language: !js-->

    var Quo = function (string) {
        this.status = string;
    };

    Quo.prototype.get_status = function () {
        return this.status;
    };

    var myQuo = new Quo("confused");
    alert(myQuo.get_status());

结合`new`前缀调用的函数称为构造器函数，以前字母大写格式命名，如果调用构造器函数时没加`new`，则可能发生问题

#### 构造器中包含return
`new`构造器时，含`return`语句时的行为

<!--language: !js-->

    function Cat(){this.name='cat';}
    function Animal(){ return new Cat;}

    var a =new Animal;
    alert(a.constructor); // function Cat(){this.name='cat';}
    alert(a.__proto__); // Cat {}

下面是`underscore.js`中一段代码

<!--language: !js-->

    var _ = function(obj) {
        if (obj instanceof _) return obj;
        if (!(this instanceof _)) return new _(obj);
        this._wrapped = obj;
    };

### apply/call调用模式
因为Js是一门函数式的面向对象编程语言，所以 __函数也可以拥有方法__

`apply`方法让我们构建一个参数数组并用其去调用函数，也允许我们 __选择`this`__ 的值：

<!--language: !js-->

    var obj = {name: "obj"};

    function getStatus(name){
        return this.name + ' A-OK';
    }
    alert(getStatus.apply(obj));

一个常用的技巧，把`arguments`改造成数组，它只是array-like，现在把它变成真正数组，方便做一些集合的处理

<!--language: !js-->

    function func(){
        var args = Array.prototype.slice.apply(arguments);
        args.forEach(function(arg){alert(arg);});
    }

    func(1,2,"a","b");

#### `apply` 与 `call`

当关心`this`的指向时，两者没有区别；但如果涉及对`arguments`相关操作时，优先使用`apply`：

- 示例1

`underscore.js`存在一个方法`without`，用于过滤集合中不需要的元素

<!--language: !js-->

    //import underscore.1.4.3
    var seq = _([1,3,4,5,7]);
    var result = seq.without(3,5,7,8);
    result.forEach(function(arg){alert(arg);});

由于`without`接受的是不定的参数，当不需要的元素已经存在于一个数组中时：

<!--language: !js-->

    //import underscore.1.4.3
    var seq = _([1,3,4,5,7]);
    var notNeed = [3,5,7,8];
    var result = seq.without.apply(seq,notNeed);
    result.forEach(function(arg){alert(arg);});

通过`apply`将 __数组参数展开传递__。另一个例子：

<!--language: !js-->

    var arr=[1,[2,3],[[4]],[[5,[6]]]];

    function flatten(seq){
        if(!(seq instanceof Array)) return [seq];
        return Array.prototype.concat.apply([],seq.map(flatten))
    }

    alert(flatten(arr));


- 示例2

对不固定参数求最大值的函数`Math.max`

<!--language: !js-->

    var arr = [1,2,7,4,5];
    alert(Math.max.apply(Math, arr));

`apply`可以理解为，将此 __函数作用于一个数组__ 上。原理同上例一样，即将数组参数展开传递。

- 示例3

利于保证`arguments`的传递

<!--language: !js-->

    function fn1() {
        function _delegate(arg){
            alert(arg);
        }
        return function () {
            _delegate.apply(null,arguments);
        };
    }

    fn1()(1);


### bind调用模式

`Function.prototype.bind(thisArg [, arg1 [, arg2, …]])` __返回一个新的函数对象__，该函数 __对象的this__ 绑定到了thisArg参数上。

它的代码（摘自underscore.js）类似：

<!--language: js-->

    function bind(func, context) {
        if (!context) return func;
        var args = _.toArray(arguments).slice(2);
        return function() {
            var a = args.concat(_.toArray(arguments));
            return func.apply(context, a);
        };
    }

示例：

<!--language: !js-->

    // Define the original function.
    var checkNumericRange = function (value) {
        if (typeof value !== 'number')
            return false;
        else
            return value >= this.minimum && value <= this.maximum;
    }

    // The range object will become the this value in the callback function.
    var range = { minimum: 10, maximum: 20 };

    // Bind the checkNumericRange function.
    var boundCheckNumericRange = checkNumericRange.bind(range);

    // Use the new function to check whether 12 is in the numeric range.
    var result = boundCheckNumericRange (12);
    alert(result);

    // Output: true


#### Function.prototype.call.bind

js中，经常需要在方法调用时传入"静态方法"的回调函数，有时存在的是"实例方法"（如下面的`String.prototype.trim`），可使用`bind`使其变成"静态方法"

<!--language: !js-->

    var arr = [" a ","  b  "," c "];

    //传统方法
    var result1 = arr.map(function(item){return item.trim();});
    alert(result1);

    //使用bind
    var result2 = arr.map(Function.prototype.call.bind(String.prototype.trim));
    alert(result2);

    //上面的bind类似于下面的call/apply
    var result3 = arr.map((function(){
            return function(){
                return String.prototype.trim.call(arguments[0]);
            }
        })()
    );
    alert(result3);


下面有关[闭包中作用域的例子](#TOC3.2.4)，返回的是一个函数数组，需要依次调用每个函数，还可以写成：

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

    fn().forEach(Function.prototype.call.bind(Function.prototype.call));


#### curry参数
参见[柯里化](#TOC3.4)

## 扩展性

### 给类型增加方法

js允许给语言的基本类型增加方法，通过给`Object.prototype`添加方法来使得该方法对所有对象可用。

通过给`Function.prototype`增加方法来使得该方法对所有函数可用：

<!--language: !js-->

    Function.prototype.method = function(name, func){
        if(!this.prototype[name]){
            this.prototype[name] = func;
            return this;
        }
    }

    Number.method('integer', function(){
       return Math[this < 0 ? 'ceil' : 'floor'](this);
    });
    alert((10/3.0).integer());
    alert((-10/3.0).integer());

    String.method('trim', function(){
        return this.replace(/^\s+|\s+$/g, '');
    });
    alert("   neat   ".trim());

### 从源对象拷贝属性
它的原型是`_.extend(destination, *sources) `，摘自underscore.js

<!--language: !js-->

    function extend(obj) {
        Array.prototype.slice.call(arguments, 1).forEach(function(source) {
            if (source)
                for (var prop in source)
                    obj[prop] = source[prop];
        });
        return obj;
    }

    var ret = extend({name : 'moe'}, {age : 50}, {name : 'moe2', gender : 'F'});
    console.log(ret); //{name: "moe2", age: 50, gender: "F"}


### 装饰器
摘自underscore.js

<!--language: !js-->

    function wrap(func, wrapper) {
        return function() {
            var args = [func];
            Array.prototype.push.apply(args, arguments);
            return wrapper.apply(this, args);
        };
    }

    var hello = function(name) { return "hello: " + name; };
    hello = wrap(hello, function(func) {
        return "before, " + func("moe") + ", after";
    });
    alert(hello());



## 链式代码
一些设置或修改对象的某个状态却不返回任何值的方法，但如果我们让这些方法返回`this`，而不是`undefined`，就可以启用级联。

示例，underscore.js中原本应用于集合的函数，变成面向对象式+链式的风格：

<!--language: !js-->

    //import underscore.1.4.3
    var stooges = [
        {name: 'curly', age: 25},
        {name: 'moe', age: 21},
        {name: 'larry', age: 23}
    ];
    var youngest = _.chain(stooges)
        .sortBy(function(stooge){ return stooge.age; })
        .map(function(stooge){ return stooge.name + ' is ' + stooge.age; })
        .first()
        .value();

    alert(youngest);

实现的方式如下：

<!--language: js-->

      // Return a sorted list of the function names available on the object.
      // Aliased as `methods`
      _.functions = _.methods = function(obj) {
          var names = [];
          for (var key in obj) {
              if (_.isFunction(obj[key])) names.push(key);
          }
          return names.sort();
      };

    // Add your own custom functions to the Underscore object.
    _.mixin = function(obj) {
      each(_.functions(obj), function(name){
        var func = _[name] = obj[name];
        _.prototype[name] = function() {
          var args = [this._wrapped];
          push.apply(args, arguments);
          return result.call(this, func.apply(_, args));
        };
      });
    };

    var result = function(obj) {
        return this._chain ? _(obj).chain() : obj;
    };

    // Add all of the Underscore functions to the wrapper object.
    _.mixin(_);


另，jquery的链式风格编程已十分流行。为了防止异步callback嵌套过多，也有通过链式代码进行进改的，如jquery.deferred。

## 缓存
用对象去记住先前操作的结果，从而避免无谓的运算。适用于前效相关性，且前效相关顺序确定的情境。

<!--language: !js-->

    var memoizer = function (memo, formula) {
        var recur = function (n) {
            var result = memo[n];
            if (typeof result !== 'number') {
                result = formula(recur, n);
                memo[n] = result;
            }
            return result;
        };
        return recur;
    };

    var fibonacci = memoizer([0, 1], function (recur, n) {
        return recur(n - 1) + recur(n - 2);
    });
    alert(fibonacci(10));

    var factorial = memoizer([1, 1], function (recur, n) {
        return n * recur(n - 1);
    });
    alert(factorial(5));

以下摘自underscore.js

<!--language: js-->

    _.memoize = function(func, hasher) {
        var memo = {};
        hasher || (hasher = _.identity);
        return function() {
            var key = hasher.apply(this, arguments);
            return _.has(memo, key) ? memo[key] : (memo[key] = func.apply(this, arguments));
        };
    };




函数式
============

## 高阶函数
在数学和计算机科学中，__高阶函数__(high-order function, HOF)是至少满足下列一个条件的函数：

> - 接受一个或多个函数作为输入
> - 输出一个函数

js中函数第一公民，高阶函数自然支持，甚至下面的其他特性，都离不开它的支持。

不存在Haskell与Python中的列表解析，但`Array.protype`中的函数对集合处理还是很方便的

<!--language: !js-->

    alert([1, 2, 3].map(function(num){ return num*2; }));


不支持lambda，但借助一些库（如[Functional Javascript](http://osteele.com/sources/javascript/functional/)，[lambda.js](http://www.javascriptoo.com/lambda-js)）可实现相关功能，或直接使用[CoffeScript](http://coffeescript.org/)

## 模式匹配
原生的js不支持，但基于js的[LiveScript](http://livescript.net/)是支持的


## 闭包
闭包是代码块和创建该代码块的上下文中数据的结合。

### 闭包与高阶函数

<!-- http://www.cnblogs.com/TomXu/archive/2012/01/31/2330252.html -->





### 生命周期与内存泄漏


### 信息隐藏与保持状态
函数内部的局部变量可以被修改，而且当再次进入到函数内部的时候，上次被修改的状态仍然持续，此为 __信息隐藏/封装对象状态__ 的惯用法。

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

### 作用域与变量共享

作用域控制着变量的可见性及生命周期，js中 __没有块级作用域__（如for/if并不能创建一个局部的上下文，仅有函数能够创建新的作用域）。它会导致在闭包中行为与有块级作用域的语言中表现不同：

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

## 递归/reduce
一些语言提供了 __尾递归__(tail recursion/trai-end recursion) 优化，即如果一个函数的最后执行递归调用语句的特殊形式的递归，那么调用的过程会被替换为一个循环，可以提高速度。但js __并没有提供__ 该优化。

`reduce`或`flod`对尾递归情形的一种模式固定 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#关键字_fold)

> ES5中的`Array.prototype.reduce`已有支持

<!--language: !js-->

    alert([1, 2, 3].reduce(function(memo, num){ return memo + num; }, 0));


## 柯里化
函数也是值，柯里化`Curry`允许我们将函数与传递它的参数相结合去产生一个新的函数 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#Curried_functions)

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


## 函数组合
组合函数三个函数`f()`, `g()`与`h()`，执行效果与`f(g(h()))`等价，能减少括号的使用，减少参数赋值，能对函数对象组合后再调用，增加流程的灵活性 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#Function_composition)，函数组合是右结合的

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

## 惰性求值

假设我们对包含1000个整数的数组进行一些操作，underscore.js版本，会生成许多巨大的中间数组：

<!--language: js-->

    var result = _.chain(array).map(square).map(inc).filter(isEven).take(5).value();

[Lazy.js](http://danieltao.com/lazy.js)版本，不会生成中间数组。

<!--language: !js-->

    //import lazy.0.3.2
    var array = Lazy.range(1000).toArray();
    function square(x) { return x * x; }
    function inc(x) { return x + 1; }
    function isEven(x) { return x % 2 === 0; }

    var result = Lazy(array).map(square).map(inc).filter(isEven).take(5);
    alert(result.toArray());

它返回的不是数组，而是带`each`方法的序列对象（生成器）。它不会生成中间数组，而且只有在调用`each`方法的时候才产生迭代

基于Lazy.js，还能生成无穷序列、实现异步迭代


异步编程
=========
## 延迟执行

### delay
`delay(function, wait, [*arguments])`

类似setTimeout，等待wait毫秒后调用function。如果传递可选的参数arguments，当函数function执行时， arguments 会作为参数传入。

<!--language: !js-->

    function delay(func, wait) {
        var args = Array.prototype.slice.call(arguments, 2);
        return setTimeout(function(){ return func.apply(null, args); }, wait);
    }

    delay(alert, 1000, 'logged later');

### defer
`defer(function, *arguments)`

延迟调用function直到当前调用栈清空为止，类似使用延时为0的setTimeout方法。对于执行开销大的计算和无阻塞UI线程的HTML渲染时候非常有用。 如果传递arguments参数，当函数function执行时， arguments 会作为参数传入。

<!--language: !js-->

    var slice = Array.prototype.slice;
    function delay(func, wait) {
        var args = slice.call(arguments, 2);
        return setTimeout(function(){ return func.apply(null, args); }, wait);
    }

    function defer(func) {
        return delay.apply(null, [func, 1].concat(slice.call(arguments, 1)));
    }

    for(var i=0;i<5;i++)
        defer(function(i){console.log(i);}, i);

### throttle
`throttle(function, wait)`

创建并返回一个像节流阀一样的函数，当重复调用函数的时候，__最多每隔 wait毫秒调用一次该函数__。 对于想控制一些触发频率较高的事件有帮助。再说的通俗一点就是函数调用的频度控制器，是连续执行时间间隔控制。主要应用的场景比如：

> 1. 鼠标移动，mousemove 事件
> 1. DOM 元素动态定位，window对象的resize和scroll 事件

默认情况下，throttle将在你调用的第一时间尽快执行这个function

以下摘自underscore.js 1.2，最新版本有变化：

<!--language: !js-->

    function throttle(func, wait) {
        var timeout;
        return function() {
            var context = this, args = arguments;
            var throttler = function() {
                timeout = null;
                func.apply(context, args);
            };
            if (!timeout) timeout = setTimeout(throttler, wait);
        };
    }

    // test
    var calltimes = 0;
    var fn = throttle(function(){console.log("real run " + new Date());}, 2000);

    (function(){
        if(calltimes<5){
            calltimes++;
            console.log("call times " + calltimes +" " +new Date());
            fn();
            setTimeout(arguments.callee,1000);
        }
    })();


### debounce
`debounce(function, wait)`

debounce和throttle很像，debounce是空闲时间必须大于或等于 一定值的时候，才会执行调用方法。`debounce`返回 function 函数的防反跳版本, 将延迟函数的执行(真正的执行)在 __函数最后一次调用时刻的 `wait` 毫秒之后__。

对于必须在一些输入（多是一些用户操作）停止到达之后执行的行为有帮助。

> 1. 渲染一个Markdown格式的评论预览, 当窗口停止改变大小之后重新计算布局
> 1. 在类似不小心点了提交按钮两下而提交了两次的情况下很有用，或防止Ajax在很短时间内的多次请求。
> 1. 比如我们做autocomplete，这时需要我们很好的控制输入文字时调用方法时间间隔。一般时第一个输入的字符马上开始调用，根据一定的时间间隔重复调用执行的方法。对于变态的输入，比如按住某一个建不放的时候特别有用

underscore.js有对throttle和debounce的封装。jQuery也有一个throttle和debounce的插件：jQuery throttle / debounce

以下摘自underscore.js 1.2，最新版本有变化，[underscore.js更详细的演进](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fraw.github.com%2Fzyxstar%2Fmd_note%2Fmaster%2Fdocs%2FFramework%2FUnderscore_throttle.md)：

<!--language: !js-->

    function debounce(func, wait) {
        var timeout;
        return function() {
            var context = this, args = arguments;
            var later = function() {
                timeout = null;
                func.apply(context, args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }

    // test
    var calltimes = 0;
    var fn = debounce(function(){console.log("real run " + new Date());}, 2000);

    (function(){
        if(calltimes<5){
            calltimes++;
            console.log("call times " + calltimes +" " +new Date());
            fn();
            setTimeout(arguments.callee,1000);
        }
    })();

### once
`once(function)`

创建一个只能调用一次的函数。重复调用改进的方法也没有效果，只会返回第一次执行时的结果。作为初始化函数使用时非常有用, 不用再设一个boolean值来检查是否已经初始化完成（高阶+闭包真是好东西）。

<!--language: !js-->

    function once(func) {
        var ran = false, memo;
        return function() {
            if (ran) return memo;
            ran = true;
            memo = func.apply(this, arguments);
            func = null;
            return memo;
        };
    }

    var times = 0;
    var createApplication = function(){
        times++;
        alert("create " + times);
    }
    var initialize = once(createApplication);
    initialize();
    initialize();

### after
`after(count, function)`

创建一个函数, 只有在运行了`count`次之后才有效果。在处理同组异步请求返回结果时, 如果你要确保同组里所有异步请求完成之后才执行这个函数, 这将非常有用。有点像多线程中信号量的`WaitAll`

<!--language: !js-->

    function after(times, func) {
        return function() {
            if (--times < 1) {
                return func.apply(this, arguments);
            }
        };
    }

    var notes = [1,2,3,4,5];
    var render = function(){
        alert(notes);
    }
    var renderNotes = after(notes.length, render);
    notes.forEach(function(note) {
        //note.asyncSave({success: renderNotes});
        setTimeout(renderNotes, 500);
    });
    // renderNotes is run once, after all notes have saved.


## jquery.deferred


## 反应型编程(FRP)

<!-- 权威指南 bigpipe template qunit-->

模块管理
==========

## require.js


## sea.js


项目构建
========

build: mini + map

test: qunit jslint jshint JSLitmus

package.json
bower.json
component.json

DocumentCloud

默认值规范

<!-- http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html -->

<script>

(function fix_toc(){
    if(typeof expand_toc !== 'function') setTimeout(fix_toc,500);
    else expand_toc('md_toc',2);
})();

</script>




