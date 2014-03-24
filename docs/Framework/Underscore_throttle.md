> 2014-03-20

underscore.1.2.0
=================
`_.throttle(func, wait)`的最原始代码出现在underscore 1.2.0的版本中

它创建并返回一个像节流阀一样的函数，当重复调用函数的时候，__最多每隔 wait毫秒调用一次该函数__。 对于想控制一些触发频率较高的事件有帮助。再说的通俗一点就是函数调用的频度控制器，是连续执行时间间隔控制。主要应用的场景比如：

> 1. 鼠标移动，mousemove 事件
> 1. DOM 元素动态定位，window对象的resize和scroll 事件

## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var timeout;
      return function() {
        var context = this, args = arguments;
        var throttler = function() {
          timeout = null;
          func.apply(context, args);
        };
        if (!timeout) timeout = setTimeout(throttler, wait);
      };
    };

## test case

<!--language: !js-->

    //import underscore.1.2.0
    //import json2
    //import simpletest
    function test(){
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr(); throttledIncr(); throttledIncr();
        setTimeout(throttledIncr, 120);
        setTimeout(throttledIncr, 140);
        setTimeout(throttledIncr, 220);
        setTimeout(throttledIncr, 240);
        _.delay(function(){ equal(counter, 3, "incr was throttled");}, 400);
    }
    test();


underscore.1.2.1
=================
设`throttle`调用后返回的函数对象`f`，现在调用`f`的时候会立即自行一次, 此后才是再每隔指定时间再执行一次。

假如需要参数时，则立即执行时所需的参数是，第一次调用`f`时传入的参数；而被延迟执行`f`的参数取决于 __最近__ 对它调用时传入的值，而不是该`f`最初传入的参数

参考[#170](https://github.com/jashkenas/underscore/issues/170),[#266](https://github.com/jashkenas/underscore/pull/266)

## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var timeout, context, args, throttling, finishThrottle;
      // 频繁调用`f`，最后一个`f`执行完的wait毫秒之后，'节流'关闭
      finishThrottle = _.debounce(function(){ throttling = false; }, wait);
      return function() {
        // 保证参数是最新的
        context = this; args = arguments;
        var throttler = function() {
          timeout = null;
          func.apply(context, args);
          finishThrottle();
        };
        if (!timeout) timeout = setTimeout(throttler, wait);

        // 当不处于'节流'时，就立即执行
        // 包括首次调用f时的立即执行的场景
        // 也包括超过wait毫秒后，再次调用f时也立即执行
        if (!throttling) func.apply(context, args);

        if (finishThrottle) finishThrottle();

        // '节流'开启
        throttling = true;
      };
    };

    _.debounce = function(func, wait) {
      var timeout;
      return function() {
        var context = this, args = arguments;
        var throttler = function() {
          timeout = null;
          func.apply(context, args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(throttler, wait);
      };
    };


## test case
该测试用例，同时测试上一个版本与本版本的源代码，运行时，将`//#`置于行首，即不引入该版本源码

<!--language: !js-->

    //#import underscore.1.2.0
    //import underscore.1.2.1
    //import json2
    //import simpletest
    function test(){
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr(); throttledIncr(); throttledIncr();
        setTimeout(throttledIncr, 70);
        setTimeout(throttledIncr, 120);
        setTimeout(throttledIncr, 140);
        setTimeout(throttledIncr, 190);
        setTimeout(throttledIncr, 220);
        setTimeout(throttledIncr, 240);
        _.delay(function(){ equal(counter, 1, 'incr was called immediately');}, 30);
        _.delay(function(){ equal(counter, 4, 'incr was throttled'); }, 400);
    }
    test();

    function test_arguments(){
        var value = 0;
        var update = function(val){ value = val; };
        var throttledUpdate = _.throttle(update, 100);
        throttledUpdate(1); throttledUpdate(2); throttledUpdate(3);
        setTimeout(function(){ throttledUpdate(4); }, 120);
        setTimeout(function(){ throttledUpdate(5); }, 140);
        setTimeout(function(){ throttledUpdate(6); }, 260);
        setTimeout(function(){ throttledUpdate(7); }, 270);
        _.delay(function(){ equal(value, 1, 'updated to latest value'); }, 40);
        _.delay(function(){ equal(value, 7, 'updated to latest value'); }, 400);
    }
    test_arguments();


underscore.1.2.2
=================
上面版本中，即使只调用`f`一次也会被执行两次，修正了此bug

## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var context, args, timeout, throttling, more;
      var whenDone = _.debounce(function(){ more = throttling = false; }, wait);
      return function() {
        context = this; args = arguments;
        var later = function() {
          timeout = null;
          // '后续'，即未被直接执行过，则执行
          // 即通过setTimeout来调用此的，必须保证未被 立即执行 过的
          if (more) func.apply(context, args);
          whenDone();
        };
        if (!timeout) timeout = setTimeout(later, wait);
        if (throttling) { // 节流中，则表示为'后续'
          more = true; // 此标志，就是用来修复bug的
        } else { // 未节流（首次，或超过wait后的首次），直接执行
          func.apply(context, args);
        }
        whenDone();
        throttling = true;
      };
    };

## test case

<!--language: !js-->

    //#import underscore.1.2.1
    //import underscore.1.2.2
    //import json2
    //import simpletest
    function test(){
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr(); throttledIncr(); throttledIncr();
        setTimeout(throttledIncr, 70);
        setTimeout(throttledIncr, 120);
        setTimeout(throttledIncr, 140);
        setTimeout(throttledIncr, 190);
        setTimeout(throttledIncr, 220);
        setTimeout(throttledIncr, 240);
        _.delay(function(){ equal(counter, 1, 'incr was called immediately');}, 30);
        _.delay(function(){ equal(counter, 4, 'incr was throttled'); }, 400);
    }
    test();

    function test_arguments(){
        var value = 0;
        var update = function(val){ value = val; };
        var throttledUpdate = _.throttle(update, 100);
        throttledUpdate(1); throttledUpdate(2); throttledUpdate(3);
        setTimeout(function(){ throttledUpdate(4); }, 120);
        setTimeout(function(){ throttledUpdate(5); }, 140);
        setTimeout(function(){ throttledUpdate(6); }, 260);
        setTimeout(function(){ throttledUpdate(7); }, 270);
        _.delay(function(){ equal(value, 1, 'updated to latest value'); }, 40);
        _.delay(function(){ equal(value, 7, 'updated to latest value'); }, 400);
    }
    test_arguments();

    function test_once() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr();
        _.delay(function(){ equal(counter, 1, 'incr was called once'); }, 220);
    }
    test_once();

    function test_twice() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr(); throttledIncr();
        _.delay(function(){ equal(counter, 2, 'incr was called twice'); }, 220);
    }
    test_twice();


