js语法
==============

## 语言标识符
语言标识符可分两类：

1. 命名语法以及类型，称为“语法关键字”
1. 命名值（的存储位置），称为“变量”和“常量”

由此引入一个概念“绑定”，从标识符角度来说，就分为语法关键字与语法（语义）逻辑绑定，以及变量与它所存储的位置的绑定。

- 语法关键字对逻辑的绑定的结果，是 __作用域__ 的限定；
- 变量对位置的绑定的结果，则是 __变量生存周期__ 的限定

所谓“声明”，即是约定变量的生存周期和逻辑的作用域

- 纯粹陈述“值”的过程，称为变量和类型声明（js中无显式类型声明）
- 纯粹陈述“逻辑”的过程，称为语句（含流程控制子句）
- 陈述“值与（算法的）逻辑”的关系的过程，称为表达式

## 变量声明
js是 __弱类型__ 语言，表明该语言 __在表达式运算中不强制校验运算元的数据类型__，而并不表明该语言是否具有类型系统（并非无类型语言）

### 变量的数据类型

6种基本数据类型，任何一个变量或值的类型都可以使用 __`typeof`__ 运算符来得到，以字符串形式返回此6种类型值之一：

- undefined
- number
- string
- boolean
- function 具有多种含义：函数、方法、构造器、类、函数对象等
- object 基于原型继承的面向对象

值类型与引用类型
function object为引用类型，其它均为值类型，包括undefined，但string在赋值运算中会按引用类型方式来处理

在js中，使用“全等(===)运算符”用来对值类型/引用类型的实际数据进行比较和检查：

- 一般表达式运算的结果总是值
- 函数或方法调用的结果可以返回值或引用
- 值与引用，值与值之间即使等值(==)，也不一定全等(===)
- 两个引用之间如果等值(==)，则一定全等(===)

一个值应该与其自身等值与全等，特殊NaN值，自身并不等值，也不全等

<!-- language: !js -->

    var str="abcde";//string类型
    var obj=new String(str);//new出来的即成为object类型
    function func(val){
        val.toString=function(){
            return "hello";
        };
    }
    func(str);//值类型的toString的修改无意义
    alert(str);

    func(obj);//成功的改变了toString，影响到后面的alert
    alert(obj);

### 变量声明

- 显式声明
    - 使用var，var str='test'
    - 在for中，for(var n in obj){}
    - 函数声明，function foo(){}
    - 声明异常对象，try{}catch(ex){}

- 隐式声明
    - 发生在不使用var赋值语句中，如a=100;
    - 如果变量未被声明，则该语句是变量声明，并立即赋给值

<!-- language: !js -->

    function foo1 () {
        a=100;//隐式声明，a成为全局变量
    }
    foo1();
    alert(a);//100

    function foo2 () {
        alert(b);//undifined
        b=100;
        var b;
        alert(b);//100
    }
    foo2();
    alert(b);//未定义b

### 直接量/初始器声明

<!-- language: table -->

    |              | 类型        | 直接量/初始器声明 | 包装对象 |
    |-------------:|-------------|:------------------|:--------:|
    | **基本类型** | *undefined* | `v=undefined`     | ~~无~~   |
    | **基本类型** | *string*    | `v='..';v=".."`   | String   |
    | **基本类型** | _number_    | `v=1234 `         | Number   |
    | **基本类型** | _boolean_   | `v=true;v=false`  | Boolean  |
    | **基本类型** | _function_  | `v=function(){}`  | Function |
    | __对象__     | _object_    | `v={..};v=null`   | Object   |
    | __对象__     | _regex_     | `v=/.../..`       | RegExp   |
    | __对象__     | _array_     | `v=[...]`         | [__Array__](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)    |

#### ES5对象直接量中属性读写器
读写器(get/setter)，类似函数，但不使用function作为关键字，get/setter如果只使用其中一个表示只读/只写

<!-- language: !js -->

    var obj1={
        name:'obj1',
        get aName(){return this.name;},
        set aName(val){this.name=val;}
    }

    alert(obj1.aName);
    obj1.aName='z';
    alert(obj1.aName);


#### 初始化器与直接量区别
直接量的声明中 ___不包括___ 运算过程，而初始化器的声明中是 ___可以包括___ 运算过程的。

ES5明确了5种直接量声明(加上undefined为6种)，意味道着 __直接量__ 的值可以在声明的同时，在引擎看来即是编译期就确知的：`undefined`,`null`,`true/false`,`数值`,`字符串`,`正则表达式`

而数组和与对象的声明形式就不具备这一点，所以称为 __初始器__，并归类于 __表达式__ 中

## 表达式运算
### 一般表达式运算
js中一般表达式运算只操作两种运算数：数值（加减乘除、位运算）和布尔值，这些运算的 __运算元与结果值总是同一类型__

### 逻辑运算
一般语言中，逻辑运算与布尔运算是等义的，其运算元与目标类型都是布尔值，但js中除了支持纯布尔运算（上述），还有一种逻辑运算，它的 __表达式结果是不确定__ 的：

- 运算符`||`和`&&`既 __ *不改变* 运算元的数据类型，也 *不强制* 运算结果的数据类型__
- 运算符会将运算元理解为布尔值，以进行布尔运算
- 运算过程（与普通布尔运算一样）是支持布尔短路的

表达式的运算元可以是值或其它表达式（包括函数调用），因此连续的逻辑运算也可以用来替代语句，__通过表达式消灭分支语句__

<!-- language: !js -->

    var str="hello";
    var obj={};
    x=str || obj;
    y=str && obj;
    alert(x);
    alert(y);

    if(str || obj){}
    z=!str && !(str || obj);
    alert(z);

### 字符串运算
只有一种字符串运算，`+`，字符串连接，总是产生一个新的字符串，完全等同于调用字符串对象的`concat()`方法

### 比较运算

#### 等值检测

- __相等/等值__ 指`==`和`!=`的运算效果
- __相同/全等__ 指`===`和`!==`的运算效果（除了看值是否相等，还要看是否有相同的数据类型）

相等运算规则：

- 两个值类型进行比较，转换成相同数据类型的值进行“数据等值”比较
- 值类型与引用类型比较，引用类型的数据转换为与值类型数据相同的数据，再进行“数据等值”比较
- 两个引用类型比较，比较引用（的地址）

相同运算规则：

- 两个值类型进行比较，如同数据类型不同，则必然不相同，如果类型相同，再进行“数据等值”比较
- 值类型与引用类型比较，必然不相同
- 两个引用类型比较，比较引用（的地址）

<!-- language: !js -->

    var str="abc";
    var obj1=new String(str);
    alert(str==obj1);//true
    alert(str===obj1);//false

    var obj2=new String(str);
    alert(obj1==obj2);//false
    alert(obj1===obj2);//false

    alert(1=='1');//true

    var obj3={
        valueOf:function(){return 1;}
    }
    alert(1==obj3);//true
    alert("1"==obj3);//true

    var obj4={
        toString:function(){return "1";}
    }
    alert(1==obj4);//true
    alert("1"==obj4);//true

#### 序列检测
比较变量在序列中的大小，即数学概念中数轴位置先后，可比较的数据类型：boolean、string、number，相关运算符：>、>=、<、<=，结果返回true/false

运算规则：

- 两个值类型进行比较，直接比较 __数据__ 在序列中的大小
- 值类型与引用类型比较，将引用类型的数据转换为与值类型数据相同的数据，再进行“序列大小”比较
- 两个引用类型比较，返回false，除非定义了valueOf()

<!-- language: !js -->

    var o1={};
    var o2={};
    var str='123';
    var num=1;
    var b0=false;
    var b1=true;
    var ref=new String();
    //值类型比较，考察布尔值与数值在序列中大小
    alert(b1<num);
    alert(b1<=num);
    alert(b1>b0);
    //值类型与引用类型比较，空字符串转换成0值
    alert(num>ref);
    //两个对象（引用类型）比较时总是返回false
    alert(o1>o2||o1<o2||o1==o2);

只有两个运算元都是字符串时，运算符才表示字符串序列检测，任意一个运算元非字符串时，将按数值来进行比较

<!-- language: !js -->

    var s1='abc';
    var s2='ab';
    var s3='101';
    var b=true;
    var i=100;
    //两个运算元为字符串，将比较每个字符的序列值，显示true
    alert(s1>s2);
    //当字符串与其它类型比较时，将字符串转换为数值比较，显示true
    alert(s3>i);
    //将字符串转换为数值时得到NaN，b转换成1，所以显示false
    alert(s1>b||s1<b||s1==b);
    //NaN不等于自身，也不大于或小于自身，显示false
    alert(s1>NaN||s1<NaN||s1==NaN);

### 赋值运算
赋值是一个运算，而不是一个语句，在赋值表达式中，运算符左右都是运算元。
赋值的效果是修改存储单元中的值，对值类型而言是复制数据，对引用类型而言，则是复制一个地址。

有一个特例，值类型的字符串是一个大的、不确定长度的连续数据块，这导致复制数据的开销很大，所以js中将字符串的赋值也变成了复制地址，引入了三条字符串处理的限制：

- 不能直接修改字符串中的字符
- 字符串连接运算必然导致写复制，这将产生新的字符串
- 不能改变字符串的长短，如修改length是无意义的

### 函数调用
运算符`()`，使函数得以执行，并且函数执行时，从 __左至右运算并传入__ ()内的参数序列

这里的函数包括function直接量，或创建自Function类的函数对象(typeof返回function，而非object)

### 特殊作用的运算符
- `typeof`(是运算符，非关键字)，返回运算元数据类型的字符串
- `void` 避免表达式返回值，总返回undefined
- `?:` 三元条件运算符
- `()` 优先级运算符
- `,` 多重求值或逗号运算符

## 语句
- 语句由语法分隔符`;`来分隔（它不是运运算符）
- 除空语句和语句中的控制子句外，语句存在返回值，由执行中最后一个子句/表达式的值决定
- 赋值语句，使用等号赋值运算符
- 变量声明语句，使用var关键字开始一个变量的声明
- 标签声明语句，使用`identifier:statement`语句开始一个标签声明

### 表达式语句
只有表达式，而没有其他语法元素的语句，表现为`表达式;`

语句的返回值由最后一个子句或表达式的值决定，因此表达式语句的值，就是该表达式运算的结果，即使不返回值（严格说返回undefined），也可以参与后续运算

#### 一般表达式语句
`eval('1+2+3')`，eval总是执行 __语句__，所以上面等同于`eval('1+2+3;')`
#### 赋值语句与变量声明语句
`str2='this is a '+(str1 = 'test string')`

`var i=100;`是在语法解释期和执行期分两次来实现的

- 变量声明语句，是一个有关键字的语句而非表达式语句，其`=`号的含义是语法分隔符，并指示在运行期进行一次赋值运算
- 赋值语句，是一个表达式语句，其`=`号是赋值运算符

js中变量作用域只能达到函数一级，而非语名一级（没块作用域）

#### 函数调用语句
没用引用的匿名函数的调用

<!-- language: !js -->

    //内侧括号(1)用于函数调用，外侧括号是强制运算符，返回调用结果
    (function(i) {
        alert(i);
        return i;
    }(1));

    //前一个包住整个function括号是强制运算符，返回函数自身，(2)代表函数调用
    (function(i) {
        alert(i);
        return i;
    })(2);

    //调用函数并忽略返回值，void用于使其后的函数表达式执行运算
    void function(i) {
        alert(i);
    }(3);

以下不能通过语法检测

<!-- language: !js -->

    //被解释成function(){};和();
    function(){
    }()

但这样可以

<!-- language: !js -->

    var f=function(i){
        return i;
    }(1);//f===1;


### 流程控制：一般子句

#### 标签声明
js中标签只能被break或continue语句所引用，可以在任何一个语句前面加上标签，以使得该语句被标签化，除了单一语句外，标签也可以作用于由大括号表示的复合语句

#### break子句
默认情况下，break子句作用于循环语句的 __最内层__（需要一次跳出多重循环时需要配合break `label`），或者整个switch，因此它不必特别指定中断语句的范围。但还有一种扩展的语法，以指示它所作用的范围，该范围用声明过的`标签`来表示：

<!-- language: !js -->

    var str=prompt('input','123456');
    my_label:{
        if(str && str.length<10){
            break my_label;//break指定的标签
        }
        str=str.substr(str.length-10);
    }
    alert(str);

    my_label2:{
        for(var i=0;i<10;i++){
            for(var j=0;j<10;j++){
                if(i===2 && j===5)
                    break my_label2;//break出多重循环
            }
        }
    }
    alert(i);
    alert(j);

#### continue子句
仅对循环语句有意义，因此只能作用于for、 for..in、 while、 do..while这些语句的内部，默认情况下，它表明停止当前循环并跳转到下一次循环迭代开始处运行。continue后面也可以带一个标签，表明从循环内部中止，并继续到标签指示处开始执行，但它对标签有要求，只能对单个循环语句有意义，只能跳转到“当前/外层的单循环语句的起始”：

<!-- language: js -->

    continueHere://此处不能加{}
        for(var i=0;i<10;i++){
            for(var j=0;j<10;j++){
                if(i===2 && j===5){
                    continue continueHere;//直接跳到外层for，后面代码不执行
                }
                //……
            }
            //……
        }

#### return子句
当return子句没有指定返回值时，该函数返回undefined

当void运算调用函数时，return子句所指定的返回值将被忽略，以下代码返回undefined

<!-- language: !js -->

    function foo(){return true;}
    var v=void foo();
    alert(v);

当执行函数逻辑没有return子句时，函数将执行到最后一条语句，并返回undefined

### 流程控制：异常

<!-- language: !js -->

    try{//在此return,仍先执行finally中代码
        return true;
    }
    finally{
        alert('finally1');
    }

<!-- language: !js -->

    mylabel:
    try{//在此break,仍先执行finally中代码
        break mylabel;
    }
    finally{
        alert('finally2');
    }


## 面向对象编程的语法概要

### 关于对象成员删除
可以删除对象成员，数组元素，全局对象的成员

<!-- language: !js -->

    var obj={method:function(){},prop:'some'};
    delete obj.method;//非继承自原型的
    delete obj['prop'];//非继承自原型的
    alert('method' in obj);
    alert('prop' in obj);
    var arr=[1,2,3,4]
    delete arr[2];
    alert(arr);
    alert(arr[2]);//undefined
    alert(arr.length);//长度未变
    golbal_val='someval'
    delete golbal_val;
    alert('golbal_val' in window);
    delete isNaN;//全局函数
    alert('isNaN' in window);

但不能删除：

- __var声明__ 的变量
- 直接 __继承自原型__ 的成员，即不能通过实例来删除原型的以及父代类原型的成员

<!-- language: !js -->

    function MyObject() {
        this.name="ins's name";
    }
    MyObject.prototype.name="proto's name";//设置 构造器的 原型对象 中的属性

    var obj=new MyObject();
    alert(obj.name);
    delete obj.name;
    alert(obj.name);


关于上述第二点，但如果真的需要删除该属性，可以对 __原型实例__ 进行操作，当然这会影响到这个类构造的所有实例：

<!-- language: !js -->

    function MyObject() {
    }
    MyObject.prototype.name="proto's name";

    var obj1=new MyObject();
    var obj2=new MyObject();

    delete obj1.constructor.prototype.name;//删除 对象的 构造器的 原型对象 中的属性
    alert(obj1.name);//undefined
    alert(obj2.name);//undefined,也受影响

delete仅在删除一个不能删除的成员时，才会返回false，其他情况下，例如删除不存在的成员，或者继承自父代类/原型的成员，应当返回true，但因“不能删除成员”而返回false的情况并不多，大概有Object.prototype，Function的lenght,prototype,arguments等，而且宿主环境不同结果也不同

### 对象及其成员检查
检查某一个对象是否具有某个成员，使用`in`运算来得到结果

<!-- language: js -->

    'prop' in obj

但这种检查并不一定可靠，早期js引擎没实现`in`，因此可以使用如下:

<!-- language: js -->

    if(obj.prop){}

但注意这里存在一个“隐含的类型强制转换”，当存在属性，但属性值为以下情况可能导致false:

<!-- language: !js -->

    var obj={};
    function _in(obj,prop) {
        if(obj[prop]) return true;
        return false;
    }
    var propValues=[0,'',[],false,undefined,null];//空数组返回true??
    for(var i=0;i<propValues.length;i++){
        obj.prop=propValues[i];
        alert(_in(obj,'prop'));
    }

所以js中推荐另一种方案

<!-- language: !js -->

    var obj={};
    function _in(obj,prop) {
        if(typeof(obj[prop])!='undefined') return true;
        return false;
    }
    var propValues=[0,'',[],false,undefined,null];//但如果undefined时，返回false
    for(var i=0;i<propValues.length;i++){
        obj.prop=propValues[i];
        alert(_in(obj,'prop'));
    }

