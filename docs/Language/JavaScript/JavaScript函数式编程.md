> 2015-10-18

- 摘自《Functional JavaScript》

简介
==========

传入`splat`(它返回的函数)的数组的元素是函数`fun`的参数，即将参数展开了

<!-- run -->

```js
function splat(fun){
  return function(array){
    return fun.apply(null, array);
  };
}

var addArrayElements = splat(function(x, y) {return x + y;});

console.log(addArrayElements([1, 2]));
```

以任意多个参数来调用任意一个函数，代替原始函数需要传入一个数组

<!-- run -->

```js
//= require underscore.1.6.0
function unsplat(fun){
  return function(array){
    return fun.call(null, _.toArray(arguments));
  };
}

var joinElements = unsplat(function(array) {return array.join(' ');});

console.log(joinElements(1, 2));
```

## 以函数为抽象单位
- 函数式编程通过使用函数来将值转换成 __抽象单元__，接着用于构建软件系统
- 函数式将问题分解为多组“动词”或函数，通过“黏合”或“组合”其他函数的方式来构建更大的函数，以实现更加抽象的行为
- 函数式系统努力减少可见的状态修改（不同于面向对象），向一个遵循函数式原则的系统添加新功能就成了理解如何在存在局限的上下文环境中，无破坏性的数据转换民，来实现新的函数


## 以函数为行为单位
排序`Array#sort`需要一个比较器

<!-- run -->

```js
var arr = [2, 3, -1, -6, 0, -108, 42, 10].sort(function(x,y) {
  if (x < y) return -1;
  if (y < x) return  1;
  return 0;
});

console.log(arr);
```

抽象出`compareLessThanOrEqual`

```js
function compareLessThanOrEqual(x, y) {
  if (x < y) return -1;
  if (y < x) return  1;
  return 0;
}
```

但问题是，它被耦合了“比较器”的概念当中，__并不容易被 *单独* 当作一个通用的比较器__ 来用：

<!-- run -->

```js
function compareLessThanOrEqual(x, y) {
  if (x < y) return -1;
  if (y < x) return  1;
  return 0;
}

if (compareLessThanOrEqual(1,1))
  console.log("less or equal");

// nothing prints
```

一个比较好的实现是`lessOrEqual`返回布尔值，但`sort`需要的是一个比较器，就需要一个转换函数`comparator`，它接收一个谓词，并将结果转化成期望的-1/0/1

<!-- run -->

```js
function existy(x) { return x != null };
function truthy(x) { return (x !== false) && existy(x) };

function lessOrEqual(x, y) {
  return x <= y;
}

function comparator(pred) {
  return function(x, y) {
    if (truthy(pred(x, y)))
      return -1;
    else if (truthy(pred(y, x)))
      return 1;
    else
      return 0;
  };
}

var arr = [2, 3, -1, -6, 0, -108, 42, 10].sort(comparator(lessOrEqual));

console.log(arr);
```

`lessOrEqual`就成为了 __更通用、更单一__ 的函数了(可测性，重用性都得到提高)，将来的`greaterThan`,`isPrime`,`isOdd`等均可以通过`comparator`转换为`sort`所需的比较器


## 数据抽象
很多时候一个应用的数据需求比类中简单得多，并不需要利用原型或闭包建立一个基于类的对象系统。函数式编程已经致力于构建能够实现更高层次行为以及能够工作在非常 __简单的数据结构上的函数__，javascript中的数组和对象，这两个简单数据类型的灵活性是惊人的

<!-- run -->

