
文本
======

## 每次处理一个字符

- __任务__：用每次处理一个字符的方式处理字符串

- __解决方案__：

python实际上并没有一个特别的类型来对应"字符"并以此和字符串区别开来。

<!--language: !python-->

    # build a list whose items are the string's characters
    thestring = "abcde"
    thelist = list(thestring)

    def do_something_with(c):
        print "char : %s" % c
        return ord(c)

    # loop directly on the string with a for statement
    for c in thestring:
        do_something_with(c)

    # in the for clause of a list comprehension
    results = [do_something_with(c) for c in thestring]
    print results

    # with the map built-in function
    results = map(do_something_with, thestring)
    print results

- __讨论__：

python中，字符就是长度为1的字符串，可以循环遍历一个字符串，依次访问它的每个字符。如果想获得是该字符串所有字符的set，还可以使用`sets.Set`

<!--language: !python-->

    import sets
    magic_chars = sets.Set('abracadabra')
    poppins_chars = sets.Set('supercalifragilisticexpialidocious')
    print ''.join(magic_chars & poppins_chars)   # set intersection

## 字符和字符值之间的转换
- __任务__：将一个字符转化为相应的ASCII或Unicode码，或反之

- __解决方案__：

<!--language: !python-->

    print ord('a')
    print chr(97)

内建的`ord`也接受长度为1的Unicode字符串作为参数，此时它返回一个Unicode的码值，最大到65535，如果想把一个数字的Unicode码值转化为长度为1的Unicode字符串，可用内建`unichr`

<!--language: !python-->

    print ord(u'\u2020')
    print repr(unichr(8224))

- __讨论__：

新手们容易混淆`chr`和`str`区别，`str`只不过是将对象字符串化

<!--language: !python-->

    print repr(chr(97))
    print repr(str(97))

想把字符串转化为一个包含各个字符的值的列表

<!--language: !python-->

    print map(ord, 'ciao')

通过一个包含了字符值的列表创建字符串

<!--language: !python-->

    print ''.join(map(chr, range(97, 100)))


## 测试一个对象是否是类字符串
- __任务__：测试一个对象是否是类字符串，或这个对象是否具有类似字符串的行为模式

- __解决方案__：

利用`isinstance`和`basestring`检查

<!--language: !python-->

    def isAString(anobj):
        return isinstance(anobj, basestring)

    print isAString("abc")
    print isAString(123)

- __讨论__：

许多程序员的第一反应是类型测试：

<!--language: !python-->

    def isExactlyAString(anobj):
        return type(anobj) is type('')

但这种方式非常糟糕，它破坏了python基于签名的多态机制，很明显Unicode对象无法通过这个测试，用户自己编写的str子类也不行。

本节推荐`isinstance`则要好很多，`basestring`是`str`和`unicode`共同基类。不幸的是python标准库中的`UserString.UserString`类的实例，不能被检测通过，它非常明显是类字符串对象，只不过它不是多`basestring`派生的。如果想支持这种类型，可以 __检查一个对象的行为是否真的像字符串__ 一样：

<!--language: !python-->

    def isStringLike(anobj):
        try: anobj + ''
        except: return False
        else: return True

    # If and when you need to check for more string-like features of the object anobj
    def isStringLike(anobj):
        try: anobj.lower( ) + anobj + ''
        except: return False
        else: return True

"it's easier to ask forgiveness than permission" (EAFP)，获得事后原谅总是比事先得到许可要容易得多。`try/except`是保证EAFP处理风格的关键工具。

## 字符串对齐
- __任务__：实现字符串对齐：左对齐，居中对齐，右对齐

- __解决方案__：

<!--language: !python-->

    print '|', 'hej'.ljust(20), '|', 'hej'.rjust(20), '|', 'hej'.center(20), '|'

- __讨论__：

<!--language: !python-->

    print 'hej'.center(20, '+')


## 去除字符串两端的空格
- __任务__：获得一个开头和末尾没有多余空格的字符串
- __解决方案__：

<!--language: !python-->

    x = '    hej   '
    print '|', x.lstrip( ), '|', x.rstrip( ), '|', x.strip( ), '|'

- __讨论__：

有时候需要给字符串添加一些空格，让其符合预先规定的固定宽度，以完成上节所需的对齐，但有时也需要从两端移除空格（空白、制表符、换行符等）。以上三个方法，也可选择去除其它字符，如下例中的'xy'：

<!--language: !python-->

    x = 'xyxxyy hejyx  yyx'
    print '|'+x.strip('xy')+'|'

## 合并字符串
- __任务__：有一些小的字符串，想把这些字符串合并成一个大字符串

- __解决方案__：

<!--language: !python-->

    pieces = ["some","string","in","a","array"]
    largeString = ''.join(pieces)
    print largeString

    small1 = "aa"
    small2 = "bb"
    largeString = '%s something %s yet more' % (small1, small2)
    print largeString

