> 2013-06-18

语言核心
=========

## 执行上下文
执行上下文可以抽象为一个简单的对象。每个上下文包含一系列属性(我们称之为 上下文状态(context’s state) ) 用以跟踪相关代码的执行过程。下图展示了上下文的结构，所需要的属性([变量对象(variable object)](#TOC1.2)，[this指针(this value)](#TOC1.4)，[作用域链(scope chain)](#TOC1.5)：

![js_execution_context](../../../imgs/js_execution_context.png)

> 本部分主要摘自[Stoyan Stefanov](http://hub.tutsplus.com/authors/stoyan-stefanov)，[zhangxinxu](http://www.zhangxinxu.com/)，[TomXu](http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html)相关内容

### 执行上下文堆栈
每次当控制器转到ECMAScript可执行代码的时候，即会进入到一个执行上下文。执行上下文(简称-EC)是ECMA-262标准里的一个抽象概念，用于同可执行代码(executable code)概念进行区分。

在ECMASscript中的代码有三种类型：`global`, `function`和`eval`。

每一种代码的执行都需要依赖自身的上下文。当然`global`的上下文可能涵盖了很多的`function`和`eval`的实例。函数的每一次调用，都会进入函数执行中的上下文,并且来计算函数中变量等的值。`eval`函数的每一次执行，也会进入`eval`执行中的上下文，判断应该从何处获取变量的值。

注意，一个`function`可能产生无限的上下文环境，因为一个函数的调用（甚至递归）都产生了一个新的上下文环境。

<!--language: js-->

    function foo(bar) {}

    // 调用相同的function，每次都会产生3个不同的上下文
    //（包含不同的状态，例如参数bar的值）
    foo(10);
    foo(20);
    foo(30);

一个执行上下文可以激活另一个上下文，就好比一个函数调用了另一个函数(或者全局的上下文调用了一个全局函数)，然后一层一层调用下去。逻辑上来说，这种实现方式是栈，我们可以称之为上下文堆栈，栈中每一个对象即为一个执行上下文。


我们可以定义执行上下文堆栈是一个数组：

<!--language: js-->

    ECStack = [];

每次进入`function` (即使`function`被递归调用或作为构造函数) 的时候或者内置的`eval`函数工作的时候，这个堆栈都会被压入。

#### 全局代码
这种类型的代码是在"程序"级处理的：例如加载外部的js文件或者本地`<script></script>`标签内的代码。全局代码不包括任何`function`体内的代码。

在初始化（程序启动）阶段，ECStack是这样的：

<!--language: js-->

    ECStack = [
      globalContext
    ];

#### 函数代码
当进入`funtion`函数代码(所有类型的funtions)的时候，ECStack被压入新元素。需要注意的是，具体的函数代码不包括内部函数(inner functions)代码。如下所示，我们使函数自己调自己的方式递归一次：

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

每次`return`的时候，都会退出当前执行上下文的，相应地ECStack就会弹出，栈指针会自动移动位置，这是一个典型的堆栈实现方式。一个抛出的异常如果没被截获的话也有可能从一个或多个执行上下文退出。相关代码执行完以后，ECStack只会包含全局上下文(global context)，一直到整个应用程序结束。

#### `eval`代码
`eval` 代码有点儿意思。它有一个概念：__调用上下文__(calling context),例如，`eval`函数调用的时候产生的上下文。`eval`(变量或函数声明)活动 __会影响__ 调用上下文。

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



## 变量对象与活动对象
声明函数和变量，以成功构建我们的系统，解释器是如何并且在什么地方去查找这些函数和变量呢？任何`function`在执行的时候都会创建一个 __执行上下文__，因为为`function`声明的变量和`function`有可能只在该`function`内部，这个上下文，在调用`function`的时候，提供了一种简单的方式来创建自由变量或私有子`function`。

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

全局对象初始创建阶段将`Math`、`String`、`Date`、`parseInt`作为自身属性，等属性初始化，同样也可以有额外创建的其它对象作为属性（其可以指向到全局对象自身）。例如，在DOM中，全局对象的`window`属性就可以引用全局对象自身

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

为什么第一个`console.log(x)` 的返回值是function，而且它还是在“x” 声明之前访问的“x” 的？为什么不是10或20呢？因为，根据规范函数声明是在当 __进入上下文__ 时填入的； 同一周期，在进入上下文的时候还有一个变量声明“x”，那么正如我们在上一个阶段所说，变量声明在顺序上跟在函数声明和形式参数声明之后，而且在这个 __进入上下文阶段，变量声明不会干扰VO中已经存在的同名函数声明或形式参数声明__，因此，在进入上下文时，VO的结构如下：

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

我们可以在第二、三个`console.log`中看到这个效果。

在下面的例子里我们可以再次看到，变量是在进入上下文阶段放入VO中的。(因为，虽然`else`部分代码永远不会执行，但是不管怎样，变量“b”仍然存在于VO中。)

<!--language: !js-->

    if (true) {
      var a = 1;
    } else {
      var b = 2;
    }

    alert(a); // 1
    alert(b); // undefined,不是b没有声明，而是b的值是undefined

## `this`关键字
`this`是执行上下文中的一个属性：

<!--language: js-->

    activeExecutionContext = {
      VO: {...},
      this: thisValue
    };

这里VO是我们前一章讨论的变量对象。

`this`与上下文中可执行代码的类型有直接关系，`this`值在进入上下文时确定，并且在上下文运行期间永久不变。

### 全局代码中`this`
在这里一切都简单。在全局代码中，this始终是全局对象本身，这样就有可能间接的引用到它了。

### 函数代码中`this`
它不是静态的绑定到一个函数，`this`是 __进入上下文时确定__，在一个函数代码中，这个值在每一次完全不同。不管怎样，在代码运行时的`this`值是不变的，也就是说，因为它不是一个变量，就不可能为其分配一个新值（相反，在Python编程语言中，它明确的定义为对象本身，在运行期间可以不断改变）。

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

影响了函数代码中`this`值的变化有几个因素：

首先，在通常的函数调用中，__`this`是由激活上下文代码的调用者来提供的__，即调用函数的父上下文(parent context )。`this`取决于调用函数的方式。为了在任何情况下准确无误的确定`this`值，有必要理解和记住这重要的一点。正是调用函数的方式影响了调用的上下文中的`this`值，没有别的什么

即使是正常的全局函数也会被调用方式的不同形式激活，这些不同的调用方式导致了不同的this值。

<!--language: !js-->

    function foo() {
      alert(this);
    }

    foo(); // global

    alert(foo === foo.prototype.constructor); // true

    // 但是同一个function的不同的调用表达式，this是不同的
    foo.prototype.constructor(); // foo.prototype

有可能作为一些对象定义的方法来调用函数，但是`this`将不会设置为这个对象。

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

那么，调用函数的方式如何影响`this`值？为了充分理解`this`值的确定，需要详细分析其内部类型之一—— __引用类型__（Reference type）。

### 引用类型
使用伪代码我们可以将引用类型的值可以表示为拥有两个属性的对象——`base`（即拥有属性的那个对象），和`base`中的`propertyName` 。

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

为了从引用类型中得到一个对象真正的值，伪代码中的`GetValue`方法可以做如下描述：

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

内部的`[[Get]]`方法返回对象属性真正的值，包括对原型链中继承的属性分析。

<!--language: js-->

    GetValue(fooReference); // 10
    GetValue(barReference); // function object "bar"

#### 属性访问器
它有两种变体：点（`.`）语法（此时属性名是正确的标示符，且事先知道），或括号语法（`[]`）。

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

### 决定`this`的值

在一个函数上下文中，`this`由调用者提供，由调用函数的方式来决定。如果调用括号`()`的左边是引用类型的值，`this`将设为引用类型值的`base`对象（base object），在其他情况下（与引用类型不同的任何其它属性），这个值为`null`。不过，实际不存在`this`的值为`null`的情况，因为当`this`的值为`null`的时候，其值会被隐式转换为全局对象。注：第5版的ECMAScript中，已经不强迫转换成全局变量了，而是赋值为`undefined`。

<!--language: !js-->

    function foo() {
      return this;
    }

    foo(); // global

我们看到在调用括号的左边是一个引用类型值（因为`foo`是一个 __标示符__）。

<!--language: js-->

    var fooReference = {
      base: global,
      propertyName: 'foo'
    };

相应地，`this`也设置为引用类型的`base`对象。即全局对象。

同样，使用 __属性访问器__：

<!--language: !js-->

    var foo = {
      bar: function () {
        return this;
      }
    };

    foo.bar(); // foo

我们再次拥有一个引用类型，其`base`是`foo`对象，在函数`bar`激活时用作`this`。

<!--language: js-->

    var fooBarReference = {
      base: foo,
      propertyName: 'bar'
    };

但是，用另外一种形式激活相同的函数，我们得到其它的`this`值。

<!--language: !js-->

    var test = foo.bar;
    test(); // global

因为`test`作为 __标示符__，__生成了引用类型的其他值__，其`base`（全局对象）用作`this`值。

<!--language: js-->

    var testReference = {
      base: global,
      propertyName: 'test'
    };

为什么用表达式的不同形式激活同一个函数会不同的`this`值，__答案在于引用类型（type Reference）不同的中间值__。

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
因此，正如我们已经指出，当调用括号的左边不是引用类型而是其它类型，这个值自动设置为`null`，结果为全局对象。让我们再思考这种表达式：

<!--language: !js-->

    (function () {
      alert(this); // null => global
    })();

在这个例子中，我们有一个函数对象但不是引用类型的对象（它 __不是标示符，也不是属性访问器__），相应地，`this`值最终设为全局对象。

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

- 第一个例子很明显——明显的引用类型，结果是，`this`为`base`对象，即`foo`。
- 在第二个例子中，组运算符并不适用，想想上面提到的，从引用类型中获得一个对象真正的值的方法，如`GetValue`。相应的，在组运算的返回中——我们得到仍是一个引用类型。这就是`this`值为什么再次设为`base`对象，即`foo`。
- 第三个例子中，与组运算符不同，赋值运算符调用了`GetValue`方法。返回的结果是函数对象（但不是引用类型），这意味着`this`设为`null`，结果是`global`对象。
- 第四个和第五个也是一样——逗号运算符和逻辑运算符（OR）调用了`GetValue`方法，相应地，我们失去了引用而得到了函数。并再次设为`global`。

### 引用类型和`this`为`null`
当调用表达式限定了call括号左边的引用类型的值，尽管`this`被设定为`null`，但结果被隐式转化成`global`。当引用类型值的`base`对象是被 __活动对象__ 时，这种情况就会出现。

下面的实例中，内部函数被父函数调用，此时我们就能够看到上面说的那种特殊情况，局部变量、内部函数、形式参数储存在给定函数的激活对象中。

<!--language: !js-->

    function foo() {
      function bar() {
        alert(this); // global
      }
      bar(); // the same as AO.bar()
    }

活动对象总是作为`this`返回，值为`null`——（即伪代码的 __`AO.bar()`相当于`null.bar()`__）。这里我们再次回到上面描述的例子，`this`设置为全局对象。

有一种情况除外：如果`with`对象包含一个函数名属性，在`with`语句的内部块中调用函数。`with`语句添加到该对象作用域的最前端，即在活动对象的前面。相应地，也就有了引用类型（通过标示符或属性访问器）， 其`base`对象不再是活动对象，__而是`with`语句的对象__。顺便提一句，它不仅与内部函数相关，也与全局函数相关，因为`with`对象比作用域链里的 __最前端的对象__ (全局对象或一个活动对象)还要靠前。

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

同样的情况出现在`catch`语句的实际参数中函数调用：在这种情况下，`catch`对象添加到作用域的最前端，即在活动对象或全局对象的前面。但是，这个特定的行为被确认为ECMA-262-3的一个bug，这个在新版的ECMA-262-5中修复了。这样，在特定的活动对象中，`this`指向全局对象。而不是`catch`对象

### 构造器调用的函数中`this`

<!--language: !js-->

    function A() {
      alert(this); // "a"对象下创建一个新属性
      this.x = 10;
    }

    var a = new A();
    alert(a.x); // 10

在这个例子中，new运算符调用“A”函数的内部的`[[Construct]]`方法，接着，在对象创建后，调用内部的`[[Call]]`方法。 函数“A”将`this`的值设置为新创建的对象。

### 函数调用中手动设置`this`
即`apply`和`call`方法，[参考](#TOC3.1.4)

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

函数上下文的作用域链在函数调用时创建的，包含活动对象和这个函数内部的`[[scope]]`属性。下面我们将更详细的讨论一个函数的`[[scope]]`属性。在上下文中示意如下：

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

函数“foo”如何访问到变量“x”？理论上函数应该能访问一个更高一层上下文的变量对象。实际上它正是这样，这种机制是通过函数内部的`[[scope]]`属性来实现的。

`[[scope]]`是所有父变量对象的层级链，处于当前函数上下文之上，在函数创建时存于其中。`[[scope]]`在函数创建时被存储－－静态（不变的），直至函数销毁。

与作用域链对比，`[[scope]]`是函数的一个属性而不是上下文。考虑到上面的例子，函数“foo”的`[[scope]]`如下：

<!--language: js-->

    foo.[[Scope]] = [
      globalContext.VO // === Global
    ];

#### 函数激活
函数调用时进入上下文，这时候活动对象被创建，`this`和作用域（作用域链）被确定。进入上下文创建AO/VO之后，上下文的Scope属性（变量查找的一个作用域链）作如下定义：

<!--language: js-->

    Scope = AO|VO + [[Scope]]

上面代码的意思是：活动对象是作用域数组的第一个对象，即添加到作用域的 __前端__。

<!--language: js-->

    Scope = [AO].concat([[Scope]]);

这个特点对于标示符解析的处理来说很重要。

标示符解析是一个处理过程，用来确定一个变量（或函数声明）属于哪个变量对象。

这个算法的返回值中，我们总有一个引用类型，它的`base`组件是相应的变量对象（或若未找到则为`null`），属性名组件是向上查找的标示符的名称。引用类型的详细信息在this中已讨论。

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

对此，我们有如下的变量/活动对象，函数的的`[[scope]]`属性以及上下文的作用域链：

- 全局上下文的变量对象是：

<!--language: js-->

    globalContext.VO === Global = {
      x: 10
      foo: <reference to function>
    };

- 在“foo”创建时，“foo”的`[[scope]]`属性是：

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

- 内部函数“bar”创建时，其`[[scope]]`为：

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
在ECMAScript中，闭包与函数的`[[scope]]`直接相关，正如我们提到的那样，`[[scope]]`在函数创建时被存储，与函数共存亡。实际上，闭包是函数代码和其`[[scope]]`的结合。因此，作为其对象之一，`[[Scope]]`包括在函数内创建的词法作用域（父变量对象）。当函数进一步激活时，在变量对象的这个词法链（静态的存储于创建时）中，来自较高作用域的变量将被搜寻。

<!--language: !js-->

    var x = 10;

    function foo() {
      alert(x);
    }

    (function () {
      var x = 20;
      foo(); // 10, but not 20
    })();

在标识符解析过程中，使用 __函数创建时__ 定义的 __词法作用域__－－变量解析为`10`，而不是`30`。此外，这个例子也清晰的表明，一个函数（这个例子中为从函数“foo”返回的匿名函数）的`[[scope]]` __持续存在__，即使是 __在函数创建的作用域已经完成__ 之后。


#### Function创建的函数
在函数创建时获得函数的`[[scope]]`属性，通过该属性访问到所有父上下文的变量。但是，这个规则有一个重要的例外，它涉及到通过函数构造函数`Function`创建的函数。

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

通过函数构造函数（Function constructor）创建的函数“bar”，是不能访问变量“y”的。但这并不意味着函数“barFn”没有`[[scope]]`属性（否则它不能访问到变量“x”）。问题在于通过函构造函数创建的函数的`[[scope]]`属性总是唯一的全局对象。考虑到这一点，如通过这种函数创建除全局之外的最上层的上下文闭包是不可能的。

#### 二维作用域链查找
在作用域链中查找最重要的一点是变量对象的属性（如果有的话）须考虑其中－－源于ECMAScript 的 __原型__ 特性。如果一个属性在对象中没有直接找到，查询将在原型链中继续。即常说的二维链查找：

> 1. 作用域链环节；
> 1. 每个作用域链－－深入到原型链环节。如果在`Object.prototype`中定义了属性，我们能看到这种效果。

<!--language: !js-->

    function foo() {
      alert(x);
    }

    Object.prototype.x = 10;

    foo(); // 10

通过作用域链找到`golbal`对象，但不存在“x”，就尝试查找`golbal`对象的原型链，它从`Object.prototype`继承而来，相应地，“x”解析为`10`。

#### 全局和`eval`中的作用域链
全局上下文的作用域链仅包含全局对象。代码`eval`的上下文与当前的调用上下文（calling context）拥有同样的作用域链。

<!--language: js-->

    globalContext.Scope = [
      Global
    ];

    evalContext.Scope === callingContext.Scope;

#### 代码执行时对作用域链影响
在代码执行阶段有两个声明能修改作用域链。这就是`with`声明和`catch`语句。它们添加到作用域链的最前端，对象须在这些声明中出现的标识符中查找。如果发生其中的一个，作用域链简要的作如下修改：

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

我们再次看到，通过`with`语句，对象中标识符的解析添加到作用域链的最前端：

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
> 1. 在`with`内部，遇到了`var`声明，当然什么也没创建，因为在进入上下文时，所有变量已被解析添加;
> 1. 在第二步中，仅修改变量“x”，实际上对象中的“x”现在被解析，并添加到作用域链的最前端，“x”为20，变为30;
> 1. 同样也有变量对象“y”的修改，被解析后其值也相应的由10变为30;
> 1. 此外，在`with`声明完成后，它的特定对象从作用域链中移除（已改变的变量“x”－－30也从那个对象中移除），即作用域链的结构恢复到`with`得到加强以前的状态。
> 1. 在最后两个`alert`中，当前变量对象的“x”保持同一，“y”的值现在等于30，在`with`声明运行中已发生改变。

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

只有这两个位置可以声明函数，也就是说，__不能在表达式位置或一个代码块中定义__(如`if,while,for`等)它。

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

该例演示是让一个匿名函数表达式赋值给变量`foo`，然后该函数可以用`foo`这个名称进行访问——`foo()`。

同时和定义里描述的一样，函数表达式也可以拥有可选的名称：

<!--language: js-->

    var foo = function _foo() {
      ...
    };

需要注意的是，在外部FE通过变量“foo”来访问——`foo()`，而在函数内部（如递归调用），有可能使用名称“_foo”。

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

我们看到函数`foo.bar`（通过`[[Scope]]`属性）访问到函数`initialize`的内部变量“x”。同时，“x”在外部不能直接访问。在许多库中，__这种策略常用来创建”私有”数据和隐藏辅助实体__。在这种模式中，初始化的FE的名称(即`initialize`)通常被忽略：

还有一个例子是：在代码执行阶段通过条件语句进行创建FE，不会污染变量对象VO。

<!--language: !js-->

    var foo = 10;

    var bar = (foo % 2 == 0
      ? function () { alert(0); }
      : function () { alert(1); }
    );

    bar(); // 0

#### 函数构造器创建的函数
既然这种函数对象也有自己的特色，我们将它与FD和FE区分开来。其主要特点在于这种函数的`[[Scope]]`属性仅包含全局对象：

<!--language: !js-->

    var x = 10;

    function foo() {
      var x = 20;
      var y = 30;

      var bar = new Function('alert(x); alert(y);');

      bar(); // 10, "y" 未定义
    }

我们看到，函数`bar`的`[[Scope]]`属性不包含`foo`上下文的Ao——变量”y”不能访问，变量”x”从全局对象中取得。顺便提醒一句，`Function`构造器既可使用`new`关键字，也可以没有，这样说来，这些变体是等价的。

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

如果在全局代码里定义（也就是程序级别），解释器会将它看做是函数声明，因为他是以`function`关键字开头，第一个例子，我们会得到`SyntaxError`错误，是因为函数声明没有名字（我们前面提到了函数声明必须有名字）。

第二个例子，我们有一个名称为`foo`的一个函数声明正常创建，但是我们依然得到了一个语法错误——没有任何表达式的分组操作符错误。在函数声明后面他确实是一个分组操作符，而不是一个函数调用所使用的圆括号。所以如果我们声明如下代码：

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

我们如果来告诉解释器：我就像在函数声明之后立即调用，答案是很明确的，你得声明函数表达式`function expression`，而不是函数声明`function declaration`，并且创建表达式最简单的方式就是用分组操作符括号，里边放入的永远是表达式，所以解释器在解释的时候就不会出现歧义。在代码执行阶段这个的`function`就会被创建，并且立即执行，然后自动销毁（如果没有引用的话）。

<!--language: !js-->

    (function foo(x) {
      alert(x);
    })(1); // 这才是调用，不是分组操作符

上述代码就是我们所说的在用括号括住一个表达式，然后通过`(1)`去调用。

注意，下面一个立即执行的函数，周围的括号不是必须的，因为函数已经处在表达式的位置，解析器知道它处理的是在函数执行阶段应该被创建的FE，这样在函数创建后立即调用了函数。

<!--language: !js-->

    var foo = {
      bar: function (x) {
        return x % 2 != 0 ? 'yes' : 'no';
      }(1)
    };

    alert(foo.bar); // 'yes'

就像我们看到的，`foo.bar`是一个字符串而不是一个函数，这里的函数仅仅用来根据条件参数初始化这个属性——它创建后并立即调用。

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

以下是关键点。当解释器在代码执行阶段遇到命名的FE时，在FE创建之前，它创建了 __辅助的特定对象，并添加到当前作用域链的最前端__。然后它创建了FE，此时（正如作用域链知道的那样）函数获取了`[[Scope]]` 属性——创建这个函数上下文的作用域链）。此后，FE的名称添加到特定对象上作为唯一的属性；这个属性的值是引用到FE上。最后一步是从父作用域链中移除那个特定的对象。让我们在伪码中看看这个算法：

<!--language: js-->

    specialObject = {};

    Scope = specialObject + Scope;

    foo = new FunctionExpression;
    foo.[[Scope]] = Scope;
    specialObject.foo = foo; // {DontDelete}, {ReadOnly}

    delete Scope[0]; // 从作用域链中删除定义的特殊对象specialObject

因此，在函数外部这个名称不可用的（因为它不在父作用域链中），但是，特定对象已经存储在函数的`[[scope]]`中，在那里名称是可用的。

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

### 原型
每个对象都有一个原型（一些系统对象除外）。原型通信是通过内部的、隐式的、不可直接访问`[[Prototype]]`原型属性来进行的，原型可以是一个对象，也可以是`null`值。

<!--language: !js-->

    function A() {}
    var a = new A();
    alert(a.constructor); // function A() {}, by delegation
    alert(a.constructor === A); // true

存在着一个误区：`constructor`构造属性作为新创建对象自身的属性是错误的，但是，正如我们所看到的的，这个属性属于原型并且通过继承来访问对象。

通过继承`constructor`属性的实例，可以间接得到的 __原型对象__ 的引用：

<!--language: !js-->

    function A() {}
    A.prototype.x = new Number(10);

    var a = new A();
    alert(a.constructor.prototype); // [object Object]

    alert(a.x); // 10, 通过原型
    // 和a.[[Prototype]].x效果一样
    alert(a.constructor.prototype.x); // 10

    alert(a.constructor.prototype.x === a.x); // true

函数的`constructor`和`prototype`属性在对象创建以后都可以重新定义的。在这种情况下，对象失去上面所说的机制。如果通过函数的`prototype`属性去编辑元素的`prototype`原型的话（添加新对象或修改现有对象），__实例上将看到新添加的属性__。

然而，如果我们彻底改变函数的`prototype`属性（通过分配一个新的对象），那 __原始构造函数的引用就是丢失__，这是因为我们创建的对象不包括`constructor`属性

<!--language: !js-->

    function A() {}
    A.prototype = {
      x: 10
    };

    var a = new A();
    alert(a.x); // 10
    alert(a.constructor === A); // false!

因此，对函数的原型引用需要手工恢复：

<!--language: !js-->

    function A() {}
    A.prototype = {
      constructor: A,
      x: 10
    };

    var a = new A();
    alert(a.x); // 10
    alert(a.constructor === A); // true

注意虽然手动恢复了`constructor`属性，和原来丢失的原型相比，`{DontEnum}`特性没有了，也就是说`A.prototype`里的`for..in`循环语句不支持了，ES5里，通过`[[Enumerable]]`特性提供了控制可枚举状态`enumerable`的能力。

<!--language: !js-->

    var foo = {x: 10};

    Object.defineProperty(foo, "y", {
      value: 20,
      enumerable: false // aka {DontEnum} = true
    });

    console.log(foo.x, foo.y); // 10, 20

    for (var k in foo) {
      console.log(k); // only "x"
    }

    var xDesc = Object.getOwnPropertyDescriptor(foo, "x");
    var yDesc = Object.getOwnPropertyDescriptor(foo, "y");

    console.log(
      xDesc.enumerable, // true
      yDesc.enumerable  // false
    );

显式`prototype`和隐式`[[Prototype]]`属性，通常，一个对象的原型通过函数的`prototype`属性显式引用是不正确的，他引用的是同一个对象，对象的`[[Prototype]]`属性

<!--language: js-->

    a.[[Prototype]] ----> Prototype <---- A.prototype

实例的`[[Prototype]]`值确实是在构造函数的`prototype`属性上获取的。

然而，提交`prototype`属性不会影响已经创建对象的原型（只有在构造函数的`prototype`属性改变的时候才会影响到)，就是说新创建的对象才有有新的原型，而已创建对象还是引用到原来的旧原型（这个原型已经不能被再被修改了）。

<!--language: js-->

    // 在修改A.prototype原型之前的情况
    a.[[Prototype]] ----> Prototype <---- A.prototype

    // 修改之后
    A.prototype ----> New prototype // 新对象会拥有这个原型
    a.[[Prototype]] ----> Prototype // 引导的原来的原型上

例如：

<!--language: !js-->

    function A() {}
    A.prototype.x = 10;

    var a = new A();
    alert(a.x); // 10

    A.prototype = {
      constructor: A,
      x: 20
      y: 30
    };

    // 对象a是通过隐式的[[Prototype]]引用从原油的prototype上获取的值
    alert(a.x); // 10
    alert(a.y) // undefined

    var b = new A();

    // 但新对象是从新原型上获取的值
    alert(b.x); // 20
    alert(b.y) // 30

因此，有的文章说“动态修改原型将影响所有的对象都会拥有新的原型”是错误的，新原型仅仅在原型修改以后的新创建对象上生效。

这里的主要规则是：对象的原型是对象的创建的时候创建的，并且在此之后不能修改为新的对象，如果依然引用到同一个对象，可以通过构造函数的显式`prototype`引用，对象创建以后，只能对原型的属性进行添加或修改。

#### 非标准的`__proto__`属性
有些实现（例如SpiderMonkey），提供了非标准的__proto__显式属性来引用对象的原型：

<!--language: !js-->

    function A() {}
    A.prototype.x = 10;

    var a = new A();
    alert(a.x); // 10

    var __newPrototype = {
      constructor: A,
      x: 20,
      y: 30
    };

    // 引用到新对象
    A.prototype = __newPrototype;

    var b = new A();
    alert(b.x); // 20
    alert(b.y); // 30

    // "a"对象使用的依然是旧的原型
    alert(a.x); // 10
    alert(a.y); // undefined

    // 显式修改原型
    a.__proto__ = __newPrototype;

    // 现在"а"对象引用的是新对象
    alert(a.x); // 20
    alert(a.y); // 30

ES5提供了`Object.getPrototypeOf(O)`方法，该方法直接返回对象的`[[Prototype]]`属性——实例的初始原型。 然而，和`__proto__`相比，它只是`getter`，它不允许`set`值。

<!--language: !js-->

    var foo = {};
    Object.getPrototypeOf(foo) == Object.prototype; // true

#### 对象独立于构造函数
实例的原型独立于构造函数和构造函数的`prototype`属性，构造函数完成了自己的主要工作（创建对象）以后可以删除。原型对象通过引用`[[Prototype]]`属性继续存在：

<!--language: !js-->

    function A() {}
    A.prototype.x = 10;

    var a = new A();
    alert(a.x); // 10

    // 设置A为null - 显示引用构造函数
    A = null;

    // 但如果.constructor属性没有改变的话，
    // 依然可以通过它创建对象
    var b = new a.constructor();
    alert(b.x); // 10

    // 隐式的引用也删除掉
    delete a.constructor.prototype.constructor;
    delete b.constructor.prototype.constructor;

    // 通过A的构造函数再也不能创建对象了
    // 但这2个对象依然有自己的原型
    alert(a.x); // 10
    alert(b.x); // 10

#### instanceof操作符的特性
通过构造函数的`prototype`属性来显示引用原型的，这和`instanceof`操作符有关。该操作符是和 __原型链__ 一起工作的，__而不是构造函数__。

这不是用来检测对象`foo`是否是用`Foo`构造函数创建的，所有`instanceof`运算符只需要一个对象属性——`foo.[[Prototype]]`，在原型链中从`Foo.prototype`开始检查其是否存在。`instanceof`运算符是通过构造函数里的内部方法`[[HasInstance]]`来激活的。

<!--language: !js-->

    function A() {}
    A.prototype.x = 10;

    var a = new A();
    alert(a.x); // 10

    alert(a instanceof A); // true

    // 如果设置原型为null
    A.prototype = null;

    // ..."a"依然可以通过a.[[Prototype]]访问原型
    alert(a.x); // 10

    // 不过，instanceof操作符不能再正常使用了
    // 因为它是从构造函数的prototype属性来实现的
    alert(a instanceof A); // 错误，A.prototype不是对象

另一方面，可以由构造函数来创建对象，但如果对象的`[[Prototype]]`属性和构造函数的`prototype`属性的值设置的是一样的话，`instanceof`检查的时候会返回`true`：

<!--language: !js-->

    function B() {}
    var b = new B();

    alert(b instanceof B); // true

    function C() {}

    var __proto = {
      constructor: C
    };

    C.prototype = __proto;
    b.__proto__ = __proto;

    alert(b instanceof C); // true
    alert(b instanceof B); // false


### 原型链

原型对象也只是简单的对象，并可能有自己的原型。原型链是由 __有限__(finite) 的对象链接而成，可以 __实现__ 继承(inheritance) 与 __共享__ 属性。

<!--language: js-->

    var a = {
      x: 10,
      calculate: function (z) {
        return this.x + this.y + z
      }
    };

    var b = {
      y: 20,
      __proto__: a
    };

    var c = {
      y: 30,
      __proto__: a
    };

    // 调用继承过来的方法
    b.calculate(30); // 60
    c.calculate(40); // 80

如果在对象b中找不到`calculate`方法(也就是对象b中没有这个`calculate`属性), 那么就会沿着原型链开始找。如果这个`calculate`方法在b的`prototype`中没有找到，那么就会沿着原型链找到a的`prototype`，一直遍历完整个原型链。记住，一旦找到，就返回第一个找到的属性或者方法。因此，第一个找到的属性成为继承属性。如果遍历完整个原型链，仍然没有找到，那么就会返回`undefined`。

除了创建对象，构造函数(constructor) 还做了另一件有用的事情—自动为创建的新对象设置了原型对象(prototype object) 。原型对象存放于`ConstructorFunction.prototype` 属性中。

<!--language: !js-->

    // 构造函数
    function Foo(y) {
      // 构造函数将会以特定模式创建对象：被创建的对象都会有"y"属性
      this.y = y;
    }

    // "Foo.prototype"存放了新建对象的原型引用
    // 所以我们可以将之用于定义继承和共享属性或方法
    // 所以，和上例一样，我们有了如下代码：

    // 继承属性"x"
    Foo.prototype.x = 10;

    // 继承方法"calculate"
    Foo.prototype.calculate = function (z) {
      return this.x + this.y + z;
    };

    // 使用foo模式创建 "b" and "c"
    var b = new Foo(20);
    var c = new Foo(30);

    // 调用继承的方法
    b.calculate(30); // 60
    c.calculate(40); // 80

    // 让我们看看是否使用了预期的属性

    console.log(

      b.__proto__ === Foo.prototype, // true
      c.__proto__ === Foo.prototype, // true

      // "Foo.prototype"自动创建了一个特殊的属性"constructor"
      // 指向a的构造函数本身
      // 实例"b"和"c"可以通过授权找到它并用以检测自己的构造函数

      b.constructor === Foo, // true
      c.constructor === Foo, // true
      Foo.prototype.constructor === Foo, // true

      b.calculate === b.__proto__.calculate, // true
      b.__proto__.calculate === Foo.prototype.calculate // true

    );

上述代码可表示为如下的关系：

![constructor_proto_chain](../../../imgs/constructor_proto_chain.png)


### 得到原型被重写前属性
以下以`toString`被重写时情况为例：

<!--language: !js-->

    function Foo(){}
    Foo.prototype.toString = function(){return "foo";};

    var foo = new Foo();
    alert(foo);

    function native_property(native_constructor, property, obj){
        var args = Array.prototype.slice.call(arguments,3);
        return window[native_constructor]['prototype'][property].apply(obj, args);
    }
    alert(native_property('Object', 'toString', foo));

但如果最上层的`Object`的原型都被重写的话，则需要以下方式：

<!--language: !js-->

    Object.prototype.toString = function(){return "my object";};

    var o = {};
    alert(o);

    function native_property(native_constructor, property, obj){
        var ifr = document.createElement('IFRAME');
        document.documentElement.appendChild(ifr);
        var args = Array.prototype.slice.call(arguments,3);
        var ret = ifr.contentWindow[native_constructor]['prototype'][property].apply(obj, args);
        document.documentElement.removeChild(ifr);
        return ret;
    }
    alert(native_property('Object', 'toString', o));

## new关键字

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

在[ECMAScript面向对象实现](http://dmitrysoshnikov.com/ecmascript/chapter-7-2-oop-ecmascript-implementation/#algorithm-of-objects-creation)表述为：

<!--language: js-->

    F.[[Construct]](initialParameters):

        O = new NativeObject();

        // 属性[[Class]]被设置为"Object"
        O.[[Class]] = "Object"

        // 引用F.prototype的时候获取该对象g
        var __objectPrototype = F.prototype;

        // 如果__objectPrototype是对象，就:
        O.[[Prototype]] = __objectPrototype
        // 否则:
        O.[[Prototype]] = Object.prototype;
        // 这里O.[[Prototype]]是Object对象的原型

        // 新创建对象初始化的时候应用了F.[[Call]]
        // 将this设置为新创建的对象O
        // 参数和F里的initialParameters是一样的
        R = F.[[Call]](initialParameters); this === O;
        // 这里R是[[Call]]的返回值
        // 在JS里看，像这样:
        // R = F.apply(O, initialParameters);

        // 如果R是对象
        return R
        // 否则
        return O

内部方法`[[Construct]]`是通过使用带`new`运算符的构造函数来激活的，正如我们所说的这个方法是负责内存分配和对象创建的。如果没有参数，调用构造函数的括号也可以省略

### 构造器中包含return
`new`构造器时，含`return`语句时的行为

<!--language: !js-->

    function Cat(){this.name='cat';}
    function Animal(){ return new Cat;}

    var a =new Animal;
    alert(a.constructor); // function Cat(){this.name='cat';}
    alert(a.__proto__); // Cat {}

虽然在初始化阶段我们可以返回不同的对象（忽略第一阶段创建的tihs对象）

<!--language: !js-->

    function A() {
      // 更新新创建的对象
      this.x = 10;
      // 但返回的是不同的对象
      return [1, 2, 3];
    }

    var a = new A();
    console.log(a.x, a); //undefined, [1, 2, 3]

下面是`underscore.js`中一段代码

<!--language: !js-->

    var _ = function(obj) {
        if (obj instanceof _) return obj;
        if (!(this instanceof _)) return new _(obj);
        this._wrapped = obj;
    };

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
        var Ctor = function() {};  //中间构造函数创建
        Ctor.prototype = superClass.prototype; //保证原型链传递
        subClass.prototype = new Ctor();
        subClass.prototype.constructor = subClass; //恢复原始构造函数

        subClass.superProto = superClass.prototype; //保留父类的原型对象
        if(superClass.prototype.constructor == Object.prototype.constructor) {
            superClass.prototype.constructor = superClass;
        }
    }

    function BasePiece(name) {
        this.name = name;
    }

    BasePiece.prototype.say = function(){
        return this.name;
    };

    extend(SubPiece, BasePiece);

    function SubPiece() {
        //利用保留的父类的原型对象，来实现父类方法的调用
        SubPiece.superProto.constructor.call(this, "sub");
    }

    alert((new SubPiece).say());


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
上面的继承模式没法 __保护隐私__，可选择方式的有 __模块化__（[javascript_module](http://www.ruanyifeng.com/blog/2012/10/javascript_module.html)） 和 __闭包__

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

### 原始值类型
6种用于ECMAScript程序的数据类型，前5种是原始值类型，包括`Undefined、Null、Boolean、String、Number、Object`。

原始值类型例子：

<!--language: !js-->

    var a = undefined;
    var b = null;
    var c = true;
    var d = 'test';
    var e = 10;

这些值是在底层上直接实现的，他们不是object，所以没有原型，没有构造函数。

> 这些原生值和我们平时用的(Boolean、String、Number)虽然名字上相似，但不是同一个东西。所以`typeof(true)`和`typeof(new Boolean(true))`结果是不一样的

想知道数据是哪种类型用`typeof`是最好不过了，有个例子需要注意一下，如果用`typeof`来判断`null`的类型，结果是`object`，因为规范就是这么规定的

### 特殊包装类
布尔对象，字符串对象，数字对象，这些对象的创建，是通过相应的内置构造器创建，并且包含原生值作为其内部属性，这些对象可以转换省原始值，反之亦然。

<!--language: !js-->

    var c = new Boolean(true); console.log(typeof c); //object
    var d = new String('test'); console.log(typeof d); //object
    var e = new Number(10); console.log(typeof e); //object

    // 转换成原始值
    // 使用不带new关键字的函数
    var c2 = Boolean(c); console.log(typeof c2); //boolean
    var d2 = String(d); console.log(typeof d2); //string
    var e2 = Number(e); console.log(typeof e2); //number

    // 重新转换成对象
    var c3 = Object(c2); console.log(typeof c3); //object
    var d3 = Object(d2); console.log(typeof d3); //object
    var e3 = Object(e2); console.log(typeof e3); //object

如果对原始值进行属性访问器取值，访问之前会先对原始值进行对象包装（包括原始值），然后通过包装的对象进行访问属性，属性访问以后，包装对象就会 __被删除__。

<!--language: !js-->

    var a = 10; // 原始值

    // 但是可以访问方法（就像对象一样）
    alert(a.toString()); // "10"

    // 此外，我们可以在a上创建一个心属性
    a.test = 100; // 好像是没问题的

    // 但，[[Get]]方法没有返回该属性的值，返回的却是undefined
    alert(a.test); // undefined

为什么整个例子里的原始值可以访问`toString`方法，而不能访问新创建的`test`属性呢？使用属性访问器以后，它已经不是原始值了，而是一个包装过的中间对象（整个例子是使用`new Number(a)`），而`toString`方法这时候是通过 __原型链__ 查找到的

任何原始值如果经常用在访问属性的话，时间效率考虑，都是直接用一个对象替代它；与此相反，如果不经常访问，或者只是用于计算的话，到可以保留这种形式。


也有对象是由特殊的内置构造函数创建： `Function`（函数对象构造器）、`Array`（数组构造器） `RegExp`（正则表达式构造器）、`Math`（数学模块）、 `Date`（日期的构造器）等等，这些对象也是`Object`对象类型的值，他们彼此的区别是由内部属性管理的。


### valueOf方法
将对象转化成原始值可以用valueOf方法，如果一个数据是 引用类型，并且该数据需要进行值运算，js引擎将先调用它(或经过 包装后 的对象)的valueOf()方法求值，然后以该值参与运算

<!--language: !js-->

    var a = new Number(1);
    var primitiveA = Number(a); // 隐式"valueOf"调用
    var alsoPrimitiveA = a.valueOf(); // 显式调用

    alert([
      typeof a, // "object"
      typeof primitiveA, // "number"
      typeof alsoPrimitiveA // "number"
    ]);

这种方式允许对象参与各种操作，例如：

<!--language: !js-->

    var a = new Number(1);
    var b = new Number(2);

    alert(a + b); // 3

    // 甚至
    var c = {
      x: 10,
      y: 20,
      valueOf: function () {
        return this.x + this.y;
      }
    };

    var d = {
      x: 30,
      y: 40,
      // 和c的valueOf功能一样
      valueOf: c.valueOf
    };

    alert(c + d); // 100

对象还有一个更原始的代表性——字符串展示。 这个`toString`方法是可靠的，它在某些操作上是自动使用的：

<!--language: !js-->

    var a = {
      valueOf: function () {
        return 100;
      },
      toString: function () {
        return '__test';
      }
    };

    // 这个操作里，toString方法自动调用
    alert(a); // "__test"

    // 但是这里，调用的却是valueOf()方法
    alert(a + 10); // 110

    // 但，一旦valueOf删除以后
    // toString又可以自动调用了
    delete a.valueOf;
    alert(a + 10); // "_test10"

### Object.prototype.toString
`Object.prototype`上定义的`toString`方法具有特殊意义，它间接得到的内部`[[Class]]`属性值，该方法应该返回下列字符串： "[object " + [[Class]] + "]" （例如，`[object Number]` ，`[object String]`等）。

<!--language: !js-->

    var toString = Object.prototype.toString;

    var n = Object(1);
    alert(toString.call(1)); // [object Number]
    alert(toString.call(n)); // [object Number]

    var s = Object('test');
    alert(toString.call('test')); // [object String]
    alert(toString.call(s)); // [object String]

    // 一些类似，使用new操作符也可以
    var b = new Object(true);
    alert(toString.call(true)); // [object Boolean]
    alert(toString.call(b)); // [object Boolean]

    // 应用参数new Object的话创建的是简单对象
    var o = new Object();
    alert(toString.call(o)); // [object Object]

    // 如果参数是一个现有的对象
    // 那创建的结果就是简单返回该对象
    var a = [];
    alert(a === new Object(a)); // true
    alert(a === Object(a)); // true


### typeof运算符

js是弱类型的，任何一个变量或值的类型都可以使用 __`typeof`__ 运算符来得到，以字符串形式返回此6种类型值之一：

> - undefined
> - number
> - string
> - boolean
> - function 具有多种含义：函数、方法、构造器、类、函数对象等
> - object 基于原型继承的面向对象

通过typeof运算考察变量时，要么是对象(`object`)，要么是非对象(`number`,`undefined`,`string`,`boolean`,`function`)

其中`function`,`object`为引用类型，其它均为值类型，包括`undefined`，但`string`在赋值运算中会按引用类型方式来处理

以下是特殊几个值的`typeof`的结果

<!--language: !js-->

    var arr = [null, undefined, NaN, '', 0,
               false, /\s/i, function(){}, new Function()];
    arr.forEach(function(item){
        console.log(typeof item);
        //object,undefined,number,string,number,boolean,object,function,function
    });
    arr.forEach(function(item){
        console.log(typeof Object(item));
        //object,object,object,object,object,object,object,function,function
    });


### instanceof运算符
对象是不是另一个类的实例，使用`instanceof`运算符，会检测类的继承关系，因此一个子类的实例，在对祖先类做`instanceof`运算时，仍然得到`true`

该操作符是和 __原型链__ 一起工作的，__而不是构造函数__ [详见](#TOC1.7.1.3)。

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

    var toString = Object.prototype.toString;

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

    //= require underscore.1.4.3
    var seq = _([1,3,4,5,7]);
    var result = seq.without(3,5,7,8);
    result.forEach(function(arg){alert(arg);});

由于`without`接受的是不定的参数，当不需要的元素已经存在于一个数组中时：

<!--language: !js-->

    //= require underscore.1.4.3
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

- 示例4

提高代码的精简度

<!--language: !js-->

    function sum(a, b) {
        return a + b;
    }

    var arr = [[1,2],[4,3],[5,7],[8,3]];
    alert(arr.map(function(item){return sum.apply(null, item);}));

如果不存在`apply`，对于工具函数`sum`，要么需要改写它的参数签名，接受一个具有两个元素的数组参数，但导致`sum`不够通用，否则在`map`的过程中，将`item`展开后再传递`return sum(item[0],item[1])`，但导致代码不够简洁。


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


下面有关[闭包中作用域的例子](#TOC4.3.6)，返回的是一个函数数组，需要依次调用每个函数，还可以写成：

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
参见[curry化](#TOC4.6)

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

    //= require underscore.1.4.3
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
> [函数式思维: 为什么函数式编程越来越受关注](http://www.ibm.com/developerworks/cn/java/j-ft20/)

> [coolshell上函数式编程](http://coolshell.cn/articles/10822.html)

## 高阶函数
在数学和计算机科学中，__高阶函数__(high-order function, HOF)是至少满足下列一个条件的函数：

> - 接受一个或多个函数作为输入
> - 输出一个函数

js中函数第一公民，高阶函数自然支持，甚至下面的其他特性，都离不开它的支持。

不存在Haskell与Python中的列表解析，但`Array.protype`中的函数对集合处理还是很方便的

<!--language: !js-->

    alert([1, 2, 3].map(function(num){ return num*2; }));


js原生不支持lambda，但借助一些库（如[Functional Javascript](http://osteele.com/sources/javascript/functional/)，[lambda.js](http://www.javascriptoo.com/lambda-js)）可实现相关功能，或直接使用[CoffeScript](http://coffeescript.org/)

### map与reduce
以下概念对mongodb等分布式并发NoSql具有指导意义，对于js编程可作为参考

指定一个Map（映射）函数，用来把一组键值对映射成一组新的键值对，指定并发的Reduce（化简）函数，用来保证所有映射的键值对中的每一个共享相同的键组

简单来说，一个映射函数就是对一些独立元素组成的概念上的列表（例如，一个测试成绩的列表）的每一个元素进行指定的操作（比如，有人发现所有学生的成绩都被高估了一分，他可以定义一个“减一”的映射函数，用来修正这个错误。）。事实上，每个元素都是被独立操作的，而原始列表没有被更改，因为这里创建了一个新的列表来保存新的答案。这就是说，Map操作是可以高度并行的（js中并非并行），这对高性能要求的应用以及并行计算领域的需求非常有用。

而化简操作指的是对一个列表的元素进行适当的合并（继续看前面的例子，如果有人想知道班级的平均分该怎么做？他可以定义一个化简函数，通过让列表中的奇数（odd）或偶数（even）元素跟自己的相邻的元素相加的方式把列表减半，如此递归运算直到列表只剩下一个元素，然后用这个元素除以人数，就得到了平均分）。虽然他不如映射函数那么并行，但是因为化简总是有一个简单的答案，大规模的运算相对独立，所以化简函数在高度并行环境下也很有用。

### 自定义控制结构
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

## 模式匹配
原生的js不支持，但基于js的[LiveScript](http://livescript.net/)是支持的

## 闭包
闭包是代码块和 __创建该代码块的上下文中数据__ 的结合。

### 面向堆栈动态作用域
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

### 静态词法作用域

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

同一个上下文中创建的闭包是共用一个[[Scope]]属性的，[如下面](#TOC4.3.6)

### 闭包中返回
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


### 生命周期与内存泄漏
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



### 信息隐藏与保持状态
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


### 作用域与变量共享

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


## 递归
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

### reduce
一些语言提供了 __尾递归__(tail recursion/trai-end recursion) 优化，即如果一个函数的最后执行递归调用语句的特殊形式的递归，那么调用的过程会被替换为一个循环，可以提高速度。但js __并没有提供__ 该优化。

`reduce`或`flod`对尾递归情形的一种模式固定 [参考](http://learnyouahaskell-zh-tw.csie.org/zh-cn/high-order-function.html#关键字_fold)，不使用变量（无空间污染，无副作用），而通过参数与返回值来保持中间结果

> ES5中的`Array.prototype.reduce`已有支持

<!--language: !js-->

    // sum
    alert([1, 2, 3].reduce(function(memo, num){ return memo + num; }, 0));
    // join
    alert([1, 2, 3].reduce(function(memo, num){ return memo + num; }, ""));

## 偏函数应用
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

### Map和Filter中偏函数用法

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

## Curry化
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


### 偏函数 vs Curry化
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



## 函数组合
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

在[前面map](#TOC4.5.1)例子中，我们遍历了数组两次，一次用来获取users，一次为了获取names。如果能在一次map映射操作中同时做这两件事情，效率会高很多。

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


## 惰性求值
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

### 实现yield







异步编程
=========
> 本节主要摘自《Async JavaScript》
## 异步事件
### 事件的调度
如果想让JavaScript中的某段代码将来再运行，可以将它放在回调中。回调就是一种普通函数，只不过它是传给像`setTimeout`这样的函数，或者绑定为像`document.onready`这样的属性。运行回调时，我们称已触发某事件（譬如延时结束或页面加载完毕）。

#### 现在还是将来运行
```!js
for (var i = 1; i <= 3; i++) {
  setTimeout(function(){ console.log(i); }, 0);
}
```
要理解为什么输出是4，4，4，需要知道以下3件事：

- 这里只有一个名为i的变量，其 __作用域__ 由声明语句`var i`定义（该声明语句在不经意间让`i`的作用域不是循环内部，而是扩散至蕴含循环的那个最内侧函数）。
- `i`一直递增，直到不再满足条件`i<=3`为止，循环结束后`i===4`
- JavaScript事件处理器在 __线程空闲之前__ 不会运行。

#### 线程的阻塞
```!js
var start = new Date;
setTimeout(function(){
  var end = new Date;
  console.log('Time elapsed:', end - start, 'ms');
}, 500);
while (new Date - start < 1000) {};
```
按照多线程的思维定势，我会预计500毫秒后计时函数就会运行。不过这要求中断欲持续整整一秒钟的循环。如果运行代码，会得到类似这样的结果：`Time elapsed: 1021ms`，这个数字肯定至少是1000，因为`setTimeout`回调在`while`循环结束运行之前 __不__ 可能被触发。

#### 队列
调用`setTimeout`的时候，会有一个延时事件排入队列。然后`setTimeout`调用之后的那行代码运行，接着是再下一行代码，直到再也没有任何代码。

用户单击一个已附加有单击事件处理器的`DOM`元素时，会有一个单击事件排入队列。但是，该单击事件处理器要等到当前所有正在运行的代码 __均已结束后__（可能还要等其他此前已排队的事件也依次结束）才会执行。因此，使用JavaScript的那些网页一不小心就会变得毫无反应。

这隐含着一个意思，即触发的每个事件都会位于堆栈轨迹的底部。事件的易调度性是JavaScript语言最大的特色之一。像`setTimeout`这样的异步函数只是简单地做延迟执行，而不是孵化新的线程。JavaScript 代码永远不会被中断，这是因为代码在运行期间只需要排队事件即可，而这些事件在代码运行结束之前不会被触发。

### 异步函数的类型
#### 异步的IO函数
创造Node.js，并不是为了人们能在服务器上运行JavaScript，仅仅是因为Ryan Dahl想要一个建立在某高级语言之上的 __事件驱动型__ 服务器框架。JavaScript碰巧就是适合干这个的语言。为什么？因为JavaScript语言可以完美地实现非阻塞式I/O。

在其他语言中，一不小心就会“阻塞”应用（通常是运行循环）直到完成I/O请求为止。而在JavaScript中，这种阻塞方式几乎沦为无稽之谈。相反，我们需要附加一个事件处理器，随即返回事件队列。不论是在等待用户的按键行为，还是在等待远程服务器的批量数据，所需要做的就是 __定义一个回调__ (除非JavaScript环境提供的某个同步I/O函数已经替我们完成了阻塞，如Ajax方法有一个可设置为`false`的`async`选项，在Node.js中，同步的API方法在名称上会有明确的标示，譬如`fs.readFileSync`)

在现代浏览器中操纵DOM对象时，从脚本角度看，更改是即时生效的，但从视效角度看，在返回事件队列之前不会渲染这些DOM对象更改。这可以防止DOM对象被渲染成不一致的状态。

JavaScript采用了非阻塞式I/O，这对新手来说是最大的一个障碍，但这同样也是该语言的核心优势之一。有了非阻塞式I/O，就能自然而然地写出高效的基于事件的代码。

#### 异步的计时函数
有些时候，我们仅仅是因为需要异步而想要异步性。换句话说，我们想让一个函数在将来某个时刻再运行——这样的函数可能是为了作动画或模拟。

```!js
var fireCount = 0;
var start =new Date;
var timer = setInterval(function() {
  if(new Date-start > 1000) {
    clearInterval(timer);
    console.log(fireCount);
    return;
  }
  fireCount++;
}, 0);
```

如果使用`setInterval`调度事件且延迟设定为0毫秒，则会尽可能频繁地运行此事件，现代浏览器中，大约为200次/秒。在Node环境下，此事件的触发频率大约能达到1000次/秒（若使用`setTimeout` 来调度事件，重复这些实验也会得到类似的结果）。如果将setInterval替换成简单的while循环，则在Chrome中此事件的触发频率将达到400万次/秒，而在Node中会达到500万次/秒！`setTimeout`和`setInterval`就是想 __设计成慢吞吞__ 的！事实上，HTML规范（这是所有主要浏览器都遵守的规范）推行的延时/时隔的最小值就是4毫秒！

如果需要更细粒度的计时，该怎么办呢？有些运行时环境提供了备选方案：

- 在Node中，`process.nextTick`允许将事件调度成尽可能快地触发。对于笔者的系统，`process.nextTick`事件的触发频率可以超过10万次/秒。
- 一些现代浏览器（含IE9+）带有一个`requestAnimationFrame`函数。此函数有两个目标：一方面，它允许以60+帧/秒的速度运行JavaScript 动画；另一方面，它又避免后台选项卡运行这些动画，从而节约CPU周期。在最新版的Chrome浏览器中，甚至能实现亚毫秒级的精度。


### 异步函数的编写
JavaScript 中的每个异步函数都构建在其他某个或某些异步函数之上。凡是异步函数，从上到下（一直到原生代码）都是异步的！任何函数只要使用了异步的函数，就必须以异步的方式给出其操作结果。

#### 何时称函数为异步的
如果称一个函数为“异步的”，其意思是这个函数会导致 __将来__ 再运行另一个函数，后者取自于事件队列（若后面这个函数是作为参数传递给前者的，则称其为回调函数，简称为 __回调__）。

异步函数还涉及另一个术语，即 __非阻塞__。非阻塞这个词强调了异步函数的 __高速度__：异步MySQL数据库驱动程序做一个查询可能要花上一小时，但负责发送查询请求的那个函数却能以微秒级速度返回。这对于那些需要快速处理海量请求的网站服务器来说，绝对是个福音。

遗憾的是，要想确认某个函数异步与否，唯一的方法就是审查其源代码。有些同步函数却拥有看起来像是异步的API，这或者是因为它们将来可能会变成异步的，又或者是因为回调这种形式能 __方便地返回多个参数__。

#### 间或异步的函数
有些函数某些时候是异步的，但其他时候却不然。举个例子，jQuery的同名函数（通常记作`$`）可用于延迟函数直至DOM已经结束加载。但是，若DOM早已结束了加载，则不存在任何延迟，`$`的回调将会立即触发。

#### 缓存型异步函数
间或异步的函数有一个常见变种是 __可缓存结果的异步请求类函数__。举例来说，假设正在编写一个基于浏览器的计算器，它使用了网页`Worker`对象以单独开一个线程来进行计算。

```js
var calculationCache = {},
    calculationCallbacks = {},
    mathWorker = newWorker('calculator.js');
mathWorker.addEventListener('message', function(e) {
  varmessage = e.data;
  calculationCache[message.formula] = message.result;
  calculationCallbacks[message.formula](message.result);
});
functionrunCalculation(formula, callback) {
  if(formula in calculationCache) {
    return callback(calculationCache[formula]);
  };
  if (formula in calculationCallbacks) {
    return setTimeout(function() {
      runCalculation(formula, callback);
    }, 0);
  };
  mathWorker.postMessage(formula);
  calculationCallbacks[formula] = callback;
}
```

在这里，当结果已经缓存时，`runCalculation`函数是同步的，否则就是异步的。存在3种可能的情景：

- 公式已经计算完成，于是结果位于`calculationCache`中。这种情况下，`runCalculation`是同步的
- 公式已经发送给`Worker`对象，但尚未收到结果，即`formula in calculationCallbacks`。这种情况下，`runCalculation`设定了一个延时以便再次调用自身；重复这一过程直到结果位于`calculationCache`中为止。
> 如果把`if (formula in calculationCallbacks)` 中代码去掉，如在未到收结果前多次发送，将多次激发监听器，导致不必要的消息发送。
- 公式尚未发送给`Worker`对象。这种情况下，将会从`Worker`对象的`message`事件监听器激活回调，并设置`calculationCallbacks[formula]`。

[完整的例子](http://webworkersandbox.com/5009efc12245588e410002cf)：

```js
var $output = $('#sandbox').html('<p></p>').children(),
    calculationCache = {},
    calculationCallbacks = {},
    mathWorker = new Worker('worker.js');

mathWorker.addEventListener('message', function(e) {
  var message = e.data;
  $output.append('computed: ' + message.result + '<br />');
  calculationCache[message.formula] = message.result;
  calculationCallbacks[message.formula](message.result + 'listen');
});

function runCalculation(formula, callback) {
  if (formula in calculationCache) {
    return callback(calculationCache[formula] + 'cache');
  };
  if (formula in calculationCallbacks) {
    return setTimeout(function() {
      runCalculation(formula, callback);
    }, 0);
  };
  mathWorker.postMessage(formula);
  calculationCallbacks[formula] = callback;
}

function showResult(result) {
  $output.append('result: ' + result + '<br />');
}
runCalculation('52 * 25', showResult);
runCalculation('52 * 25', showResult);
setTimeout(function() {
  runCalculation('52 * 25', showResult);
}, 100);
```

worker.js

```js
self.addEventListener('message', function(e) {
  // In a real application, we'd do some kind of parsing here.
  // Instead, we just eval the given formula directly.
  var message = {
    formula: e.data,
    result: eval(e.data)
  };
  self.postMessage(message);
});
```

#### 异步递归与回调存储
在`runCalculation`函数中，为了等待`Worker`对象完成自己的工作，或者通过延时而重复相同的函数调用（即异步递归），或者简单地存储回调结果。

异步递归有一点很可怕，即在等待任务完成期间，可触发之延时的次数是不受限的！此外，异步递归还毫无必要地复杂化了应用程序的事件结构。基于这些原因，应将异步递归视作一种“反模式”的方式。

在这个计算器例子中，为了避免异步递归，可以为每个公式存储一个回调数组（当然也避免了多次消息发送）

```js
var calculationCache = {},
    calculationCallbacks = {},
    mathWorker =new Worker('calculator.js');
mathWorker.addEventListener('message',function(e) {
  var message = e.data;
  calculationCache[message.formula] = message.result;
  calculationCallbacks[message.formula]
  .forEach(function(callback) { // beautiful
    callback(message.result);
  });
});
function runCalculation(formula, callback) {
  if (formula in calculationCache) {
    return callback(calculationCache[formula]);
  };
  if(formula in calculationCallbacks) {
    return calculationCallbacks[formula].push(callback);
  };
  mathWorker.postMessage(formula);
  calculationCallbacks[formula] = [callback];
}
```

没有了延时，我们的代码要直观得多，也高效得多。总的来说，请 __避免异步递归__。仅当所采用的库提供了异步功能但没有提供任何形式的回调机制时，异步递归才有必要。

#### 返回与回调的混搭
永远不要定义一个潜在的同步，而返回值却有可能用于回调的函数。

一个反例：

```js
var webSocketCache = {};
function openWebSocket(serverAddress, callback) {
  var socket;
  if (serverAddress in webSocketCache) {
    socket = webSocketCache[serverAddress];
    if(socket.readyState === WebSocket.OPEN) {
      callback();
    } else {
      socket.onopen = _.compose(callback, socket.onopen);
    }
  } else {
    socket = new WebSocket(serverAddress);
    webSocketCache[serverAddress] = socket;
    socket.onopen = callback;
  }
  return socket;
};
```

这段代码的问题在于，如果套接字已经缓存且打开，则会在函数返值之前就运行回调，这会使以下代码崩溃（`socket`未定义）。

```js
var socket = openWebSocket(url, function(){
  socket.send('Hello, server!');
});
```

怎么解决呢？将回调封装在`setTimeout`中即可

```js
if (socket.readyState === WebSocket.OPEN) {
  setTimeout(callback, 0);
} else{
  // ...
}
```

一些编写异步函数的最佳实践：

- 请勿依赖那些看似始终异步的函数，除非已经阅读其源代码。
- 请避免使用计时器方法来等待某个会变化的东西。
- 如果同一个函数既返值又运行回调，则请确保回调在返值之后才运行。


### 异步错误的处理
#### 回调内抛出的错误
```!js
setTimeout(function A() {
  setTimeout(function B() {
    setTimeout(function C() {
      throw new Error('Something terrible has happened!');
    }, 0);
  }, 0);
}, 0);
```

上述应用的结果是一条极其简短的堆栈轨迹

```shell
Error: Something terrible has happened!
  at C
```

A和B发生了什么事？为什么它们没有出现在堆栈轨迹中？这是因为运行C的时候，A和B并不在内存堆栈里。这3个函数都是从事件队列直接运行的。

基于同样的理由，利用`try/catch`语句块 __并不能__ 捕获从异步回调中抛出的错误。

```!js
try {
  setTimeout(function() {
    throw new Error('Catch me if you can!');
  }, 0);
}catch(e) {
  console.error(e);
}
```

这里的`try/catch`语句块只捕获`setTimeout`函数自身内部发生的那些错误。因为`setTimeout` 异步地运行其回调，所以即使延时设置为0，回调抛出的错误也会直接流向应用程序的未捕获异常处理器

最常见的模式是，针对成败这两种情形各规定一个单独的回调。jQuery 的Ajax方法就遵循了这个模式。

```js
$.get('/data', {
  success: successHandler,
  failure: failureHandler
});
```

不管API形态像什么，始终要记住的是，__只能在回调内部处理源于回调的异步错误__。异步尤达大师会说：“做，或者不做，没有试试看一说。”

#### 未捕获异常的处理
如果是从回调中抛出异常的，则由那个调用了回调的人负责捕获该异常。但如果异常从未被捕获，又会怎么样？这时，不同的JavaScript环境有着不同的游戏规则

在浏览器环境中:

```js
window.onerror = function(err) {
  return true; //彻底忽略所有错误
};
```

在Node.js环境中使用`Domain`对象

```js
varmyDomain = require('domain').create();
myDomain.run(function() {
  setTimeout(function() {
    throw new Error('Listen to me!')
  }, 50);
});
myDomain.on('error', function(err) {
  console.log('Error ignored!');
});
```

`Domain`对象让`throw`语句生动了很多。仅在Node 0.8+环境中才能使用`Domain`对象

不管在浏览器端还是服务器端，全局的异常处理器都应被视作最后一根救命稻草。请仅在调试时才使用它。

#### 抛出还是不抛出
在任的Node开发负责人）就主张`try/catch`是一种“反模式”的方式，是包装着漂亮花括弧的`goto`语句。

如果想让整个应用停止工作，请勇往直前地大胆使用`throw`。否则，请认真考虑一下应该如何处理错误。是想给用户显示一条出错消息吗？是想重试请求吗？那就这么处理吧，只是请尽可能地靠近错误源头。

### 嵌套式回调的解嵌套
```js
function checkPassword(username, passwordGuess, callback) {
  var queryStr = 'SELECT * FROM user WHERE username = ?';
  db.query(queryStr, username, function(err, result) {
    if(err)throwerr;
    hash(passwordGuess, function(passwordGuessHash) {
      callback(passwordGuessHash === result['password_hash']);
    });
  });
}
```

如果试图向其添加新特性，它就会变得毛里毛躁、险象环生，比如去处理那个数据库错误，而不是抛出错误、记录尝试访问数据库的次数、阻塞访问数据库，等等。

嵌套式回调诱惑我们通过添加更多代码来添加更多特性，而不是将这些特性实现为可管理、可重用的代码片段。`checkPassword`有一种可以避免出现上述苗头的等价实现方式，如下

```js
function checkPassword(username, passwordGuess, callback) {
  var passwordHash;
  var queryStr = 'SELECT * FROM user WHERE username = ?';
  db.query(qyeryStr, username, queryCallback);
  function queryCallback(err, result) {
    if(err) throwerr;
    passwordHash = result['password_hash'];
    hash(passwordGuess, hashCallback);
  }
  function hashCallback(passwordGuessHash) {
    callback(passwordHash === passwordGuessHash);
  }
}
```

这种写法更啰嗦一些，但读起来更清晰，也更容易扩展。由于这里赋予了异步结果（即`passwordHash`）更宽广的作用域，所以获得了更大的灵活性。__请避免两层以上的函数嵌套__，关键是找到一种在激活异步调用函数的之外部存储异步结果的方式即`passwordHash`），这样回调本身就没有必要再嵌套了。


PubSub模式是一种将回调赋值给已命名事件的回调组织方式，而Promise对象是一种表示一次性事件的直观对象。

## 分布式事件
### PubSub模式
Publish/Subscribe，意为“发布/订阅”，在其帮助下，我们能 __解嵌套那些嵌套式回调__，减少重复冗余，最终编写出易于理解的事件驱动型代码。

从软件架构的角度看，jQuery将link元素的事件发布给了任何想订阅此事件的人。这正是称其为PubSub模式的原因

Node的API架构师因为太喜欢PubSub，所以决定包含一个一般性的PubSub实体。这个实体叫做`EventEmitter`（事件发生器），其他对象可以继承它。Node中几乎所有的I/O源都是`EventEmitter`对象：文件流、HTTP服务器，甚至是应用进程本身。

很多MVC框架，如Backbone.js和Spine，都提供了自己的`类EventEmitter`模块

#### `EventEmitter`对象
Node的`EventEmitter`对象作为PubSub 接口的例子。`EventEmitter`有着简单而近乎最简化的设计。

```js
emitter.on('evacuate', function(message) {
  console.log(message);
});

emitter.emit('evacuate');
emitter.emit('evacuate', 'Woman and children first!');
```

`EventEmitter`对象的所有方法都是公有的，但一般约定只能从`EventEmitter`对象的“内部”触发事件。也就是说，如果有一个对象继承了`EventEmitter`原型并使用了`this.emit`方法来广播事件，则不应该从这个对象之外的其他地方再调用其`emit`方法。

#### 自己设计PubSub
PubSub模式的实现如此简单，以至于用十几行代码就能建立自己的PubSub实现。

```js
PubSub = {handlers: {}};
PubSub.on = function(eventType, handler) {
  if(!(eventType in this.handlers)) {
    this.handlers[eventType] = [];
  }
  this.handlers[eventType].push(handler);
  return this;
}
PubSub.emit = function(eventType) {
  var handlerArgs = Array.prototype.slice.call(arguments, 1);
  for (var i = 0; i < this.handlers[eventType].length; i++) {
    this.handlers[eventType][i].apply(this, handlerArgs);
  }
  return this;
}
```

现在只实现了Node之`EventEmitter`对象的核心部分。还没实现的重要部分只剩下移除事件处理器及附加一次性事件处理
器等功能

jQuery团队注意到jQuery库里到处都在用几个不同的PubSub实现，于是决定在jQuery 1.7中将它们抽象为`$.Callbacks`。这样就不再用数组来存储各种事件类型对应的事件处理器，而可以转用`$.Callbacks`实例。

很多PubSub实现负责解析事件字符串以提供一些特殊功能：jQuery 的名称空间化事件：如果绑定了名称为"click.tbb"和"hover.tbb"的两个事件，则简单地调用unbind(".tbb")就可以同时解绑定它们。Backbone.js允许向"all"事件类型绑定事件处理器，这样不管发生什么事，都会导致这些事件处理器的触发。jQuery 和Backbone.js都支持用空格隔开多个事件来同时绑定或触发多种事件类型，譬如"keypress mousemove"。

#### 同步性
尽管PubSub模式是一项处理异步事件的重要技术，但它内在跟异步没有任何关系。

如果事件按顺序触发了过多的处理器，就会有阻塞线程且导致浏览器不响应的风险。更糟糕的是，如果事件处理器本身触发了事件，还很容易造成无限循环。

这个问题有一个很好的解决方案，就是对那些无需即刻发生的事情维持一个队列，并使用一个计时函数定时运行此队列中的下一项任务。首次尝试编码的结果可能像这样：

```js
var tasks = [];
setInterval(function() {
  var nextTask;
  if(nextTask = tasks.shift()) {
    nextTask();
  };
}, 0);
```

### 事件化模型
只要对象带有PubSub接口，就可以称之为事件化对象。特殊情况出现在用于存储数据的对象因内容变化而发布事件时，这里用于存储数据的对象又称作模型。模型就是MVC中的那个M，老式的JavaScript 靠输入事件的处理器直接改变DOM。新式的JavaScript先改变模型，接着由模型触发事件而导致DOM的更新。在几乎所有的应用程序中，这种关注层面的分离都会带来更优雅、更直观的代码。

#### 模型事件的传播
作为最简形式，MVC三层架构只包括相互联系的模型和视图：“如果模型是这样变化的，那么DOM就要那样变化。”不过，MVC三层架构最大的利好出现在change（变化）事件冒泡上溯数据树的时候。不用再去订阅数据树每片叶子上发生的事件，而只需订阅数据树根和枝处发生的事件即可。

为此，Backbone的Model对象常常组织成Backbone集合（Collection）的形式，其本质是事件化数组。我们可以监听什么时候对这些 __数组增减__ 了Model对象。Backbone集合可以自动传播其内蕴Model对象所发生的事件。

#### 事件循环与嵌套式变化
如果每次有个对象上的事件引发了一系列事件并最终对这个对象本身触发了相同的事件，则结果就是事件循环。如果这种事件循环还是同步的，那就造成了堆栈上溢。

Backbone中的两道保险：

- 当新值等于旧值时，`set`方法不会导致触发`change`事件。
- 模型正处于自身的`change`事件期间时，不会再触发`change`事件。

第二道保险代表了一种自保哲学。假设模型的一个变化导致同一个模型又一次变化。由于第二次变化被“嵌套”在第一次变化内部，所以这次变化的发生悄无声息。外面的观察者没有机会回应这种静默的变化。

而另一个重要的MVC框架，即Ember.js，采用了一种完全不同的方式：双向绑定必须作显式声明。一个值发生变化时，另一个值会通过延时事件作异步更新。于是，在触发这个异步更新事件之前，应用程序的数据将一直处于不一致的状态。

多个事件化模型之间的数据绑定问题不存在简单的解决方案。在Backbone中，有一种审慎绕过这个问题的途径就是`silent`标志。如果在`set`方法中添加了`{silent:true}`选项，则不会触发`change`事件。因此，如果多个彼此纠结的模型需要同时进行更新，一个很好的解决方法就是悄无声息地设置它们的值。然后，当这些模型的 __状态已经一致__ 时，才调用它们的`change`方法以触发对应的事件。

### jQuery自定义事件
在jQuery中，可以使用`trigger`方法基于任意DOM元素触发任何想要的事件。

如果以前用过DOM事件，则肯定熟悉冒泡技术。只要某个DOM元素触发了某个事件（譬如'click'事件），其父元素就会接着触发这个事件，接着是父元素的父元素，以此类推，一直上溯到根元素（即document），除非在这条冒泡之路的某个地方调用了事件的`stopPropagation`方法。（如果事件处理器返回`false`，则jQuery会替我们自动调用`stopPropagation`方法。）但你是否也知道jQuery自定义事件的冒泡技术呢？举个例子，假设有个名称为“soda”的span元素嵌套在名称为“bottle”的div元素中，代码如下。

```js
$('#soda, #bottle').on('fizz', function() {
  console.log(this.id + ' emitted fizz');
});
$('#soda').trigger('fizz');
```

得到的输出如下：

```shell
soda emitted fizz
bottle emitted fizz
```

这种冒泡方式并非始终受人欢迎，幸运的是，jQuery样提供了非冒泡式的`triggerHandler`方法。

jQuery自定义事件是PubSub模式的忤逆产物，因为这里由可选择的DOM元素而不是脚本中的对象来触发事件。事件化模型更像是一种直观表达状态相关事件的方式，而jQuery 的自定义事件允许直接通过DOM来表达DOM相关的事件，不必再把DOM变化的状态复制到应用程序的其他地方。

PubSub模式不适用于一次性事件，一次性事件要求对异步函数执行的一次性任务的两种结果（完成任务或任务失败）做不同的处理。（Ajax请求就是常
见的一次性事件实例。）用于解决一次性事件问题的工具叫做Promise

## `Promise`和`Deferred`
jQuery1.5以后所有Ajax 函数（`$.ajax` 、`$.get`及`$.post` ）现在都会返回`Promise`（承诺）对象。`Promise`对象代表一项有两种可能结果（成功或失败）的任务，它还持有多个回调，出现不同结果时会分别触发相应的回调。举个例子，jQuery 1.4 中的代码必须写成这样：

```js
$.get( '/mydata', {
  success: onSuccess,
  failure: onFailure,
  always: onAlways
});
```

而到了jQuery 1.5+ ，可以写成这样：

```js
var  promise = $.get('/mydata');
promise.done(onSuccess);
promise.fail(onFailure);
promise.always(onAlways);
```

这种变化能有什么好处呢？为什么非得在触发Ajax调用之后再附加回调呢？一言以蔽之：封装。如果Ajax 调用要实现很多效果（既要触发动画，又要插入HTML ，还要锁定/ 解锁用户输入，等等），那么仅由负责发出请求的那部分应用代码来处理所有这些效果，显然很蠢很拙劣。

只传递`Promise`对象就会优雅得多。传递`Promise`对象就相当于声明：“你感兴趣的某某事就要发生了。想知道什么时候完事吗？给这个`Promise`对象一个回调就行啦！”`Promise`对象也`EventEmitter`对象一样，允许向同一个事件绑定任意多的处理器（堆积技术）。对于多个Ajax 调用分享某个功能小片段（譬如“正加载”动画）的情况，堆积技术也会使降低代码重复度容易很多。

不过使用`Promise` 对象的最大优势仍然在于，它可以轻松从现有`Promise` 对象 __派生__ 出新的`Promise` 对象。我们可以要求代表着 __并行__ 任务的两个`Promise`对象 __合并__ 成一个`Promise`对象，由后者负责通知前面那些任务都已完成。也可以要求代表着任务系列中 __首任务__ 的`Promise` 对象派生出一个能代表任务系列中 __末任务__的`Promise` 对象，这样后者就能知道这一系列任务是否均已完成。待会儿我们就会看到，`Promise`对象天生就适合用来进行这些操作。

### 生成`Promise`对象

假设我们提示用户应敲击Y 键或N 键。为此要做的第一件事就是生成一个`$.Deferred`实例以代表用户做出的决定。

<!--language: web-->

    <!--language: html-->
    <input id='playGame' type='text'/>

    <!--language: js-->
    //= require jquery.1.9.0
    var promptDeferred = new $.Deferred();
    promptDeferred.always(function(){ console.log( 'A choice was made:' ); });
    promptDeferred.done(function(){ console.log('Starting game...'); });
    promptDeferred.fail(function(){ console.log('No game today.'); });

    $('#playGame').focus().on('keypress', function(e) {
      var Y = 121, N = 110;
      if (e.keyCode === Y) {
        promptDeferred.resolve();
      } else if (e.keyCode === N) {
        promptDeferred.reject();
      } else {
        return false; //  这里的Deferred 对象保持着挂起状态
      };
    });


`Deferred` 就是`Promise`！更准确地说，`Deferred` 是`Promise`的超集，它比 `Promise`多了一项关键特性：可以直接触发。__纯 `Promise`实例只允许添加多个调用，而且必须由其他什么东西来触发这些调用__。

使用`resolve`（执行）方法和 `reject`（拒绝）方法均可触发 `Deferred`
对象。

执行了`Deferred`（即对`Deferred` 对象调用了`resolve`方法）之后，即运行该对象的`always`（恒常）回调和`done`（已完成）回调。（会按照绑定回调的 __次序__ 来运行回调）

拒绝了`Deferred`（即对`Deferred` 对象调用了`reject`方法）之后，即运行该对象的`always` 回调和`fail`（失败）回调。注意，始终会按照绑定回调的次序来运行回调。如果最后绑定的是 `always` 回调，则控制台的输出行顺序会反过来。

再试着反复敲击Y 键和N 键。第一次做出选择之后，就再也没有反应了！这是因为 `Promise`只能执行或拒绝一次，之后就失效了。我们断言，`Promise` 对象会一直保持 __挂起__ 状态，直到被执行或拒绝。对`Promise`对象调用`state`（状态）方法，可以查看其状态是"`pending`"、"`resolved`"，还是"`rejected`"。

如果正在进行的一次性异步操作的结果可以笼统地分成两种（如成功/ 失败，或接受/ 拒绝），则生成`Deferred` 对象就能直观地表达这次任务。

#### 纯`Promise`对象
如何得到一个不是`Deferred` 对象的`Promise`对象呢？很简单，对 `Deferred`对象调用`promise`方法即可。

```js
var promptPromise = promptDeferred.promise();
```

`promptPromise` 只是`promptDeferred` 对象的一个没有`resolve`/ `reject` 方法的副本。我们把回调绑定至Deferred 或其下辖的Promise并无不同，因为这两个对象本质上分享着同样的回调。它们也分享着同样的`state`（返回的状态值为"pending"、"resolved"或"rejected"）。这意味着，对同一个 Deferred 对象生成多个Promise对象是毫无意义的。事实上，jQuery 给出的只不过是 __同一个__ 对象。

```js
var promise1 = promptDeferred.promise();
var promise2 = promptDeferred.promise();
console.log(promise1 === promise2);  // true
console.log(promise1 === promise1.promise());  // true
```

使用 `promise`方法的唯一理由就是 __“封装”__。如果传递 `promptPromise`对象，但 __保留promptDeferred 对象为己所用__，则可以肯定的是，除非是你自己想触发那些回调，否则任何回调都不会被触发。纯`Promise` 对象，只能读取其状态及附加回调。

#### jQuery中的Promise对象
Promise 也同样适用于本地的一些异步操作，譬如动画。

任何动画方法都可以接受传入的 __回调__(早期做法)，以便在完成动画时发出通知。

```js
$(' .error ').fadeIn(afterErrorShown);
```

在jQuery 1.6+ 中，可以转而要求 jQuery 对象生成Promise，后者代表了这个对象已附加动画的完成情况，即是否完成了目前正处于挂起状态的动画。

```js
var errorPromise = $( '.error' ).fadeIn().promise();
errorPromise.done(afterErrorShown);
```

对同一个jQuery 对象附加的多个动画会排入队列 __按顺序运行__。仅当调用promise 方法之时已入列的 __全部动画均已执行之后__，相应的Promise 对象才会执行。因此，这会产生两个不同的、按顺序执行的Promise对象（或者根本就不执行，若先调用stop 方法的话）。

```js
var $flash = $('.flash ');
var showPromise = $flash.show().promise();
var hidePromise = $flash.hide().promise();
```

如果使用Deferred 对象的resolve方法作为动画的回调，即可自行轻松生成一个行为完全相同的动画版Promise对象。

```js
var slideUpDeferred = new $.Deferred();
$('.menu').slideUp(slideUpDeferred.resolve);
var slideUpPromise = slideUpDeferred.promise(); //给外部加done回调
```

动画版 Promise附加了额外的信息，其中包括动画运行过程中的计算值props （这对调试非常有价值）。此外，对动画版Promise对象，还可以获得进度通知，以及即时调整动画。[有关这些新特性](https://gist.github.com/54829d408993526fe475)

#### 阮一峰文摘

```!js
//= require jquery.1.9.0
var wait = function(){
  var dtd = $.Deferred(); //在函数内部，新建一个Deferred对象
  var tasks = function(){
    alert("执行完毕！");
    dtd.resolve(); // 改变deferred对象的执行状态
  };
  setTimeout(tasks,2000);
  return dtd.promise(); // 返回promise对象
};
$.when(wait())
  .done(function(){ alert("哈哈，成功了！"); })
  .fail(function(){ alert("出错啦！"); });
```

异步编程同步写法，异步对回调的位置有要求，容易形成多层嵌套，改成同步写法后，位置扁平了

```!js
//= require jquery.1.9.0
var wait = function(){
  var dtd = $.Deferred();
  var tasks = function(){
    alert("执行完毕！");
    dtd.resolve();
  };
  setTimeout(tasks,2000);
  return dtd.promise();
};

var promise = wait();
promise.done(function(){ alert("执行完成前加入的,等执行完响应"); })

setTimeout(function(){
  promise.done(function(){ alert("执行完成后加入的,立即响应"); })
},3000);
```

### 向回调传递数据
Promise对象可以向其回调提供额外的信息，以下代码是等效的：

```js
$.get(url, successCallback);

var fetchingData = $.get(url);
fetchingData.done(successCallback);
```

执行或拒绝Deferred对象时，提供的任何参数都会转发至相应的回调。

```!js
//= require jquery.1.9.0
var aDreamDeferred = new $.Deferred();
aDreamDeferred.done(function (subject) {
  alert('I had the most wonderful dream about ', subject);
});
aDreamDeferred.resolve( 'the JS event model');
```

还有一些特殊的方法能实现在 __特定上下文中__ 运行回调，：`resolveWith` 和`rejectWith`。此时只需传递上下文环境作为第一个参数，同时以数组的形式传递所有其他参数。

```js
var slashdotter = {
  comment: function (editor){
    console.log('Obviously', editor, 'is the best text editor.');
  }
};
var grammarDeferred = new $.Deferred();
grammarDeferred.done( function (verb, object) {
  this[verb](object);
});
grammarDeferred.resolveWith(slashdotter, [' comment', 'Emacs']);
```

对于前面那个例子，使用以下代码亦可得到同样的结果，省去将参数打包成数组的痛苦。

```js
grammarDeferred.resolve.call(slashdotter, ' comment', 'Emacs');
```

### 进度通知
jQuery遵守Promises/A 规范，于是在jQuery 1.7 中为Promise 对象新添了一种可以调用无数次的回调。这个回调叫做progress （进度）。

<!--language: web-->

    <!--language: html-->
    <input id='doc' type='text'/><label id='indicator'/>

    <!--language: js-->
    //= require jquery.1.9.0
    var nanowrimoing = $.Deferred();
    var wordGoal = 5;
    nanowrimoing.progress(function (wordCount) {
      var percentComplete = Math.floor(wordCount / wordGoal * 100);
      $('#indicator').text(percentComplete + '% complete');
    });
    nanowrimoing.done(function(){
      $('#indicator').text('Good job!');
    });

    $('#doc').on('keypress', function(){
      var wordCount = $(this).val().split(/\s+/).length;
      if (wordCount >= wordGoal) {
        nanowrimoing.resolve();
      };
      nanowrimoing.notify(wordCount);
    });

Deferred 对象的notify （通知）调用会调用我们设定的progress回调。就像resolve和reject 一样，notify 也能接受任意参数。请注意，一旦执行(resolve)了nanowrimoing 对象，则再作nanowrimoing.notify 调用将不会有任何反应，这就像任何额外的 resolve调用及reject 调用也会被直接无视一样。

### Promise对象的合并
```js
var gameReadying = $.when(tutorialPromise, gameLoadedPromise);
gameReadying.done(startGame);
```

when 相当于Promise执行情况的逻辑与运算符（AND）。一旦给定的所有Promise均已执行，就立即执行when 方法产生的Promise对象；或者，一旦给定的任意一个Promise被拒绝，就立即拒绝when 产生的Promise。

when 方法的绝佳用例是合并多重Ajax 调用。假设需要马上进行两次post 调用，而且要在这两次调用都成功时收到通知，这时就无需再为每次调用请求分别定义一个回调。

```js
$.when($.post('/1', data1), $.post('/2', data2))
.then(onPosted, onFailure);
```

调用成功时，when 可以访问下辖的各个成员Promise 对象的回调参数，不过这么做很复杂。这些回调参数会当作参数列表进行传递，传递的次序和成员Promise对象传递给when 方法时一样。如果某个成员Promise对象提供多个回调参数，则这些参数会先转换成数组。

因此，要想根据赋予$.when 方法的所有成员Promise 对象获得全部回调参数，可能会写出像下面这样的代码（不推荐这么做）。

```js
$.when(promise1, promise2)
.done( function (promise1Args, promise2Args) {
  // ...
});
```

在这个例子中，如果执行 promise1 时用到了一个参数'complete'，执行promise2 时用到了3 个参数（1、2、3），则 promise1Args就是字符串'complete'，promise2Args就是数组[1,2,3]。

虽然有可能，但如果不是绝对必要，我们不应该自行解析when 回调的参数，相反应该直接向那些传递至when 方法的成员Promise对象附加回调来收集相应的结果。

```js
var serverData = {};
var getting1 = $.get('/1')
.done( function (result) {serverData[ '1' ] = result;});
var getting2 = $.get( '/2')
.done( function (result) {serverData[ '2' ] = result;});
$.when(getting1, getting2)
.done( function () {
  // 获得的信息现在都已位于serverData……
});
```

#### 函数的Promise用法
$.when 及其他能取用Promise 对象的jQuery 方法均支持传入非Promise 对象作为参数。这些非Promise 参数会被 __当成因相应参数__ 位置已赋值而执行的Promise对象来处理。例如

```!js
//= require jquery.1.9.0
var promise = $.Deferred().resolve(' manchu ');
$.when('foo',promise).done(function(){console.log(arguments)})
```

这带来了一个问题：$.when 如何知道参数是不是Promise对象呢？答案是：jQuery 负责检查$.when 的各个参数是否带有promise方法，如果有就使用该方法返回的值。(Promise对象的promise 方法会直接返回自身。)

$.when 方法强行将那些带promise 方法的jQuery 对象转换成了jQuery 动画版Promise对象。因此，如果想生成一个在抓取某些数据且已完成#loading 动画之后执行的Promise对象，只需写下下面这样的代码：

```js
var  fetching = $.get( '/myData');
$.when(fetching, $('#loading'));
```

只是请记住，必须要在动画开始之后再执行$.when 生成的那个Promise 对象。如果#loading的动画队列为空，则立即执行相应的Promise对象。

### 管道连接未来
在JavaScript中常常无法便捷地执行一系列异步任务，一个主要原因是无法在第一个任务结束之前就向第二个任务附加处理器。举个例子，假设我们要从一个URL 抓取数据（GET ），接着又将这些数据发送给另一个URL （POST）。

```js
var getPromise = $.get('/query' );
getPromise.done( function (data) {
  var postPromise = $.post('/search', data);
  //  现在我们想给postPromise 附加处理器……
});
```

在 GET 操作成功之前我们无法对 postPromise对象绑定回调，因为这时postPromise 对象还不存在！除非我们已经得到因$.get调用而异步抓取的数据，否则甚至无法进行那个负责生成postPromise 对象的$.post 调用！

这正是jQuery 1.6 为Promise对象新增pipe（管道）方法的原因。pipe好像在说：“请针对这个 Promise 对象给我一个回调，我会 __归还一个Promise对象__ 以表示回调运行的结果。”

```js
var getPromise = $.get('/query' );
var postPromise = getPromise.pipe(function (data) {
  return $.post( '/search', data);
});
```

pipe 最多能接受3个参数，它们对应着Promise对象的3 种回调类型：done、fail 和progress 。也就是说，我们在上述例子中只提供了执行 getPromise时应运行的那个回调。当这个回调返回的Promise对象已经执行/拒绝时，pipe 方法返回的那个新Promise对象也就可以执行/拒绝。

从效果上看，pipe 就是 __通向未来__ 的一扇窗户！

如果pipe 方法的回调返回值不是Promise/Deferred对象，它就会变成回调参数。举例来说，假设有个Promise对象发出的进度通知表示成 0 与1 之间的某个数，则可以使用 pipe 方法生成一个完全相同的Promise对象，但它发出的进度通知却转变成可读性更高的字符串。

```js
var promise2 = promise1.pipe(null, null, function (progress) {
  return Math.floor(progress * 100) + ' % complete';
});
```

#### 管道级联技术
pipe 方法并不要求提供所有的可能回调。事实上，我们通常只想写成这样：

```js
var pipedPromise = originalPromise.pipe(successCallback);
```

或是这样：

```js
var pipedPromise = originalPromise.pipe(null, failCallback);
```

这种级联技术非常有用，因为它让我们不费吹灰之力就能定义异步任务的分化逻辑。假设有这样一个分成3 步走的进程。

```js
var step1 = $.post(' /step1 ', data1);
var step2 = step1.pipe(function () {
  return  $.post( '/step2' , data2);
});
var lastStep = step2.pipe( function () {
  return  $.post('/step3 ', data3);
});
```

这里的lastStep 对象当且仅当所有这3 个Ajax 调用都成功完成时才执行，其中任意一个Ajax 调用未能成功完成，lastStep 均被拒绝。如果只在乎整体进程，则可以省略掉前面的变量声明。

```js
var posting = $.post('/step1 ', data1)
  .pipe( function () {
    return  $.post('/step2 ', data2)
    .pipe(function () {
       return  $.post('/step3 ', data3);
    });
  });
```

当然，这会重现金字塔厄运。大家应该了解这种书写风格，不过请尽量逐一声明pipe 生成的那些Promise对象。也许并不需要这些变量名称，但它们能让代码更加自文档化。

### jQuery与Promises/A的对比
Q.js 库是最流行的Promises/A 实现，其提供的方法甚至能与jQuery 的Promise和谐共存。这两者的区别只是形式上的，即用相同的词语表示不同的含义。

3.2节中提到过，jQuery 使用resolve 作为fail 的反义词，而Promises/A使用的是fulfill。在 Promises/A 规范中，Promise对象不管是已履行还是已失败，都称“已执行”。

在jQuery 1.8 问世之前，jQuery 的then 方法只是一种可以同时调用done、fail 和progress 这3 种回调的速写法，而Promises/A 的then在行为上更像是jQuery 的pipe。jQuery 1.8 订正了这个问题，使得then 成为pipe 的同义词。

当然还有其他一些细微的差别。例如，在Promises/A 规范中，由then方法生成的Promise对象是已执行还是已拒绝，取决于由then 方法调用的那个回调是返回值还是抛出错误。（在jQuery 的Promise对象的回调中抛出错误是个糟糕的主意，因为错误不会被捕获。）

### 用Promise对象代替回调函数
理想情况下，开始执行异步任务的任何函数都应该返回Promise对象。遗憾的是，大多数 JavaScript API （包括所有浏览器及Node.js均使用的那些原生函数）都基于回调函数，而不是基于 Promise对象。在本节中，我们将看到如何在基于回调函数的API 中使用Promise对象。

在基于回调函数的API 中使用Promise对象的最直接的方法是，生成一个Deferred 对象并传递其触发器函数作为API 的回调参数。例如，我们可以把Deferred对象的`resolve`方法传递给一个像setTimeout这样的简单异步函数。

```js
var timing = new $.Deferred();
setTimeout(timing.resolve, 500);
```

考虑到API 可能会出错，我们还要写一个根据情况指向resolve 或reject 的回调函数。例如，我们会这样写Node风格的回调：

```js
var fileReading = new $.Deferred();
fs.readFile(filename, 'utf8',  function (err) {
  if (err) {
    fileReading.reject(err);
  } else {
    fileReading.resolve(Array.prototype.slice.call(arguments, 1));
  };
});
```

总这么写很麻烦，所以何不写一个工具函数以根据任何给定 Deferred对象来生成Node风格的回调呢？

```js
deferredCallback = function (deferred) {
  return function(err) {
    if (err) {
      deferred.reject(err);
    } else {
      deferred.resolve(Array.prototype.slice.call(arguments, 1));
    };
  };
}

var fileReading = new $.Deferred();
fs.readFile(filename, ' utf8', deferredCallback(fileReading));
```

## Async.js的工作流控制
PubSub这种抽象设计允许应用程序把来源层的事件发布至其他层（譬如，在 MVC三层架构设计模式中把从视图层的事件发布至模型层）。Promise这种抽象设计允许将简单任务表示成对象，进而合并这些对象来表示更复杂的任务。总之，这些抽象设计一直在致力于帮助我们 __解决意面式回调__ 这一问题。

不过我们的武器库仍然有一处短板：迭代。假设需要执行一组 I/O 操作（或者并行执行，或者串行执行），该怎么做呢？这个问题在Node中非常常见，以至于有了个专有名称：工作流控制（也称作控制工作流）。就像 Underscore.js 可以大幅度简化同步代码中的迭代一样，优秀的工作流控制库也可以消解异步代码中的套话。

### 异步工作流的次序问题
假设想先按字母顺序读取recipes（菜谱）目录中的所有文件，接着把读取出的这些内容连接成一个字符串并显示出来。

```js
var fs = require( 'fs');
process.chdir('recipes'); //  改变工作目录

var concatenation = '';

fs.readdirSync('.')
  .filter(function (filename) {
    // 跳过不是文件的目录
    return  fs.statSync(filename).isFile();
  })
    .forEach( function (filename) {
      // 内容添加到输出上
      concatenation += fs.readFileSync(filename, 'utf8')
    });

console.log(concatenation);
```

不过，所有这种 I/O 阻塞的效率都极其低下，尤其是当应用程序还能同时做点其他事情的时候。问题在于不能单纯地将下面这行代码替换掉

```js
concatenation += fs.readFileSync(filename, 'utf8');
//--------------
fs.readFile(filename, ' utf8',  function (err, contents) {
  if (err) throw err;
  concatenation += contents;
});
```

因为这么做根本无法保证按照做出 readFile 调用的次序来触发readFile 调用的回调。readFile 仅仅负责告诉操作系统开始读取某个文件。对操作系统而言，读取短文件通常比读取长文件更快一些。因此，菜谱内容添加到concatenation 字符串的次序是不可预知的。而且，在触发所有回调之后，必须要运行console.log 。

### 异步的数据收集方法
先尝试在不借助任何工具函数的情况下来解决这个问题。最简单的方法是：因前一个 readFile 的回调运行下一个readFile ，同时跟踪记录迄今已触发的回调次数，并最终显示输出。

```js
var fs = require('fs');
process.chdir('recipes'); // 改变工作目录
var concatenation = '';

fs.readdir('.',  function (err, filenames) {
  if (err) throw err;

  function  readFileAt(i) {
    var  filename = filenames[i];
    fs.stat(filename, function (err, stats) {
       if (err) throw err;
       if (! stats.isFile()) return  readFileAt(i + 1);

      fs.readFile(filename, 'utf8',  function (err, text) {
         if (err) throw err;
        concatenation += text;
         if (i + 1 === filenames.length) {
          //  所有文件均已读取，可显示输出
          return  console.log(concatenation);
        }
        readFileAt(i + 1);
      });
    });
  }
  readFileAt(0);
});
```

如你所见，异步版本的代码要比同步版本多很多。如果使用filter、forEach这些同步方法，代码的行数大约只有一半，而且读起来也要容易得多。如果这些漂亮的迭代器存在异步版本该多好啊！使用Async.js就能做到这一点！

#### Async.js 的函数式写法
我们想把同步迭代器所使用的filter 和forEach方法替换成相应的异步方法。Async.js给了我们两个选择

- `async.filter`和`async.forEach` ，它们会并行处理给定的数组。
- `async.filterSeries` 和`async.forEachSeries`，它们会顺序处理给定的数组。

并行运行这些异步操作应该会更快，那为什么还要使用序列式方法呢？原因有两个

- 前面提到的工作流次序不可预知的问题。我们确实可以先把结果存储成数组，然后再 joining（联接）数组来解决这个问题，但这毕竟多了一个步骤。
- Node及其他任何应用进程能够同时读取的文件数量有一个上限。如果超过这个上限，操作系统就会报错。如果能顺序读取文件，则无需担心这一限制。

```js
var async = require('async');
var fs = require('fs');
process.chdir('recipes'); //  改变工作目录
var concatenation = '';

var dirContents = fs.readdirSync('.');

async.filter(dirContents, isFilename,  function (filenames) {
  async.forEachSeries(filenames, readAndConcat, onComplete);
});

function isFilename(filename, callback) {
  fs.stat(filename, function (err, stats) {
    if (err) throw err;
    callback(stats.isFile());
  });
}

function readAndConcat(filename, callback) {
  fs.readFile(filename, 'utf8' , function(err, fileContents) {
    if (err) return callback(err);
    concatenation += fileContents;
    callback();
  });
}

function onComplete(err) {
  if (err) throw err;
  console.log(concatenation);
}
```

现在我们的代码 __漂亮地分成了两个部分__：__*任务概貌*__（表现形式为async.filter调用和async.forEachSeries 调用）和 __*实现细节*__（表现形式为两个迭代器函数和一个完工回调onComplete）。

#### Async.js的错误处理技术
Async.js遵守Node的约定。这意味着所有的 I/O 回调都形如callback(err, results...) ，唯一的例外是结果为布尔型的回调。布尔型回调的写法就是 callback(result) ，所以上一代码示例中的isFilename迭代器需要自己亲自处理错误。

要怪就怪Node的fs.exists 首开这一先河吧！而这也意味着使用了Async.js数据收集方法（filter/filterSeries 、reject/rejectSeries 、detect / detectSeries、some、every 等）的迭代器均无法报告错误。

对于非布尔型的所有Async.js迭代器，传递非 null/ undefined 的值作为迭代器回调的首参数将会立即因该错误值而调用完工回调。这正是readAndConcat 不用throw也能工作的原因。

所以，如果callback(err) 确实是在readAndConcat 中被调用的，则这个err 会传递给完工回调（即 onComplete）。Async.js只负责保证onComplete只被调用一次，而不管是因首次出错而调用，还是因成功完成所有操作而调用。

Node的错误处理约定对Async.js 数据收集方法而言也许并不理想，但对于Async.js的６所有其他方法而言，遵守这些约定可以让错误干净利落地从各个任务流向完工回调。

### Async.js的任务组织技术
Async.js 的数据收集方法解决了一个异步函数如何运用于一个数据集的问题。但如果是一个函数集而不是一个数据集，又该怎么办呢？下面将探讨Async.js 中一些可以派发异步函数并收集其结果的强大工具。

#### 异步函数序列的运行
假设我们希望某一组异步函数能依次运行。在不使用工具函数的情况下，可能会编写出类似这样的代码：

```js
funcs[0](function () {
  funcs[1]( function () {
    funcs[2](onComplete);
  })
});
```

幸好我们还有async.series 和async.waterfall。这两个方法均接受一组函数（即任务列表）作为参数并按顺序运行它们，二者给任务列表中的每个函数均传递一个Node风格的回调。async.series与async. waterfall 之间的差别是，前者提供给各个任务的只有回调，而后者还会提供任务列表中 __前一任务__ 的结果。（所谓“ 结果” ，指的是各个任务传递给其回调的非错误的值。）

```!js
//= require async.0.2.5
var start = new Date;
async.series([
  function (callback) { setTimeout(callback, 100); },
  function (callback) { setTimeout(callback, 300); },
  function (callback) { setTimeout(callback, 200); }
],  function (err, results) {
  alert('Completed in ' + ( new  Date -start) + 'ms');
});
```

Async.js传递给任务列表中每个函数的回调好像在说：“ 出错了吗（回调的首参数是否为错误）？如果没出错，我就要收集结果（回调的次参数）并运行下一个任务了。”

#### 异步函数的并行运行
Async.js提供了async.series的并行版本，即async.parallel。就像async.series 一样，async.parallel 也接受一组形为function(callback) {...}的函数作为参数，但会再加上一个（可选的）在触发最末回调后运行的完工事件处理器。

```!js
//= require async.0.2.5
var start = new Date;
async.parallel([
  function (callback) { setTimeout(callback, 100); },
  function (callback) { setTimeout(callback, 300); },
  function (callback) { setTimeout(callback, 200); }
],  function (err, results) {
  alert('Completed in ' + ( new  Date -start) + 'ms');
});
```

Async.js按照任务列表的次序向完工事件处理器传递结果，而不是按照生成这些结果的次序。这样，我们既拥有了并行机制的性能优势，又没有失去结果的可预知性。

### 异步工作流的动态排队技术
大多数情况下，前两节介绍的那些简单方法足以解决我们的异步窘境，但async.series和async.parallel 均存在各自的局限性。

- 任务列表是静态的。一旦调用了async.series 或async.parallel，就再也不能增减任务了。
- 不可能问：“已经完成多少任务了？”任务处于黑箱状态，除非我们自行从任务内部派发更新信息。
- 只有两个选择，要么是完全没有并发性，要么是不受限制的并发性。这对文件I/O 任务可是个大问题。如果要操作上千个文件，当然不
想因按顺序操作而效率低下，但如果试着并行执行所有操作，又很
可能会激怒操作系统。

Async.js提供了一种可以解决上述所有问题的全能方法：async.queue。

#### 深入理解队列
async.queue 的底层基本理念令人想起DMV（Dynamic Management View，动态管理视图）。它可以同时应对很多人（最多时等于在岗办事员的数目），但并不是每位办事员前面各排一个队，而是维持着一个排号队列。人到了就排队，并取得一个排队号码。任何一个办事员空闲时，就会叫下一个排队号码。

async.queue 的接口比async.series和async.parallel 稍微复杂一些。async.queue 接受的参数有两个：一个是 worker （办事员）函数，而不是一个函数列表；一个是代表着concurrency（并发度）的值，代表了办事员最多可同时处理的任务数。async.queue 的返回值是一个队列，我们可以向这个队列推入任意的任务数据及可选的回调。

```!js
//= require async.0.2.5
function worker(data, callback) {
  console.log(data);
  callback();
}
var concurrency = 2;
var queue = async.queue(worker, concurrency);
queue.push(1);
queue.push(2);
queue.push(3);
```

并发度为 2 时，需要两轮才能遍历事件队列；如果并发度为1，则需要3 轮才能遍历，每轮输出一行代码；如果并发度为3 或更大的值，则只需要1 轮即可遍历。并发度为0 的队列不会做任何事情。如果想要最大的并发度，请直接使用Infinity 关键字。

#### 任务的入列
```js
queue.push([1, 2, 3]);
```

等价于

```js
queue.push(1);
queue.push(2);
queue.push(3);
```

这意味着 __不能直接使用数组__ 作为任务的数据。不过可以使用其他任何东西（甚至函数）作为任务的数据。事实上，如果想让async.queue像async.series/ async.parallel 那样也使用一组函数作为任务列表，只需定义一个其次参数会直接传递给其首参数的 worker 函数。

```!js
//= require async.0.2.5
function  worker(task, callback) {
  task(callback);
}
var concurrency = 2;
var queue = async.queue(worker, concurrency);
var tasks = [function(){console.log(1);},function(){console.log(arguments);}]
queue.push(tasks);
```

async.queue 中的每次push 调用可附带提供一个回调函数。如果提供了，该回调函数会直接送给worker 函数作为其回调参数。因此，（假设worker 函数确实运行了其回调，即它未因抛出错误而直接关停）下面这个例子将会触发3 次输出事件，即输出3 次'Task complete!' 。

```!js
//= require async.0.2.5
function worker(data, callback) {
  console.log(data);
  callback();
}
var concurrency = 2;
var queue = async.queue(worker, concurrency);
queue.push([1, 2, 3], function (err, result) {
  console.log('Task complete!');
});
```

对async.queue 而言，push 方法的回调函数非常重要，因为async. queue不像async.series/ async.parallel 那样可以在内部存储每次任务的结果。如果想要这些结果，就必须自行去捕获。

#### 完工事件的处理
和async.series 及其类似方法一样，我们也可以给async.queue指定一个完工事件处理器。不过，这时并不是传递完工事件处理器作为async.queue 方法的参数，而是要附加它作为async.queue 对象的`drain`（排空）属性。（请想象有一个盆，里面满是待完成的任务。当最后一个任务也排空流出盆时，就会触发完工事件的回调。）下面用计时器做了一个示例。

```!js
//= require async.0.2.5
function  worker(data, callback) {
  setTimeout(callback, data);
}
var concurrency = 2;
var queue = async.queue(worker, concurrency);
var start = new Date;
queue.drain = function () {
  console.log('Completed in ' + ( new Date - start) + 'ms');
};

queue.push([100, 300, 200]);
```

注意，可以一直调用push 方法向队列推入更多后续任务，而且，每当队列中的末任务结束运行时，都会触发一次 drain（也就是说，如果任务队列完工之后再让新任务入列，新任务队列的完工也同样会触发完工事件处理器）。遗憾的是，这意味着async.queue 不能像async.waterfall 那样提供清晰排序的结果。如果想收集那些入列的任务的结果数据，就只能靠自己了。

#### 队列的高级回调方法
尽管drain常常是我们唯一要用到的事件处理器，但async.queue还是提供了其他一些事件及其处理器。

- 队末任务开始运行时，会调用队列的 empty方法。（队末任务运行结束时，会调用队列的drain方法。）
- 达到并发度的上限时，会调用队列的saturated 方法。
- 如果提供了一个函数作为push 方法的次参数，则在结束运行给定任务时会调用该函数，或在给定任务列表中的每个任务结束运行时均调用一次该函数。

### 极简主义者Step的工作流控制
Step 接受一个函数列表作为参数，例子如下。

```js
Step(task1, task2, task3);
```

其中的每一个函数都有3 种控制工作流的方式。

- 它可以调用this，让Step 直接运行列表中的下一个函数。
- 它可以调用n 次由this.parallel 或this.group生成的回调，以告诉Step 应运行列表中的下一个函数n 次。
- 它可以返回一个值，这也会让Step 运行列表中的下一个函数。这简化了同步函数与异步函数的混合使用。

下一个函数会收到上一个函数的结果（或者是其返回的值，或者是其传递给this 的参数），或者是上一函数所有实例的结果（如果因this.parallel 或this.group而运行该函数的话）。其区别在于，this. parallel 会将结果作为一个个单独的参数来提供，而this.group会将结果合并成数组。

这种极简主义的不足是，要想理解Step生成的工作流，只有去通读其中的每一个函数。而无所不包、无所不能的Async.js所生成的工作流一般更能够自我解释。  不过，如果想自己动手、亲历亲为，那么用 Step 来写一些类似于Async.js的工具函数会是一种很好的练习。例如，下面只用11 行代码就实现了等价的async.map。

```js
var Step = require(' step');

function  stepMap(arr, iterator, callback) {
  Step(
    function() {
       var  group = this.group();
       for  (var  i = 0; i < arr.length; i++) {
        iterator(arr[i], group());
      }
    },
    callback
  );
}
```

使用Async.js主要是想找到最适合任务的那个工具函数，而Step 却能鼓励我们透彻地思考问题并编写出优雅高效的解决方案。

## worker对象的多线程技术
应用程序的主线程可以这样跟worker 说：“ 去，开一个单独的线程来运行这段代码。”worker 可以给我们发送消息（反之亦可），其表现形式是事件队列中运行的回调（还能是别的吗？）。简而言之，与不同线程进行交互的方式与在JavaScript中进行I/O 操作一模一样。

### 网页版worker对象
要想生成worker 对象，只需以脚本URL 为参数来调用全局Worker 构造函数即可。

```js
var worker = new Worker('worker.js');
worker.addEventListener('message',  function (e) {
  console.log(e.data); //  重复由postMessage 发送的任何东西
});
```

通常，我们只用到 message事件的data 属性。如果已经把同一个事件处理器绑定至多个worker 对象，则可能还要用到e.target 以定位是哪一个worker 对象触发了事件。

现在我们知道如何监听 worker 对象了。worker 对象的交互接口呈现出便利的对称设计：我们可以使用 worker.postMessage 来发送消息，而worker 本身可以使用 self.addEventListener('message',...)来接收消息。下面是一个[完整的例子](http://webworkersandbox.com/53b3b5ce02a3ad0000000210)。

主脚本

```js
var worker = new Worker( 'worker.js');
worker.addEventListener('message', function (e) {
  console.log(e.data);
});
worker.postMessage('football ');
worker.postMessage('baseball ');
```

worker.js

```js
self.addEventListener('message',  function (e) {
  self.postMessage('Bo knows  ' + e.data);
});
```

#### 网页版worker 对象的局限性
网页版worker 对象的首要目标是，在不损害DOM响应能力的前提下处理复杂的计算。它有以下几种潜在用法：

- 解码视频。流入的视频采用Broadway实现的H.264编解码器
- 采用斯坦福的JavaScript加密库加密通信。
- 解析网页式编辑器中的文本，如Ace 编辑器

事实上，Ace 默认已经这么做了。在基于Ace 的编辑器中键入代码时，Ace 需要先进行一些相当繁重的字符串分析，然后再使用恰当的语法高亮格式更新DOM。在现代浏览器中，这种分析工作通常会另开一个独立线程来做，以确保编辑器能保持平滑度和响应度。

通常情况下，worker 对象会把自己的计算结果发送给主线程，由主线程去更新页面。为什么不直接更新页面呢？这主要是为了 __保护JavaScript异步抽象概念__，使其免受影响。如果worker 对象可以改变页面的标记语言，那么最终的下场就会和 Java 一样——必须将DOM操控代码封装成互斥量和信号量以避免竞态条件。

基于类似的理由，worker 对象也看不到全局的window 对象和主线程及其他worker 线程中的其他任何对象。通过 postMessage 发送的对象会透明地作序列化和反序列化，想想 JSON.parse(JSON. stringify(obj)) 吧。因此，原始对象的变化不会影响该对象在其他线程中的副本。

worker 对象甚至不能使用老实可靠的console 对象。它只能看到自己的全局对象self，以及 self 已捆绑的所有东西：标准的JavaScript对象（如setTimeout和Math），以及浏览器的 Ajax 方法。

worker 对象可以随意使用XMLHttpRequest。如果浏览器端支持的话，它甚至能使用WebSocket。这意味着worker 可以直接从服务器拉取数据。如果要处理大量数据（譬如说，流传输一段需要解码的视频），相比于使用postMessage 来序列化这些数据，将数据封闭在一个线程中会更好。

还有一个特殊的importScripts 函数可以同步地加载并运行指定的 脚本。

```js
importScripts('https://raw.github.com/gist/1962739/danika.js');
```

正常情况下，同步加载是绝对禁忌，不过worker 对象跑的是另一个线程。既然worker 已经无所事事了，那么阻塞就阻塞好啦！

### cluster带来的Node版worker
略

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

以下摘自underscore.js 1.2，最新版本有变化，[underscore.js更详细的演进](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fraw.gitcafe.com%2Fzyxstar%2Fmd_note%2Fmaster%2Fdocs%2FFramework%2FUnderscore_throttle.md)：

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


## 反应型编程(FRP)


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


参考网站
=============
- [阮一锋](http://javascript.ruanyifeng.com/)


<script>

(function fix_toc(){
    if(typeof expand_toc !== 'function') setTimeout(fix_toc,500);
    else expand_toc('md_toc',2);
})();

</script>


<!--
http://yuilibrary.com/yui/docs/tutorials/gbs/

http://www.cnblogs.com/nuysoft/archive/2011/11/14/2248023.html

http://www.cnblogs.com/caishen/default.html?page=3&OnlyTitle=1


http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html

http://www.cnblogs.com/zhongweiv/p/nodejs_module.html
http://www.zhangxinxu.com/wordpress/2010/12/css-box-flex%E5%B1%9E%E6%80%A7%EF%BC%8C%E7%84%B6%E5%90%8E%E5%BC%B9%E6%80%A7%E7%9B%92%E5%AD%90%E6%A8%A1%E5%9E%8B%E7%AE%80%E4%BB%8B/
-->

