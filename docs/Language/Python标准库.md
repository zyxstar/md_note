文本
======
## string-文本常量和模板
包含处理文本的常量和类，原先仅在这个模板中实现的很多函数则被移植为`str`和`unicode`对象的方法。`string`模块保留了很多有用常量和类，用来处理`string`和`unicode`对象。

### 函数
还有两个函数未从`string`模块移出: `capwords()`和`maketrans()`，前者作用是将一个字符串中所有单词的首字母大写

<!--language: !python-->

    import string
    s = 'The quick brown fox jumped over the lazy dog.'
    print s
    print string.capwords(s)

`maketrans()`函数将创建转换表，可以用来结合`translate()`方法将一组字符串修改为另一组字符串，这种做法比反复调用`replace()`更为高效

<!--language: !python-->

    import string
    leet = string.maketrans('abegiloprstz', '463611092572')
    s = 'The quick brown fox jumped over the lazy dog.'
    print s
    print s.translate(leet)

### 模板