但该方案不能解决，存在某属性，但值为undefined时的情况，所以早期DOM约定“如果一个属性没有初值，则应该设置为null”(不能不设置，也不能设置为undefined)，但后期的js版本基本上都实现了`in`

对象是不是另一个类的实例，使用`instanceof`运算符

<!-- language: js -->

    obj instanceof MyCls

`instanceof`会检测类的继承关系，因此一个子类的实例，在对祖先类做`instanceof`运算时，仍然得到true

### 可列举性
当某个对象成员不存在或它不可列举时，则使用`propertyIsEnumerable()`将返回false，比较常见的情况是，js对象的内置成员不能被列举，在这些情况下，可以用`in`运算检测到该成员，但不能用`for..in`语句来列举它

<!-- language: !js -->

    var obj={};
    alert(obj.propertyIsEnumerable('constructor'))//false
    alert('constructor' in obj)//true

但ECMAScript对它的规范是不检测对象的原型链的，所以原型链上的成员是可以被`for..in`的，但不它的`propertyIsEnumerable()`返回false，即“只检测对象的非(自原型链继承而来的)继承属性”，基本上等同于`hasOwnProperty()`，略有区别见[stackoverflow](http://stackoverflow.com/questions/10968962/hasownproperty-vs-propertyisenumerable)

### 默认对象的指定
`with`，以便开发人员能在一个代码块中显示地指定默认对象，对于闭包来说，具有某些特别的语法效果，但ES5严格模式中禁用with

<!-- language: !js -->

    var obj={}
    obj.val=100;
    with(obj){
        val*=2;
    }
    alert(obj.val);

## ES5严格模式下的语法限制
严格模式需要使用字符串序列

<!-- language: js -->

    "use strict"

有7种语法在严格模式中被禁用了，下面示例将抛出`SyntaxError`

- 在对象直接量声明中，存在相同的属性名

<!-- language: !js -->

    "use strict"

    var obj={name:1,name:2};

- 在函数声明中，参数表中带有相同的参数名

<!-- language: !js -->

    "use strict"

    function foo(x,x,z){}

- 不能重写eval和arguments这两个标识符，意味着
    - 不能出现在赋值运算符的左边
    - 不能使用var语句来声明
    - catch子句的参数不能使用这两个标识符
    - 具名函数的函数名不能使用这两个标识符
    - 不能使用delete去删除

<!-- language: !js -->

    "use strict"

    eval={};
    var arguments;
    try{}catch(eval){}
    function arguments(){}
    function foo(){delete arguments;}

- 使用0前缀声明的8进制直接量

<!-- language: !js -->

    "use strict"

    var num=012;

- 用delete删除显式声明的标识符、名称或具名函数，在非严格模式下，这些操作通常是无效的，但不会抛出异常，但严格模式下解析不能通过

<!-- language: !js -->

    "use strict"

    var x;
    delete x;

    function foo(){}
    delete foo;

    function foo(x){delete x;}

    try{}catch(e){delete e;}

- 在代码中使用一些扩展的保留字，implements,interface,let,package,private,protected,public,static,yield

<!-- language: !js -->

    "use strict"

    var yield;
    function let(){}

- 在代码中包括with语句，在严格模式下with语句直接被禁止了







## 运算符的二义性
### 加号`+`的二义性
如果表达式中存在字符串，则优先按字符串连接进行运算，特别在DOM模型中，许多style属性的看起来是数字，实际却是字符串，如`borderWidth=1px`时：

<!-- language: !js -->

    var div=document.createElement('div');
    div.id='top';
    div.style.borderWidth='1px';
    document.documentElement.appendChild(div);

    var el=document.getElementById('top');
    alert(el.style.borderWidth+=10);//'1px10'，并且赋值不成功

### 括号`()`的二义性
充当`if`、 `while`、 `do..while`语句中的词法元素时，括号会有“将表达式结果转换为布尔值”的副作用

括号用于强制表达式运算，基本含义是我们常说的强制运算优先级，但事实上，不管有没有优先级的问题，括号总是会强制其内部的代码 __作为表达式运算__:

<!-- language: !js -->

    var str1=typeof(123);//强制123作为表达式运算，结果还是123，再进行typeof运算
    var str2=('please',100);//强制两个单值表达式做连续运算，返回100
    function foo(){return(1+2);}//return后面的括号表示“强制表达式”，而不是把return当成函数

当括号作为运算符时，它只作用于表达式运算，而不能作用于语句

### 大括号`{}`的二义性

<!-- language: js -->

    if(true){
        entry:1
    }

“语句优先”，时序上优先代码执行，entry会被理解为标签

<!-- language: !js -->

    var code='if(true){entry:1}';
    alert(eval(code));//1

    code='if(true){{entry:1}}';
    alert(eval(code));//1

    code='if(true)({entry:1})';
    alert(eval(code));//[object Object]

### 逗号`,`的二义性

<!-- language: !js -->

    a=(1,2,3);//a为3，表达式结果为3
    a=1,2,3;//a为1，表达式结果为3
    var a=1,2,3;//语法解析期出错，逗号被理解为分隔多个变量的语法，而不是连续运算，而2,3显然不是合法的标识符

### 方括号`[]`的二义性

<!-- language: !js -->

    a=[[1][1]];//第一个[1]是数组，第二[1]是取数组下标，返回undefined，a为[undefined]
    alert(a);

    tab1=[
    ['A',1,2,3]
    ['B',3,4,5],
    ['C',5,6,7]
    ]//['A',1,2,3]为数组，['B',3,4,5]中逗号为语法分隔或连续运算符，返回5，取前者下标为5的值，结合返回undefined，tab1=[undefined,['C',5,6,7]]
    alert(tab1);

    tab2=[
    ['A',1,2,3]
    ['B',3,4,0],
    ['C',5,6,7]
    ]//tab2=['A',['C',5,6,7]]
    alert(tab2);

    tab3=[
    ['A',1,2,3]
    ['B','length'],
    ['C',5,6,7]
    ]//length作为数组的成员存取，tab3=[4,['C',5,6,7]]
    alert(tab3);

# js非函数式语言特性

## 命令式语言
命令式语言演化分为“结构化编程”和“面向对象编程”两个阶段

### 结构化编程的疑难
抽象层次过低带来的问题
- 结构体与实体直接相关，并且将这种相关性直接呈现在使用者的面前，因此开发人员必须面临数据的具体含义与关系
- 结构体的抽象更面向于数据存储形式的表达和算法的实现方法，脱离了具体使用环境和算法的结构缺乏通用性
- 僵化的类型题壁僵化的逻辑并存，影响了业务逻辑的表达

### 面向对象语言的突破
- 通过细化可见性的设定，实现更好的数据封装性及数据域管理
- 继承被用来解决结构体的通用性问题
- 解决“强类型”与业务逻辑表达之间的冲突依靠多态

### 更高层次的抽象：接口
只暴露数据体的逻辑行为能力，而不暴露这能力的实现方法和基于的数据特性，有了接口观念之后，系统间的关系变得清晰：用或不用

## 基本语法的结构化
### 语法作用域之间的相关性
- 相同级别的语法作用域可以相互嵌套
- 高级的语法作用域能够包含低级的语法作用域
- 低级的语法作用域不能包含高级别的语法作用域，由于不存在包含关系，因此语言实现时，一般理解为"平行"关系

<!-- language: !js -->

    if(true){
        function foo(){}
    }
    //等同于
    if(true){}
    function foo(){}

### 执行流程及其变更
- `break label`不能跨越函数的语法作用域
- `return`子句可以跨越任何语句的作用域而退出函数
- `throw`可以起到return语句的效果，退出了函数作用域，并使流程回到全局作用域中，直到它遇到一个异常捕获程序，或交由全局的或宿主的onerror事件处理程序

### 模块化的效果：变量作用域
变量作用域又叫变量的可见性，语法作用域是语法分析阶段对代码块组织结构的理解，而变量作用域是代码执行阶段对变量存储的理解，四个语法作用域：表达式、语句、函数、全局，但变量作用域上，并没有“语句”这个级别。

<!-- language: !js -->

    var obj={val:10};
    with(obj){//依赖语法作用域的限定
        function foo2() {
            function foo(){
                val*=2;//依赖变量作用域的限定，找不到val，出错
            }
            foo();
        }
        foo2();
    }

#### 级别1：表达式
存在有价值的表达式级别的变量作用域，通常会是匿名函数和对象直接量了

<!-- language: !js -->

    foo=(100+function(){return 100;}());
    i='ABCDE'+new Date();
    alert(i);

新对象在被创建出来之后就被立即使用，在该语句中表达式执行完之后，你就再也访问不到它了

#### 级别2：语句
js __没有__ 语句作用域（块级作用域）

<!-- language: !js -->

    for(var i=0;i<10;i++){}
    alert(i);//10

#### 级别3：函数（局部）
变量的作用域在函数之内，离开函数，这些变量都不可见了

#### 级别4：全局
两大不利特性：
- 在全局范围内任意声明变量，尤其在全局范围内使用for循环同时声明变量
- 在函数内部不使用var声明而直接对一个变量赋值时，该变量会隐式地在全局声明，或读写到一个全局的变量

#### 变量作用域的次序问题
显式声明的变量在函数和全局作用域中，是没有次序限制的，可以先声明再使用，也可以先使用，再显式的声明，因为显式声明总是在代码执行前，语法解释阶段处理的

<!-- language: !js -->

    function foo(){
        if(!bool) alert(bool);
        var bool=true;
    }
    //等同于
    function foo(){
        var bool;
        if(!bool) alert(bool);
        bool=true;
    }

函数声明中交叉引用

<!-- language: js -->

    TMyClass=Class(Tobject,foo);//由于foo是函数声明的标识符，在执行的任意位置都可以直接引用
    function foo(){
        this.class=TMyClass;
    }

#### 变量作用域与生存周期
一个变量的创建是在：

- 引擎做语法分析，发现显式声明时；
- 引擎做代码执行，发现试图写（如赋值）一个未被创建的变量时。

而它的释放，则在：

- 引擎执行到函数结束/退出操作时，将清除函数内的未被引用的变量；
- 引擎执行到全局的代码终结或引擎卸载和重载入时，将清除全局的变量和数据的引用。

## js中原型继承
### 空对象(null)与空的对象
js中，__空对象(null)__ 是作为一个保留字存在，不是“空的对象”，而是代表这样一个对象
- 属于对象类型；
- 对象是空值的

它不是自object()构造器实例而来：

<!-- language: !js -->

    alert(typeof null);//object

    var num=0;
    for(var pro in null){//可被列举属性，只是没能任何属性而已
        num++;
        alert(pro);
    }
    alert(num);//0
    alert(null instanceof Object);//false

__空的对象__，obj=new Object();obj={};也称“裸对象、干净的对象、空白对象、empty objects”，具有“对象”的一切特性，因此可以存取预定义的属性和方法（toString、valueOf）,而instanceof运算也会返回true

### 原型继承的基本性质
js的语言和对象系统的实现来讲，__对象(Object Instance)并 *没有* 原型__，__而构造器(Constructor) *有* 原型(构造器.prototype)__，对象只有“构造自某个原型”的问题，并不存在“持有（或拥有）某个原型”的问题

原型其实也是一个对象实例，原型的含义是：如果构造器有一个原型对象A，则由该构造器创建的实例(instance)，都必然复制自A。所以实例必然继承了A的所有属性、方法和其他性质。

原型也是对象实例，这是它与“类继承体系”在本质上的不同，后者中，类不必是“对象”（但ruby中类也是对象），因此类也不必具有对象的性质。

### 空的对象是所有对象的基础

<!-- language: !js -->

    proto=Object.prototype;
    var num=0;
    for(var pro in proto){
        num++;
    }
    alert(num);//0

可见Object()构造器的原型就是一个空的对象

obj={}; => obj=clone(Object.prototype);

### 构造复制?写时复制?读遍历?
假设每构造一个实例，都从原型中复制出一个实例，新的实例与原占用了相同的内存空间，内存空间的消耗会急速增加

写时复制，典型示例就是操作系统中的动态链接库。只在在系统中指明obj等同于它构造器的原型，这样的 __读取的时候，只需顺着指示去读原型即可，当需要 ___写___ 对象属性时，就 ___复制___ 一个原型映像出来__，并使以后的操作指向该映像就行，不过对于经常进行写操作的系统来说，这种法子并不比第一种经济

js采用第三种法子：把 __写复制的粒度从原型变成了成员__，仅当 __写某个实例成员__ 时，将成员的信息复制到实例映像中，该对象还指向原型的引用（一个原型引用，一张成员列表），这样写操作并不导致大量的内存分配，该对象需要维护一张成员列表，描述在该对象中发生了修改的成员名、值与类型。这张表是否与原型一致并不重要，此时产生两条规则：

- 保证在读取时被首先访问到
- 如果该对象中没有指定属性（这张表中没有），则尝试遍历对象的整个原型（.＿＿proto＿＿）链，直到原型为空（null 即Object.prototype.＿＿proto＿＿==null）或找到该属性

存取实例中的属性，比原型中的属性效率要高

### 构造过程：从函数到构造器
函数的prototype的实现逻辑：

<!-- language: js -->

    //设定__prototype是函数内置成员，get_prototype()是它的读方法
    var __prototype=null;
    function get_prototype(){
        if(!__prototype){//延迟创建
            __prototype=new Object();//函数的 原型对象为 空的对象
            __prototype.constructor=this;//该原型对象的构造器是本函数，方便在实例化构造器时，实例的constructor(通过该原型找到)能够找到类型信息(该函数)
        }
        return __prototype;
    }

    //产生如下效果
    function foo(){}
    foo.prototype={constructor:foo};

函数的原型总是一个标准的、系统内置的Object()构造器的实例，证明如下:

<!-- language: !js -->

    function MyObject(){}

    alert(MyObject.prototype.constructor==MyObject);//true

    delete MyObject.prototype.constructor
    alert(MyObject.prototype.constructor==Object);//true
    alert(MyObject.prototype.constructor==new Object().constructor);//true

### 预定义属性与方法
js中的对象实例上质上只是“一个指向其原型的，并持有一个属性列表的结构”。

“空的对象是所有对象的基础”，也就是Object.prototype是所有对象的最顶层的原型

<!-- language: !js -->

    function A(){}
    a=new A();
    alert(a.__proto__.__proto__==Object.prototype)//true

    //但下面不等
    alert(a.constructor.prototype.constructor.prototype==Object.prototype)
    //问题出在a.constructor.prototype.constructor，还指向A构造器，它的prototype还是{constructor:A}


因此所谓的“空的对象”，只不过满足以下条件的一个结构：

- ^proto指向Object.prototype;
- ^props指向一个空表

所有实例，之所以具有对象的某些性质，是因为它们的共同原型Object.prototype具有这些性质：toString、toLocaleString、valueOf、constructor、propertyIsEnumerable、hasOwnProperty、isPrototypeOf

<!-- language: !js -->

    ['toString','toLocaleString','valueOf','constructor','propertyIsEnumerable','hasOwnProperty','isPrototypeOf'].map(function(prop){
        alert(prop+" : "+(prop in Object.prototype))
    })

构造器Object()，它的constructor是Function，所具有的特殊成员：call、apply、caller、arguments、length、prototype，许多是Function实例的成员

### 原型链的维护
#### 两个原型链
- 构造器prototype属性的 __构造器原型链__
- 对象实例＿＿proto＿＿属性（IE中不可见）的 __内部原型链__

由于内部原型链是依赖＿＿proto＿＿属性，在某些宿主中无法访问，所以通过对象实例的constructor属性来维护

#### constructor属性的维护
一个构造器的产生的实例，其constructor属性默认总是指向该构造器

<!-- language: !js -->

    function MyObj(){}
    var obj=new MyObj();
    alert(obj.constructor==MyObj);

而究其根源，在于构造器的原型的constructor指向了自身

<!-- language: !js -->

    //MyObj.prototype={constructor:MyObj}
    alert(MyObj.prototype.constructor==MyObj);

一般情况下，我们可以通过constructor找到构造器，并进而找到它的原型

<!-- language: !js -->

    alert(obj.constructor.prototype);

但不要以为就此我们就可以访问整个原型链

<!-- language: !js -->

    function MyObj(){}
    function MyObjEx(){}
    proto=new MyObj();
    MyObjEx.prototype=proto;//原型继承

    var obj1=new MyObj();
    var obj2=new MyObjEx();
    alert(obj1.constructor==obj2.constructor)//true

obj1与obj2由不同的构造器产生的实例，但它们的constructor却 __相同__，为了修正该原型的构造器，一般建议如下：

<!-- language: js -->

    MyObjEx.prototype=proto;
    MyObjEx.prototype.constructor=MyObjEx; //还原回来，适合不需要回溯原型链

但这样是有了实例的构造器信息，但由于 __丢掉__ proto自身的constructor信息，事实上也 __切断__ 了与原型父类的关系

能不能让obj1、obj2等实例的constructor指向正确的构造器，而MyObjEx.prototype.constructor还指向MyObj，以方便原型链的访问

<!-- language: !js -->

    function MyObj(){}
    function MyObjEx(){
        this.constructor=MyObjEx;//保证构造器
        //this.constructor=arguments.callee;
    }
    MyObjEx.prototype=new MyObj();//保证原型链
    var obj2=new MyObjEx();
    alert(obj2.constructor==MyObjEx);//true保证构造器
    alert(obj2.constructor.prototype instanceof MyObj);//true保证原型链


其实不管怎么改MyObj.prototype.constructor==MyObj，指向不到Object，而Object.prototype.constuctor==Object，也指向不到null

只对象实例的＿＿proto＿＿属性（内部原型链）却可以保真的访问到所有原型链上的对象

内部原型链保证了父类与子类的一致性，即子类不能delete父类中属性（能重写），即使修改了constructor，也不用担心实例与父类的不一致

#### ES5中原型链维护
可以代替对象实例的＿＿proto＿＿属性，ES5使用`Object.getPrototypeOf()`来获得一个对象的真实原型，无须访问构造器或维护外部的原型链

<!-- language: !js -->

    function MyObj1() {}
    function MyObj2() {}
    function MyObj3() {}

    MyObj2.prototype=new MyObj1();
    MyObj3.prototype=new MyObj2();

    MyObj2.prototype.constructor=MyObj2;
    MyObj3.prototype.constructor=MyObj3;

    var obj=new MyObj3();
    var proto=Object.getPrototypeOf(obj);

    while(proto){
        alert(">> "+proto.constructor);
        proto=Object.getPrototypeOf(proto);
    }
    alert(">> "+proto);


### 原型继承的实质
#### 原型修改
原型是一个对象，在属性访问时，如果没有子类对象该属性，则访问其原型的成员列表

<!-- language: !js -->

    function MyObj(){}
    var obj=new MyObj();
    alert('name' in obj);//false
    MyObj.prototype.name='some';
    alert('name' in obj);//true

#### 原型继承

<!-- language: !js -->

    function Animal(){}
    function Mammal(){}//哺乳类
    function Canine(){}//犬科
    function Dog(){}
    function Cat(){}

    Mammal.prototype=new Animal();
    Canine.prototype=new Mammal();
    Dog.prototype=new Canine();
    Cat.prototype=new Mammal();

    function isAnimal(obj){
        return obj instanceof Animal;
    }

    var dog=new Dog();
    var cat=new Cat();
    alert(isAnimal(dog));

    Animal.prototype.respire=function(){}

    alert('respire' in cat);
    alert('respire' in dog);

#### 原型继承实质：从无到有
__原型继承__ 关注于 __类层关系上的层次__，__原型修改__ 关注 __具体对象的行为特性__，两者是相互独立的，将对象的继承关系与对象的行为描述分离

这与“基于类继承的对象系统”存在本质的不同，后者必须预先考虑好某个类是否具有某种属性和方法，如果某个类的成员设计得不正确，则它的子类在使用中都将遇到问题，因而“重构”是必然的，所以原型继承中：

- "类属关系的继承性"总是一开始就能被正确的设计
- 成员的修改是正常的、标准的构造对象系统的方法

原型修改正是动态语言与面向对象继承交汇的关键点，这是一种“从无到有”的模式，首先为每一个构造器分配了一个原型，并通过修改原型构造整个对象树，我们可以先构造一个“没有任何成员”的类属关系的继承系统，然后通过“不断的修改原型”，从而获得一个完整的对象系统。

#### 如何理解继承来的成员
子类从父类继承对象性质时，也会继承成员的可读写性与可见性，但重写该成员时，随引擎不同表现不同，JScript认为重写的成员的可读写性与可见性不变，SpiderMonkey JavaScript则认为可读写性与可见性被重置了。

<!-- language: !js -->

    var obj={constructor:null};
    for(var n in obj)alert(n);//显示constructor则是第二种

## js的对象系统
### 封装
在js中，基于“js变量作用域”来实现时，只能实现public和private这两种封装：

<!-- language: !js -->

    function MyObj(){
        var data=100;//private
        function _run(v){alert(v);}//private
        this.value="the data is ";//public
        this.run=function(){_run(this.value+data);};//public
    }
    var obj=new MyObj();
    obj.run();

js中封装不是一种源代码级别的“说明（或声明）”，它依赖于代码运行期间的“可见性”的效果

闭包也是封装的一种手段

### 多态
js是弱类型的，通过typeof运算考察变量时，要么是对象(object)，要么是非对象(number,undefined,string等)，由于没有严格的类型检测，因此可以对任何对象调用任何方法，而无须考虑它是否“被设计为”拥有该方法，对象的多态性被转换为运行期间的动态特性，我们可 __动态的添加对象的方法，使它看起来像某个对象（鸭子类型）__

<!-- language: !js -->

    function Bird(){
        var bird=(arguments.length===1?arguments[0]:this);
        bird.fly=function(){alert('fly');};
        return bird;
    }
    var obj={};
    var bird=Bird(obj);
    bird.fly();//fly
    obj.fly();//fly

能飞只取决于它有没有fly方法，而不取决它是不是某种类型的对象

### 事件
js引擎自身没有事件系统，本质上来说，js中方法，其实是属性存取与函数调用的连续运算效果，即方法也是被“模拟”的。但用js实现一个简单的事件框架是很容易的

<!-- language: !js -->

    function MyObj(){}
    MyObj.prototype.OnError=undefined;
    MyObj.prototype.doAction=function(str){
        try{return eval(str);}
        catch(e){if(this.onError) this.onError(e);}
    }

    var obj=new MyObj();
    obj.onError=function(e){alert(e);};//add handler
    obj.doAction("aObj.tag=100");

### 类抄写?或原型继承?
构造对象系统的5种方法

<!-- language: !js -->

    //在构造器中写this实例引用
    function MyObj_1(){
        this.name="Bob";
        this.do=function(){};
    }

    //在构造器中直接返回对象实例
    function MyObj_2(){
        var data=this;
        return{
            name:"Bob",
            do:function(){}
        };
    }

    //修改原型
    function MyObj_3(){}
    MyObj_3.prototype.name="Bob";
    MyObj_3.prototype.do=function(){};

    //重写原型
    function MyObj_4(){}
    MyObj_4.prototype={
        name:"Bob",
        do:function(){}
    }

    //继承原型
    function MyObj_5(){}
    MyObj_5.prototype=new MyObj_1();

只有方法5，使得js维护对象系统的原型链，也是唯一一种影响内部原型链的形式

#### 类抄写

<!-- language: !js -->

    //公共函数: 子类派生
    extend=function(subClass,baseClass){
        //暂存父类构造器
        subClass.baseConstructor=baseClass;
        subClass.base={};
        //复制父类成员到 子类构造器的base属性中，方便子类对象还能使用父类中定义的方法
        baseClass.call(subClass.base);
    }

    //构建对象系统
    function Mouse(){}

    function Animal(name){
        this.name=name;
        this.say=function(msg){
            alert(this.name+": "+msg);
        };
        this.eat=function(){
            this.say("Yum!");
        };
    }

    function Cat(name){
        Cat.baseConstructor.call(this,name);//Cat的父类构造器来构造this，抄写了父类的成员
        this.eat=function(food){
            if(food instanceof Mouse)
                Cat.base.eat.call(this);//相当于实现了super
            else
                this.say("Not my food");
        };
    }
    extend(Cat,Animal);

    function Lion(name){
        Lion.baseConstructor.call(this,name);
    }
    extend(Lion,Cat);

    //测试
    var cat=new Cat('cat');
    var lion=new Lion('lion');
    var mouse=new Mouse();
    var unknow={};

    cat.eat(mouse);
    cat.eat(unknow);
    lion.eat(mouse);

`extent()`来维护了一个`baseConstructor`成员，这个成员总是指向父类的构造器，而子类实例的构造逻辑就变成了：`Cat.baseConstructor.call(this,name)`先向父类传入this引用以抄写父类方法，再向子类传入this引用以抄写子类方法，并且可以覆盖父类中同名成员，由于整个构造过程，都是在不断地从“类构造器”向`this引用`抄写成员，所以我们将这种方法为“类抄写”

类抄写有两个问题：

- 以 __内存开销__ 换取效率，每次创建Animal的实例时，都会给实例初始化`say()`、`eat()`方法，每个实例之间，它们的say()并不是同一个函数：

<!-- language: !js -->

    function Animal(){//同上代码
        this.say=function(msg){
        };
    }
    var obj1=new Animal();
    var obj2=new Animal();
    alert(obj1.say==obj2.say);//false

- 第二个问题是系统并 __不维护原型继承链__，因此不能使用`instanceof`运算来检测继承关系，需自行维护一个类继承树，并用特定的方法来检测实例与类（构造器）的关系

#### 原型继承
原型继承存在的问题：

- 在维护构造器引用(constructor)和外部原型链之间无法平衡
- 没的提供调用父类方法的机制
- 以时间换空间，子类中读写一个成员无法直接存取到该成员时，将会回溯原型链以查找名字的成员
- “写复制”在引用类型与值类型数据中表现不一致

### 内置/原生对象(构造器)
- 内置对象与原生对象区别：前者总是在引擎初始化阶段就被创建好的对象，是后者的一个子集，而后者包括了一些在运行过程中动态创建的对象
- 内置对象有11个，`Number`,`Boolean`,`String`,`Object`,`Function`,`Array`,`RegExp`,`Error`,`Date`,`Math`,`Global`
- 原生对象还包括`Arguments`
- `Number`,`Boolean`,`String`的直接量形式声明将会定义值类型数据而非对象，值类型到对象类型可以通过包装类的形式来转换
- 内置对象中，只以构造器形式存在，而没有直接量声明的只有`Error()`与`Date()`两个
- `Math`是一个对象而非构造器，因此不能被实例化
- `Global`对象则在引擎初始化时就被实例化一个单例，作为全局可访问的对象使用
- `Arguments`对象是有关函数调用的一个对象，它总是由引擎在函数调用时动态创建并添加在函数闭包中

### 不能通过继承得到的效果
内置对象具有在对象系统的封装、多态与继承性之外的特殊效果

- `Number`,`Boolean`,`String`值类型与它的包装类之间的关系是在引擎内部设定的，既不可能通过重写构造器(的标识符)来替代这种效果，也不能通过继承来使得这些包装类的子类继承这种效果
- `Array`隐式的维护`length`属性和下标
- `Date`日期对象的方法（如格式转换）仅能作用在由`Date()`构造器产生的对象实例上
- “派生引擎原生对象”的想法在js中不切实际，安全的子类派生只有`Object()`,`Error()`及`用户自己声明的构造器函数`

## ES5可定制的对象属性
对象是一个属性集，一个属性的性质(attributes)也并不用于描述该属性的类型或作用，而是主要用于描述一个属性与这个集合的种种关系，这些关系有三种`Writable`,`Enumerable`,`Configurable`，除此之外，一个属性本身还具有两种性质：`Name`,`Value`

### 属性描述符

#### 一般的数据属性描述符
它可能会是如下形式

<!-- language: js -->

    name:{
        value: 'prop',
        writable: true,
        enumerable: true,
        configurable: true
    }

我们使用了"名称/属性描述符对象"的形式替代了原来的"名/值"语法，数据属性描述符的构成：

<!-- language: table -->

    |        |属性        |值类型 |默认值   |含义                  |
    |--------|------------|-------|---------|----------------------|
    |数据描述|value       |(任意) |undefined|基本"名/值"定义       |
    |数据描述|writable    |Boolean|false    |可否重写,false表示只读|
    |性质描述|enumerable  |Boolean|false    |可否列举,false表示不能被for...in列举|
    |性质描述|configurable|Boolean|false    |可否新配置,false时,Writable与Enumerable值是可修改的,且属性是可用delete删除的|

#### 带读写器的存取属性描述符
带读写器的存取属性描述符与上述的数据描述只能存在一种，而不能同时存在，其格式为：

<!-- language: js -->

    name:{
        get: function(){...},
        set: function(newVal){...},
        enumerable: true,
        configurable: true
    }

存取描述符构成：

<!-- language: table -->

    |        |属性        |值类型  |默认值   |含义             |
    |--------|------------|--------|---------|-----------------|
    |存取描述|get         |Function|undefined|取值函数(getter) |
    |存取描述|set         |Function|undefined|赋值函数(setter) |
    |性质描述|enumerable  |Boolean |false|可否列举,false表示不能被for...in列举|
    |性质描述|configurable|Boolean |false|可否新配置,false时,Writable与Enumerable值是可修改的,且属性是可用delete删除的|

### 定制对象属性
#### 属性声明以及获取属性描述符
`Object.defineProperty()`来向对象添加一个新属性并定制属性描述符，与此相关的三个方法都是`Object()构造器`的成员：

<!-- language: table -->

    |分类    |方法                         |说明              |
    |--------|-----------------------------|------------------|
    |属性声明|defineProperty(obj,name,desc)|为对象声明一个属性|
    |属性声明|defineProperties(obj,props)  |为对象声明一组属性|
    |属性声明|getOwnPropertyDescriptor(obj,name)|取对象的属性描述符列表(props)|

desc指上述两种描述符中之一

<!-- language: !js -->

    var obj={};
    Object.defineProperty(obj,'aName',{
        get:function(){return 'zyx'},
        configurable:true
      }
    );
    alert(obj.aName);

    Object.defineProperties(obj,{
        firstName:{
          get:function(){return 'yx';}
        },
        lastName:{
          get:function(){return 'z';}
        }
      }
    );
    alert(obj.firstName+' '+obj.lastName);
    var p=Object.getOwnPropertyDescriptor(obj,'aName');
    var keys=Object.keys(p);
    alert(keys);

#### 新的对象创建方法:Object.create()
认为"构造器函数"本身是无意义的，在新的机制中，对象变成了简单的“原型继承+属性定义”，不再需要“构造器”这样一层语义

<!-- language: !js -->

    var aPrototypeObj={name1:'value1'};
    var aNewInstance=Object.create(aPrototypeObj,{
        name2:{value:'value2'},
        name3:{get:function(){return 'value3'}}
    });

    alert(aNewInstance.name1+aNewInstance.name2+aNewInstance.name3);
    alert(aNewInstance.constructor);
    alert(aNewInstance.__proto__===aPrototypeObj);

Object.create()的实现过程类似如下(省略了参数检测)

<!-- language: js -->

    function aCreator (proto,props) {
        var obj,__proto__=Object.prototype;
        try{
            Object.prototype=proto;
            obj=new Object();
        }finally{
            Object.prototype=__proto__;
        }

        Object.defineProperties(obj,props);
        return obj;
    }

### 属性状态维护
属性状态维护方法：

<!-- language: table -->

    |分类      |方法                    |说明                        |
    |----------|------------------------|----------------------------|
    |取属性列表|getOwnPropertyNames(obj)|取对象自有的属性名数组      |
    |取属性列表|keys(obj)               |取对象自有的、可见属性名数组|
    |状态维护  |preventExtensions(obj)  |使实例obj不能添加新属性     |
    |状态维护  |seal(obj)|使实例obj不能添加新属性，也不能删除既有属性|
    |状态维护|freeze(obj)|使实例obj所有属性只读，且不能添加、删除属性|
    |状态检查  |isExtensible(obj)       |返回preventExtensions状态   |
    |状态检查  |isSealed(obj)           |返回seal状态                |
    |状态检查  |isFrozen(obj)           |返回freeze状态              |

其中preventExtensions、seal、freeze针对对象来操作的，会影响到所有属性的性质的设置，需要强调：

- 由原型继承来的性质同样会受到影响
- 以当前对象作为原型时，子类可以通过重新定义同名属性来覆盖这些状态

<!-- language: !js -->

    var obj={name:'obj'};
    Object.freeze(obj);
    obj.name='xxx';//修改无效
    alert(obj.name);//'obj'

    var obj2=Object.create(obj);
    obj2.name='xxx';//修改无效
    alert(obj2.name);//'obj'

    //通过definePropert(y|ies)来重新定义
    Object.defineProperty(obj2,'name',{value:'obj2',writable:true});
    alert(obj2.name);//'obj2'
    obj2.name='xxx';//修改有效
    alert(obj2.name);//'xxx'

#### 取属性列表
<!-- language: !js -->

    var obj1={n1:100};
    var obj2=Object.create(obj1,{n2:{value:200,enumerable:true}});
    Object.defineProperty(obj2,'n3',{value:300});

    alert(Object.keys(obj2))//自有的，可见的
    alert(Object.getOwnPropertyNames(obj2));//自有的，所有的
    alert(function (obj) {//继承+自有的，可见的
        var _ret=[];
        for(var n in obj) _ret.push(n);
        return _ret
    }(obj2));

#### 使用defineProperty来维护属性的性质
在definePropert(y|ies)中操作某个属性时，如果该名字的属性未声明则新建它，如果已存在，则使用描述符中新性质来覆盖旧的性质值

<!-- language: !js -->

    var pOld, pNew;
    var obj = {data: 'oldValue'};

    pOld = Object.getOwnPropertyDescriptor(obj, 'data');
    alert(Object.keys(pOld));

    Object.defineProperty(obj, 'data', function(old) {
        return {
            get: function() {
                return oldValue;
            },
            configurable: false
        }
    }(obj.data))

    pNew = Object.getOwnPropertyDescriptor(obj, 'data');
    alert(Object.keys(pNew));

    try{
        Object.defineProperty(obj,'data',{value:100});
    }catch(e){
        alert(e.message);
    }

#### 修改继承自原型的属性
如果某个从原型继承来的属性是可写的，并使用“数据属性描述符”，那么在子类中修改该值，将 __隐式的创建一个属性描述符__

<!-- language: !js -->

    var obj1={n1:100};
    var obj2=Object.create(obj1);

    alert(Object.keys(obj1));//'n1'
    Object.defineProperty(obj1,'n1',{enumerable:false});
    alert(Object.keys(obj1));//空

    alert(Object.getOwnPropertyNames(obj2));//空,还不是自有属性
    obj2.n1="newVal";
    alert(Object.getOwnPropertyNames(obj2));//'n1'

如果属性使用是“存取属性描述符”，那么无论读写性为何，都 __不会新建__ 属性描述，对子类中该属性的读写，都只会忠实的调用（继承而来、原型中的）读写器

#### 重写原型继承来的属性描述符
一个原型对象中只读属性在子对象中可覆盖成可读写，并且一旦delete该属性时，又恢复原型对象中的值和性质


js函数式语言特性
==================

## 函数式语言中函数
并不是一个语言支持函数就叫做“函数式语言”，函数式语言中函数除了能调用外，还具有一些其他性质：

### 函数是运算元
支持函数作为运算元参与运算，即函数可作为变量、参数、返回值。ps：js中函数作为参数时，传引用，并没有地址概念，不同于C中的函数指针，杜绝了“内存访问违例”的隐患

### 在函数内保存数据
以编译型（X86）平台上命令式语言来说，由于代码总是在代码段中执行，而代码段不可写，因此函数中数据只能是静态数据，此外，函数内私有变量（局部变量）是不能保存的，局部变量是在栈上分配，函数执行结束后，所占用的栈被释放，因此函数内的数据不可能被保存。

但js的函数中，函数内的私有变量可以修改，而且当再次“进入”到该函数内部时，这个被修改的状态仍然持续，这种在函数内保持数据的特性称为 __"闭包"__。

如果一个数据能够在函数内持续保存，那么该函数（作为构造器）赋给实例的方法就可以使这些数据进行运算，而多个实例间，由于数据存在不同的闭包中，因此不会产生相互影响：

<!-- language: !js -->

    function MyObj(){
        var value=100;
        this.setValue=function(v){value=v;};
        this.getValue=function(){return value;};
    }
    var obj1=new MyObj(),
        obj2=new MyObj();

    obj1.setValue(200);
    alert(obj2.getValue());//100,不受obj1的修改影响

### 函数内运算对函数外无副作用
运算对函数外无副作用，是函数式语言应当达到的一种特性，但在js中，只能通过开发人员的编程习惯来保证，无副作用的含义是：

- 函数的使用入口参数进行运算，而不修改它（作为值参数而不是变量参数使用）
- 在运算过程中不会修改函数外部的其他数据的值（如全局变量）
- 运算结束后通过函数返回向外部系统传值

## 从运算式语言到函数式语言
为什么“连续求值”会成为函数式语言的基本特性，是因为函数式语言是基于对Lambda演算的实现而产生的，其基本运算模型就是：__表达式运算产生结果，结果(值)用于更进一步的运算__

函数式语言中“函数”并不是真正的精髓，真正的精髓在于“运算”，而函数只是封装“运算”的一种手段

### js中几种连续运算
#### 连续赋值

<!-- language: js -->

    var a=b=c=d=100;

#### 三元表达式的连用
在函数式语言中，三元表达式甚至被推荐连用：

<!-- language: !js -->

    var objTypes = ["String", "Array", "Number", "Boolean", "RegExp", "Object"];
    objTypes.map(function(objType) {
        var cls = ((objType === 'String') ? String :
            (objType === 'Array') ? Array :
            (objType === 'Number') ? Number :
            (objType === 'Boolean') ? Boolean :
            (objType === 'RegExp') ? RegExp :
            Object
        );
        var obj = new cls();
        alert(obj instanceof String ? "String" :
            obj instanceof Array ? "Array" :
            obj instanceof Number ? "Number" :
            obj instanceof Boolean ? "Boolean" :
            obj instanceof RegExp ? "RegExp" :
            "Object"
        );
    });

显然“运算产生值并参与运算”这一特性，使得上述代码成为可能，否则可能会使用多态或工厂去实现。这也许没什么问题，但问题是，你在使用函数式语言时，是否利用了函数式语言的特性。

#### 一些运算连用
也许这个代码可读性不好

<!-- language: js -->

    child = (!LC && !RC) ? 0 : (!LC ? RC : LC);

但至少可以改成如下

<!-- language: js -->

    child = (LC || RC) ? (LC ? LC : RC) : 0;

甚至改成如下

<!-- language: js -->

    child = !(LC || RC) ? 0 :
        LC ? LC :
        RC;

但事实上js中，我们无非想得到LC、RC、0值之一，这时可以借助逻辑“或(||)”运算得到

<!-- language: js -->

    child = LC || RC || 0;

其实也说明了js逻辑运算的实质，逻辑“或”运算并非为布尔值专设，布尔运算只是逻辑运算的一种特例

#### 函数与方法调用
孤立的来看，我们要得到一个对象的类类型，并不需要复杂的三元表达式，类工厂可能是不错的主意，但js另外还有一个特性，__对象的构造、函数与方法的调用等，本质上都是表达式运算，而非语句__。

<!-- language: !js -->

    var obj = new ((objType==='String') ? String : Object);

这行代码用一个运算符作为new运算的入口参数，注意，new不是语句的关键字，而是运算符。上例中可改为

<!-- language: !js -->

    var objTypes = ["String", "Array", "Number", "Boolean", "Regexp", "Object"];
    objTypes.map(function(objType) {
        var obj = new ((objType === 'String') ? String :
            (objType === 'Array') ? Array :
            (objType === 'Number') ? Number :
            (objType === 'Boolean') ? Boolean :
            (objType === 'RegExp') ? RegExp :
            Object
        );
        alert(obj instanceof String ? "String" :
            obj instanceof Array ? "Array" :
            obj instanceof Number ? "Number" :
            obj instanceof Boolean ? "Boolean" :
            obj instanceof RegExp ? "RegExp" :
            "Object"
        );
    });

再作一些修改

<!-- language: !js -->

    var objTypes = ["String", "Array", "Number", "Boolean", "Regexp", "Object"];
    objTypes.map(function(objType) {
        alert(
            (new(
                (objType === 'String') ? String :
                (objType === 'Array') ? Array :
                (objType === 'Number') ? Number :
                (objType === 'Boolean') ? Boolean :
                (objType === 'RegExp') ? RegExp :
                Object
                )
            ).toString()
        );
    });

new外侧的括号（强制运算符）不是必须的，因为`"."运算`优先级低于"new"运算

### 运算式语言
运算式语言，它满足说明式语言的两种特性，一是陈述运算，二是求值

#### 运算的实质，是值运算
js中所有运算都产生"值类型"的结果值，正因为“运算都产生值类型的结果”，且“所有逻辑语句结构都可被消灭”，所以结论是“系统的结果必然是值，并且可以通过一系列的运算来得到这一结果”

所以运算的实质其实是值运算，至于像“指针”、“对象”这样抽象结构，在运算系统来看，其实只是定位到“值”以进行后续运算的工具而已。

IE10以下，css支持expression，已具备整个js语言的编程能力

### 如何消灭掉语句
对于“运算式语言”这种新的语言风格，如果要让它成为一种纯粹的且完备的语言（范型），那么我们需要让它仅通过“表达式运算”就能完成全部的程序逻辑，这包括其他语言中三种基本逻辑结构：顺序、分支与循环

#### 通过表达式消灭分支语句

<!-- language: !js -->

    if(tag>1) alert('true');

    //改成如下
    (tag>1) && alert('true');

    if(tab>1) alert('true');
    else alert('false');

    //改成如下
    (tag>1)?alert('true'):alert('false');

#### 通过函数递归消灭循环语句

<!-- language: !js -->

    var loop=100;
    var i=loop;

    do{
        //do something
        i--;
    }while(i>0);

    //改成如下
    function foo(i){
        //do something
        if(--i>0) foo(i);
    }
    foo(loop);

    //或如下，更能展现函数式风格
    void function(i){
        //do something
        (--i>0) && arguments.callee(i);
    }(loop);

js不支持尾递归的优化

## 函数:对运算式语言的补充和组织

### 函数是必要的补充
js不支持尾递归的优化，但循环可以在函数内部来使用，而函数可以认为是值的含义，所以以下仍认为是表达式

<!-- language: css -->

    /*css中的expression*/
    #right_span{
        left:expression(
            (function(el,className){
                while(el=el.parentElement &&
                    el.className!=className);//null loop
                return (el?el.clientWidth:0);
              }
            )(this,'boxSpan') - parseInt(this.currentStyle.width)
        );
    }