underscore.1.3.2
=================
支持`f`调用返回值，由于`f`有可能被延迟执行，但 __返回值是立即返回__ 的，所以当`f`被延迟执行时，则返回值为上次立即执行（首次/超过wait后首次）时的结果

## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var context, args, timeout, throttling, more, result;
      var whenDone = _.debounce(function(){ more = throttling = false; }, wait);
      return function() {
        context = this; args = arguments;
        var later = function() {
          timeout = null;
          if (more) func.apply(context, args);
          whenDone();
        };
        if (!timeout) timeout = setTimeout(later, wait);
        if (throttling) {
          more = true;
        } else {
          result = func.apply(context, args);
        }
        whenDone();
        throttling = true;
        return result;
      };
    };

## test case

<!--language: !js-->

    //import underscore.1.3.2
    //import json2
    //import simpletest
    function test_once() {
        var counter = 0;
        var incr = function(){ return ++counter; };
        var throttledIncr = _.throttle(incr, 100);
        var result = throttledIncr();
        _.delay(function(){
            equal(result, 1, 'throttled functions return their value');
            equal(counter, 1, 'incr was called once');
        }, 220);
    }
    test_once();

    function test_twice() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100);
        throttledIncr(); throttledIncr();
        _.delay(function(){ equal(counter, 2, 'incr was called twice'); }, 220);
    }
    test_twice();

underscore.1.4.0
=================
修正`f`的返回值为 __上次执行后__ 的结果 [#465](https://github.com/jashkenas/underscore/issues/465)

## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var context, args, timeout, throttling, more, result;
      var whenDone = _.debounce(function(){ more = throttling = false; }, wait);
      return function() {
        context = this; args = arguments;
        var later = function() {
          timeout = null;
          if (more) {
            // 延迟执行的，也保存结果值
            result = func.apply(context, args);
          }
          whenDone();
        };
        if (!timeout) timeout = setTimeout(later, wait);
        if (throttling) {
          more = true;
        } else {
          throttling = true; // 位置调整，代码意思一样
          result = func.apply(context, args);
        }
        whenDone();
        return result;
      };
    };

## test case