```js
//= require underscore.1.6.0
//tools
function isIndexed(data) {
  return _.isArray(data) || _.isString(data);
}

function nth(a, index) {
  if (!_.isNumber(index)) fail("Expected a number as the index");
  if (!isIndexed(a)) fail("Not supported on non-indexed type");
  if ((index < 0) || (index > a.length - 1))
    fail("Index value is out of bounds");
  return a[index];
}

function second(a) {
  return nth(a, 1);
}

//logic
function lameCSV(str) {
  return _.reduce(str.split("\n"), function(table, row) {
    table.push(_.map(row.split(","), function(c) { return c.trim()}));
    return table;
  }, []);
}

function selectNames(table) {
  return _.rest(_.map(table, _.first));
}

function selectAges(table) {
  return _.rest(_.map(table, second));
}

function selectHairColor(table) {
  return _.rest(_.map(table, function(row) {
    return nth(row, 2);
  }));
}
var mergeResults = _.zip;

//app
var peopleTable = lameCSV("name,age,hair\nMerble,35,red\nBob,64,blonde");
console.log(peopleTable);
//=> [["name",  "age",  "hair"],
//    ["Merble", "35",  "red"],
//    ["Bob",    "64",  "blonde"]]

console.log(selectNames(peopleTable));
//=> ["Merble", "Bob"]

console.log(selectAges(peopleTable));
//=> ["35", "64"]

console.log(selectHairColor(peopleTable));
//=> ["red", "blonde"]

console.log(mergeResults(selectNames(peopleTable), selectAges(peopleTable)));
//=> [["Merble", "35"], ["Bob", "64"]]
```

以处理集合为中心的函数式方式更适合处理与人有关的数据，将自己约束在对简单数据的操作上，增加了灵活性


一等函数与Applicative编程
=========================
## 函数是一等公民
- 可以存储为变量
- 可以存储为数组中的一个元素
- 可以作为对象的成员变量
- 可以在使用时直接创建出来
- 可以作为参数
- 可以作为返回值

99瓶啤酒的程序，命令式解决

<!-- run -->

```js
//= require fake
var lyrics = [];
for (var bottles = 99; bottles > 0; bottles--) {
  lyrics.push(bottles + " bottles of beer on the wall");
  lyrics.push(bottles + " bottles of beer");
  lyrics.push("Take one down, pass it around");
  if (bottles > 1) {
    lyrics.push((bottles - 1) + " bottles of beer on the wall.");
  }
  else {
    lyrics.push("No more bottles of beer on the wall!");
  }
}
console.log(lyrics);
```

以函数式解决

<!-- run -->

```js
//= require underscore.1.6.0
//= require fake

function lyricSegment(n) {
  return _.chain([])
    .push(n + " bottles of beer on the wall")
    .push(n + " bottles of beer")
    .push("Take one down, pass it around")
    .tap(function(lyrics) {
      if (n > 1)
        lyrics.push((n - 1) + " bottles of beer on the wall.");
      else
        lyrics.push("No more bottles of beer on the wall!");
     })
    .value();
}

console.log(lyricSegment(9));

function song(start, end, lyricGen) {
  return _.reduce(_.range(start,end,-1),
    function(acc,n) {
      return acc.concat(lyricGen(n));
    }, []);
}

console.log(song(99, 0, lyricSegment));
```

`lyricSegment`做很少的事情，只产生给定数量的单一歌词，从一般的歌词装配过程中分离出逻辑区域，并将多个抽象的函数再组装回去

## Applicative编程

### 集合中心编程

<!-- run -->

```js
//= require underscore.1.6.0
//= require fake

var result;
result = _.map({a: 1, b: 2}, _.identity);
console.log(result);
//=> [1,2]

result = _.map({a: 1, b: 2}, function(v,k) {
  return [k,v];
});
console.log(result);
//=> [['a', 1], ['b', 2]]

result = _.map({a: 1, b: 2}, function(v,k,self) {
  return [k, v, _.keys(self)];
});
console.log(result);
//=> [['a', 1, ['a', 'b']], ['b', 2, ['a', 'b']]]
```

首位图灵奖获得者Alan Perlis：

> 用100个函数操作一个数据结构，比用10个函数操作10个数据结构要好

### 定义几个Applicative函数
- 定义一个函数，让它接收一个函数，然后调用它

一个简单的、接受一定数量的参数并连接它们的函数并不是applicative

<!-- run -->

```js
//= require underscore.1.6.0
//= require fake
function existy(x) { return x != null };

function cat() {
  var head = _.first(arguments);
  if (existy(head))
    return head.concat.apply(head, _.rest(arguments));
  else
    return [];
}
console.log(cat([1,2,3], [4,5], [6,7,8]));

function construct(head, tail) {
  return cat([head], _.toArray(tail));
}
console.log(construct(42, [1,2,3]));
//=> [42, 1, 2, 3]
```

