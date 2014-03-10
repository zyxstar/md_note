> 2013-06-18

对象
======


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

> 类似ES5中的`Object.create`，缺点是新对象的`constructor`指向原型对象的构造器

原型连接在更新时是不起作用的，当我们对某个对象做出改变时，不会触及到该对象的原型，原型连接只有在检索值的时候才被用到。

## 反射
检查对象并确定对象有什么属性是很容易的事情，只要试着去检查该属性并验证取得的值。`typeof`操作符对确定属性的类型很有帮助

<!--language: !js-->

    function Flight(){
        this.number = 10;
        this.status = "";
        this.arrival = {};
    }

    flight = new Flight;

    alert(typeof flight.number);
    alert(typeof flight.status);
    alert(typeof flight.arrival);
    alert(typeof flight.mainfest);
    alert(typeof flight.toString);
    alert(typeof flight.constructor);

有两个方法去处理这些不需要的属性。

- 第一个是让程序检查并剔除函数值。一般来说，做反射的目标是数据，一些值可能会是函数
- 另一个方法是使用`hasOwnProperty`方法，如果对象拥有独有的属性，它将返回`true`，它不会检查原型链

函数
========
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


## 调用
js中共有四种调用模式：方法调用模式、函数调用模式、构造器调用模式、`apply`调用模式，这些模式在如何初始化关键参数`this`上存在差异

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

### apply调用模式
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

通过`apply`将 __数组参数展开传递__。

- 示例2

假如存在一个对不固定参数求和的函数`sum`

<!--language: !js-->

    function sum(){
        return [].slice.call(arguments).reduce(function(acc,x){return acc+x;},0);
    }

    var arr = [1,2,3,4,5];
    alert(sum.apply(null, arr));

所以当存在对不固定参数求值的函数时，`apply`可以理解为，将此 __函数作用于一个数组__ 上。原理同上例一样，即将数组参数展开传递。

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


## 给类型增加方法

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

## 递归
一些语言提供了 __尾递归__(tail recursion/trai-end recursion) 优化，即如果一个函数的最后执行递归调用语句的特殊形式的递归，那么调用的过程会被替换为一个循环，可以提高速度。但js __并没有提供__ 该优化。

## 作用域
作用域控制着变量与参数的可见性及生命周期，但js中 __没有块级作用域__。它会导致在闭包中行为与有块级作用域的语言中表现不同：

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

## 链式代码
一些设置或修改对象的某个状态却不返回任何值的方法，但如果我们让这些方法返回`this`，而不是`undefined`，就可以启用级联。

## 柯里化
函数也是值，柯里化`Curry`允许我们将函数与传递它的参数相结合去产生一个新的函数

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
    alert(add2_x(6));

    function add3(x,y,z){return x+y+z;}
    var add3_x = add3.curry(1);
    var add3_x_y = add3_x.curry(2);
    alert(add3_x_y(6));

## 缓存
用对象去记住先前操作的结果，从而避免无谓的运算。

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


继承
===========

## new的过程

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
1. `p.__proto__ = Person.prototype;` 实例的`__proto__`正巧指向构造器的`prototype`上，通过此找到原型
1. `Person.apply(p, arguments);` 构造`p`，修正`this`指向，使实例得到构造器定义的相关属性/方法













<script>

(function fix_toc(){
    if(typeof expand_toc !== 'function') setTimeout(fix_toc,500);
    else expand_toc('md_toc',6);
})();

</script>