### 函数是代码的组织形式
是一种消减代码复杂性的组织形式

### 重新认识“函数”
#### 函数 == lambda
lambda演算是一套用于研究函数定义、函数应用和递归的系统

#### 当运算符等义于某个函数时

<!-- language: !js -->

    function calc(b, x, y) {
        if (b) return x + y;
        else return x * y;
    }
    //调用calc(bTrue, 3, 4)
    [true, false].map(function(bTrue) {
        alert(calc(bTrue, 3, 4));
    })

作为运算式语法，我们来重写一下

<!-- language: !js -->

    //bTrue ? 3 + 4 : 3 * 4
    [true, false].map(function(bTrue) {
        alert(bTrue ? 3 + 4 : 3 * 4);
    })

但值3与值4是重复出现的，是否可以像第一个例子处理成参数

<!-- language: !js -->

    function f_add(x, y) {
        return x + y;
    }
    function f_mul(x, y) {
        return x * y;
    }
    //调用(bTrue ? f_add : f_mul)(3, 4);
    [true, false].map(function(bTrue) {
        alert((bTrue ? f_add : f_mul)(3, 4));
    })

f_add()和f_mul()其实本身并没有运算能力，而只是将"+"和"*"运算的结果直接返回，换言之，"+"和"*"运算符分别等义于f_add()和f_mul()这两个函数。注意以下到Scheme格式的转换

