> 2009-09-06

为什么建模
==========
- 构建模型就是为了弄清楚某些东西是否可行
- 当模型比要构建的真实实体便宜得多时，我们就会使用模型来研究设计
- 当有一些确定的东西需要测试，并且使用UML要比使用代码测试起来代价更低一些时，就使用UML

## UML概观

UML可以用来绘制关于问题领域、候选软件设计以及完成了的软件实现的图示，即 __概念级、规格说明和实现级__。本文关注后两个级别。

规格说明级和实现级的图示与源代码间有很强的关联，事实上，规格说明级图示的目的就是为了能够转换成源代码。同样，实现级图示是为了描绘已呢的源代码。这两上级别的图示必须 __遵循某些规则和语义__。

概念级的图示和源代码没有很强的关联，更多地是和人的语言相关，用于描绘存在于问题领域的概念和抽象的一种速记方法，因此可 __不必遵循__ 强的语义规则，其含义可以是模糊，而 __通过解释__ 来确定。而直接通过概念级图示去说明源代码是程序员和分析师之间产生误解的根源：__描述问题解决方案的规范说明 *不必* 与描述问题本身的概念图有什么相像之处__。

## 有效使用UML
图的详细程度应该只是达成目标所必需的，请保持图示简单、干净，不应该当作声明所有方法、变量和关系的地方

### 与他人交流
如果你有一些想法需要和他人进行交流，UML是非常有用的

#### 设计想法(适合)

![effective_uml_01](../../imgs/effective_uml_01.png)

如上图示就非常清楚，可以看到`LoginPage`继承自`Page`，并使用了`UserDatabase`。很明显`UserDatabase`和`HttpResponse`都是`LoginPage`所需的。当一组开发人员站在一块白板前 __*讨论*__ 类似图示的场景，__非常清晰__ 地表达出了代码结构看起来的样子。

#### 算法细节(不适合)

![effective_uml_02](../../imgs/effective_uml_02.png)

![effective_uml_03](../../imgs/effective_uml_03.png)

上图是描述冒泡排序算法的类图与序列图，这方面UML并 __不是非常合适__，图示并不比代码更易读，__绘制也难__ 得多

### 脉络图(适合)
在创建大型系统的结构脉络图(road map)方面，UML也很有用，可使得开发者快速能找到类之间的依赖关系，并提供了一份关于 __*整个系统结构*__ 的参考

![effective_uml_04](../../imgs/effective_uml_04.png)

上图很容易的看出`Space`对象含有`PolyLine`，而后者是由许多继承自`LinearObject`的`Line`组成，`LinearObject`包含两个`Point`。在代码中发现这个结构是一件令人厌烦的工作，而在脉络图中这个结构是显而易见的

### 项目结束文档(适合)

编写需要保存的设计文档的最好时机是在 __项目结束__ 的时候，并把它作为团队的最后一项工作，这种文档会精确的反映出设计的状态，对后继团队来说非常有用。

UML图必须经过仔细考虑，__不需要数千页的顺序图__，要的是那些 __*描述系统关键要点*__ 的 __少量重要__ 的图示。

## 迭代式改进
要灵光一现时绘制UML吗？要先画类图然后再画顺序图吗？应该在涉及任何细节之前先搭建起系统的整体结构吗？

所有的回答是 __不__。人类能够做好的，都是采取 __*小步前进*__，然后对结果进行评估的方法 的事情。而采取大步跳跃，经常是做不好的。

### 行为优先(协作图)

> ps. 在分析层面，觉得还是四色建模之类的静态图来表达领域模型。在设计时可考虑这种做法，它能方便的找到类的 __*协作*__ 关系

从行动开始，UML会助于思考，首先绘制简单的顺序图或协作图。以手机控制软件为例：

- 想象软件检查着每一次按键，并向控制拨号的某个对象发送消息，因此我们会绘制出一个`Button`对象和一个`Dialer`对象，以及`Button`向`Dialer`发送多条`digit`消息（星号表示多条）

![effective_uml_05](../../imgs/effective_uml_05.png)

- 当`Dialer`收到一条`digit`消息时会做什么呢？得把这个数字显示在屏幕上，也许会向`Screen`对象发送`diaplayGigit`消息

![effective_uml_06](../../imgs/effective_uml_06.png)

- 接下来，`Dialer`最好能让扬声器发出声音，因此，得让它向`Speaker`对象发送`tone`消息