- __讨论__：

`+`操作符也能够将字符串拼接起来，或用`reduce`

<!--language: !python-->

    small1 = "aa"
    small2 = "bb"
    largeString = small1 + ' something ' + small2 + ' yet more'
    print largeString

    import operator
    pieces = ["some","string","in","a","array"]
    largeString = reduce(operator.add, pieces, '')
    print largeString

但python中字符串是无法改变(immutable)的，任何对字符串的操作，包括拼接，都将产生一个新的字符串对象，而不是修改原有的对象，因此拼接N个字符串将涉及创建并丢弃N-1个中间结果，影响性能。所以使用格式化操作符`%`通常是更好的选择

当数据并不是一开始已经就位，可使用一个list作为中间数据结构来容纳它们，使用`append`或`extend`来添加新的数据，在取得所有数据之后，再调用`''.join(pieces)`就可得到合并之后的大字符串。

## 把字符串逐字符或逐词反转

- __解决方案__：

<!--language: !python-->

    astring = "astring"
    revchars = astring[::-1]
    print revchars

    astring = "has some words"
    revwords = astring.split()     # string -> list of words
    revwords.reverse()             # reverse the list in place
    revwords = ' '.join(revwords)  # list of strings -> string
    print revwords

    revwords = ' '.join(astring.split()[::-1])
    print revwords

    revwords = ' '.join(reversed(astring.split()))
    print revwords

## 字符串中是否含某字符集中字符
- __任务__：检查字符串中是否包含某字符集合中的字符

- __解决方案__：

以下方法最简单

<!--language: !python-->

    def containsAny(seq, aset):
        """ Check whether sequence seq contains ANY of the items in aset. """
        for c in seq:
            if c in aset: return True
        return False

使用`itertools`来提高性能

<!--language: !python-->

    import itertools
    def containsAny(seq, aset):
        for item in itertools.ifilter(aset.__contains__, seq):
            return True
        return False

`ifilter`需要传入一个predicate(一个返回bool的函数)，和一个可迭代对象

- __讨论__：

一个纯粹基于集合的方法应该像这样

<!--language: !python-->

    def containsAny(seq, aset):
        return bool(set(aset).intersection(seq))

    seq = "abcd"
    aset = "def"
    print containsAny(seq, aset)


计算交集，不过这种方法意味着`seq`中每个成员都不可避免地要被检查，而上面的则是 __"短路法"__。但如果没办法短路，如下面的`containsAll`，最好使用内建`set`

<!--language: !python-->

    def containsAll(seq, aset):
        """ Check whether sequence seq contains ALL the items in aset. """
        return not set(aset).difference(seq)

如果需要处理的`seq`和`aset`的字符串(非Unicode)，可以尝试更特殊的方式，`str.translate(table[, deletechars]) `方法

<!--language: !python-->

    import string
    notrans = string.maketrans('', '')      # identity "translation"

    def containsAny(astr, strset):
        return len(strset) != len(strset.translate(notrans, astr))

    def containsAll(astr, strset):
        return not strset.translate(notrans, astr)

`strset.translate(notrans, astr)`是`strset`的子序列，`astr`是需要删除的字符，而且由所有不属于`astr`的字符组成，如果这个子序列和`strset`有同样长度，说明没有删除任何字符，意味着`strset`中任何一个字符都不属于`astr`

## Title
- __任务__：P18

- __解决方案__：

<!--language: !python-->

- __讨论__：


文件
======

- 文件基础
文件对象是内建类型`file`的实例，内建函数`open`会创建并返回一个文件对象

<!--language: python-->

    input = open('data', 'r')
    output = open('/tmp/spam', 'w')

`open`接受由`/`分隔开的目录和文件名构成的文件路径，而不管操作系统本身的影响（即windows下一样有效）

python代码总可以依赖`\n`作为文本行结束标记

<!--language: python-->

    for line in input.readlines():
        process(line)

对性能影响比较大，__推荐__ 如下方式：

<!--language: python-->

    for line in input:
        process(line)

    # Portability and Flexibility
    def scanner(fileobject, linehandler):
        for line in fileobject:
            linehandler(line)

    def firstword(line):
        print line.split( )[0]
    file = open('data')
    scanner(file, firstword)

`scanner`没有对类型进行假设，所以`StringIO`,`cStringIO`,甚至是 __实现了`__iter__`的对象__ 均可使用

## 读取文件
- __任务__：从文件中读取文本或数据

- __解决方案__：

<!--language: !python-->

    all_the_text = open('thefile.txt').read( )    # all text from a text file
    all_the_data = open('abinfile', 'rb').read( ) # all data from a binary file

    file_object = open('thefile.txt')
    try:
        all_the_text = file_object.read( )
    finally:
        file_object.close( )

- __讨论__：





## Title
- __任务__：

- __解决方案__：

<!--language: !python-->

- __讨论__：