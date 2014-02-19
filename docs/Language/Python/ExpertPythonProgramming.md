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
## 装饰器
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