![effective_uml_07](../../imgs/effective_uml_07.png)

- 在某个时刻，用户会按下`Send`按钮，表示号码已经拨完。此时，让手机无线通信装置去连接手机网络并把所拨的电话号码传递出去

![effective_uml_08](../../imgs/effective_uml_08.png)

- 一旦连接建立起来，Radio就可以让`Screen`点亮"In-Use"指示灯，这条消息无疑得从一个不同的控制线程中发出（这一点是通过消息序号前面的字母来表示的）。最终的协作图如下

![effective_uml_09](../../imgs/effective_uml_09.png)

### 检查结构(类图)
上面展示了如何从零开始构建协作图。请注意如何创建对象的，我们无法提前知道将会出现这些对象；我们 __只知道我们得让某些事情发生，于是就创造对象来完成这些事情__

现在，在继承前进之前，得 __检查__ 一下这幅协作图对于代码结构来说意味着什么，因此我们创建了一幅类图来补充该协作图。类图包含了协作图中的每个对象的类，以及它们的 __依赖__ 关系

![effective_uml_10](../../imgs/effective_uml_10.png)

发现我们忽略了聚合和组合，这是故意的，会有充足的时间来考虑是否需要应用这些关系。

- 当前最重要的是 __*分析一下依赖关系*__，为何`Button`要依赖于`Dialer`呢？如果考虑这点，会发现这是非常丑陋的，这意味着如下代码：

<!--language: csharp-->

    public class Button {
        private Dialer itsDialer;
        public Button(Dialer dialer){
            itsDialer = dialer;
        }
        // ...
    }

- `Button`就是可以在 __不同上下文__ 中使用的类，比如可使用`Button`类来控制on/off开关、菜单按钮或话机上的其他控制按钮

- 可以通过`Button`和`Dialer`之间 __插入一个接口__ 来解决这个问题，每个`Button`都被赋予一个用于标识它的token，当检查到按钮按下时，`Button`类就调用`ButtonListener`接口听`buttonPressed`方法，并把标记传过去。这就解除了`Button`对于`Dialer`的依赖。这个改变 __并没有对上面的动态图造成任何影响，*对象完全一样，只是类发生了改变*__

![effective_uml_11](../../imgs/effective_uml_11.png)

- 糟糕的是，现在我们让`Dialer`知道了一点`Button`的东西，`Dialer`为什么要从`ButtonListener`获取它的输入呢，它为何要有一个名为`buttonPressed`的方法呢？通过使用一些小 __适配器__，我们可以解决这个问题：

![effective_uml_12](../../imgs/effective_uml_12.png)

### 想象代码
使用图示时非常重要的一点就是能够想象出代码，我们把图示作为代码的速记，而不替代，如果画的图不能想象出它所表示的代码，那么就是构建空中楼阁。

<!--language: csharp-->

    public class ButtonDialerAdapter : ButtonListener {
        private int digit;
        private Dialer dialer;
        public ButtonDialerAdapter(int digit, Dialer dialer) {
            this.digit = digit;
            this.dialer = dialer;
        }

        public void ButtonPressed() {
            dialer.Digit(digit);
        }
    }

### 图的演化
在最后所做的改变使得动态模型变得无效，动态模型对适配器一无所知，需要更改一下。__以动态关系为开始，然后探索这些动态性所蕴涵的静态关系，再依据一些设计原则来更改静态关系，接着需要返回去改进动态图__

![effective_uml_13](../../imgs/effective_uml_13.png)

每一个步骤都是微小的，不要在动态图上投入超过5分钟的时间不去考虑其所蕴涵的静态结构，也中要在改进静态结构上超过5分钟而不去考虑其对动态行为造成的影响，应该使用非常短的周期来一起演化这两种图示。

## 何时绘制

### 何时要画
- 几个人都需要理解设计的某个特定部分的结构，因为他们都将同时工作于其上，当每个人都认为已经理解时，就停止
- 希望团队能够达成一致，但是有两个或者更多的人不同意某个特定元素的设计。把讨论限定在一个时间盒内，然后选择一种决策手段，时间盒终止或能够做出决策时就结束
- 尝试一个设计想法，并且图示有助进行思考时，当可以使用代码完成思考时就停止
- 要向他人或者自己解释代码某些部分的结构，当通过浏览代码可以更好地进行解释时就停止
- 快要到达项目的尾声，并且客户要求把图示做为向他人提供的一组文档中的一部分