<!--language: !js-->

    //import underscore.1.4.0
    //import json2
    //import simpletest
    function test_repeatedly_results() {
        var counter = 0;
        var incr = function(){ return ++counter; };
        var throttledIncr = _.throttle(incr, 100);
        var results = [];
        var saveResult = function() { results.push(throttledIncr()); };
        saveResult(); saveResult(); saveResult();
        setTimeout(saveResult, 70);
        setTimeout(saveResult, 120);
        setTimeout(saveResult, 140);
        setTimeout(saveResult, 190);
        setTimeout(saveResult, 240);
        setTimeout(saveResult, 260);
        _.delay(function() {
            equal(results[0], 1, 'incr was called once');
            equal(results[1], 1, 'incr was throttled');
            equal(results[2], 1, 'incr was throttled');
            equal(results[3], 1, 'incr was throttled');
            equal(results[4], 2, 'incr was called twice');
            equal(results[5], 2, 'incr was throttled');
            equal(results[6], 2, 'incr was throttled');
            equal(results[7], 3, 'incr was called thrice');
            equal(results[8], 3, 'incr was throttled');
        }, 400);
    }
    test_repeatedly_results();


underscore.1.4.3
=================
去掉了对`_debounce`的依赖，重构了代码，减少了对标志变量的依赖

修复的bug，参考[#820](https://github.com/jashkenas/underscore/issues/820)


## code

<!--language: js-->

    _.throttle = function(func, wait) {
      var context, args, timeout, result;
      var previous = 0;
      var later = function() {
        previous = new Date;
        timeout = null;
        result = func.apply(context, args);
      };
      return function() {
        var now = new Date;
        var remaining = wait - (now - previous);
        context = this;
        args = arguments;
        if (remaining <= 0) { //首次 或过了wait后的首次
          clearTimeout(timeout);
          timeout = null; //为非首次准备
          previous = now;
          result = func.apply(context, args); //直接执行
        } else if (!timeout) {
          // 非首次的使用setTimeout
          // 并且等函数被执行后（timeout==false时）再次可以使用setTimeout
          timeout = setTimeout(later, remaining);
        }
        return result;
      };
    };

## test case

<!--language: !js-->

    //#import underscore.1.4.0
    //import underscore.1.4.3
    //import json2
    //import simpletest
    function test_trailing_call_after_repeatedly_invoked() {
        var expected;
        var counter = 0;
        var limit = 80;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 32);

        var stamp = new Date;
        while ((new Date - stamp) < limit) {
            throttledIncr();
        }
        _.delay(function() {
            expected = counter + 2;
            throttledIncr();
            throttledIncr();
        }, 64);

        _.delay(function() {
            equal(counter, expected);
        }, 128);

        ok(counter > 1, {counter: counter});
    }
    test_trailing_call_after_repeatedly_invoked();


underscore.1.4.4
=================
源代码未变，重构了测试代码，更精简，间隔时间取2^N。

## test case

<!--language: !js-->

    //import underscore.1.4.3
    //import json2
    //import simpletest
    function test(){
    var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 32);
        throttledIncr(); throttledIncr();

        equal(counter, 1, "incr was called immediately");
        _.delay(function(){ equal(counter, 2, "incr was throttled"); }, 64);
    }
    test();

    function test_arguments(){
        var value = 0;
        var update = function(val){ value = val; };
        var throttledUpdate = _.throttle(update, 32);
        throttledUpdate(1); throttledUpdate(2);
        _.delay(function(){ throttledUpdate(3); }, 64);
        equal(value, 1, "updated to latest value");
        _.delay(function(){ equal(value, 3, "updated to latest value"); }, 96);
    }
    test_arguments();

    function test_once() {
        var counter = 0;
        var incr = function(){ return ++counter; };
        var throttledIncr = _.throttle(incr, 32);
        var result = throttledIncr();
        _.delay(function(){
            equal(result, 1, "throttled functions return their value");
            equal(counter, 1, "incr was called once");
        }, 64);
    }
    test_once();

    function test_twice() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 32);
        throttledIncr(); throttledIncr();
        _.delay(function(){ equal(counter, 2, "incr was called twice"); }, 64);
    }
    test_twice();

    function test_repeatedly_results() {
        var counter = 0;
        var incr = function(){ return ++counter; };
        var throttledIncr = _.throttle(incr, 64);
        var results = [];
        var saveResult = function() { results.push(throttledIncr()); };
        saveResult(); saveResult();
        _.delay(saveResult, 32);
        _.delay(saveResult, 80);
        _.delay(saveResult, 96);
        _.delay(saveResult, 144);
        _.delay(function() {
            equal(results[0], 1, "incr was called once");
            equal(results[1], 1, "incr was throttled");
            equal(results[2], 1, "incr was throttled");
            equal(results[3], 2, "incr was called twice");
            equal(results[4], 2, "incr was throttled");
            equal(results[5], 3, "incr was called trailing");
        }, 192);
    }
    test_repeatedly_results();

    function test_trailing_call_when_invoked_repeatedly() {
        var counter = 0;
        var limit = 48;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 32);

        var stamp = new Date;
        while ((new Date - stamp) < limit) {
          throttledIncr();
        }
        var lastCount = counter;
        ok(counter > 1, {counter: counter});

        _.delay(function() {
            ok(counter > lastCount, {counter: counter, lastCount: lastCount});
        }, 96);
    }
    test_trailing_call_when_invoked_repeatedly();


