> 2014-11-01

> 摘自《Async JavaScript》

异步事件
=========
## 事件的调度
如果想让JavaScript中的某段代码将来再运行，可以将它放在回调中。回调就是一种普通函数，只不过它是传给像`setTimeout`这样的函数，或者绑定为像`document.onready`这样的属性。运行回调时，我们称已触发某事件（譬如延时结束或页面加载完毕）。

### 现在还是将来运行
```!js
for (var i = 1; i <= 3; i++) {
  setTimeout(function(){ console.log(i); }, 0);
}
```
要理解为什么输出是4，4，4，需要知道以下3件事：

- 这里只有一个名为i的变量，其 __作用域__ 由声明语句`var i`定义（该声明语句在不经意间让`i`的作用域不是循环内部，而是扩散至蕴含循环的那个最内侧函数）。
- `i`一直递增，直到不再满足条件`i<=3`为止，循环结束后`i===4`
- JavaScript事件处理器在 __线程空闲之前__ 不会运行。

### 线程的阻塞
```!js
var start = new Date;
setTimeout(function(){
  var end = new Date;
  console.log('Time elapsed:', end - start, 'ms');
}, 500);
while (new Date - start < 1000) {};
```
按照多线程的思维定势，我会预计500毫秒后计时函数就会运行。不过这要求中断欲持续整整一秒钟的循环。如果运行代码，会得到类似这样的结果：`Time elapsed: 1021ms`，这个数字肯定至少是1000，因为`setTimeout`回调在`while`循环结束运行之前 __不__ 可能被触发。

### 队列
调用`setTimeout`的时候，会有一个延时事件排入队列。然后`setTimeout`调用之后的那行代码运行，接着是再下一行代码，直到再也没有任何代码。

用户单击一个已附加有单击事件处理器的`DOM`元素时，会有一个单击事件排入队列。但是，该单击事件处理器要等到当前所有正在运行的代码 __均已结束后__（可能还要等其他此前已排队的事件也依次结束）才会执行。因此，使用JavaScript的那些网页一不小心就会变得毫无反应。

这隐含着一个意思，即触发的每个事件都会位于堆栈轨迹的底部。事件的易调度性是JavaScript语言最大的特色之一。像`setTimeout`这样的异步函数只是简单地做延迟执行，而不是孵化新的线程。JavaScript 代码永远不会被中断，这是因为代码在运行期间只需要排队事件即可，而这些事件在代码运行结束之前不会被触发。

## 异步函数的类型
### 异步的IO函数
创造Node.js，并不是为了人们能在服务器上运行JavaScript，仅仅是因为Ryan Dahl想要一个建立在某高级语言之上的 __事件驱动型__ 服务器框架。JavaScript碰巧就是适合干这个的语言。为什么？因为JavaScript语言可以完美地实现非阻塞式I/O。

在其他语言中，一不小心就会“阻塞”应用（通常是运行循环）直到完成I/O请求为止。而在JavaScript中，这种阻塞方式几乎沦为无稽之谈。相反，我们需要附加一个事件处理器，随即返回事件队列。不论是在等待用户的按键行为，还是在等待远程服务器的批量数据，所需要做的就是 __定义一个回调__ (除非JavaScript环境提供的某个同步I/O函数已经替我们完成了阻塞，如Ajax方法有一个可设置为`false`的`async`选项，在Node.js中，同步的API方法在名称上会有明确的标示，譬如`fs.readFileSync`)

在现代浏览器中操纵DOM对象时，从脚本角度看，更改是即时生效的，但从视效角度看，在返回事件队列之前不会渲染这些DOM对象更改。这可以防止DOM对象被渲染成不一致的状态。

JavaScript采用了非阻塞式I/O，这对新手来说是最大的一个障碍，但这同样也是该语言的核心优势之一。有了非阻塞式I/O，就能自然而然地写出高效的基于事件的代码。

### 异步的计时函数
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


## 异步函数的编写
JavaScript 中的每个异步函数都构建在其他某个或某些异步函数之上。凡是异步函数，从上到下（一直到原生代码）都是异步的！任何函数只要使用了异步的函数，就必须以异步的方式给出其操作结果。

### 何时称函数为异步的
如果称一个函数为“异步的”，其意思是这个函数会导致 __将来__ 再运行另一个函数，后者取自于事件队列（若后面这个函数是作为参数传递给前者的，则称其为回调函数，简称为 __回调__）。