### 何时不要画
- 工作流程的要求
- 认为这是好的设计者要做的事
- 为了编码前创建面面俱到的设计阶段文档
- 为了让其他人编码，真正架构师要参与到自己设计的编码中

### 文档呢
文档必须编写，对于什么不需要文档化和什么需要文档的选择一样重要。复杂的通信协议需要文档化，复杂的关系模型需要文档化，复杂的可重用的框架需要文档化，但不需要数百页的UML，应该简明扼要

文档包括了重要模块高层结构的UML图、关系模型ER图、一两页系统构建说明、测试指导、源码控制指导等。并放到wiki之类的 __协作式写作工具__ 中，团队每个人都可以在屏幕上浏览，并在需要时进行搜索和更改

状态图
========
在描述有限状态机(FSM)方面，UML提供是丰富的符号，在GUI、通信协议以及任何基于事件系统中，FSM都可以使用

## 基础知识
下图展示了一个简单的状态迁移图(STD)，描绘了控制用户登录系统的FSM，

![effective_uml_19](../../imgs/effective_uml_19.png)

- 左上角的实心圆称为初始状态，PSM从这个伪状态开始，根据变迁规则，状态机一开始就迁移到`Prompting for Login`状态
- 在`Sending Password Failed`和`Sending Password Succeeded`状态外画了一个 __超状态__(superstate)，因为这两个状态都对`OK`事作做出反应，并都迁移到`Prompting for Login`状态，不想画两个完全一样的箭头，可使用这个手段

### 特定事件
状态图标的下层格间含有`event/action`对，`entry`和`exit`是标准事件，如果需要，可以提供自己的事件

![effective_uml_20](../../imgs/effective_uml_20.png)

如下图，自反迁移不仅会触发myAction，还会触发`exit`和`entry`动作

![effective_uml_21](../../imgs/effective_uml_21.png)

### 超状态
如登录的FSM那样，当许多状态以同样的方式响应某些不同的事件时，使用它

![effective_uml_22](../../imgs/effective_uml_22.png)

可以的画出子状态的迁移，__重写超状态的的迁移__，如下图（当然它们与继承关系不是等价的），

![effective_uml_23](../../imgs/effective_uml_23.png)

超状态有 __自己__ 的`entry`，`exit`及特定事件：

![effective_uml_24](../../imgs/effective_uml_24.png)

## 使用FSM图示
状态图用在理解那些 __已知__ 子系统的状态机方面非常有用，不过 __频繁变化__ 的系统，使用状态图时，随着时间的更改，图示的变化会成为一种负担。这时候可以考虑文本，它总是有地方来增加新的文本行，对于那些演化的系统，以 __文本方式创建状态迁移表STT__，而不是STD

如地铁旋转门的STD

![effective_uml_18](../../imgs/effective_uml_18.png)

通过状态图，可以研究系统在 __未预料到__ 的情形下该如何动作，如：当用户没有正当理由的情况下投入一枚硬币后，接着又投入了另一枚硬币

对应的STT为

<!--language: table-->

    |Current State |Event |New State  |Action |
    |--------------|------|-----------|-------|
    |Locked        |coin  |Unlocked   |Unlock |
    |Locked        |pass  |Locked     |Alarm  |
    |Unlocked      |coin  |Unlocked   |Refund |
    |Unlocked      |pass  |Locked     |Lock   |

