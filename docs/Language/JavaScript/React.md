> 2016-04-07

# 组件的生命周期

## 实例化
- 初次被创建时，下面方法被依次调用
    + `getDefaultProps` 对于组件类来说，只会被调用一次，任何复杂的值（对象和数组），都会在所有的实例中共享
    + `getInitialState` 对于组件的每一个实例来说，这个方法调用次数有且只有一次，在这里可以访问到`this.props`
    + `componentWillMount` 首次渲染前被调用，在`render`前可以修改组件`state`的最后一次机会
    + `render` 这里会创建一个虚拟DOM（随后会把它和real DOM做对比），对于一个组件而言，它是唯一必须的方法，该方法需要满足以下几点：
        * 只能通过`this.props`和`this.state`访问数据
        * 可以返回`null`,`false`或任何React组件
        * 只能出现一个顶级组件
        * 必须纯净，不能改变组件的状态或修改DOM的输出
    + `componentDidMount` 可在内部通过`this.getDOMNode()`来访问real DOM，此时可使用jquery之类的库来操作dom了
- 对于该组件类的所有后续应用，则下面方法依次被调用
    + `getInitialState`
    + `componentWillMount`
    + `render`
    + `componentDidMount`

## 存在期
- 随着应用状态的改变，以及组件逐渐受到影响，如用户改变了组件的`state`，便会有新的`state`流入组件树
    + `componentWillReceiveProps` 在任意时间，组件的`props`都可通过父辈组件来更改，出现这种情况时，该方法被调用，获得更改`props`对象及`state`的机会
    + `shouldComponentUpdate` 确定某个组件不需要渲染新的`props`或`state`，则返回`false`，用于提高效率
    + `componentWillUpdate` 与`componentWillMount`类似
    + `render`
    + `componentDidUpdate` 与`componentDidMount`类似

## 销毁清理期
- `componentWillUnmount` 在`componentDidMount`中添加的所有任务需要清除，比如定时器，事件等


