> 2013-06-18

语言核心
=========
本部分主要摘自[Stoyan Stefanov](http://hub.tutsplus.com/authors/stoyan-stefanov)，[zhangxinxu](http://www.zhangxinxu.com/)，[TomXu](http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html)相关内容

## 执行上下文
执行上下文可以抽象为一个简单的对象。每个上下文包含一系列属性(我们称之为 上下文状态(context’s state) ) 用以跟踪相关代码的执行过程。下图展示了上下文的结构，所需要的属性([变量对象(variable object)](#TOC1.2)，[this指针(this value)](#TOC1.4)，[作用域链(scope chain)](#TOC1.5)：

![js_execution_context](../../../imgs/js_execution_context.png)


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



非标准的`__proto__`属性








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
      Foo.prototype.constructor === Foo // true

      b.calculate === b.__proto__.calculate, // true
      b.__proto__.calculate === Foo.prototype.calculate // true

    );

上述代码可表示为如下的关系：

![constructor_proto_chain](../../../imgs/constructor_proto_chain.png)














### 使用原型被重写前属性
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


js原生不支持lambda，但借助一些库（如[Functional Javascript](http://osteele.com/sources/javascript/functional/)，[lambda.js](http://www.javascriptoo.com/lambda-js)）可实现相关功能，或直接使用[CoffeScript](http://coffeescript.org/)

### map与reduce
以下概念对mongodb等分布式并发NoSql具有指导意义，对于js编程可作为参考

指定一个Map（映射）函数，用来把一组键值对映射成一组新的键值对，指定并发的Reduce（化简）函数，用来保证所有映射的键值对中的每一个共享相同的键组

简单来说，一个映射函数就是对一些独立元素组成的概念上的列表（例如，一个测试成绩的列表）的每一个元素进行指定的操作（比如，有人发现所有学生的成绩都被高估了一分，他可以定义一个“减一”的映射函数，用来修正这个错误。）。事实上，每个元素都是被独立操作的，而原始列表没有被更改，因为这里创建了一个新的列表来保存新的答案。这就是说，Map操作是可以高度并行的（js中并非并行），这对高性能要求的应用以及并行计算领域的需求非常有用。

而化简操作指的是对一个列表的元素进行适当的合并（继续看前面的例子，如果有人想知道班级的平均分该怎么做？他可以定义一个化简函数，通过让列表中的奇数（odd）或偶数（even）元素跟自己的相邻的元素相加的方式把列表减半，如此递归运算直到列表只剩下一个元素，然后用这个元素除以人数，就得到了平均分）。虽然他不如映射函数那么并行，但是因为化简总是有一个简单的答案，大规模的运算相对独立，所以化简函数在高度并行环境下也很有用。


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

另一种方式是将中间结果作为函数对象的属性进行保持：

<!--language: !js-->

    function fn() {
        var fnarr = [];
        for (var i = 0; i < 3; i++) {
            (fnarr[i] = function(){alert(arguments.callee.x);}).x = i;
        }
        return fnarr;
    }

    fn().forEach(function(_f){_f();});


## 递归/reduce
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

[Lo-Dash](http://lodash.com/)也将加入惰性求值

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

第6章依赖管理
CommonJS
模块的声明
模块和浏览器
模块加载器
Yabble
RequireJS
包装模块
模块的按需加载
LABjs
无交互行为内容的闪烁（FUBC）

第9章测试和调试
单元测试
断言
QUnit
Jasmine
驱动
无界面的测试
Zombie
Ichabod

<!-- http://www.cnblogs.com/TomXu/archive/2011/12/15/2288411.html -->
<!-- http://www.cnblogs.com/zhongweiv/p/nodejs_module.html -->
<script>

(function fix_toc(){
    if(typeof expand_toc !== 'function') setTimeout(fix_toc,500);
    else expand_toc('md_toc',2);
})();

</script>




<!-- http://www.cnblogs.com/caishen/default.html?page=3&OnlyTitle=1 -->