异步函数还涉及另一个术语，即 __非阻塞__。非阻塞这个词强调了异步函数的 __高速度__：异步MySQL数据库驱动程序做一个查询可能要花上一小时，但负责发送查询请求的那个函数却能以微秒级速度返回。这对于那些需要快速处理海量请求的网站服务器来说，绝对是个福音。

遗憾的是，要想确认某个函数异步与否，唯一的方法就是审查其源代码。有些同步函数却拥有看起来像是异步的API，这或者是因为它们将来可能会变成异步的，又或者是因为回调这种形式能 __方便地返回多个参数__。

### 间或异步的函数
有些函数某些时候是异步的，但其他时候却不然。举个例子，jQuery的同名函数（通常记作`$`）可用于延迟函数直至DOM已经结束加载。但是，若DOM早已结束了加载，则不存在任何延迟，`$`的回调将会立即触发。

### 缓存型异步函数
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

### 异步递归与回调存储
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

### 返回与回调的混搭
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


## 异步错误的处理
### 回调内抛出的错误
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

### 未捕获异常的处理
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

### 抛出还是不抛出
在任的Node开发负责人）就主张`try/catch`是一种“反模式”的方式，是包装着漂亮花括弧的`goto`语句。

如果想让整个应用停止工作，请勇往直前地大胆使用`throw`。否则，请认真考虑一下应该如何处理错误。是想给用户显示一条出错消息吗？是想重试请求吗？那就这么处理吧，只是请尽可能地靠近错误源头。

## 嵌套式回调的解嵌套
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

分布式事件
=========
## PubSub模式
Publish/Subscribe，意为“发布/订阅”，在其帮助下，我们能 __解嵌套那些嵌套式回调__，减少重复冗余，最终编写出易于理解的事件驱动型代码。

从软件架构的角度看，jQuery将link元素的事件发布给了任何想订阅此事件的人。这正是称其为PubSub模式的原因

Node的API架构师因为太喜欢PubSub，所以决定包含一个一般性的PubSub实体。这个实体叫做`EventEmitter`（事件发生器），其他对象可以继承它。Node中几乎所有的I/O源都是`EventEmitter`对象：文件流、HTTP服务器，甚至是应用进程本身。

很多MVC框架，如Backbone.js和Spine，都提供了自己的`类EventEmitter`模块

### `EventEmitter`对象
Node的`EventEmitter`对象作为PubSub 接口的例子。`EventEmitter`有着简单而近乎最简化的设计。

```js
emitter.on('evacuate', function(message) {
  console.log(message);
});

emitter.emit('evacuate');
emitter.emit('evacuate', 'Woman and children first!');
```

`EventEmitter`对象的所有方法都是公有的，但一般约定只能从`EventEmitter`对象的“内部”触发事件。也就是说，如果有一个对象继承了`EventEmitter`原型并使用了`this.emit`方法来广播事件，则不应该从这个对象之外的其他地方再调用其`emit`方法。

### 自己设计PubSub
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

### 同步性
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

## 事件化模型
只要对象带有PubSub接口，就可以称之为事件化对象。特殊情况出现在用于存储数据的对象因内容变化而发布事件时，这里用于存储数据的对象又称作模型。模型就是MVC中的那个M，老式的JavaScript 靠输入事件的处理器直接改变DOM。新式的JavaScript先改变模型，接着由模型触发事件而导致DOM的更新。在几乎所有的应用程序中，这种关注层面的分离都会带来更优雅、更直观的代码。

### 模型事件的传播
作为最简形式，MVC三层架构只包括相互联系的模型和视图：“如果模型是这样变化的，那么DOM就要那样变化。”不过，MVC三层架构最大的利好出现在change（变化）事件冒泡上溯数据树的时候。不用再去订阅数据树每片叶子上发生的事件，而只需订阅数据树根和枝处发生的事件即可。

为此，Backbone的Model对象常常组织成Backbone集合（Collection）的形式，其本质是事件化数组。我们可以监听什么时候对这些 __数组增减__ 了Model对象。Backbone集合可以自动传播其内蕴Model对象所发生的事件。

### 事件循环与嵌套式变化
如果每次有个对象上的事件引发了一系列事件并最终对这个对象本身触发了相同的事件，则结果就是事件循环。如果这种事件循环还是同步的，那就造成了堆栈上溢。

Backbone中的两道保险：