<!-- language: js -->

    //直接使用运算符，不使用函数
    (bTrue ? "+" : "-")(3,4);

    //运算符提前，运算元之间分隔符使用空格
    (?: bTrue "+" "-") 3 4)

    //if来代替三元运算元
    //运算符可以作为运算元，即`+`和`*`本身是运算符，但也可作为if的运算元出现
    //对于布尔true使用标识#f
    ((if #f + *) 3 4)

这就是完全的“函数式语言”，Scheme认为 ___运算符等义于某个函数___，但js中并不是这样的，但 __函数来替换运算符__ 是没有问题的

<!-- language: !js -->

    function f_add(x, y) {
        return x + y;
    }
    function f_mul(x, y) {
        return x * y;
    }
    function f_if(b, v1, v2) {
        return b ? v1 : v2;
    }

    //调用f_if(bTrue, f_add, f_mul)(3, 4);
    [true, false].map(function(bTrue) {
        alert(f_if(bTrue, f_add, f_mul)(3, 4));
    })

### js中函数式编程
- 在函数外消除语句，__只使用表达式和函数__，通过连续求值来组织代码
- 在 __值概念__ 上（运算元），函数可作为 __运算元__ 参与表达式运算
- 在 __逻辑概念__ 上（运算符），__函数等义于表达式运算符__，其参数是运算元，返回运算结果
- 函数严格强调无副作用

## js中的函数
### 可变参数与值参数传递
函数入口参数有多种调用约定，例如值参数、变量参数、传出参数等

<!-- language: table -->

    |调用约定    |传参顺序  |清除参数责任|实现目的                |
    |------------|----------|------------|------------------------|
    |__Pascal__  |->由左至右|例程自身    |与旧有过程兼容，较少使用|
    |__Cdecl__   |由右至左<-|调用者      |与C/C++模块交互         |
    |__Stdcall__ |由右至左<-|例程自身    |Windows API使用的约定   |

js中函数参数值只支持一种调用约定：

- 从左至右向函数传入参数
- 传入参数的引用，函数内对它的任何修改都不会被传出（指改变是参数的引用不会影响到外部，但通过调用参数[如果是对象或Array]的成员是可以改变该对象的状态的）
- 传入参数个数（相对于函数声明时的形式参数）

有形参，但未传实参，则函数内部认为该参数为undefined，如果传参时使用undefined值时，可使用arguments.length来确定实际传入参数个数

### 非惰性求值

<!-- language: !js -->

    var i=100;
    alert(i+=20,i*=2);
    alert(i);

尽管alert没有接受第二个参数，但是用于传入第二个参数值的表达式"i*=2"却完成了运算，并实际地向alert()传入了，只不过alert()没有使用而已

js不支持惰性求值，丢掉了函数式语言的一种很好的特性

### 函数是第一型
第一型，通常指基础类型：

- 能够表达为匿名的直接量（立即值）
- 能被变量存储
- 能被其他数据结构存储
- 有独立而确定的名称（如语法关键字）
- 可（与其他数据实体）比较的
- 可作为例程参数传递
- 可作为函数结果返回
- 在运行期可创建
- 能够以序列化的形式表达
- 可（以自然语言的形式）读的
- 能在分布的或运行中的进行中传递与存储

“函数既可以是运算符，也可以是运算元（被运算的数据）”，亦即函数是可以作为函数的参数/返回值，这一特性叫 __“高阶函数”__

### 可遍历的调用栈
一个静态的、未调用的函数F只是一个值，一旦它（例如函数F）调用时，系统就将当前正在运行的函数A入栈，并保留函数A的执行指针，在函数F执行完之后，函数A出栈，并继续执行指针后的代码

<!-- language: !js -->

    function F(){}
    function A(){F(x,y,z);}

下面形式代码说明上述代码执行过程中栈的构造，以及函数调用的初始化过程

<!-- language: !js -->

    //基本数组的构造器，拥有length属性，支持下标存取
    function BaseArray(){}

    //指定的代码指针(cp,Code Point)入栈
    function _stack_push(cp){}

    //指定的代码指针出栈，返回一个cp
    function _stack_pop{}

    //让引擎从指定的cp位置开始执行，引擎会分析cp位置所在的闭包并装载该闭包环境
    function _set_cp(cp){}

    //将指定函数foo装入引擎，并获取代码指针
    function _cp_function(foo){}

以下为形式代码"F(x,y,z);"的形式代码

<!-- language: js -->

    function _A(){
        _stack_push($$CP);

        //F(x,y,z)
        F.arguments=new BaseArray(x,y,z);
        F.arguments.callee=F;
        F.caller=_A;
        _set_cp(_cp_function(F));

        _set_cp(_stack_pop().next());
    }

#### callee:我是谁
由于匿名函数和函数可被重写的特性的存在，使得callee变得不可或缺：

<!-- language: !js -->

    //匿名函数调用自身
    void function(){
        //在这里如何调用函数自身？
    }();

    //函数在递归过程中被重写
    var loop=0;
    function myFunc(){
        otherFunc();
        alert(loop++);//只alert一次
        myFunc();
    }
    function otherFunc(){
        myFunc=null;
    }
    myFunc();

如果不考虑函数名重写或匿名问题，下面等式是成立的

<!-- language: !js -->

    function myFunc(){
        arguments.callee===myFunc.arguments.callee;
        arguments.callee===myFunc;
    }

这样一来，无论是否匿名，函数的递归总可以写成如下：

<!-- language: !js -->

    //栈会溢出
    void function(){
        arguments.callee();
    }();


#### caller:谁呼(叫)我
caller是arguments.callee的属性，下面给出遍历 __调用栈__ 的代码

<!-- language: !js -->

    //获取函数名的正则
    var _r_function=/^function\b\s*([$\w\u00FF-\uFFFF]+)\s*\(/m;

    //callback函数，用于显示函数的信息
    function showIt(func){
        var name=(!(func instanceof Function)? 'unknow type':
            _r_function.test(func.toString()) ? RegExp.$1:
            'anonymous');
        alert('-> '+name);
    }

    //遍历调用栈
    function enumStack(callback){
        var f=arguments.callee;
        while(f.caller){
            callback(f=f.caller);
        }
    }

    //代码演示
    function level_n(){
        enumStack(showIt);
    }

    function level_2(){
        level_n();
    }

    function test(){
        level_2();
    }
    test();

但这个调用栈的方法，在栈上不能出现两次、多次同一个函数，否则会死循环，但引擎内部会维护正确的调用栈。

<!-- language: !js -->

    var i=0;
    var i_max=1;//大于1时，将死循环
    function f1(){f2();}
    function f2(){
        if(++i<i_max) f1();
        enumStack(showIt)
    }
    f1();

## 闭包
用函数来组织代码，意味着“函数”将大量出现，为了保证“函数式风格”，需要强调“在内部保存数据和对外无副作用”，而这在js中是通过函数闭包来实现的

### 什么是闭包
一个函数只是一段静态代码，以及 __编译期的、静态__ 的概念；而闭包则是函数代码在运行过程中的一个动态环境，是一个 __运行期的、动态__ 的概念。由于引擎对每个函数建立其独立的上下文环境，因此当函数被再次执行或通过某种方法进入函数体内时，就可以得到闭包内的全部信息

闭包两个特点：

- 闭包作为与函数成对的数据，在函数执行过程中处于激活（即可访问）状态
- 闭包在函数运行结束后，保持运行过程的最终数据状态

总的来说，闭包所对应的函数代码如何访问数据，以及闭包内的数据何时销毁，前者涉及 ___作用域（可见性）___ 的问题，后者涉及 ___数据引用___ 的识别

闭包包括的是函数运行实例的引用、环境（用来查找全局变量的表），以及一个由所有upvalue引用组成的数组

### 什么是函数实例与函数引用
在编译语言中，函数被编译成确定的代码，并放在确定的内存位置执行，一段代码文本与运行期的代码实例是等同的、一对一的概念，又由于函数可以被多个不同变量引用，所以一个函数的代码块在运行期可对应于多个变量（函数入口地址指针）

js中不单单只有这种关系，更为复杂的情况是，一个函数代码可以有多份函数实例，一个实例可以有多个函数引用

<!-- language: !js -->

    function MyObj(){
        function func(){}
        this.doFunc=func;
    }

    var obj1=new MyObj();
    var obj2=new MyObj();

    alert(obj1.doFunc==obj2.doFunc)//false，两个不同函数实例
    alert(obj1.doFunc.toString()===obj2.doFunc.toString())//true，代码块相同

也许有人认为这是new的效果，事实上这与new无关，也与js对象系统无关，看下例：

<!-- language: !js -->

    function MyFunc(){
        function func(){}
        this.doFunc=func;
    }

    var obj={};
    MyFunc.call(obj);
    var func=obj.doFunc;//套用一个函数实例的引用，暂存
    MyFunc.call(obj);//再次进入myFunc
    alert(func==obj.doFunc);//false
    alert(func.toString()===obj.doFunc.toString());//false

### 每个函数实例至少拥有一个闭包
在被调用时,每个函数实例至少拥有一个闭包，闭包概念不能讲清，原因之一，在于不能将函数（代码块）、函数实例、函数引用分开，关键在于：__闭包是对应于运行期的 *函数实例* 的__，而不是对应函数（代码块）的。

下面 __只产生引用，不产生实例__

<!-- language: !js -->

    function MyFunc(){}
    var f1=MyFunc;
    var f2=MyFunc;
    alert(f1==f2);//true

对象的实例只持有原型中的方法的一个引用，因此也 __不产生函数的实例__

<!-- language: !js -->

    function MyObj(){}
    MyObj.prototype.method=function(){};

    var obj1=new MyObj();
    var obj2=new MyObj();
    alert(obj1.method==obj2.method);//true

下面会 __产生多个函数实例__

<!-- language: !js -->

    function MyObj(){
        this.method=function(){};
    }
    var obj1=new MyObj();
    var obj2=new MyObj();
    alert(obj1.method==obj2.method);//false

结合两种构造对象的方法

<!-- language: !js -->

    function MyObj () {
        var ins_data='ins_data';
        this.getInsData=function(){return ins_data;};
        this.setInsData=function(v){ins_data=v;};
    }

    void function(){
        var cls_data='cls_data';
        this.getClsData=function(){return cls_data;};
        this.setClsData=function(v){cls_data=v;};
    }.call(MyObj.prototype);

    var obj1=new MyObj(),
        obj2=new MyObj();

    //obj1与obj2的getInsData是不同函数实例，因此在访问不同的闭包upvalue
    obj1.setInsData("obj1_instance");
    alert(obj2.getInsData());//ins_data

    //obj1与obj2的getClsData是同一个函数实例，因此在访问相同的闭包upvalue
    obj1.setClsData("obj1_cls");
    alert(obj2.getClsData());//obj1_cls

除了构造对象实例的情况，我们也为常在函数中将内部函数作为返回值

<!-- language: !js -->

    function aFunc () {
        function MyFunc(){}
        return MyFunc;
    }
    var f1=aFunc(),
        f2=aFunc();//多次调用，产生多个函数实例
    alert(f1==f2);//false

我们也可以返回同一个函数实例

<!-- language: !js -->

    var aFunc=function ( ) {
        var MyFunc=function(){};
        //返回一个函数到aFunc
        return function(){
            return MyFunc;
        }
    }();
    //多次调用，将得到MyFunc同一个实例
    var f1=aFunc(),
        f2=aFunc();
    alert(f1==f2);//true

在aFunc内部创建了一个匿名函数，这个匿名函数（即使是多个实例）访问upvalue时，将会得到相同的数据，因此多次调用aFunc时，就会得到同一个MyFunc()实例——匿名函数的upvalue中的那个实例

以上各形式的本质就是 __“产生”__ 与 __“不产生”__ __函数实例__ 两种形式，进而形成两种闭包、两种保护数据和提供运行上下文环境的形式

### 函数闭包与调用对象
《JavaScript权威指南》对“调用对象”作出了三点说明：

- 对象属性与变量没有本质差异；
- 全局变量其实是“全局对象”的属性；
- 局部变量其实是“调用对象”的属性

<!-- language: !js -->

    function myFunc(x,y,z){
        var i,j,k;
        var m=100;
        var foo=function(){};

        function func(){}

        i=m*100;
        if(...){...}
    }

产生如下结构（JavaScript执行环境，Execution Context），TheContext描述函数作为对象时的外在表现

<!-- language: js -->

    TheContext={
        type:'Function',
        name:'myFunc',
        params:[...],
        boyd:ScriptObject
    }

    ScriptObject={
        type:'SCRIPT',
        varDecls:[...],//第2~4行的i,j,k,m,foo
        funDecls:[...],//第6行的func
        source:'...'//第8~9行code
    }

这个`ScriptObject`包含了函数代码体中全部的语法分析结构，包括：内部变量表`varDecls`，内嵌函数表`funDecls`，以及除此之外的全部代码`source`，所谓 ___“调用对象”___，是指当前正在调用函数的`ScriptObject`结构

#### "调用对象"的局部变量维护规则
规则一：在函数开始执行时，`varDecls`中所有值将被置为undefined，因此我们无论如何访问函数，变量初始值总是undefined

<!-- language: !js -->

    function myFunc(){
        alert(i);
        var i=100;
    }
    myFunc();//undefined
    myFunc();//undefined

由于`varDecls`总在语法分析阶段就已经创建好了，因此在myFunc()内部，即使在"var i"这个声明之前访问该变量，也不会有语法错误，而是访问到位于`ScriptObject`结构内`varDecls`中该变量的初值：undefined，由于`varDecls`总在执行前被初始化，因此第二次调用myFunc()时，值仍是undefined

规则二：函数执行结束并退出时，`varDecls`并不被重置，正因为此，js中函数能提供“在函数内保存数据”这种特性。

规则三：函数内数据持续的生存周期，取决于该函数实例是否存在活动引用，如果没有，则“调用对象”被销毁

#### "全局对象"的变量维护规则
`ScriptObject`结构对全局对象来说，仍然适用，唯一不同的是，从代码语义分析上，我们找不到一个全局的`TheContext`结构，不过在js引擎内部，全局的`TheContext`仍然存在，我们可以把全局代码看成是某个函数的`SCRIPT`代码块，这个函数实例被系统所持有

- 由于该函数从来不被再次进入，因此不会被重新初始化
- 由于该函数仅有一个被系统持有的实例，因此它自身和内部数据总不被销毁

#### 函数闭包与"调用对象"的生存周期
上述的“调用对象”其实在语法分析期就可以得到，有了这个在语法分析期得到的`ScriptObject`作为原型，在运行期该函数实例有一个函数闭包，所以在执行它时，引擎将会：

- 创建一个函数实例；然后，
- 为该函数实例创建一个闭包；然后，
- 为该函数实例（及其闭包）的运行环境从`ScriptObject`复制一个“调用对象”

<!-- language: !js -->

    function up_level() {
        var x=100;
        var f=function(){};

        function myFunc(d){
            var y=20+d;//d是值类型
            f();
        }

        myFunc(x);
    }

myFunc()访问了两个存在的依赖的变量，其一是参数d，通过函数入口传入，这种情况下，它可能 __在函数内被持有，或仅作求值运算__，本例中，在myFunc()退出后，d的引用将被复位。第二个变量f就与此不同，变量f是在闭包中通过upvalue访问到up_level()中变量，由于f是闭包外的引用，__当函数myFunc()还存活时，函数up_level()必须存活__，由于myFunc还可能被外部引用，所以即使ul_level()结束后，f也不应该被清除

因此一个闭包A使用了upvalue，那么upvalue所在的闭包B，就为A所依赖

<!-- language: !js -->

    function up_level(){
        var f=function(){};
        function myFunc(func){
            var _func=func;//func是引用
            function aFunc(){
                _func();
            }
        }
        myFunc(f);
    }

这个例子中，函数f被myFunc()的变量_func所持有了一个引用，且函数f是通过入口参数传递的，但仍然进行了引用计数，前例d是值类型，入口参数仅是求值运算，由于动态语言，一个入口参数是否为引用类型，只能运行期动态的获得，无法通过代码进行静态推演

#### 引用与泄漏
js中最常见的其实是对象属性的引用，主要指如下：
- 对象在构造时，使用this引用进入构造器函数
- 对象在调用方法时，使用this引用进入函数
- 某个函数使用call/apply方法调用，并传入某个对象作为this引用
- 调用一个函数时，对象从参数入口传入

下面以第四种情况描述函数实例被创建和引用的过程

<!-- language: !js -->

    function MyObj(obj){
        var foo=function(){};
        if(!obj) return;
        obj.method=foo;
    }

    //例1
    MyObj();

    //例2
    MyObj(new Object());

    //例3
    var obj=new Object();
    MyObj(obj);

例1中，MyObj()被调用，在函数内部有一个匿名函数的实例被创建，并被赋值给foo变量，但因参数obj为undefined，所以该函数实例没有被返回到MyObj()函数外，因此MyObj()执行结束后，闭包内的数据未被外部引用，闭包随即销毁（这与内存回收算法有关），foo指向的匿名函数也被销毁

例2中，传入参数obj是一个有效的对象，于是匿名函数被赋值给obj.method，因此建立了一个引用，在MyObj()执行结束的时候，该匿名函数与MyObj()都不能被销毁，但随后，由于传入的对象未被任何变量引用，因此随即销毁了，而后obj.method的引用得以释放，这时foo指向的匿名函数没有任何引用、MyObj()内部也没有其他数据被引用，因此开始MyObj()闭包的销毁过程

例3中，开始的过程与例2一致，但由于obj是一个在MyObj()之外具有独立生存周期的外部变量，js引擎必须对这种持有MyObj()闭包中的foo变量（所指向的匿名函数实例）的关联关系加以持续地维护，直到该变量被销毁，或它的指定方法（obj.method）被重置、删除时，它对foo的引用才会得以释放：

<!-- language: !js -->

    //重新置值，关联关系被清除
    obj.method=new Function();

    //删除成员时，关联关系被清除
    delete obj.method;

    //变量销毁（或重新置值）导致的关联关系清除
    obj=null;

然而有些对象总不能被销毁（例如在DOM中存的泄漏）或在销毁时不能通知到js引擎，因此也就有些js闭包总不能被销毁

### 函数实例拥有多个闭包的情况
一般情况下，一个函数实例只有一个闭包，在闭包中的数据没有被引用时，该函数实例与闭包就被同时回收了，但也存在函数实例有多个闭包的情况：

<!-- language: !js -->

    var checker;

    function myFunc() {
        if(checker) checker();
        alert('do myFunc: '+str);
        var str='test.';
        if(!checker){
            checker=function(){
                alert('do Check: '+str);
            };
        }
        return arguments.callee;
    }
    //连续执行两次myFunc()
    myFunc()();

运行将输出三个信息：

<!-- language: plain -->

    do myFunc: undefined
    do Check: test.
    do myFunc: undefined

第一、三个信息输出undefined在于，函数被调用时，函数内的局部变量被声明并被初始化为undefined；局部变量表被保存的该函数闭包的varDecls域中。

关于第二个输出test，是在第二次调用myFunc时输出，但所输出的值，却是第一次调用中完成赋值的，它是一个局部函数引用，第一次myFunc调用结束后，闭包并没有被销毁（return arguments.callee;被外部引用）

所以，js中函数实例可能拥有多个闭包

### 语句或语句块中的闭包问题
一般情况下，当一个函数实例被创建时，它唯一对应的闭包也就被创建，下面由于构造器被执行再次，因此内部的foo函数也被创建了两个函数实例，并赋给this对象的成员

<!-- language: !js -->

    function MyObj() {
        function foo(){}
        this.method=foo;
    }
    var obj1=new MyObj(),
        obj2=new MyObj();
    alert(obj1.method == obj2.method);//false

尽管多个实例，但它们仍然共享一个外层函数闭包中的upvalue值：

<!-- language: !js -->

    var obj={};
    var events={m1:"clicked",m2:"changed"};

    for(e in events){
        obj[e]=function() {
            alert(events[e]);
        };
    }
    alert(obj.m1==obj.m2);//false
    obj.m1();
    obj.m2();//两者输出相同值

m1()和m2()取决于for..in语句最后一次迭代中对变量e的置值，所以输出结果是一样的。代码本意想每个函数实例输出不同的值，可以再加一外层函数，利用“在函数内保存”

<!-- language: !js -->

    var obj={};
    var events={m1:"clicked",m2:"changed"};

    for (e in events) {
        obj[e] = function(aVlue) {//闭包lv1
            return function() {//闭包lv2
                alert(events[aVlue]);
            };
        }(e);
    }
    obj.m1(); //clicked
    obj.m2(); //changed

或

<!-- language: !js -->

    var obj={};
    var events={m1:"clicked",m2:"changed"};

    for (e in events) {//在闭包内通过局部变量保存数据
        void function() {//闭包lv1
            var aValue = e;
            obj[e] = function() {//闭包lv2
                alert(events[aValue]);
            };
        }();
    }
    obj.m1(); //clicked
    obj.m2(); //changed

上两例中，闭包lv2引用了闭包lv1中的入口参数或局部变量，在obj未被清除之前，两个闭包都不会被销毁，但多了一层闭包增加了系统消耗，可以使用以下：

<!-- language: !js -->

    var obj={};
    var events={m1:"clicked",m2:"changed"};

    for (e in events) {
        (obj[e]=function () {
            alert(events[arguments.callee.aValue]);
        }).aValue=e;//相当于obj[e].aValue=e; 给函数加上属性
    }
    obj.m1(); //clicked
    obj.m2(); //changed

### 闭包中的标识符(变量)特例
函数闭包内的标识系统应该包括：

- this;
- 局部变量(varDecls);
- 函数形式参数名(argsName);
- arguments;
- 函数名(funcName)

以上标识符系统时是按优先顺序的：

<!-- language: !js -->

    //输出hi，而非函数foo.toString()
    function foo(foo) {
        alert(foo);
    }
    foo('hi');

    //输出number，而非object
    function foo2(arguments){
        alert(typeof arguments);
    }
    foo2(100);

    //输出object，而非function
    function argumetns(){
        alert(typeof arguments);
    }
    argumetns();

形式参数名优先于内置对象arguments，后者优先于函数名，下面看下局部varDecls

<!-- language: !js -->

    //输出test
    function foo(str) {
        var str;
        alert(str);
    }
    foo('test');

    //输出member
    function foo2(str) {
        var str='member';
        alert(str);
    }
    foo2('test');

当形式参数与未赋值的局部变量名重复时，取形式参数值；当形式参数与有值的局部变量名重复时，取局部变量值

### 函数对象的闭包及其效果
这里的“函数对象”特指Function()构造器创建的函数，它与函数直接量声明、匿名函数不同，它在任意位置创建的实例，都处于全局闭包中，亦即 __Function()的实例的upvalue总是指向全局闭包__

<!-- language: !js -->

    var value='this is global';
    function myFunc() {
        var value='this is local';
        var foo=new Function('\
            alert(value);\
        ');
        foo();
    }
    myFunc();//输出'this is global'

由此带来的便利就是，在任意多层的函数内部，都可以通过Function()创建函数而不至于导致闭包不能释放，所以3.5.6的示例还可以写成：

<!-- language: !js -->

    var obj={};
    var events={m1:"clicked",m2:"changed"};

    for (e in events) {
        obj[e] = new Function("\
            alert(events['" + e + "']);\
        ");
    }
    obj.m1(); //clicked
    obj.m2(); //changed

但有一个限制，在Function()构造器中，在通过字符串参数来保证没有对变量的引用时，变量是先转换为字符串值，因此，如果该变量在转换时失去原意，如Object等，这个方法显然不可行

### 闭包与可见性

#### 函数闭包带来的可见性效果
- 变量在全局声明（用或不用var），则在全局可见
- 变量在代码任何位置隐式声明（不用var），则在全局可见
- 变量在函数内显式声明（用var），则在函数内可见
- 可见性传递：变量A在函数内可见，则在其所有内嵌函数中可见
- 可见性覆盖：在函数内显式声明变量A，将覆盖当前可见的同名函数

上面规则具体实现上，要求在语法分析期：

- 将一个函数代码体中所有var关键字声明的变量记入它自己的srciptObject的varDecls域，然后，
- 设定访问规则“如果当前函数的srcriptObject.varDecls域中不存在该变量声明，则通过'闭包.parent'来取得上一层函数的scriptObject.varDecls作为upvalue”，该规则就产生了可见性传递与覆盖
- 变量在代码任何位置隐式声明（不用var），则在全局可见的实质是，回溯到顶层的全局函数闭包中的varDecls，并在该位置“隐式地”声明一个变量

每一个函数实例都有一份scriptObject的副本，因此不同的的闭包访问到的私有变量不一致

#### 对象闭包带来的可见性效果
所谓对象闭包，指使用with语句时与with()所指示对象相关的闭包，__该闭包被动态创建并添加到执行环境当前的闭包链顶端__

<!-- language: !js -->

    function foo() {
        with(this){
            var value;
        }
        alert(value);//显示undefined
    }
    foo();

var value被理解为foo()中的变量声明，且该变量初值为undefined，如果with所指示的对象闭包中存在同名的属性：

<!-- language: !js -->

    var obj={value:'hello'};
    function foo() {
        with(obj){
            var value=10;
            alert(obj.value);//显示10
        }
        alert(value);//显示undefined
    }
    foo();

由于对象obj存在value属性，当前最顶层的闭包是obj的对象闭包，且该闭包存在value这个标识符（属性名），因此应当向它置值

对象闭包中，对“对象直接量”闭包如何访问该对象自身：

<!-- language: !js -->

    function self(x) {
        return x.self=x;
    }

    with(self({})){
        self.value=100;
        for(var i in self){
            alert(i+' : '+self[i]);
        }
    }

函数闭包与对象闭包交叉

<!-- language: !js -->

    var obj={value:200};
    var value=1000;
    with(obj){
        function foo() {//具名函数的闭包
            value *= 2;//依赖于函数静态位置所决定的闭包链
        }
        foo();
    }
    alert(obj.value);//200
    alert(value);//2000

wiht的限定只对第7行有效，第5行通过变量作用域影响到全局变量value，foo()闭包链是在语法分析期决定的，它被添加在全局闭包之后，而with是在运行期打开了obj对象闭包

#### 匿名函数的闭包与可见性效果
有一种做法可以避免上述foo()不能访问obj.value的问题：

<!-- language: !js -->

    var obj={value:200};
    var value=1000;
    with(obj){//对象闭包
        obj.foo=function() {//匿名函数的闭包
            value *= 2;//依赖于函数闭包所在的当前闭包链
        }
        obj.foo();
    }
    alert(obj.value);//400
    alert(value);//1000

匿名函数的闭包也如果对象闭包一样，动态的添加到 __当前闭包链(正好是with所打开的obj对象闭包)__ 的顶端，匿名函数作为一个直接量创建时，由引擎动态添加在闭包链上，创建位置决定了它所在闭包链位置，无关该匿名函数添加为obj的方法，以下效果同上：

<!-- language: !js -->

    var obj={value:200};
    var value=1000;
    with(obj){
        void function () {
            value*=2;
        }();
    }
    alert(obj.value);//400
    alert(value);//1000

让外部持有一个foo2()内部的一个匿名函数引用，通过它可以访问到foo2()内部成员（及闭包upvalue）

<!-- language: !js -->

    function foo() {
        function foo2() {
            var msg = 'hello.';
            getVarDecl = function(varName) {
                return eval(varName);
            }
        }
        foo2();
        var getVarDecl;
        alert(getVarDecl('msg'));
    }
    foo();

## ES5严格模式下执行限制
- 在严格模式下向不存在标识符赋值时将导致"引用异常ReferenceError"，而在非严格模式下将会隐式的在全局闭包中创建该标识并完成赋值

<!-- language: !js -->

    "use strict"

    try{
        aName=123;
    }catch(e){
        alert(e.name+' '+e.message);
    }

- 运算符处理一些不可处理的操作数时，将导致"类型异常TypeError"，表明操作数是不适当的类型或具有不适当的属性描述符性质

<!-- language: !js -->

    "use strict"

    try{
        var obj={x:100};
        Object.preventExtensions(obj);
        obj.y=100;

        Object.seal(obj);
        delete obj.x;

        delete Function.prototype;

        Object.defineProperty(obj,'x',{writable:false});
        obj.x=200;

    }catch(e){
        alert(e.name+' '+e.message);
    }

- 访问arguments.callee或函数的caller属性将导致"类型异常TypeError"

<!-- language: !js -->

    "use strict"

    function foo(){
        alert(typeof arguments.callee);
        alert(typeof arguments.callee.caller);
        alert(typeof foo.caller);
    }

    try{
        foo();
    }catch(e){
        alert(e.name+' '+e.message);
    }

- 对arguments[n]或形参的修改将不再相互影响

<!-- language: !js -->

    "use strict"

    function f(x){arguments[0]=100;return x;}
    alert(f('abc'));
    function f2(x){x=100;return arguments[0];}
    alert(f2('abc'));

<!-- language: !js -->

    function f(x){arguments[0]=100;return x;}
    alert(f('abc'));
    function f2(x){x=100;return arguments[0];}
    alert(f2('abc'));

js动态语言特性
==============
## 概述
所谓语言，包括语法、语义和语用，在计算机系统中实现某种语言时，如果语言陈述时无法确定，必须在运行时才能确定三者之间的关系，称该语言是动态语义的，如js中`a+b`，我们不能确定是字符串连接还是数值求和

js“标识符指称不确定”，表现为动态类型、动态重写、动态存储数据结构；“上下文环境不确定”表现为动态的变量、动态语法作用域、闭包作用域

SNOBOL与APL是一种在标识符上动态绑定“数据类型”与“存储位置”含义的语言，变量理解为一个无类型的指针，只有指针被分配一个确定的内存空间时，才可以获知该指针指向内存区的内容以及数据类型（python也如此?）

js动态语言特性：动态类型绑定、动态存储绑定、动态执行

## 动态执行
随时可以载入一段源代码文本并执行它，动态执行分为动态装载与动态执行两个阶段。

`eval()`只接受唯一一个字符串值，如果参数是其他类型的数据，包括字符串对象实例，那么eval()只是原封不动地返回该值，而不会有其他任何效果

### 动态执行与闭包
由于代码总要运行在一个闭包中，因此eval()也需要一个闭包环境

<!-- language: !js -->

    var i=100;
    function myFunc() {
        var i='test';
        eval('i="hello"');//只修改了局部的i
        alert(i);//'hello'
    }
    myFunc();
    alert(i);//100

它修改了局部变量，虽然`eval()`是Golbal对象的方法，但下面代码有区别：

<!-- language: !js -->

    var i=100;
    function myFunc() {
        var i='test';
        window.eval('i="hello"');//修改了全局的i
        alert(i);//'test'
    }
    myFunc();
    alert(i);//'hello'

低版本IE中，本例与上例一致。

#### eval使用全局闭包
上面的例子中，SpiderMonkey JavaScript认为eval()方法是所有对象的一个方法，所以`window.eval('...')`认为是相当于`eval.call(window,'...')`

<!-- language: !js -->

    var i=100;
    function myFunc() {
        var i='test';
        eval.call(window,'i="hello"');//修改了全局的i
        alert(i);//'test'
    }
    myFunc();
    alert(i);//'hello'

低版本IE中没有这种能力，无论window.eval('...')或eval.call(window,'...')，JScript使用另一种：window.execScript()，请在IE中执行

<!-- language: !js -->

    var i=100;
    function myFunc() {
        var i='test';
        window.execScript('i="hello"');//修改了全局的i
        alert(i);//'test'
    }
    myFunc();
    alert(i);//'hello'

JsScript中execScript还提供跨脚本语言的代码能力，execScript('dim i="test"','VBScript');

#### eval使用当前函数的闭包
eval()使用当前函数闭包，影响局部变量。

<!-- language: !js -->

    var i=100;
    function myFunc() {
        eval('var i="hello"');
        return i;
    }
    alert(myFunc());//"hello"
    alert(i);//100

js代码解释执行的过程：

- 在js中，代码文本是先被解释为语法树，然后按照语法树来执行的；
- 在每次执行语法树中的一个函数（的实例）时，会将语法树中与该函数相关的形式参数、函数局部变量、upvalue，以及内层的嵌套函数等信息复制到一个结构中，该结构称ScriptObject
- ScriptObject动态关联到一个闭包，闭包与ScriptObject具有不同的生存周期
- 按照语法树来执行函数体中的代码，需要访问变量时，先考察ScriptObject中的局部变量等，然后考察upvalue

上例中eval()试图在当前闭包环境中新声明一个变量，因此该 __变量名__ 需要被添加到 __ScriptObject__ 中，而它的一个 __值__ 则需要被存放在当前 __闭包__

这显然意味着ScriptObject与闭包都是需要 __动态__ 维护的。

### 动态执行过程中的语句、表达式与值
eval()总是将被执行的代码文本视为一个“代码块(block)”，代码块中包含的是语句、复合语句或语句组，由于表达式可解释为表达式语句，代码块事实上变成了“由单个或多个语句组成”

语句是存在返回值的，所以，eval()总试图返回所执行代码块的值（由代码块最后一个语句决定）

<!-- language: !js -->

    alert(eval('for(var i=0;i<10;i++);'));//undefined'

循环的代码体，以及循环的控制子句都不会有返回值，所以返回undefined

<!-- language: !js -->

    alert(eval('i=6;for(var i=0;i<10;i++);;;'));//6

循环体中，以及后面的空语句不影响返回值，返回赋值表达式语句`i=6`

<!-- language: !js -->

    alert(eval('6;'));
    alert(eval('6'));

以上两句都认为“单值表达式语句”，返回数字6

eval()表示的是一个动态的语句执行系统，而非一个动态的“取值/赋值”系统，“返回值”只是js中语句的副作用导致的结果，而不是为了“值/对象的序列化”而专门设计的，所以尽管我们可以取得字符串、数值或布尔值：

<!-- language: !js -->

    alert(eval('true'));
    alert(eval('"this is a string"'));
    alert(eval('3'));

但不能用同样的代码得到一个直接量对象

<!-- language: !js -->

    alert(eval('{name:"zyx",age:30}'));
    //Uncaught SyntaxError: Unexpected token :

原因在于eval()将大括号视为一个复合语句的标识，第一个冒号成了标签声明，name成了标签，"zyx"成了字符串直接量，age成了一个变量标识符，第二个冒号不能合理作语法分析，出现语法分析期异常

同理，下面代码不会异常，但返回为数值1，而不是对象：

<!-- language: !js -->

    alert(eval('{value:1}'));

解决这个问题的方法，将这里的直接量声明，变成一般表达式语句，明确指出“由大括号引导”的一段代码为“值”，使用强制表达式运算符`()`，eval()将这段代码视作由单个“一般表达式语句”组成的“语法块”：

<!-- language: !js -->

    alert(eval('({value:1})'));

不过低版本IE的JScript中存在一个例外，函数直接量（指匿名函数）不能通过这种方式获得，输出undefined

<!-- language: !js -->

    var func=eval('(function(){})');
    alert(typeof func);

这种情况可通过具名函数获得

<!-- language: !js -->

    var func=eval('(function func(){})');
    alert(typeof func);

但可能遇到必须使用匿名函数的情况，可使用：

<!-- language: !js -->

    var func1=eval('(function(){}).prototype.constructor');//取原型对象的构造器
    var func2=eval('({$:function(){}}).$');//通过对象直接量的属性
    var func3=eval('[function(){}][0]');//取函数数组中项

    alert(typeof func1);
    alert(typeof func2);
    alert(typeof func3);

### 奇特的、甚至是负面的影响
某些情况下，动态执行会导致变量作用域发生变化

<!-- language: !js -->

    var i=100;
    function myFunc(ctx) {
        alert(i);
        eval(ctx);
        alert(i);
    }
    myFunc('var i=10;');//先输出100，再输出10

引擎全面支持动态执行，则编译过程必须以明码形式保存标识符表，否则代码是不可逆的，执行系统无法在运行期查找标识符。

<!-- language: !js -->

    function myName (ctx) {
        eval(ctx);
        alert(typeof Array);//string
        eval.call(window,'myName=Array.prototype.splice');
    }
    myName('var Array="..."');
    alert(typeof Array);//function
    alert(myName.toString());//function splice(){[native code]}

其结果是，myName被重写了，而Array由于使用了var在myName中为局部变量，并且在eval()是使用了window作为this，所以取的是原生Array，最终是myName被重写为原生Array原型对象的splice方法

js的伪编译方案，“语法树+标识符表”

## 动态方法调用(call与apply)
js中有三种执行体，一种是eval()函数入口参数中指定的字符串，该字符串总是被作为 __当前函数上下文__ 中的语句来执行；第二种是new Function()中传入的字符串，该字符串总是被作为一个 __全局的、匿名函数闭包__ 中的语句行被执行；第三种情况，执行体就是一个函数，可以通过函数调用运算符“()”来执行。

### 动态方法调用中指定this对象
js中所谓方法，只是用成员存取运算符，句号.或中括号[]，找到对象成员，然后将该成员作为函数执行而已，调用对象方法，与作为普通函数调用该成员并没有本质不同，唯一差异仅在二者传入的this引用。

所以将一个普通函数作为一个对象的方法调用，或者将A对象的方法作为B对象的方法调用，唯一要做的是，也仅是改变一个this引用

<!-- language: !js -->

    function foo() {
        alert(this.name);
    }
    var obj1={name:'obj1'};
    var obj2=new Object();
    obj2.name='obj2';

    foo.call(obj1);
    foo.apply(obj2);

由于方法调用中this引用指向当前的实例，因此我们也能够用下面代码来传送this引用

<!-- language: !js -->

    function foo() {
        alert(this.name);
    }
    function MyObj(){
        this.name='MyObj';
    }
    MyObj.prototype.doAction=function(){
        foo.call(this);//this引用指向当前的实例
    }
    var obj3=new MyObj();
    obj3.doAction();

### 通过apply来方便传递参数

<!-- language: !js -->

    function calc_area(w,h) {
        alert(w*h);
    }

    function do_calc1(){
        //apply将参数组成的数组传入，参数将被展开供函数执行
        calc_area.apply(null,arguments);
    }
    do_calc1(3,4);

    function do_calc2(){
        arguments[0]*=2;//直接修改arguments
        calc_area.apply(null,arguments);
    }
    do_calc2(3,4);

    function do_calc3(w){
        w*=2;//修改形参也影响到arguments
        calc_area.apply(null,arguments);
    }
    do_calc3(3,4);

### 丢失的this引用

<!-- language: !js -->

    try{
        print=document.writeln;
        print('this is a test');
    }catch(e){
        alert(e.message);
    }

上面代码会导致异常，因为在方法内需要使用this引用，简单做法是

<!-- language: !js -->

    print=document.writeln;
    print.call(document,'this is a test');

或重写一下`document.writeln`，由`方法`变成`普通函数`

<!-- language: !js -->

    document.writeln = function(f) {
        return function() {
            f.apply(document, arguments);
        }
    }(document.writeln);

    print = document.writeln;
    print('this is a test');

### 栈的可见与修改

<!-- language: !js -->

    function func_1(val,ref){
        val=200;
        ref.val=200;
    }
    function func_2(val,ref){
        func_1.apply(this,arguments);//尝试修改arguments
        alert(val);
        alert(ref.val);
    }
    func_2(100,{val:100});

尽管func_1与func_2使用的arguments是同一个，但在func_1.apply()调用时，arguments被做了一次复制：值数据被复制，引用数据被创建引用，形成两套数据。但这种隔离是非常微弱的：

<!-- language: !js -->

    function func_3() {
        arguments.callee.caller.arguments[0]=200;
    }
    function func_4(val) {
        func_3();
        alert(val);
    }
    func_4(100);

以上示例有争议，firefox/chrome中`arguments.callee.caller.arguments`中数据是只读的，而ie中可修改，所以前者还显示100，后者显示200，但增加arguments是可以的

<!-- language: !js -->

    function func_5() {
        Array.prototype.push.call(arguments.callee.caller.arguments,200);
    }
    function func_6(val) {
        func_5();
        alert(arguments.length);//显示2
    }
    func_6(100);

### 低版本中的call()与apply()
call()与apply()总保证函数运行在“函数自己所在的闭包环境”中，低版本中没有call()与apply()时，可通过下面实现兼容

<!-- language: !js -->

    Function.prototype.apply2=function (thisArg,argArray) {
        thisArg=(thisArg==null)?window:Object(thisArg);
        thisArg.__applyTemp__=this;//保证被拦截方法的this指向thisArg

        var parameters=[],
            length=(argArray||'').length >>> 0;//确保length
        for(var i=0;i<length;i++){
            parameters[i]='argArray['+i+']';//参数展开传递
        }

        var functionCall='thisArg.__applyTemp__('+parameters+')';
        try{
            return eval(functionCall);
        }finally{
            try{delete thisArg.__applyTemp__;}catch(e){}
        }
    }

    Function.prototype.call2=function(thisArg){
        var slice=Array.prototype.slice;
        //截取arguments中下标1到结束的数组，并作为apply2中的arrArray参数
        return this.apply2(thisArg,slice.apply2(arguments,[1]));
    }

    //test
    alert(Array.prototype.concat.apply2([0],[1,2,3,[4,5]]));
    alert(Array.prototype.concat.call2([0],1,2,3,[4,5]));

但此版本的apply()忽略了一个问题，不同引擎的eval()对被执行代码中的函数调用栈理解是不同的：

<!-- language: !js -->

    function foo() {
        alert(arguments.callee.caller);
    }
    function myFunc() {
        eval('foo()');
    }
    myFunc();

在低版本IE中显示null，firefox/chrome显示myFunc的定义，即eval()既保证代码在myFunc()闭包中，也保证继承myFunc()的调用栈。所以为了保证低版本IE也具备访问调用栈的能力

<!-- language: js -->

    Function.prototype.apply3=function (thisArg,argArray) {
        thisArg=(thisArg==null)?window:Object(thisArg);
        thisArg.__applyTemp__=this;

        var parameters=[],
            length=(argArray||'').length >>> 0;
        for(var i=0;i<length;i++){
            parameters[i]='argArray['+i+']';
        }

        var functionCall='return thisArg.__applyTemp__('+parameters+')';
        try{
            var __local_function__=eval('(function(){'+functionCall+'})');
            return __local_function__();
        }finally{
            try{delete thisArg.__applyTemp__}catch(e){}
        }
    }

local_function是apply3()闭包中的一个局部变量，因此可以访问到apply3()闭包中各个标签符，又因为它是被作为函数调用的，因此可以继承apply3()调用栈(与原生的apply的栈深度不同)

### ES5中的call()与apply()
严格模式与非严格模式区别:

- 在非严格模式中，如果call()/apply()方法第一个参数传入null或undefined，那么在函数内的this指向全局对象，而在严格模式中，this仍将使用null或undefined

<!-- language: !js -->

    "use strict"

    function f(){
        var msg=(this===undefined) ? 'undefined'
            : (this===null) ? 'null'
            : '';
        alert(msg || typeof this);
    }

    f.call(null);
    f.call(undefined);

<!-- language: !js -->

    function f(){
        var msg=(this===undefined) ? 'undefined'
            : (this===null) ? 'null'
            : '';
        alert(msg || typeof this);
    }

    f.call(null);
    f.call(undefined);

- 在非严格模式中，如果call()/apply()方法第一个参数传入值类型的数据，那么它会先包装为对象再作为this引用，在严格模式中并不会发生包装过程

<!-- language: !js -->

    "use strict"

    function f(){
        var msg=(this===undefined) ? 'undefined'
            : (this===null) ? 'null'
            : '';
        alert(msg || typeof this);
    }

    f.call('abc');
    f.call(2);
    f.call(true);

<!-- language: !js -->

    function f(){
        var msg=(this===undefined) ? 'undefined'
            : (this===null) ? 'null'
            : '';
        alert(msg || typeof this);
    }

    f.call('abc');
    f.call(2);
    f.call(true);

### ES5中bind()与函数的延迟调用
bind()将函数绑定到一个对象上，并返回绑定后的函数，绑定后的函数总是作为该对象的一个方法来调用，__即使该函数赋值为其他对象的属性__：

<!-- language: !js -->

    obj={};
    function foo(){return this;}

    foo2=foo.bind(obj);

    obj2={};
    obj2.foo=foo;
    obj2.foo2=foo2;

    alert(obj===foo2());
    alert(obj===obj2.foo2());
    alert(obj2===obj2.foo());

bind()也采用类似call()传入一系统参数，这些参数 __暂存__ 到调用该函数时才使用，而不是当前使用：

<!-- language: !js -->

    obj={msg:'message: '};
    function foo(a){
        alert(this.msg+a);
    }

    foo2=foo.bind(obj,'abc');
    foo2();

bind()绑定后的函数仍然可以作为 __构造器使用__，并允许使用上述的传入参数，这种情况下，构造出来的对象既是“绑定后的函数”的一个实例，但同时也是原来的(绑定前的)函数的一个实例

<!-- language: !js -->

    obj={};
    function foo(a) {
        alert(this===obj);
        alert(a);
    }
    Foo=foo.bind(obj,'abc');

    Foo();//true 'abc'
    newIns=new Foo('123');//false 'abc'

    alert(newIns instanceof foo);//true
    alert(newIns instanceof Foo);//true

bind()方法的第一个参数在严格与非严格模式下区别同call()/apply()

## 重写
js重写是一个代码执行期的行为，在代码的语法分析期的，引擎既不对重写行为进行任何的预期，也不对其进行限制。

### 原型重写
原型是可以重写的，所以可以用同一个构造器构造两个完全不同的实例来：

<!-- language: !js -->

    function MyObj(){}
    var obj1=new MyObj();

    MyObj.prototype.type='MyObj';
    MyObj.prototype.value='test';
    var obj2=new MyObj();

    MyObj.prototype={
        constructor:MyObj,
        type:'Bird',
        fly:function() {}
    };
    var obj3=new MyObj();

    alert(obj1.type);//'MyObj'
    alert(obj2.type);//'MyObj'
    alert(obj3.type);//'Bird'

    alert(obj1 instanceof MyObj);//false
    alert(obj2 instanceof MyObj);//false
    alert(obj3 instanceof MyObj);//true

    alert(obj1 instanceof obj1.constructor);//false
    alert(obj1.constructor == MyObj);//true

obj1和obj2是类同的两个实例，8-12行将原型重写成一个新的对象，破坏了原型继承链，结果是obj3与obj1完全不同，影响到继承关系`instanceof`的识别上，一个构造器的原型对象被重写，意味着，“此前一个原型被废弃”，旧的实例使用这个被废弃的原型(obj1.＿＿proto＿＿)，并受该原型的影响，新创建的实例则使用重写后的原型(MyObj.prototype)，受新原型影响。

`instanceof`判断对象b是否是A的实例，是根据b.＿＿proto＿＿是否在A.prototype原型链上 [参考](http://www.cnblogs.com/fool/archive/2010/10/14/1850910.html)

### 构造器重写
显示的构造器修改也会导致继承关系丢失

<!-- language: !js -->

    function MyObj() {}
    var obj1 = new MyObj();

    MyObj = function() {};
    var obj2 = new MyObj();

    alert(obj1 instanceof MyObj); //false
    alert(obj2 instanceof MyObj); //true
    alert(obj1 instanceof obj1.constructor); //true
    alert(obj1.constructor==MyObj); //false

第4行重写了MyObj()，导致第7行`instanceof`不能通过检查，与原型重写不同的是，第9行`obj1 instanceof obj1.constructor`表明它并不影响实例的继承关系

上例中`obj1 instanceof obj1.constructor===false`，本例中`obj1 instanceof obj1.constructor===true`，是因为上例obj1的constructor并未改变，但constructor的prototype变了，而instanceof又是检查原型链的，所以不匹配，而本例一直指向之前的constructor，它的prototype也未改变，所以保持匹配

如果上例改成如下

<!-- language: !js -->

    function MyObj() {}
    var obj1 = new MyObj();

    function MyObj() {}
    var obj2 = new MyObj();

    alert(obj1 instanceof MyObj); //true
    alert(obj2 instanceof MyObj); //true
    alert(obj1 instanceof obj1.constructor); //true
    alert(obj1.constructor==MyObj); //true

表明两个MyObj()函数声明在语法分析期出现了标识符覆盖，在执行期只有后一个声明是真正的MyObj()

#### 语法声明与语句含义不一致问题

<!-- language: !js -->

    (Object=function(){
    }).prototype.value=100;

    var obj=new Object();
    alert(obj.value);//100

上面的重写是一行赋值语句，它的返回值也是重写后的构造器函数，下面的是 __语法声明阶段重写__ :

<!-- language: !js -->

    (function Object(){
    }).prototype.value=100;

    var obj=new Object();
    alert(obj.value);//undefined

其实是在“执行期”修改一个匿名函数的prototype（'Object'只是在语法分析期时的标识符，运行期已没有），而该匿名函数在该行代码之后没有任何引用，也就立即释放了，未对系统有任何影响

<!-- language: !js -->

    var MyObj2=(function MyObj1(){});
    alert(typeof MyObj1);//undefined
    alert(MyObj2);

MyObj1()在运行期是不存在的

<!-- language: !js -->

    var obj={};
    obj.foo=function foo() {
        alert(foo.toString());
        alert(arguments.callee.toString());
        alert(foo===arguments.callee);
    }
    obj.foo();
    alert(typeof foo);

IE10及chrome、firefox不承认在表达式中声明的标识符。所以下面不会有Object()构造器重写:

<!-- language: !js -->

    myFunc=(function Object(){});
    myFunc2=function Object(){};

所以在IE10及chrome、firefox中，需要重写构造器，__必须要在语法声明阶段重写__ ，但表达式作为表达式语句时，仍然具有标识符含义：

<!-- language: !js -->

    //foo();//throw error
    if(true)
        function foo() {
            alert('foo');
        }
    foo();//can call

但下句会出错

<!-- language: !js -->

    if(true)
        (function foo() {
            alert('foo');
        })
    foo();

#### 对象检测的麻烦
因为构造器可以被重写，导致`instanceof`时，存在对象误判，需要保留一个局部的引用以避免其他代码重写这些构造器：

<!-- language: !js -->

    checkInstance=function () {
        var _Array=Array;//保留原生引用
        var _String=String;
        var _Object=Object;

        return function (obj) {
            switch(obj.constructor){
                case _Array:return "array";
                case _String:return "string";
                case _Object:return "object";
            }
        }
    }();//不要忘记这个括号

    var str=new String("");
    alert(str instanceof String);//true
    String=function(){};
    alert(str instanceof String);//false
    alert(checkInstance(str));//'string'

#### 构造器的原型(prototype属性)不受重写影响
构造器的原型创建自系统内部的对象系统，而不是可被外部覆盖的标识符Object，因此原型总能被创建：

<!-- language: !js -->

    var PureObject=Object;//备份系统内部Object

    Object=function(){};//重写
    function MyObj(){}//声明构造器

    //删除自己constructor,以便访问到父类同一属性
    delete MyObj.prototype.constructor;
    alert(MyObj.prototype.constructor==PureObject);//true

所以构造器的原型属性，并不受Object()类的重写影响，它总是创建自一个系统引擎中的对象构造器

#### “内部对象系统”不受影响
如果我们修改上面的PureObject的原型修改与重写，会影响整个对象系统。

#### 让用户对象系统影响内部对象系统

<!-- language: !js -->

    var PureObject=Object;

    Object=function(){};
    //Object.prototype={}.constructor.prototype;
    Object.prototype=PureObject.prototype;
    Object.prototype.hello=function (){
        alert('hello');
    }
    function MyObj(){}
    var obj=new MyObj();
    obj.hello();

#### 构造器重写对直接量声明的影响
直接量(string,boolean,number,function,regexp,object,array)声明总会调用“内部对象系统”为构造对象，但构造器与直接量却是两套系统，二者只是在初始时是关联的，在构造器被重写后，__直接量只是重写前的构造器(原生)相关__，重写不会对直接量声明构成什么影响

<!-- language: !js -->

    var str1="abc";
    String=function(){};
    String.prototype.name='myString';
    var str2="123"
    alert(str1.name);//undefined
    alert(str2.name);//undefined

#### 构造绑定
上述的例子在早期SpiderMonkey JavaScript中，string,boolean,number,function,regexp是不受构造器重写影响，但Object与Array却受影响

#### 内置构造器重写的概述
不建议重写内置构造器，确实需要时(框架底层)：

<!-- language: !js -->

    var arr1 = new Array(10);

    Array = function(_Array) {
        function NewArray() {
            alert('init array');//自定义拦截
            return _Array.apply(null, arguments);
        }
        NewArray.prototype = _Array.prototype;
        return NewArray;
    }(Array);

    var arr2 = new Array(10);
    //因为prototype一样，所以instanceof一致
    alert(arr1 instanceof Array);//true
    alert(arr2 instanceof Array);//true

### 对象成员的重写
#### 成员重写的检测
hasOwnProperty()

#### 成员重写的删除
一旦一个属性是在对象的原型中添加的，那么就不能直接从对象中删除它，只有从原型（以及其父代原型）中删除，但这过程并不安全，它会影响到该类的其他实例

<!-- language: !js -->

    function deleteProperty(obj,prop) {
        if(prop in obj){
            do{
                if(obj.hasOwnProperty(prop)) break;
            }while(obj.constructor && (obj=obj.constructor.prototype));
        }
        delete obj[prop];
    }

    function MyObj(){}
    MyObj.prototype.value=100;

    //需要正确维护原型链
    function MyObjEx() {
        this.constructor=arguments.callee;
    }
    MyObjEx.prototype=new MyObj();

    var obj1=new MyObjEx();
    var obj2=new MyObjEx();

    deleteProperty(obj1,"value");
    alert(obj1.value);
    alert(obj2.value);//所有子类对象中该成员也被删除

### 宿主对象写的限制
多数不能重写的情况，是出自宿主环境的限制。

### 引擎对重写的限制
#### this的重写

<!-- language: !js -->

    window.this='hello';
    alert(this);
    alert(this.this);

    var aObj={value:100};
    var value=1000;
    with({this:aObj}){
        alert(this.value);//1000;
    }

#### 语句语法中的重写
大多数语句本身不会导致重写

<!-- language: !js -->

    var obj={name:'n',value:10};
    for(var i in obj){
        obj={};
        alert(i);
        alert(obj[i]);
    }

#### 结构化异常处理中重写

<!-- language: !js -->

    var ex=100;
    try{
        alert(ex);
        throw new Error('Error');
    }catch(ex){
        alert(ex);
    }finally{
        alert(ex);
    }
    alert(ex);

在JScript中，ex在finally及全局中成为了Error对象

<!-- language: !js -->

    function foo1(x) {
        try{return x;}
        finally{x=x*2;}
    }
    alert(foo1(100));

    function foo2(x) {
        try{return x;}
        finally{return x*2;}
    }
    alert(foo2(100));

    function foo3(x) {
        try{return x;}
        finally{x.push(100);}
    }
    alert(foo3([1,2,3]));

## 包装类:面向对象的妥协
元数据经过“包装类”包装后得到的对象，与原来的元数据并不再是同一数据，只是二者的值相等而已

### 显式包装元数据
将 __类构造器当普通函数(不要new)__ 使用，首先包括三种值类型的包装类，Number(),Boolean(),String()

<!-- language: !js -->

    var v1=100,
        v2='hello,world',
        v3=true;
    alert(Number(v1));
    alert(typeof v1);//number
    alert(typeof Number(v1));//number
    alert(String(v2));
    alert(Boolean(v3));

这与强制类型转换不同，强制转换是在同一数据（相同内存地址的不同引用）上进行，上例产生一个新的数据，类型相同。

使用Object()转换，只能传入number,string,boolean三种值类型之一，转换成Object对象

<!-- language: !js -->

    var v1=100,
        v2='hello,world',
        v3=true;
    alert(Object(v1));
    alert(typeof Object(v1));//object
    alert(Object(v2));
    alert(Object(v3));

### 隐式包装的过程与检测方法

<!-- language: !js -->

    var data=100;
    //instanceof运算不对原数据进行“包装”
    data instanceof Number
    //如下导致异常，因为不能对元数据做in运算
    'constructor' in data
    //成员存取运算时，“包装”发生在存取行为过程中
    data.constructor
    //所谓方法调用，“包装”发生与上面相同
    data.toString()
    //做delete运算时，“包装”行为仍然发生在成员存取时
    delete data.toString

所谓元数据到对象的“隐式包装”，其实总是发生在成员存取运算中，然而如何检测被包装后的这个对象，我们知道，对象方法调用时，会传入一个this引用，而这个this引用必然是一个“真实的对象”，所以可通过如下方式取得：

<!-- language: !js -->

    Object.prototype.getSelf=function(){
        return this;
    }

    var data=100;
    var _this=data.getSelf();
    alert(typeof _this);

完整的检测

<!-- language: !js -->

    Object.prototype.getSelf=function(){
        return this;
    };
    Object.prototype.getClass=function(){
        return this.constructor;
    };
    Object.prototype.getTypeof=function(){
        return (typeof this);
    };
    Object.prototype.getInstanceof=function(){
        return (this instanceof this.getClass());
    };

    var samples=['',100,true,function(){},{},[],/./];

    var v1,v2,attr,cls;
    for(var i=0;i<samples.length;i++){
        v1=samples[i];
        v2=v1.getSelf();
        cls=v1.getClass();

        attr=[v1,typeof v1,v1.getTypeof(),v1 instanceof cls,v1.getInstanceof()];
        alert(attr);
    }

### 包装值类型数据的必要性与问题

<!-- language: !js -->

    Number.prototype.showDataType=function(){
        alert('value:'+this+' , type:'+(typeof this));
    };

    var n=100;
    alert(typeof n);
    n.showDataType();

在函数外部调用typeof查看n类型是会是number，而当调用n的showDataType()方法时，n的类型却变成了object，看起来像`new Number(n).showDataType()`,当调用结束后（该对象的生存周期结束时），临时创建的包装对象也将被清理掉。

显然变量n不等同于包装后的"new Number(n)"，因此“值类型数据的方法调用”其实被临时隔离在另外一个对象中完成的，无论我们如何修改这个新对象的成员，不会影响到原来的值

<!-- language: !js -->

    var str='s1';
    str.toString=function () {
        return "s2";
    };
    alert(str.toString());//不受str.toString重写影响

    String.prototype.toString=function(){
        return "s3";
    }
    alert(str.toString());//受到prototype.toString重写影响

## 关联数组:对象与数组的动态特性
所谓索引数组，指以序数类型作为下标变量，按序存取元素的数组，而关联数组则是指以非序数类型作为下标变量来存取的数组。

js中，索引数组的下标必须是值类型，如果是引用类型，则将它们转换为字符串值来使用

<!-- language: !js -->

    var dic = {'key1': 'value1', 'key2': 'value2'};
    var k = {toString: function(){return 'key2';}};
    alert(dic[k]);

    var arr = [5, 6, 7];
    k = {toString: function(){return '1';}};
    alert(arr[k]);

### 关联数组是对象系统的基础
### 用关联数组实现的索引数组
js中索引数组只是用数字的形式（内部仍然是字符串的形式）来存取的一个关联数组，并不是真正“连续分配”的有序个元素，因此索引方法并不会真正的带来效率，当数组变得更加无序，自由存取时，列举（递增/递减）效率会很差：

<!-- language: !js -->

    var aArray=new Array(10000);
    aArray[0]=1;
    aArray[1]=2;
    aArray[2]=undefined;

    var aCount=0;
    for(var i=0,imax=aArray.length;i<imax;i++){
        //将会列举imax次
        aCount++;
    }
    alert(aCount);//10000

    var bCount=0;
    for(var j in aArray){
        //只列举真实存在的元素数　+　原型成员
        bCount++;
    }
    alert(bCount);//3

第二种方法需要注意，我们不能保证for..in是准确地按照索引数组那样的增/减序列来列举

可参考[稀疏数组与密集数组](http://www.cnblogs.com/ziyunfei/archive/2012/09/16/2687165.html)

稀疏数组

<!-- language: !js -->

    var a = new Array(3);
    //当你遍历它时,会跳过这些缝隙
    a.forEach(function (x, i) { alert(i+". "+x); });//未alert
    alert(a.map(function (x, i) { return i; }));//显示""
    a[0]=0;
    a[100]=100;
    a.forEach(function (x, i) { alert(i+". "+x); });//alert两次

密集数组([Brandon Benvie](http://bbenvie.com/) 在[es-discuss邮件讨论组](http://www.ecmascript.org/community.php)中提到了一个[创建密集数组的技巧](https://mail.mozilla.org/pipermail/es-discuss/2012-April/022273.html))

<!-- language: !js -->

    var a = Array.apply(null, Array(3));
    //现在可以遍历到这些数组元素了,还可以为每个元素重新赋值:
    a.forEach(function (x, i) { alert(i+". "+x); });
    alert(a.map(function (x, i) { return i; }));

当被赋值，即使是undefined，也可被for..in和forEach遍历，否则不被遍历。

js在以下情况中隐式维护length属性：

- 使用push,pop,shift,unshift方法在数组首尾增删元素时
- 使用splice方法在数组中删除元素时

又因为js的数组是动态数组，所以具有以下特性：

- 给大于或等于length值的指定下标赋值时，会隐式的重设length属性值
- 可以显式地重写length属性来调整数组大小

js的索引数组的连续性，只是一种表现效果：如果某个下标没有存放值，取值为undefined，和访问对象不存在属性一样，正是因为这是一种表现效果，所以for语句增/减列举时，表现为索引数组，用for..in列举时，表现为关联数组

sort()排序时，undefined是不参与运算的，不管undefined是该下标没有存放值，或该下标存放值为undefined

<!-- language: !js -->

    var aArray=new Array(10000);
    aArray[1]=3;
    aArray[3]=1;
    aArray[5]=5;
    aArray[8]=undefined;//不参与sort()
    aArray[9999]=9;

    function func(v1,v2){
        alert([v1,v2]);
        return (v1>v2 ? 1 :v1==v2 ? 0:-1);
    }
    aArray.sort(func);//只有四个元素参与比较

ps:不同浏览器sort()算法实现得并不一样

### 干净的对象
干净的对象指没有声明任何成员的对象直接量，也就是通过"{}"或"new Object()"创建的对象

## 类型转换
js中数据类型并不是由变量声明来决定的，而是由它包含什么值来决定的，类型只能在代码执行过程中才能获知。

### 宿主环境下的特殊类型系统
### 值运算:类型转换的基础
引用类型的价值：

- 标识一组值数据
- 提供一组存取值数据的规则
- 在函数中传递与存储引用（标识）

所以所谓的类型转换其实是指：

- 值类型之间的转换
- 将引用类型转换为值类型（以参与运算）

### 隐式转换
#### 运算导致的类型转换
js运算过程中的隐式类型转换，既受到基于运算符的 __“类型推定”__ 的影响：

- 如一元运算符`+`和`-`所作用的类型是数值

又受到基于运算元类型的 __“运算符推定”__ 的影响：

- 如二元运算符`+`，当运算元中有任意一个是字符串时，运算符视为“字符串连接”，因为字符串连接运算将操作两个字符串，所以另一个运算符需要被隐式的转换成字符串

还有在一些函数调用时可能执行一些 __隐含的强制转换__：

- `String.search()`会将第一个参数作为正则表达式

其它类型推定

<!-- language: !js -->

    var x=[1,2,3,4];
    alert(x[1,2,3]);

`[]`是成员存取，`1,2,3`被作为表达式理解，并隐式的将结果值转换成字符串，等价于x['3']

<!-- language: !js -->

    var x=y={};
    alert(x+y);

在该例中`+`，引擎先检测其x，y对象`valueOf()`返回值的类型，随后调用它们的`toString()`方法来转换为字符串类型[参考](#TOC4.7.5)，因此变成了`'[object Object]'+'[object Object]'`

#### 语句(语义)导致的类型转换
一些语句在语义分析时也会做一些强制转换的操作，如if语句会把表达式结果转换为布尔值。在对象声明中，存在标识符到字符串的转换，如`obj={prop:100}`中的`prop`将成为字符串，因为js引擎内部，对象成员名是以字符串形式存在并访问的。

switch试图对表达式求值，并用该值与case分支中的值进行比较，采用类似`===` __全等__ 的比较，即优先进行类型检测，而 __不会发生类型转换__

<!-- language: !js -->

    var obj=new Number(2);
    switch(obj){
        case 2: alert('hello');//不会被运行到
    }

### 值类型之间的转换
#### undefined的转换
任意值都不能“转换为undefined”，但反过来却不是，`undefined`能转换为特殊数字`NaN`，因此它与数值运算结果将会是`NaN`，而不会导致异常　

<!-- language: !js -->

    var value=undefined;
    alert(value+10);

`undefined`能转换成字符串`'undefined'`与布尔值`false`，它总是恒定的得到这两个值

<!-- language: !js -->

    var value=undefined;
    if(!value) alert(value+'');

#### number的转换
任何值都可以被转换到number类型的值，如果转换得不到一个有效的数值，结果就是`NaN`，这样的目的使得表达式可能尽量的求值，而不是抛出一个异常。

number值转换到布尔值时，非零值都转换成true，零与NaN转换成false

<!-- language: !js -->

    var nums = {
        '0': 0,
        'NaN': NaN,
        'Number.NaN': Number.NaN,
        'Number.MAX_VALUE': Number.MAX_VALUE,
        'Number.MIN_VALUE': Number.MIN_VALUE,
        'Infinity': Infinity,
        'Number.POSITIVE_INFINITY': Number.POSITIVE_INFINITY,
        'Number.NEGATIVE_INFINITY': Number.NEGATIVE_INFINITY
    };

    for (var prop in nums) {
        var flag = false;
        if (nums[prop])
            flag = true;
        alert(prop + ' toBoolean is ' + flag + ', toString is ' + nums[prop]);
    }

#### boolean的转换
boolean的值true和false总是被转换成数值1和0，以下示例均显示1

<!-- language: !js -->

    var checked=true;
    var i=false;
    if(checked)i++;
    alert(i);

    i=false;
    i+=checked;
    alert(i);

    checked='TRUE';
    i=false;
    i+=!!checked;
    alert(i);

    alert(true+false);

它们转换成字符串时，总是`'true'`和`'false'`

<!-- language: !js -->

    alert(''+true);
    alert(''+false);

#### string的转换
如果一个字符串是一个合法的数值时，那么它总能转换为数值，这种情况下，它被当作 __十进制__ 处理，注意这里只有四则运算中的`-*/`

<!-- language: !js -->

    alert('0022'-1);
    alert('00.22'*10);
    alert('.22'*'100.');

    alert('0xF'*20);//16进制
    alert('033'-20);//10进制，非8进制
    alert(033-20);//8进制

尽管boolean值会转换成`'true'`和`'false'`，但反过来却不行，只有空字符串会转换为false，其它任何字符串转换boolean均为true

#### 值类型数据的显式转换
`parseInt()`和`parseFloat()`，即使字符串中只有前缀部分能被转换，那么该转换也将成功进行，方便web编程中，遇到css属性为px时，简单使用`parseFloat()`去清除它

### 从引用到值:探究valueOf()方法
如果一个数据是 __引用类型__，并且该数据需要进行值运算，js引擎将先调用它(或经过 __包装后__ 的对象)的`valueOf()`方法求值，然后以该值参与运算

<!-- language: table -->

    |对象实例|valueOf()返回值|备注                                |
    |--------|---------------|------------------------------------|
    |Boolean |布尔值         |                                    |
    |Number  |数字值         |                                    |
    |String  |字符串值       |                                    |
    |Array   |字符串值       |与Array.toString和Array.join方法相同|
    |Date    |数字值         |从1970-1-1 0:0:0开始计的毫秒数      |
    |Function|函数本身       |引用类型                            |
    |Object  |对象本身       |引用类型                            |
    |RegExp  |正则对象本身   |引用类型                            |
    |Error   |错误对象本身   |引用类型                            |

我们重写`valueOf()`来捕获这一运算过程，然后显示运算后的值

<!-- language: !js -->

    void function () {
        _valueOf=Boolean.prototype.valueOf;
        Boolean.prototype.valueOf=function(){
            alert(typeof this);//Object
            return _valueOf.call(this);
        };
    }();

    var obj=new Boolean(true);//它的valueOf()被重写了
    var v=false;//值类型不会调valueOf()

    alert(v+obj);//1

但是仍有一部分引用类型数据，不能过通它的`valueOf()`得到一个有效的值，如表格中`Object,Error,RegExp,Function`，在这种情况下，__引擎 _会再次_ 调用`toString()`方法(自动调用，无须人工参与)以取得一个字符串值__，字符串值总是可以以值类型形式参与运算的，这样就保证了js内部任意数据总可以直接参与值运算。

<!-- language: !js -->

    void function () {
        _valueOf=Boolean.prototype.valueOf;
        _toString=Boolean.prototype.toString;
        Boolean.prototype.valueOf=function(){
            alert("do valueOf...");
            return {}; //返回一个引用类型,导致toString会被调用
        };
        Boolean.prototype.toString=function(){
            alert("do toString...");
            return _toString.call(this);
        };
    }();

    var obj=new Boolean(true);
    var v=false;

    alert(v+obj);//'falsetrue'

变量v是被隐式转换成字符串'false'，值类型在类型转换中并不会调用`toString()`或`valueOf()`方法，也不会为此进行包装类操作。

### 到字符串类型的显式转换
- 如果数据是值类型，则直接参与值运算；否则，
- 如果`valueOf()`返回一个值类型数据，则以该数据参与值运算；否则，
- 使用`toString()`返回的字符串参与运算

#### 重写toString()方法
string对象也有`toString()`方法，也能被重写，到底是string对象值还是`toString()`值存在运算意义呢，根据前面规则，由于string对象的`valueOf()`值返回一个值类型的字符串，因此我们应该明确运算过程中并不需要调用`toString()`方法

<!-- language: !js -->

    String.prototype.toString=function(){
        return 'hi';
    };
    var str=new String('hello');
    alert(str+' world');//'hello world'

我们要让`toString()`生效，比较正确的方法就是让`valueOf()`返回 __引用__ 而不是值：

<!-- language: !js -->

    String.prototype.valueOf=function(){
        return this;
    };
    String.prototype.toString=function(){
        return 'hi';
    };
    var str=new String('hello');
    alert(str+' world');//'hi world'


#### 从数值到字符串的显式转换
`aNumber.toString([radix])`，其中radix表示进制：

<!-- language: !js -->

    var value=123456;
    alert(value.toString(16));//16进制
    alert(value.toString(8));//8进制

#### 其他类型的显式转换
在js各对象中，存在一组以"to"为前缀的方法，这些方法均用于转换为字会串：

- Date: `toGMTString()`,`toUTCString()`,`toDateString()`,`toTimeString()`,`toLocalDateString()`,`toLocalTimeString()`
- String: `toUpperCase()`,`toLowerCase()`,`toLocalUpperCase()`,`toLocalLowerCase()`
- Number: `toFixed()`,`toExponential()`,`toPrecision()`
- Object,Array,Date,Number: `toLocalString()`













