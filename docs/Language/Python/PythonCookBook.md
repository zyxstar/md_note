
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


<!--
## Title
- __任务__：

- __解决方案__：

<!-language: !python->

- __讨论__：

-->

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

注意，不需要将`open`放到`try`子句中，如果在打开文件的时候就发生了错误，那就没有什么需要关闭

最简单、最快，也最具Python风格的方法是逐物读取文本文件内容，并将读取的数据放置到一个字符串列表中：

<!--language: !python-->

    list_of_all_the_lines = file_object.readlines( )

这样读出每行末尾带有`\n`符号，如果不想这样，有一些替代方法

<!--language: !python-->

    list_of_all_the_lines = file_object.read( ).splitlines( )
    list_of_all_the_lines = file_object.read( ).split('\n')
    list_of_all_the_lines = [L.rstrip('\n') for L in file_object]

- __讨论__：在不同操作系统平台上，换行的符号表示不一样，如果不确定某文本文件会用什么样的换行符，可将`open`第二个参数设定为`rU`，指定通用换行符转化，无论你的代码在什么平台上运行，各种换行符都被映射成`\n`

## 写入文件
- __任务__：写入文本或二进制数据到文件中

- __解决方案__：下面是最方便的将一个长字符串写入文件的方法

<!--language: !python-->

    open('thefile.txt', 'w').write(all_the_text)  # text to a text file
    open('abinfile', 'wb').write(all_the_data)    # data to a binary file

许多时候想写入是一个字符串列表，应该使用`writelines`，这个方法并不局限于行写入，__二进制文件和文本行都适用__

<!--language: !python-->

    file_object.writelines(list_of_text_strings)
    open('abinfile', 'wb').writelines(list_of_data_strings)

当然也可以先把子串用`''.join`拼接成大字符串，再调用`write`写入，或在循环澡官入，但直接调用`writelines`比上面两种方式快得多

- __讨论__：如果想把新数据添加在原有的数据之后，应该使用`a`或`ab`选项来打开文件

## 搜索和替换文件中文本
- __任务__：需要将文件中的某个字符串改变成另一个

- __解决方案__：字符串对象的`replace`方法提供了字符串替换的最简单的方法。下面支持一个特定的文件（或标准输入）读取数据，然后写入一个指定的文件（或标准输出）：

<!--language: !python-->

    for s in input_file:
        output_file.write(s.replace(stext, rtext))

## 从文件中读取指定的行
- __任务__：根据给出的行号，从文本文件中读取一行数据

- __解决方案__：标准库`linecache`模块非常适合这个任务

<!--language: !python-->

    import linecache
    theline = linecache.getline(thefilepath, desired_line_number)

- __讨论__：如果文件非常大，而你只需要其中一行，`linecache`显得不是那么必要，下面可以获得上一些提升：

<!--language: !python-->

    def getline(thefilepath, desired_line_number):
        if desired_line_number < 1: return ''
        for current_line_number, line in enumerate(open(thefilepath, 'rU')):
            if current_line_number == desired_line_number-1: return line
        return ''

## 计算文件的行数
- __任务__：

- __解决方案__：

<!--language: !python-->

- __讨论__：



<!--
## Title
- __任务__：

- __解决方案__：

<!-language: !python->

- __讨论__：

-->


时间和财务计算
===============
- 时间模块常用的一个函数就是获取当前时间的函数`time.time`，在未初始化的情况下返回一个浮点数，代表了从某个特定时间点（纪元）开始所经历的秒数，一般是1970-1-1年夜。

<!--language: !python-->

    import time
    print time.asctime(time.gmtime(0))

`time.gmtime`将任何时间戳（从纪元开始所经历秒数）转化为一个元组，该元组代表了人类容易理解一种时间格式，在未进行任何时区转化（GMT代表格林威治标准时间，UTC世界标准时间的另一种说法），如果使用`time.localtime`，它会根据当前时区进行时间转化

从返回元组中获得本地时间的方法

<!--language: !python-->

    import time
    year,month,mday,hour,minute,second,wday,yday,isdst = time.localtime()
    print time.localtime().tm_mon

忽略了传递给`localtime`,`gmtime`,`asctime`时，默认使用当前时间

- `strftime`根据返回的时间元组构建一个字符串，`strptime`与前者完全相反，返回一个时间元组

- 标准库引入了`datetime`模块，提供了更好的对应于抽象的日期和时间和各种类型，如`time`,'date','datetime'类型

<!--language: !python-->

    import datetime
    today = datetime.date.today( )
    birthday = datetime.date(1977, 5, 4)      #May 4
    currenttime = datetime.datetime.now( ).time( )
    lunchtime = datetime.time(12, 00)
    now = datetime.datetime.now( )
    epoch = datetime.datetime(1970, 1, 1)
    meeting = datetime.datetime(2005, 8, 3, 15, 30)

更进一步，通过属性和方法，这些类型提供了很方便的获取和操作信息的方法

<!--language: !python-->

    import datetime
    today = datetime.date.today( )
    next_year = today.replace(year=today.year+1).strftime("%Y.%m.%d")
    print next_year

`datetime`模块通过`timedelta`类型为时间差提供了一些基本支持

<!--language: !python-->

    import datetime
    NewYearsDay = datetime.date(2005, 01, 01)
    NewYearsEve = datetime.date(2004, 12, 31)
    oneday = NewYearsDay - NewYearsEve
    print oneday

- `decimal.Decimal`用于财务计算，比标准的二进制float提供更高级功能

<!--language: !python-->

    import decimal
    print 1.1 == 1.1000000000000001
    print 2.3 == 2.2999999999999998
    print decimal.Decimal("1.1")==decimal.Decimal("1.1000000000000001")
    print decimal.Decimal("2.3")==decimal.Decimal("2.2999999999999998")

    print 0.1 + 0.1 + 0.1 - 0.3
    d1 = decimal.Decimal("0.1")
    d3 = decimal.Decimal("0.3")
    print d1 + d1 + d1 - d3

它的精度可由用户设置

<!--language: !python-->

    import decimal
    decimal.getcontext( ).prec = 6   # set the precision to 6...
    print decimal.Decimal(1) / decimal.Decimal(7)

    decimal.getcontext( ).prec = 60  # ...and to 60 digits
    print decimal.Decimal(1) / decimal.Decimal(7)

为什么还用`float`主要是性能

<!--language: !bash-->

    python -mtimeit -s'from decimal import Decimal as D' 'D("1.2")+D("3.4")'
    python -mtimeit -s'from decimal import Decimal as D' '1.2+3.4'


## 计算昨天和明天的日期

- __解决方案__：

<!--language: !python-->

    import datetime
    today = datetime.date.today( )
    yesterday = today - datetime.timedelta(days=1)
    tomorrow = today + datetime.timedelta(days=1)
    print yesterday, today, tomorrow

- __讨论__：Python中的"in the face of ambiguity, refuse the temptation to guess."，拒绝猜测，`yesterday = today - 1`，中的`1`是1天还是1秒？

如果想在日期和时间的计算上有点新花样，可使用第三方包`dateutil`和`mx.DateTime`

<!--language: !python-->

    from dateutil import relativedelta
    nextweek = today + relativedelta.relativedelta(weeks=1)
    print nextweek

然而"always do the simplest thing that can possibly work." ，本节使用了datetime.timedelta`

## 寻找上一个星期五
- __任务__：

- __解决方案__：

<!--language: !python-->

- __讨论__：

<!--
## Title
- __任务__：

- __解决方案__：

<!-language: !python->

- __讨论__：

-->