- 当新值等于旧值时，`set`方法不会导致触发`change`事件。
- 模型正处于自身的`change`事件期间时，不会再触发`change`事件。

第二道保险代表了一种自保哲学。假设模型的一个变化导致同一个模型又一次变化。由于第二次变化被“嵌套”在第一次变化内部，所以这次变化的发生悄无声息。外面的观察者没有机会回应这种静默的变化。

而另一个重要的MVC框架，即Ember.js，采用了一种完全不同的方式：双向绑定必须作显式声明。一个值发生变化时，另一个值会通过延时事件作异步更新。于是，在触发这个异步更新事件之前，应用程序的数据将一直处于不一致的状态。

多个事件化模型之间的数据绑定问题不存在简单的解决方案。在Backbone中，有一种审慎绕过这个问题的途径就是`silent`标志。如果在`set`方法中添加了`{silent:true}`选项，则不会触发`change`事件。因此，如果多个彼此纠结的模型需要同时进行更新，一个很好的解决方法就是悄无声息地设置它们的值。然后，当这些模型的 __状态已经一致__ 时，才调用它们的`change`方法以触发对应的事件。

## jQuery自定义事件
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

`Promise`和`Deferred`
=========
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

## 生成`Promise`对象

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

### 纯`Promise`对象
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

### jQuery中的Promise对象
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

### 阮一峰文摘

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

## 向回调传递数据
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

## 进度通知
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

## Promise对象的合并
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

### 函数的Promise用法
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

## 管道连接未来
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

### 管道级联技术
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

## jQuery与Promises/A的对比
Q.js 库是最流行的Promises/A 实现，其提供的方法甚至能与jQuery 的Promise和谐共存。这两者的区别只是形式上的，即用相同的词语表示不同的含义。

3.2节中提到过，jQuery 使用resolve 作为fail 的反义词，而Promises/A使用的是fulfill。在 Promises/A 规范中，Promise对象不管是已履行还是已失败，都称“已执行”。

在jQuery 1.8 问世之前，jQuery 的then 方法只是一种可以同时调用done、fail 和progress 这3 种回调的速写法，而Promises/A 的then在行为上更像是jQuery 的pipe。jQuery 1.8 订正了这个问题，使得then 成为pipe 的同义词。

当然还有其他一些细微的差别。例如，在Promises/A 规范中，由then方法生成的Promise对象是已执行还是已拒绝，取决于由then 方法调用的那个回调是返回值还是抛出错误。（在jQuery 的Promise对象的回调中抛出错误是个糟糕的主意，因为错误不会被捕获。）

## 用Promise对象代替回调函数
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

Async.js的工作流控制
=========
PubSub这种抽象设计允许应用程序把来源层的事件发布至其他层（譬如，在 MVC三层架构设计模式中把从视图层的事件发布至模型层）。Promise这种抽象设计允许将简单任务表示成对象，进而合并这些对象来表示更复杂的任务。总之，这些抽象设计一直在致力于帮助我们 __解决意面式回调__ 这一问题。

不过我们的武器库仍然有一处短板：迭代。假设需要执行一组 I/O 操作（或者并行执行，或者串行执行），该怎么做呢？这个问题在Node中非常常见，以至于有了个专有名称：工作流控制（也称作控制工作流）。就像 Underscore.js 可以大幅度简化同步代码中的迭代一样，优秀的工作流控制库也可以消解异步代码中的套话。

## 异步工作流的次序问题
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

## 异步的数据收集方法
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

### Async.js 的函数式写法
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

### Async.js的错误处理技术
Async.js遵守Node的约定。这意味着所有的 I/O 回调都形如callback(err, results...) ，唯一的例外是结果为布尔型的回调。布尔型回调的写法就是 callback(result) ，所以上一代码示例中的isFilename迭代器需要自己亲自处理错误。

要怪就怪Node的fs.exists 首开这一先河吧！而这也意味着使用了Async.js数据收集方法（filter/filterSeries 、reject/rejectSeries 、detect / detectSeries、some、every 等）的迭代器均无法报告错误。

对于非布尔型的所有Async.js迭代器，传递非 null/ undefined 的值作为迭代器回调的首参数将会立即因该错误值而调用完工回调。这正是readAndConcat 不用throw也能工作的原因。

所以，如果callback(err) 确实是在readAndConcat 中被调用的，则这个err 会传递给完工回调（即 onComplete）。Async.js只负责保证onComplete只被调用一次，而不管是因首次出错而调用，还是因成功完成所有操作而调用。