underscore.1.5.0
=================
增加了增加一个`options`参数，如果你想禁用第一次首先执行的话，传递`{leading: false}`，还有如果你想禁用最后一次执行的话，传递`{trailing: false}`。

参考[#926](https://github.com/jashkenas/underscore/issues/926)，[#1122](https://github.com/jashkenas/underscore/issues/1122)

## code

<!--language: js-->

    _.throttle = function(func, wait, options) {
      var context, args, result;
      var timeout = null;
      var previous = 0;
      options || (options = {});
      var later = function() {
        previous = new Date;
        timeout = null;
        result = func.apply(context, args);
      };
      return function() {
        var now = new Date;
        if (!previous && options.leading === false) previous = now;
        var remaining = wait - (now - previous);
        context = this;
        args = arguments;
        if (remaining <= 0) {
          clearTimeout(timeout);
          timeout = null;
          previous = now;
          result = func.apply(context, args);
        } else if (!timeout && options.trailing !== false) {
          timeout = setTimeout(later, remaining);
        }
        return result;
      };
    };

## test case

<!--language: !js-->

    //import underscore.1.5.0
    //import json2
    //import simpletest
    function test_leading_false() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 60, {leading: false});

        throttledIncr(); throttledIncr();
        equal(counter, 0);

        _.delay(function() {
            equal(counter, 1);
        }, 96);
    }
    test_leading_false();

    function test_trailing_false() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 60, {trailing: false});

        throttledIncr(); throttledIncr(); throttledIncr();
        equal(counter, 1);

        _.delay(function() {
            equal(counter, 1);

            throttledIncr(); throttledIncr();
            equal(counter, 2);

            _.delay(function() {
                equal(counter, 2);
            }, 96);
        }, 96);
    }
    test_trailing_false();


underscore.1.6.0
=================
修复了throttle arguments的内存泄漏

## code

<!--language: js-->

    _.throttle = function(func, wait, options) {
      var context, args, result;
      var timeout = null;
      var previous = 0;
      options || (options = {});
      var later = function() {
        previous = options.leading === false ? 0 : _.now();
        timeout = null;
        result = func.apply(context, args);
        context = args = null;
      };
      return function() {
        var now = _.now();
        if (!previous && options.leading === false) previous = now;
        var remaining = wait - (now - previous);
        context = this;
        args = arguments;
        if (remaining <= 0) {
          clearTimeout(timeout);
          timeout = null;
          previous = now;
          result = func.apply(context, args);
          context = args = null;
        } else if (!timeout && options.trailing !== false) {
          timeout = setTimeout(later, remaining);
        }
        return result;
      };
    };


## test case
新增了测试用例（与修复内存泄漏无关）

<!--language: !js-->

    //import underscore.1.6.0
    //import json2
    //import simpletest

    function test_more_throttle() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 30);
        throttledIncr(); throttledIncr();
        ok(counter == 1);
        _.delay(function(){
          ok(counter == 2);
          throttledIncr();
          ok(counter == 3);
        }, 85);
    }
    test_more_throttle();

    function test_more_throttle_with_leading_false() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100, {leading: false});

        throttledIncr();
        _.delay(throttledIncr, 50);
        _.delay(throttledIncr, 60);
        _.delay(throttledIncr, 200);
        ok(counter === 0);

        _.delay(function() {
          ok(counter == 1);
        }, 250);

        _.delay(function() {
          ok(counter == 2);
        }, 350);
    }
    test_more_throttle_with_leading_false();

    function test_one_more_throttle_with_leading_false() {
        var counter = 0;
        var incr = function(){ counter++; };
        var throttledIncr = _.throttle(incr, 100, {leading: false});

        var time = new Date;
        while (new Date - time < 350) throttledIncr();
        ok(counter <= 3, {counter: counter});

        _.delay(function() {
          ok(counter <= 4, {counter: counter});
        }, 200);
    }
    test_one_more_throttle_with_leading_false();


