`cat`不能指望接受任何函数作为参数，虽然`construct`中用到了它，但它将未将`cat`作为参数传入，不符合applicative的要求。

定义一个`mycat`符合applicative

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake
function mapcat(fun, coll) {
  return cat.apply(null, _.map(coll, fun));
}

var result = mapcat(function(e) {
  return construct(e, [","]);
}, [1,2,3]);
console.log(result);
//=> [1, ",", 2, ",", 3, ","]
```

`mapcat`接收一个函数参数（是applicative的），使用`map`对给定集合中的每个元素进行一个`fun`的调用，然后再被`cat`，它串联`map`结果中所有元素。

这样串连的结果末尾了一个`,`，需要处理


<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake
function mapcat(fun, coll) {
  return cat.apply(null, _.map(coll, fun));
}

function butLast(coll) {
  return _.toArray(coll).slice(0, -1);
}

function interpose(inter, coll) {
  return butLast(mapcat(function(e) {
    return construct(e, [inter]);
  },
  coll));
}

var result = interpose(",", [1,2,3]);
console.log(result);
//=> [1, ",", 2, ",", 3]
```

用较低级别的函数来逐步定义和使用离散功能，一串函数链一个接一个地调用，每一个函数将逐渐转变的结果传递到后一个函数，最后得到解决方案

## 数据思考
`_.keys`, `_.values`,  `_.pluck`提供了对数据映射的操作，`_.pairs`接收一个对象，变成嵌套数组，`_.object`再次还原成对象

### Table-Like数据

```table
title          |isbn        |ed
---------------|------------|------
SICP           | 0262010771 | 1
SICP           | 0262510871 | 2
Joy of Clojure | 1935182641 | 1
```

每一行相当于一个javascript对象，每一格是对象的一个键/值对，假设需要执行SQL`SELECT title FROM library`对应的是

<!-- run -->

```js
//= require underscore.1.6.0
//= require fake

var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

var result = _.pluck(library, 'title');
console.log(result);
//=> ["SICP", "SICP", "Joy of Clojure"]
```

问题在于该结果不同于表的抽象（返回的不是对象数组，只是字符串数组），不利于结果集再次被类似SQL语句的执行，函数`project`作为`SELECT`的替身

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

function project(table, keys) {
  return _.map(table, function(obj) {
    return _.pick.apply(null, construct(obj, keys));
  });
}

var editionResults = project(library, ['title', 'isbn']);
console.log(editionResults);

var isbnResults = project(editionResults, ['isbn']);
console.log(isbnResults);
//=> [{isbn: "0262010771"},{isbn: "0262510871"},{isbn: "1935182641"}]
```

`project`函数用`_.pick`作用在数组中的每个对象，并选出匹配白名单的对象，从而保持了抽象的表，它返回的结果，还可以用`project`做进一步处理得到`isbnResults`

最后，才可以有意的选择所需的数据来打破抽象

```js
_.pluck(isbnResults, 'isbn');
//=> ["0262010771", "0262510871", "1935182641"]
```

函数式程序员会深入思考应用的数据，以及每一层交接的格式，想象高层次的以数据为中心的思维。

`SELECT ed AS edition FROM library;`对应的`as`，但是之前先实现一个工具函数`rename`

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

function rename(obj, newNames) {
  return _.reduce(newNames, function(o, nu, old) {
                                   /*memo,val,key*/
    if (_.has(obj, old)) {
      o[nu] = obj[old];
      return o;
    }
    else
      return o;
  },
  _.omit.apply(null, construct(obj, _.keys(newNames))));
}

var result = rename({a: 1, b: 2}, {'a': 'AAA'});
console.log(result);
//=> {AAA: 1, b: 2}
```

接下来实现`as`


<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

function as(table, newNames) {
  return _.map(table, function(obj) {
    return rename(obj, newNames);
  });
}

var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