基于此，可以使用状态模式来编写代码，甚至可以使用[SMC](http://www.objectmentor.com/resources/downloads.html)来生成相应代码（[本地下载](../../data/smcJava.zip)），显然比维护图示容易得多

## 代码实现
### 解析迁移表

<!--language: java-->

    import java.util.Vector;
    public class TurnStile{
        // States
        public static final int LOCKED = 0;
        public static final int UNLOCKED = 1;

        // Events
        public static final int COIN = 0;
        public static final int PASS = 1;

        int state = LOCKED;
        private TurnstileController turnstileController;
        private Vector transitions = new Vector();

        private interface Action{ void execute(); }

        private class Transition{
            int curState, event, newState; Action action;
            public Transition(int curState, int event, int newState, Action action){
                this.curState = curState;
                this.event = event;
                this.newState = newState;
                this.action = action;
            }

            public Turnstile(TurnstileController action){
                turnstileController = action;
                addTransition(LOCKED, COIN, UNLOCKED, unlock());
                addTransition(LOCKED, PASS, LOCKED, alarm());
                addTransition(UNLOCKED, COIN, UNLOCKED, fefund());
                addTransition(UNLOCKED, PASS, LOCKED, lock());
            }

            private void addTransition(int curState, int event, int newState, Action action){
                transitions.add(new Transition(curState, event, newState, action));
            }

            private Action unlock(){return new Action(){public void execute(){doUnlock();}};}
            private Action alarm(){return new Action(){public void execute(){doAlarm();}};}
            private Action fefund(){return new Action(){public void execute(){doRefund();}};}
            private Action lock(){return new Action(){public void execute(){doLock();}};}

            private void doUnlock(){turnstileController.unlock();}
            private void doAlarm(){turnstileController.alarm();}
            private void doRefund(){turnstileController.fefund();}
            private void doLock(){turnstileController.lock();}

            public void event(int event){
                for(int = 0; i < transitions.size(); i++){
                    Transition transition = (Transition)transitions.elementAt(i);
                    if(state == transition.curState && event == transition.event)
                        state = transition.newState;
                        transition.action.execute();
                }
            }
        }
    }

书中提及的优点是维护方便，缺点是：第一对于大型状态机而言，迁移表的遍历时间影响性能；第二，存在数量从多的小函数。其实对于缺点一，完全可使用哈希表；缺点二主要是因为语言的限制，java早期版本不支持lambda，所以对函数指针的引用，只能通过"接口+匿名类+转发"的方式曲线完成。以下以python为例，看一下代码的改进：

<!--language: !python-->

    class Turnstile:
        def __init__(self, ctrl):
            self.state = "LOCKED"
            self.trans = {}
            self.ctrl = ctrl
            self._key_format = "%s_%s"
            self._loadTrans()

        def _loadTrans(self):
            self._addTrans("LOCKED","COIN","UNLOCKED",self.ctrl.unlock)
            self._addTrans("LOCKED","PASS","LOCKED",self.ctrl.alarm)
            self._addTrans("UNLOCKED","COIN","UNLOCKED",self.ctrl.refund)
            self._addTrans("UNLOCKED","PASS","LOCKED",self.ctrl.lock)

        def _addTrans(self,curState,event,newState,action):
            _key = self._key_format % (curState,event)
            self.trans[_key] = (curState,event,newState,action)

        def event(self, event):
            _key = self._key_format % (self.state,event)
            if not self.trans.has_key(_key): raise Exception("...")
            _,__,self.state, _action = self.trans[_key]
            _action();

    class Ctrl:
        def unlock(self): print "unlock"
        def alarm(self): print "alarm"
        def refund(self): print "refund"
        def lock(self): print "lock"

    t = Turnstile(Ctrl())
    t.event("COIN"); print t.state
    t.event("PASS"); print t.state
    t.event("PASS"); print t.state


### 状态模式

![effective_uml_56](../../imgs/effective_uml_56.png)

<!--language: csharp-->

    public interface TurnstileState{
        void Coin(Turnstile t);
        void Pass(Turnstile t);
    }

    internal class LockedTurnstileState : TurnstileState{
        public void Coin(Turnstile t) {
            t.SetUnlocked();
            t.Unlock();
        }
        public void Pass(Turnstile t) {
            t.Alarm();
        }
    }

    internal class UnlockedTurnstileState : TurnstileState{
        public void Coin(Turnstile t) {
            t.Thankyou();
        }

        public void Pass(Turnstile t) {
            t.SetLocked();
            t.Lock();
        }
    }

    public class Turnstile{
        internal static TurnstileState lockedState =
          new LockedTurnstileState();

        internal static TurnstileState unlockedState =
          new UnlockedTurnstileState();

        private TurnstileController turnstileController;
        internal TurnstileState state = unlockedState;

        public Turnstile(TurnstileController action){
            turnstileController = action;
        }

        public void Coin(){
            state.Coin(this);
        }
        public void Pass(){
            state.Pass(this);
        }

        public void SetLocked(){
            state = lockedState;
        }
        public void SetUnlocked(){
            state = unlockedState;
        }

        public bool IsLocked(){
            return state == lockedState;
        }
        public bool IsUnlocked(){
            return state == unlockedState;
        }

        internal void Thankyou(){
            turnstileController.Thankyou();
        }
        internal void Alarm(){
            turnstileController.Alarm();
        }
        internal void Lock(){
            turnstileController.Lock();
        }
        internal void Unlock(){
            turnstileController.Unlock();
        }
    }




对象图
=======
## 即时快照
展示了在系统执行的某处特定时候的一组对象和关系，看作是一个 __内存的快照__。
当需要展示系统在某个特定时刻或者某个特定状态下的内部结构时，对象图很有用的，描绘了某些类和关系将要被使用的方式，有助于展示系统是如何随着各种输入而变化的。当系统的结构是 __动态构建__ 起来而不是静态的类结构决定时，__但大多数情况下，都可以从相应的类图中直接推导出来，因此没有多少用途__

## 主动对象
在 __多线程__ 系统中，对象图也非常有用，比如下面的代码，允许去编写`socket`服务器，而无需关心那些和`socket`相关的讨厌的线程和同步问题：

<!--language: csharp-->

    using System.Collections;
    using System.Net;
    using System.Net.Sockets;
    using System.Threading;

    namespace SocketServer {
        public interface SocketService {
            void Serve(Socket s);
        }

        public class SocketServer {
            private TcpListener serverSocket = null;
            private Thread serverThread = null;
            private bool running = false;
            private SocketService itsService = null;
            private ArrayList threads = new ArrayList();

            public SocketServer(int port, SocketService service) {
                itsService = service;
                IPAddress addr = IPAddress.Parse("127.0.0.1");
                serverSocket = new TcpListener(addr, port);
                serverSocket.Start();
                serverThread = new Thread(new ThreadStart(Server));
                serverThread.Start();
            }

            public void Close() {
                running = false;
                serverThread.Interrupt();
                serverSocket.Stop();
                serverThread.Join();
                WaitForServiceThreads();
            }

            private void Server() {
                running = true;
                while (running) {
                    Socket s = serverSocket.AcceptSocket();
                    StartServiceThread(s);
                }
            }

            private void StartServiceThread(Socket s) {
                Thread serviceThread =
                    new Thread(new ServiceRunner(s, this).ThreadStart());
                lock (threads) {
                    threads.Add(serviceThread);
                }
                serviceThread.Start();
            }

            private void WaitForServiceThreads() {
                while (threads.Count > 0) {
                    Thread t;
                    lock (threads) {
                        t = (Thread) threads[0];
                    }
                    t.Join();
                }
            }

          internal class ServiceRunner {
              private Socket itsSocket;
              private SocketServer itsServer;

              public ServiceRunner(Socket s, SocketServer server) {
                  itsSocket = s;
                  itsServer = server;
              }

              public void Run() {
                  itsServer.itsService.Serve(itsSocket);
                  lock (itsServer.threads) {
                      itsServer.threads.Remove(Thread.CurrentThread);
                  }
                  itsSocket.Close();
              }

              public ThreadStart ThreadStart() {
                  return new ThreadStart(Run);
              }
          }
        }
    }

它的类图如下，但很难看出这段代码的意图：

![effective_uml_25](../../imgs/effective_uml_25.png)

但再看下面的对象图，该图对结构的表达要比类图好得多

![effective_uml_26](../../imgs/effective_uml_26.png)

`SocketServer`持有`serverThread`，并且`serverThread`运行在一个名为`Server()`的代理中，还展示出`serverThread`负责创建所有的`ServiceRunner`实例

- 围绕着`Thread`实例的粗体边框，代表 __主动对象__(active object)，主动对象 __管理着一个控制线程__。它们具有用来控制线程的方法，如`Start`,`Abort`,`Sleep`等，这幅图中所有主动对象都是`Thread`的实例，所有的处理都是在代理中完成的，`Thread`的实例持有对这些代理的引用。


用例
=========
用例是一种非常好的思想，但有时更多的关注形式而非内容，在前置条件、后置条件、主参与者、辅助参与者等一堆根本不重要的事情上争论不休

- __保持用例简单，不用担心用例的格式，不要尝试记录所有用例，那是不可能完成的任务__
- 关于用例，有一点要牢记，就是“明天，它们将会变化”，那么就不必今天就记录下它的细节，要做的是把细节记录推迟到最后一刻。

## 编写用例
- 用例 __不是图示__，是从一个特定视角进行编写关于 __行为需求__ 的 __文本描述__
    - 用例图是图示，但根本没有包含任何关于行为需求的信息
- 用例是对系统行为的描述，描述从让系统完成一些特定工作的 __用户的视角__ 编写的，记录了系统响应 __单个用户__ 刺激所经历的 __可视事件序列__
    - 它属于操作层人员关心的，属于具体需求
    - 可视事件指用户能够看得到的事件。用例根本不用描述看不见的行为，也不描述那些看不到的系统机制，只描述用户能够看得到的东西。

### 基本流程
一个销售终端系统的一个典型用例

- 卖出商品
    1. 收银员在扫描器上划过商品，扫描器读取UPC码
    1. 商品的价格、描述以及当前价格总数出现在朝向顾客的显示器上。价格和描述也出现在收银员的屏幕上
    1. 价格和描述打印在收条上
    1. 系统发出可以听到的"确认"声音以通知收银员UPC码正确读取

- 这是一个用例的基本流程，不需要任何更复杂的东西，
- 事实上，如果用例不是一会儿就要实现，那么即使是上面几个简单的步骤可能也过于详细了，__如果用例不需要在几天或者一周内就要实现，不要去记录这些细节__
- 如果没有记录用例的细节，那记录什么？用例的名字即可，在电子表格中保持用例名字的清单，当接近实现时填入细节

### 备选流程

- 无法读取UPC码
    1. 如果扫描器无法读取UPC码，系统应该发出"重新扫描"声音，以通知收银员再试一次。如果重试三次仍然失败，那么收银员应该手工输入UPC码

- 没有UPC码
    1. 如果商品上没有UPC码，那么收银员应该手工输入价格

### 其他东西
前置条件、后置条件、主参与者、辅助参与者是怎么回事？不必担心这些东西，__大多数系统而言，都不必知道这些内容__

## 用例图
除了表达系统边界外，几乎没什么用，而用例的关系，如`<<extends>>`,`<<generalization>>`反而容易成为争论的焦点

顺序图
========
不要给每个类的每个方法去创建顺序图，这是极大的浪费时间。__只有当你需要立即向某个人解释一组对象的协作方式或者自己想要把这种协作关系可视化时，才使用顺序图__。

## 基本知识

### 对象,生命线,消息等

![effective_uml_27](../../imgs/effective_uml_27.png)

- 顶部显示了协作中涉及的对象和类，__对象的名字下面有下划线，而类没有__
- 左边的人形线条（参与者）表示一个 __匿名对象__，它是协作中消息的源和接收者，不是所有顺序图都有一个匿名参与者
- 从对象和参与者垂下来的虚线称为 __生命线__(life line)
- 从一个对象发送到另一个对象的 __消息__ 显示为两条生命线之间的箭头线，每个消息都标记有消息的名字，参数要么出现在消息名后面的括号中，要么和 __数据标记__(data token) 相邻（尾端带有圆圈的小箭头线）
- __时间轴__ 是垂直方向的，消息出现的位置越低，就越晚发送
- `Page`对象生命线上的窄条小矩形称为 __激活__(activation)，它是可选的，大部分的图不需要它，它表示了一个函数的执行时间。本例中显示了`Login`函数的运行时长
- 那条没有标注的虚箭头表示`Login`函数返回到参与者并传回返回值
- `GetEmployee`消息中的`e`变量，它代表`GetEmployee`的返回值，同样请注意`Employee`对象的名字也是`e`，它们是同一个对象，`GetEmployee`的返回值就是指向`Employee`对象的引用
- `EmployeeDB`是一个类，名字下面没有下划线，意味着`GetEmployee`是一个静态方法

### 创建和析构

- 以表示一个对象的创建

![effective_uml_28](../../imgs/effective_uml_28.png)

对应的代码是：

<!--language: csharp-->

    public class ShapeFactory {
        public Shape MakeSquare() {
            return new Square();
        }
    }

- 下面是销毁对象，有垃圾回收机制的语言无需显示指定，但C++则需要显式析构

![effective_uml_29](../../imgs/effective_uml_29.png)

<!--language: csharp-->

    public class TreeMap {
        private TreeNode topNode;
        public void Clear() {
            topNode = null;
        }
    }

### 简单循环
![effective_uml_30](../../imgs/effective_uml_30.png)

但是试图在顺序图中表示算法是不明智的，顺序图是用来提示对象之间的连接，而不是一个算法的详细细节

### 时机和场合
不要绘制大量对象和消息的顺序图，没有人能够理解，也没有人愿意看，__对于UML图来说，共同点要比差异重要得多，使用图形来展示 *公共的主题和实践* __。当代码可以清楚的表达自己时，图示就是多余的

下面就是多余的过度复杂的顺序图

![effective_uml_31](../../imgs/effective_uml_31.png)

其实它的代码反而更容易理解

<!--language: csharp-->

    public class Payroll {
        private PayrollDB itsPayrollDB;
        private PaymentDisposition itsDisposition;
        public void DoPayroll() {
            ArrayList employeeList = itsPayrollDB.GetEmployeeList();
            foreach (Employee e in employeeList) {
                if (e.IsPayDay()) {
                    double pay = e.CalculatePay();
                    double deductions = e.CalculateDeductions();
                    itsDisposition.SendPayment(pay - deductions);
                }
            }
        }
    }

- 首先致力于编写具有表达力的代码
- 如果顺序图是必须的，能否把它分成一小组场景，将上面的大顺序图分解成几个小一些的、更加易读的顺序图

![effective_uml_32](../../imgs/effective_uml_32.png)

- 考虑下，我们需要上面这样的低层操作细节吗，还是要系统全局流程的 __高层视图__，后者可能更有用些

![effective_uml_33](../../imgs/effective_uml_33.png)

## 高级概念
### 循环和条件

![effective_uml_34](../../imgs/effective_uml_34.png)

- `payEmployee`消息前面有一个循环表达式`*[foreach id in idList]`，星号表示这是一个迭代；消息会被重复发直到中括号中的__监护__(guard)表达式为`false`
- `payEmployee`消息终止在一个激活矩形上，该矩形和第一个矩形有所偏移，表示现在有同一个对象的两个函数在执行，因为`payEmployee`消息是循环的，因此第二次激活也是循环的。

- 靠近`[payday]`监护条件的激活矩形，表示一个`if`语句，仅当条件为真时，第二个激活才获取控制

- 其实还是上面的代码是更好的表达算法的方式

### 耗费时间的消息
有些时候，发消息是要花时间的

正常的电话呼叫

![effective_uml_35](../../imgs/effective_uml_35.png)

失败的电话呼叫

![effective_uml_36](../../imgs/effective_uml_36.png)

上面两个箭头线的交叉点称为竞争条件，当两个异步实体可以同时调用不相容的方法时，就会出现竞争条件，telco调用了`ring`操作，而callee却摘了机，在这一刻，所有实体所理解的系统状态是不同的：caller正在等待 "Hello"，而telco认为它的工作已经完成，而callee则在等待拨号音

### 异步消息
在分布式或多线程系统中，发送消息的对象可以立即要回控制权，而接收消息的对象则在另外一个控制线程中执行，称为异步消息，以开放箭头表示

![effective_uml_37](../../imgs/effective_uml_37.png)
![effective_uml_38](../../imgs/effective_uml_38.png)

### 多线程
异步消息隐含着多个控制线程，通过在每个消息名上标记上线程标识符来在UML图中展示多个同的控制线程，线程标识符不必对应于代码中的名字。

![effective_uml_39](../../imgs/effective_uml_39.png)

### 主动对象
表示具有独立内部线程的对象，它显示为粗体边框

![effective_uml_40](../../imgs/effective_uml_40.png)

### 向接口发送消息

- 向接口发送消息

![effective_uml_41](../../imgs/effective_uml_41.png)

- 通过接口向其派生类型发送消息

![effective_uml_42](../../imgs/effective_uml_42.png)


## 顺序图vs协作图


![effective_uml_16](../../imgs/effective_uml_16.png)

- 描绘了`TreeMap`的`Add`方法是如何实现的
- 人形线条图表示了一个未知调用者，调用了`TreeMap`对象的`Add`方法。如果`topNode`变量为`null`，`TreeMap`就创建一个新的`TreeMapNode`对象并把它赋给`topNode`，否则，`TreeMap`就向`topNode`发送`Add`消息
- 方括号中的布尔表达式称为 __监护条件__(guard)，它们指示出应该选择哪条路径。
- 终结点在`TreeMapNode`图标上的消息箭头表示 __对象构造__。
- 带有小圆圈的箭头称为 __数据标记__(data token)，本例中，描述了对象构造的参数
- `TreeMap`下面的窄矩形条称为 __激活__(activation)，表示`add`方法执行了多少时间


![effective_uml_17](../../imgs/effective_uml_17.png)

- 描绘了`TreeMap.Add`中`topNode`不会`null`的情况
- 协作图包含了顺序图中所包含的 __同样的信息__
- 顺序图是为了清楚的表达出 __消息的顺序__，而协作图为了清楚地表达出 __对象之间的关系__
- 对象被称为链(link)的关系连接起来。只要一个对象可以向另外一个对象发送消息，就存在链关系，在链之上传递的正是消息本身，表示为一些小箭头。
- 消息上标记有消息名称、消息顺序号以及任何使用的监护条件
- 带点的顺序号表示 __调用的层次结构__。`TreeMap.Add`（消息1）调用了`TreeMapNode.Add`（消息1.1），因此，消息1.1 是 消息1 所调用的第一条消息




类图
========
评估系统的 __依赖结构__ 方面：

- 使用图示要比源代码容易得多，图形可视地显现出某些依赖关系
- 可以看到依赖关系环，可以决定如何以最好的方法把它们解除掉
- 可以看到何时抽象类依赖于具体类的情况，可以决定重新调整这种依赖的策略

## 基础知识

![effective_uml_14](../../imgs/effective_uml_14.png)

- 矩形表示类，箭头表示关系
- 本图中，所有关系都是关联(association)关系，关联是简单的数据关系，其中一个对象或者持有对另外一个对象的引用，或者调用了其方法
- 一般来说，和箭头相邻的数字表示该关系所包含的实例个数。如果数字比1大，意味着某种容器。
- 类图标中可以分成多个格间，最上面存放类的名字，其他描述属性与方法
- `<<interface>>`符号用来说明`IComparable`是一个接口
- 这里显示的大部分符号都是可选的
- 请注意 __关联关系是如何与实例变量对应__ 起来的，如从`TreeMap`到`TreeMapNode`的关联称为`topNode`，其对应于`TreeMap`中的`topNode`变量

### 关联
![effective_uml_43](../../imgs/effective_uml_43.png)

![effective_uml_44](../../imgs/effective_uml_44.png)

### 继承
- 继承关系

![effective_uml_45](../../imgs/effective_uml_45.png)

- 实现关系

![effective_uml_46](../../imgs/effective_uml_46.png)

- 棒棒糖状接口表示

![effective_uml_47](../../imgs/effective_uml_47.png)

### 示例
![effective_uml_48](../../imgs/effective_uml_48.png)

- 从图中可以看出`WithdrawalTransaction`使用了`CashDispenser`接口，很清楚，系统中有些类实现它，但在这幅图中，我们 __不关心哪个类__

- 没有十分详细的描绘各个UI接口的中的方法，`WithdrawalUI`所需的方法要比图中多，但全部放上去就会使图变得混乱。只在图中提供一些代表性的方法，向读者传递设计思路，这才是真正必要的。
- __横向关联和纵向关联的约定__，事务及其动作在图的左侧，各种UI接口都在右侧，UI的实现在底部；一方面三个关联关系都有一致的指向，另一方面三个继承关系，都合并到一条线上。

## 细节
UML类图可以加入许多细节和修饰，大多数情况下不应该加入的

### 类衍型

- 接口

![effective_uml_49](../../imgs/effective_uml_49.png)

- 静态工具类

![effective_uml_50](../../imgs/effective_uml_50.png)

如果想的话，可以自己创建，如`<<struct>>`,`<<C-API>>`,`<<function>>`,`<<presistent>>`等衍型，只要确保阅读你图示的人理解含义就行

### 聚合与组合
对于C++程序员会比较关注，因为组合的所有者负责所有物的生存期，所有者析构，其所有物必须一起析构，如果所有者被复制了，其所有物必须随它一起复制

而有垃圾回收机制的语言，两者区分不大。

### 多重性
对象可以含有其他对象的数组或集合，也可以在不同的实例变量中持有许多其他同类对象

![effective_uml_51](../../imgs/effective_uml_51.png)

### 关联衍型

![effective_uml_52](../../imgs/effective_uml_52.png)

`<<delegate>>`代表源类把一个成员函数调用转交给目标，许多设计模式使用了：PROXY、DECORATOR、COMPOSITE

### 内嵌类

![effective_uml_53](../../imgs/effective_uml_53.png)

### 关联类
虽然多重性关联告诉我们源和许多目标实际相连，但从图中无法看到使用了哪种容器类，可通过使用关联类描绘这一点

![effective_uml_54](../../imgs/effective_uml_54.png)

### 关联修饰符

![effective_uml_55](../../imgs/effective_uml_55.png)

通过`empid`成员变量，持有关于`Employee`的数据库键值




