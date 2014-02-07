
文本
======

## 每次处理一个字符

### 任务
用每次处理一个字符的方式处理字符串

### 解决方案
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

### 讨论
python中，字符就是长度为1的字符串，可以循环遍历一个字符串，依次访问它的每个字符。如果想获得是该字符串所有字符的set，还可以使用`sets.Set`

<!--language: !python-->

    import sets
    magic_chars = sets.Set('abracadabra')
    poppins_chars = sets.Set('supercalifragilisticexpialidocious')
    print ''.join(magic_chars & poppins_chars)   # set intersection

## 字符和字符值之间的转换
### 任务
将一个字符转化为相应的ASCII或Unicode码，或反之

### 解决方案

<!--language: !python-->

    print ord('a')
    print chr(97)

内建的`ord`也接受长度为1的Unicode字符串作为参数，此时它返回一个Unicode的码值，最大到65535，如果想把一个数字的Unicode码值转化为长度为1的Unicode字符串，可用内建`unichr`

<!--language: !python-->

    print ord(u'\u2020')
    print repr(unichr(8224))

### 讨论
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
### 任务
测试一个对象是否是类字符串，或这个对象是否具有类似字符串的行为模式

### 解决方案
利用`isinstance`和`basestring`检查

<!--language: !python-->

    def isAString(anobj):
        return isinstance(anobj, basestring)

    print isAString("abc")
    print isAString(123)

### 讨论
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

    #　If and when you need to check for more string-like features of the object anobj
    def isStringLike(anobj):
        try: anobj.lower( ) + anobj + ''
        except: return False
        else: return True

"it's easier to ask forgiveness than permission" (EAFP)，获得事后原谅总是比事先得到许可要容易得多。`try/except`是保证EAFP处理风格的关键工具。

## 字符串对齐
### 任务
实现字符串对齐：左对齐，居中对齐，右对齐

### 解决方案
<!--language: !python-->

    print '|', 'hej'.ljust(20), '|', 'hej'.rjust(20), '|', 'hej'.center(20), '|'

### 讨论
<!--language: !python-->

    print 'hej'.center(20, '+')


## 去除字符串两端的空格
### 任务
获得一个开头和末尾没有多余空格的字符串
### 解决方案
<!--language: !python-->

    x = '    hej   '
    print '|', x.lstrip( ), '|', x.rstrip( ), '|', x.strip( ), '|'

### 讨论
有时候需要给字符串添加一些空格，让其符合预先规定的固定宽度，以完成上节所需的对齐，但有时也需要从两端移除空格（空白、制表符、换行符等）。以上三个方法，也可选择去除其它字符，如下例中的'xy'：

<!--language: !python-->

    x = 'xyxxyy hejyx  yyx'
    print '|'+x.strip('xy')+'|'


## 合并字符串
### 任务
### 解决方案
<!--language: !python-->
### 讨论

## ???????????
### 任务
### 解决方案
<!--language: !python-->
### 讨论



<script type="text/javascript">setTimeout(function(){expand_toc('md_toc',2)},2000);</script>