var result = as(library, {ed: 'edition'});
console.log(result);
//=> [{title: "SICP", isbn: "0262010771", edition: 1},
//    {title: "SICP", isbn: "0262510871", edition: 2},
//    {title: "Joy of Clojure", isbn: "1935182641", edition: 1}]
```

无论`as`还是`project`都操作同一抽象，可以将调用连在一起，`SELECT ed AS edition FROM library;`的对应语句如下：

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake
var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

var result = project(as(library, {ed: 'edition'}), ['edition']);
console.log(result);
//=> [{edition: 1}, {edition: 2}, {edition: 1}];
```

实现一个类似SQL`WHERE`子句，函数为`restrict`

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

function restrict(table, pred) {
  return _.reduce(table, function(newTable, obj) {
    if (truthy(pred(obj)))
      return newTable;
    else
      return _.without(newTable, obj);
  }, table);
}

var result = restrict(library, function(book) {
  return book.ed > 1;
});
console.log(result);
//=> [{title: "SICP", isbn: "0262510871", ed: 2}]
```

像其它表抽象一样，`restrict`可以被链接

```sql
SELECT title, isbn, edition FROM (
  SELECT ed AS edition FROM library
) EDS
WHERE edition > 1;
```

等价于

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

var library = [
  {title: 'SICP',            isbn: '0262010771', ed: 1},
  {title: 'SICP',            isbn: '0262510871', ed: 2},
  {title: 'Joy of Clojure',  isbn: '1935182641', ed: 1}
];

var result = restrict(
  project(
    as(library, {ed: 'edition'}),
    ['title', 'isbn', 'edition']),
  function(book) {
    return book.edition > 1;
});
console.log(result);
//=> [{title: "SICP", isbn: "0262510871", edition: 2},]
```

变量的作用域和闭包
=================
__绑定（binding）__ 指通过`var`关键字、函数参数、`this`传递、或属性分配给javascript中的 __值分配名字__ 的行为

## 词法作用域
指一个变量的可见性，及其文本表述的模拟值，简单情况下，变量的查找开始于最接近的绑定上下文而向外扩展，直到找到第一个绑定

## 动态作用域
很少语言使用动态作用域作为绑定解析方案，模拟一个原生的动态作用域机制只需非常少代码

<!-- run -->

```js
//= require underscore.1.6.0
//= require fake
var globals = {};

function makeBindFun(resolver) {
  return function(k, v) {
    var stack = globals[k] || [];
    globals[k] = resolver(stack, v);
    return globals;
  };
}

var stackBinder = makeBindFun(function(stack, v) {
  stack.push(v);
  return stack;
});

var stackUnbinder = makeBindFun(function(stack) {
  stack.pop();
  return stack;
});

var dynamicLookup = function(k) {
  var slot = globals[k] || [];
  return _.last(slot);
};

stackBinder('a', 1);
stackBinder('b', 100);
console.log(dynamicLookup('a'));
//=> 1

console.log(globals);
//=> {'a': [1], 'b': [100]}
```

在动态作用域的方案中，在栈顶的值是绑定的当前值，如果再绑定一次

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake
stackBinder('a', 1);
stackBinder('b', 100);

stackBinder('a', '*');

console.log(dynamicLookup('a'));
//=> '*'

console.log(globals);
//=> {'a': [1, '*'], 'b': [100]}
```

动态作用域缺点：任何给定的绑定的值，在确定调用其函数之前，都是不可知的。

上述代码中，不得不显式的“取消”动态绑定，而在支持动态绑定的语言中，这个任务在关闭动态绑定的上下文中自动完成

## 闭包
闭包能够捕获作用域内的外部绑定（如，不是自己的参数），这些绑定是为之后使用（即使在该作用域已结束）

闭包背后的基本原理是，如果一个函数包含内部函数，那么它些内部函数都可以看到该函数声明的变量，这些变量称为自由变量，可以被内部函数捕获，从高阶函数的`return`实现“越狱”，以供以后使用





<hr/>

<!-- run -->

```js
//= require underscore.1.6.0
//= require functional_study
//= require fake

```

<!--

http://microjs.com/
http://www.css88.com/doc/underscore

 -->