Node的错误处理约定对Async.js 数据收集方法而言也许并不理想，但对于Async.js的６所有其他方法而言，遵守这些约定可以让错误干净利落地从各个任务流向完工回调。

## Async.js的任务组织技术
Async.js 的数据收集方法解决了一个异步函数如何运用于一个数据集的问题。但如果是一个函数集而不是一个数据集，又该怎么办呢？下面将探讨Async.js 中一些可以派发异步函数并收集其结果的强大工具。

### 异步函数序列的运行
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

### 异步函数的并行运行
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

## 异步工作流的动态排队技术
大多数情况下，前两节介绍的那些简单方法足以解决我们的异步窘境，但async.series和async.parallel 均存在各自的局限性。

- 任务列表是静态的。一旦调用了async.series 或async.parallel，就再也不能增减任务了。
- 不可能问：“已经完成多少任务了？”任务处于黑箱状态，除非我们自行从任务内部派发更新信息。
- 只有两个选择，要么是完全没有并发性，要么是不受限制的并发性。这对文件I/O 任务可是个大问题。如果要操作上千个文件，当然不
想因按顺序操作而效率低下，但如果试着并行执行所有操作，又很
可能会激怒操作系统。

Async.js提供了一种可以解决上述所有问题的全能方法：async.queue。

### 深入理解队列
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

### 任务的入列
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

### 完工事件的处理
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

### 队列的高级回调方法
尽管drain常常是我们唯一要用到的事件处理器，但async.queue还是提供了其他一些事件及其处理器。

- 队末任务开始运行时，会调用队列的 empty方法。（队末任务运行结束时，会调用队列的drain方法。）
- 达到并发度的上限时，会调用队列的saturated 方法。
- 如果提供了一个函数作为push 方法的次参数，则在结束运行给定任务时会调用该函数，或在给定任务列表中的每个任务结束运行时均调用一次该函数。

## 极简主义者Step的工作流控制
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

worker对象的多线程技术
=========
应用程序的主线程可以这样跟worker 说：“ 去，开一个单独的线程来运行这段代码。”worker 可以给我们发送消息（反之亦可），其表现形式是事件队列中运行的回调（还能是别的吗？）。简而言之，与不同线程进行交互的方式与在JavaScript中进行I/O 操作一模一样。

## 网页版worker对象
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

### 网页版worker 对象的局限性
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

## cluster带来的Node版worker
略

延迟执行
=========
## delay
`delay(function, wait, [*arguments])`

类似setTimeout，等待wait毫秒后调用function。如果传递可选的参数arguments，当函数function执行时， arguments 会作为参数传入。

<!--language: !js-->

    function delay(func, wait) {
        var args = Array.prototype.slice.call(arguments, 2);
        return setTimeout(function(){ return func.apply(null, args); }, wait);
    }

    delay(alert, 1000, 'logged later');

## defer
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

## throttle
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


## debounce
`debounce(function, wait)`

debounce和throttle很像，debounce是空闲时间必须大于或等于 一定值的时候，才会执行调用方法。`debounce`返回 function 函数的防反跳版本, 将延迟函数的执行(真正的执行)在 __函数最后一次调用时刻的 `wait` 毫秒之后__。

对于必须在一些输入（多是一些用户操作）停止到达之后执行的行为有帮助。

> 1. 渲染一个Markdown格式的评论预览, 当窗口停止改变大小之后重新计算布局
> 1. 在类似不小心点了提交按钮两下而提交了两次的情况下很有用，或防止Ajax在很短时间内的多次请求。
> 1. 比如我们做autocomplete，这时需要我们很好的控制输入文字时调用方法时间间隔。一般时第一个输入的字符马上开始调用，根据一定的时间间隔重复调用执行的方法。对于变态的输入，比如按住某一个建不放的时候特别有用

underscore.js有对throttle和debounce的封装。jQuery也有一个throttle和debounce的插件：jQuery throttle / debounce

以下摘自underscore.js 1.2，最新版本有变化，[underscore.js更详细的演进](http://chinapub.duapp.com/gen_md?src=https%3A%2F%2Fgitcafe.com%2Fzyxstar%2Fmd_note%2Fraw%2Fmaster%2Fdocs%2FFramework%2FUnderscore_throttle.md)：

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

## once
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

## after
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


反应型编程(FRP)
=========

