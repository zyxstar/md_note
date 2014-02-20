> 2011-07-25

语法最佳实践-低于类级
======================

## 列表推导
如下的代码是痛苦的：

<!--language: !python-->

    numbers = range(10)
    size = len(numbers)
    evens = []
    i = 0
    while i < size:
        if i % 2 == 0:
            evens.append(i)
        i += 1

    print evens

对于C语言或许是可行的，但在Python中它确实会使程序执行速度变慢：

- 它命名解释程序在每次循环中都要确定序系中哪一个部分被修改；
- 它命名得必须通过一个计数器来跟踪必须处理的元素

List comprehensions是这种场景下的正确选择：

<!--language: !python-->

    print [i for i in range(10) if i % 2 == 0]

Python风格语法的另一个典型例子是使用`enumerate`，这个内建函数为在循环中使用序列时提供了更加便利的获得索引的方式

<!--language: !python-->

    seq = ["one", "two", "three"]
    i = 0
    for element in seq:
        seq[i] = '%d: %s' % (i, seq[i])
        i += 1
    print seq

    seq = ["one", "two", "three"]
    for i, element in enumerate(seq):
        seq[i] = '%d: %s' % (i, seq[i])
    print seq

    seq = ["one", "two", "three"]
    def _treatment(pos, element):
        return '%d: %s' % (pos, element)
    print [_treatment(i, el) for i, el in enumerate(seq)]

> 每当要对序列的内容进行循环处理时，就应该尝试List comprehensions

## 迭代器和生成器
迭代器只不过是一个实现迭代器协议的容器对象，它基于两个方法：

- `next`返回容器的下一个项目
- `__iter__`返回迭代器本身

迭代器可通过使用一个`iter`内建函数和一个序列来创建

<!--language: !python-->

    i = iter('abc')
    print i.next(),i.next(),i.next(),i.next()

当序列遍历完时，将抛出一个`StopIteration`异常，这将使得迭代器与循环兼容，因为它们将捕获这个异常以停止循环，要创建 __定制__ 的迭代器，可以编写一个具有`next`方法的类，该类能提供返回迭代器实例的`__iter__`特殊方法

<!--language: !python-->

    class MyIterator(object):
        def __init__(self, step):
            self.step = step
        def next(self):
            """Returns the next element."""
            if self.step == 0:
                raise StopIteration
            self.step -= 1
            return self.step
        def __iter__(self):
            """Returns the iterator itself."""
            return self

    for el in MyIterator(4):
        print el

它们为生成器这一更有趣的特性提供了基础

### 生成器
基于`yield`指令，可以暂停一个函数并返回中间结果，该函数将保存执行环境并且可以必要时恢复

<!--language: !python-->

    def fibonacci():
        a, b = 0, 1
        while True:
            yield b
            a, b = b, a + b

    fib = fibonacci()
    print [fib.next() for i in range(10)]

该函数返回一个特殊的迭代器，也就是`generator`对象，它知道如何保存执行环境，对它的调用是不确定的，每次都将产生序列的下一个元素。

- 当需要将一个返回一个序列或在循环中执行的函数时，就应该考虑生成器，这些元素将被传递到另一个函数中进行后续处理时，一次返回一个元素能够 __提高整体性能__。

- Fibonacci数列是 __无穷尽__ 的，但用来生成它的生成器，不需要在提供一个值的时候，就预先占用无穷多的内存，惰性按需获取

- 生成器对降低程序复杂性也有帮助，并且能够提升基于多个序列的数据转换算法的性能，把每个序列当作一个迭代器，然后把它们合并到一个高级别的函数，是一种避免函数变庞大的好办法，还可以给整个处理链提供实时反馈，类似shell中管道

<!--language: !python-->

    def power(values):
        for value in values:
            print 'powering %s' % value
            yield value

    def adder(values):
        for value in values:
            print 'adding to %s' % value
            if value % 2 == 0:
                yield value + 3
            else:
                yield value + 2

    elements = [1, 4, 7, 9, 12, 19]
    res = adder(power(elements))
    print res.next()
    print res.next()
    print res.next()

拥有许多简单的处理序列值的可迭代代函数，要比一个复杂的，每次处理一个值的函数更好些

- 与`next`方法调用的代码 __进行交互__ 的功能

<!--language: !python-->

    def psychologist():
        print 'Please tell me your problems'
        while True:
            answer = (yield)
            if answer is not None:
                if answer.endswith('?'):
                    print ("Don't ask yourself too much questions")
                elif 'good' in answer:
                    print "A that's good, go on"
                elif 'bad' in answer:
                    print "Don't be so negative"

    free = psychologist()
    free.next()
    free.send('I feel bad')
    free.send("Why I shouldn't ?")
    free.send("ok then i should find what is good for me")

`send`的工作机制与`next`一样，但是`yield`将变成能够返回传入的值。可以根据客户代码来改变其行为。同时还增加了`throw`和`close`两个函数，以完成该行为，它们将向生成器抛出一个错误：

- `throw`允许客户代码传入要抛出的任何类型异常
- `close`会抛出特定异常`GeneratorExit`

<!--language: !python-->

    def my_generator():
        try:
            yield 'something'
        except ValueError:
            yield 'dealing with the exception'
        finally:
            print "ok let's clean"

    gen = my_generator()
    print gen.next()
    print gen.throw(ValueError('mean mean mean'))
    gen.close()
    gen.next()

有了这些方法，可用生成器来编写协同程序(coroutine)

### 协同程序
P47 41
## 装饰器

<!--
## with和contextlib

语法最佳实践-类级
======================
## 子类化内建类型
## 访问超类中的方法
## 最佳实践
## 描述符和属性
## 槽
## 元编程

选择好的名称
======================
## PEP8和命名最佳实践
## 命名风格
## 命名指南
## 参数最佳实践
## 类名
## 模块和包名称
## 使用API
## 有用的工具

编写一个包
================
## 用于所有包的公共模式
## 基于模板的方法
## 创建包模板
## 开发周期

编写一个应用程序
================
## Atomisator概述
## 整体描述
## 工作环境
## 编写各个包
## 分包Atomisator
## 包之间的依赖性

使用zc.buildout
================
## zc.buildout原理
## 发行与分发
## 小结

代码管理
================
## 版本控制系统
## 持续集成
## 小结

生命周期管理
================
## 不同的方法
## 定义生命周期
## 建立一个跟踪系统
## 小结

编写项目文档
================
## 技术性写作7条规则
## reStructuredText入门
## 构建文档
## 建立自己的工件集

测试驱动开发
================
## 我不测试
## 我测试
## 小结

优化：通用原则和剖析技术
=========================
## 优化的3条规则
## 优化策略
## 查找瓶颈
## 小结

优化：解决方案
=========================
## 降低复杂度
## 多线程
## 多进程
## 缓存
## 小结

优化：有用的设计模式
=========================
## 创建型模式
## 结构型模式
## 行为型模式
## 小结

Learning Python
=================
-->