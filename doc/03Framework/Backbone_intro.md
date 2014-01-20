引言
=====
在没有前端MVC/MVVM之类的框架时，请思考：

- 将所有逻辑功能放在UI响应事件中，还是定义出逻辑概念上的模型？
- 定义出的模型，如果需要依赖UI去做某些响应，该怎么做的？
- UI状态复杂时，依靠什么机制保证一致性？（状态机？）
    - 比如一个List编辑
    - __Normal模式__：new/edit/destroy按钮有效，create/update/cancel按钮失效
    - __New模式__：点击new按钮，new/update/edit/destory按钮失效，create/cancel按钮有效，直至点create/cancel按钮恢复到 __Normal模式__
    - __Edit模式__：选中某行点击edit按钮，new/create/edit/destory按钮失效，update/cancel按钮有效，直至点update/cancel按钮恢复到 __Normal模式__
    - __New模式__ 无法直接进入 __Edit模式__，反之亦然

- 能支持RESTful风格吗？


模型model
==========
在backbone中model是application的核心，包含了 __交互数据__ 、基于这些数据的 __逻辑__ [通信、验证、计算的属性、访问控制等]

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        initialize: function(){
            alert("Welcome to this world");
        }
    });
    var person = new Person;

`initialize`是构造方法，当new一个实例（基于model,view,collections）时被触发

## 设置属性
当需要为某些实例设置属性时:

- 在new时传入一个js对象;
- 或使用`model.set()`

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({});
    var person = new Person({ name: "Thomas", age: 67});
    alert(person.get("age"));
    delete person;

    // or we can set afterwards, these operations are equivelent
    var person = new Person();
    person.set({ name: "Thomas", age: 67});
    alert(person.get("age"));

## 设置model的默认属性
使用`defaults`

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        defaults: {
            name: "Fetus",
            age: 0,
            children: []
        }
    });
    var person = new Person();
    alert(person.get("age")); // 0
    alert(person.get("name")); // "Fetus"
    alert(person.get("children")); // []
    delete person;

    var person = new Person({ name: "Thomas", age: 67, children: ["Ryan","Bob"]});
    alert(person.get("age")); // 67
    alert(person.get("name")); // "Thomas"
    alert(person.get("children")); // ["Ryan","Bob"]

## 获取属性
使用`model.get()`可以访问到model的属性，参考上例

## 设置model方法
model可以包含多个自定义的方法，而且默认这些方法是public的

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        defaults: {
            name: "Fetus",
            age: 0,
            children: []
        },
        initialize: function(){
        },
        adopt: function( newChildsName ){
            this.get("children").push( newChildsName );
        }
    });
    var person = new Person({ name: "Thomas", age: 67, children: ["Ryan"]});
    person.adopt("John Resig");
    alert(person.get("children")); // ["Ryan", "John Resig"]

## 监听model的改变
所有属性能够被侦测到值的改变，这是backbone一个非常有用的部分，下例演示当"person"的name改变时，新的name将被alert，不能监听到model的上述 __自定义方法__ 的change，除非方法定义在attributes中

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        defaults: {
            name: "Fetus",
            age: 0,
            children: []
        },
        initialize: function(){
            this.bind("change:name", function(model){
                // var name = model.get("name"); // "Stewie Griffin"
                var name = this.get("name"); // "Stewie Griffin"
                var oldname = this.previous("name"); // "Thomas"
                alert(oldname + " to " + name );
            });
        },
        replaceNameAttr:function( name ){
            this.set({ name: name });
        }
    });
    var person = new Person({ name: "Thomas", age: 67, children: ["Ryan"]});
    person.replaceNameAttr("Stewie Griffin"); // This triggers a change and will alert()

当使用`this.bind("change",function(){})`将监听该model的所有属性，参考[bind/on方法](http://backbonejs.org/#Events-on)

## 保存、获取及删除

### toJSON取得所有属性

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    var person = new Backbone.Model({ name: "Thomas", age: 67, children: ["Ryan"]});
    var attributes = person.toJSON(); // { name: "Thomas", age: 67, children: ["Ryan"]}
    console.log(attributes);

也可以：

<!-- language: js -->

    var attributes = person.attributes;

但该方法是直接引用model的属性，修改时必须小心，推荐使用`set()`来改变

### 在set或save之前进行验证
使用`validate`来验证model，返回`string`表示抛出异常，v1.0之前在`set/save`时是默认触发，v1.0起，只有`save`默认触发，在`set`时也想触发验证，需使用`{"validate":true}`，记得先要绑定`invalid`事件。在`set`时不加`{"validate":true}`时，还可以再以`model.isValid`来验证是否通过

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        // If you return a string from the validate function,
        // Backbone will throw an error
        validate: function( attributes ){
            if( attributes.age < 0 && attributes.name != "Dr Manhatten" ){
                return "You can't be negative years old";
            }
        },
        initialize: function(){
            this.bind("invalid", function(model, error){
                // We have received an error, log it, alert it or forget it :)
                alert( error );
            });
        }
    });

    var person = new Person;
    person.set({ name: "Mary Poppins", age: -1 }, {"validate":true});
    // Will trigger an alert outputting the error
    delete person;
    var person = new Person;
    person.set({ name: "Dr Manhatten", age: -1 }, {"validate":true});
    // God have mercy on our souls


### 保存对象
首先需要为对象定义一个url属性，调用`save`方法时会 __`POST/PUT`__ (当model.`isNew`则在save时使用`POST`，否则使用`PUT`)对象的所有属性到server端，如果想 __部分更新model的属性__，可使用model.`save(attrs, {patch: true})`，将发送 __`PATCH`__ 请求到server

<!-- language: !js -->

    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    Person = Backbone.Model.extend({
        url: "/person",
        validate: function( attributes ){
            if( attributes.age < 0 && attributes.name != "Dr Manhatten" ){
                return "You can't be negative years old";
            }
        },
        initialize: function(){
            this.bind("invalid", function(model, error){
                alert( error );
            });
        }
    });

    var person = new Person;
    person.set({ name: "Mary Poppins", age: -1 });//不需要 {"validate":true}
    person.save();//将触发invalid事件

    person.set({ name: "Dr Manhatten", age: -1 });
    person.save();//将POST {"name":"Dr Manhatten","age":-1} 到 url,需查看console

save方法支持success(model, response, options)和error(model, xhr, options)的回调

<!-- language: js -->

    person.save({author: "Teddy"},{success:function(){},error:function(){}});

    //当不重置属性时，需如下代码
    person.save({},{success:function(){},error:function(){}});


### 获取对象

<!-- language: js -->

    var person = new Person;//model同上面定义
    person.fetch(); //GET请求model中定义url

    person.fetch({url:"/getperson"});//GET请求指定url，返回单个json对象

fetch方法同样支持success与error的回调，如果是collection，则使用collection.fetch，注意后端 __返回是json数组__(下面有说明)

### 删除对象

<!-- language: js -->

    book.destroy({success: function(model, response) {
        ...
    }});

destroy方法同样支持success与error的回调

服务器的异步操作都是通过`Backbone.sync`这个方法来完成的，调用这个方法的时候会自动的传递一个参数过去，根据参数向服务器端发送对应的请求。比如你save，backbone会判断你的这个对象是不是新的，如果是新创建的则参数为create，如果是已存在的对象只是进行了改变，那么参数就为update，如果你调用fetch方法，那参数就是read，如果是destory，那么参数就是delete。也就是所谓的CRUD ("create", "read", "update", or "delete")，而这四种参数对应的请求类型为POST，GET，PUT，DELETE。可以在服务器根据这个request类型，来做出相应的CRUD操作（后端使用Rails会更方便）。

### 关于urlRoot

<!-- language: !js -->

    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    var Book = Backbone.Model.extend({urlRoot : "/books"});
    var solaris = new Book({id: "1083-lem-solaris"});
    alert(solaris.url());
    solaris.fetch(); //GET "/books/1083-lem-solaris"

- create → POST   `/books`
- read → GET   `/books[/id]`
- update → PUT   `/books/id`
- delete → DELETE   `/books/id`

设置了urlRoot之后，你发送PUT和DELETE请求的时候，其请求的url地址就是：/books/[model.id]，这样你就可以在服务器端通过对url后面值的提取更新或者删除对应的对象

模型集合collection
===================
collection是一个包含model的有序的集合，所以它可在以下场合中被用到：

- Model: Student, Collection: ClassStudents
- Model: Todo Item, Collection: Todo List
- Model: Animals, Collection: Zoo

一般情况下，collection中的元素属于同类型的model，但是model可以属于多个collection

<!-- language: js -->

    var Song = Backbone.Model.extend({
        initialize: function(){
            console.log("Music is the answer");
        }
    });
    var Album = Backbone.Collection.extend({
        model: Song
    });

## 创建一个collection

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    var Song = Backbone.Model.extend({
        defaults: {
            name: "Not specified",
            artist: "Not specified"
        },
        initialize: function(){
            console.log("Music is the answer");
        }
    });
    var Album = Backbone.Collection.extend({
        model: Song
    });
    var song1 = new Song({ name: "How Bizarre", artist: "OMC" });
    var song2 = new Song({ name: "Sexual Healing", artist: "Marvin Gaye" });
    var song3 = new Song({ name: "Talk It Over In Bed", artist: "OMC" });
    var myAlbum = new Album([ song1, song2, song3]);
    console.log( myAlbum.models ); // [song1, song2, song3]

## 操作collection

<!-- language: js -->

    var myAlbum = new Album;
    myAlbum.add(song1);
    myAlbum.add(song2);
    myAlbum.add(song3);
    myAlbum.remove(song3);

    //基于underscore，可以使用each的方法获取collection中的数据
    myAlbum.each(function (song) {
        console.log((song.get("name"));
    });
    myAlbum.model[0].get("name"));

## 获取服务端数据

<!-- language: js -->

    myAlbum.fetch({
        url: "/songs/", success: function (collection, response) {
            collection.each(function (song) {
                console.log(song.get("name"));
            });
        }, error: function () {
            alert("error");
        }
    });

返回格式为json数组 [{"name":"a","artist":"a"},{"name":"b","artist":"b"}]

## 绑定reset方法
这个方法的时候是要和上面的fetch进行配合的，__collection在fetch到数据之后，会调用reset方法__，所以你需要在collection中定义reset方法或者是绑定reset方法，步骤要在fetch之前进行

<!-- language: js -->

    myAlbum.bind("reset",showAllSongs);
    showAllSongs = function(){
        myAlbum.each(function(song){
            ​//将song数据渲染到页面。
        });
    }

模型事件events
==============
Events 是一个可以被mix到任意对象的模块，它拥有让对象绑定和触发自定义事件的能力。 事件在被绑定之前是不需要事先声明的，还可以携带参数

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    var object = {};
    _.extend(object, Backbone.Events);
    object.on("alert", function(msg) {
        alert("Triggered " + msg);
    });
    object.trigger("alert", "an event");

## on/bind
`object.on(event, callback, [context])` Alias: `bind`

- 监听以空格分隔的多个事件列表

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    var book=new Backbone.Model();
    book.on("change:title change:author", function(){
        console.log(arguments);
        alert(arguments[1]);
    });
    book.set("title","backbone's guide");

- 指定someobj.render方法中this的指向

<!-- language: !js -->

    //import underscore.1.4.3
    //import backbone.1.0.0
    var model=new Backbone.Model();
    var someobj = {render: function(){
        alert(this.get("title"));
    }};
    model.on("change", someobj.render, model);
    model.set("title","backbone's guide");

- 特殊事件"all"，监听所有事件

<!-- language: js -->

    proxy.on("all", function(eventName) {
        object.trigger(eventName);
    });

- 支持键值对的写法，方便设置不同事件的callback

<!-- language: js -->

    book.on({
        "change:title": titleView.update,
        "change:author": authorPane.update,
        "destroy": bookView.remove
    });

## off/unbind
`object.off([event], [callback], [context])` Alias: `unbind`

<!-- language: js -->

    // Removes just the `onChange` callback.
    object.off("change", onChange);

    // Removes all "change" callbacks.
    object.off("change");

    // Removes the `onChange` callback for all events.
    object.off(null, onChange);

    // Removes all callbacks for `context` for all events.
    object.off(null, null, context);

    // Removes all callbacks on `object`.
    object.off();

## trigger
`object.trigger(event, [*args])`
触发 event 事件的回调函数，后续传入 trigger 的参数会被依次传入事件回调函数

## once
`object.once(event, callback, [context])`
只触发一次 event 事件的回调函数

## listenTo
`object.listenTo(other, event, callback)`
v0.9.9新增功能，__建议使用它__
取代`other.on(event, callback)`，listenTo使该view能够跟踪到object的相关events，并且这些事件响应能够被一次性全部remove掉，但不影响其它view对object的监听

<!-- language: js -->

    model.on("change",view.render);
    view.listenTo(model, "change", view.render);

## stopListening
`object.stopListening([other], [event], [callback])`，使object停止监听events，不传参代表remove该object所有registered callbacks（不会影响其它视图对该model的监听），或在以下指定情况下停止监听：指定的object，指定的event，指定的callback

<!-- language: js -->

    view.stopListening();
    view.stopListening(model);


视图view
=========

Backbone的view是反映数据模型的 __外观__ ，也被用来监听 __事件__ 和做出相应的 __反应__ 。事件包括 __模型的__(主要在初始化时`listenTo`模型)与 __DOM的__(利用`events`属性来定义，下有示例)

<!-- language: !js -->

    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    SearchView = Backbone.View.extend({
        initialize: function(){
            alert("Alerts suck.");
        }
    });
    // The initialize function is always called when instantiating a Backbone View.
    // Consider it the constructor of the class.
    var search_view = new SearchView;


## "el"属性
`el`属性引用DOM元素，每一个view有一个`el`属性，它根据view的 `tagName`, `className`, `id` 和 `attributes` 属性去产生DOM元素, 如果什么都未定义则产生一个 empty div.

制造`el`属性是div#search_container的view

<!-- language: js -->

    var search_view = new SearchView({ el: $("#search_container") });

注意，当绑定了容器元素时，__任何DOM事件的触发都必须在该元素内__

## 加载模板
`render()`函数用来呈现el元素的表现，
由于Backbone依赖`underscore.js`，则可以使用该库的[_.template()方法](http://documentcloud.github.com/underscore/#template)来加载与渲染

<!-- language: web -->

    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    SearchView = Backbone.View.extend({
        initialize: function(){
            this.render();
        },
        render: function(){
            //Pass variables in using Underscore.js Template
            var variables = { search_label: "My Search" };
            // Compile the template using underscore
            var template = _.template( $("#search_template").html(), variables );
            // Load the compiled HTML into the Backbone "el"
            $(this.el).html(template);
        }
    });

    var search_view = new SearchView({ el: $("#search_container") });

    <!-- language: html -->
    <div id="search_container"></div>

    <script type="text/template" id="search_template">
        <label><%= search_label %></label>
        <input type="text" id="search_input" />
        <input type="button" id="search_button" value="Search" />
    </script>


## 监听DOM元素事件
此事件指DOM元素的事件（与模型事件无关），使用`events`属性去监听view，注意只能监听到`el`容器的子元素（容器元素本身的不能被监听），格式为`{"event selector": "callback"}`

<!-- language: web -->

    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    SearchView = Backbone.View.extend({
        initialize: function(){
            this.render();
        },
        events: {
            "click input[type=button]": "doSearch"
        },
        doSearch: function( event ){
            // Button clicked, you can access the element that was clicked with event.currentTarget
            alert( "Search for " + $("#search_input").val() );
        },
        render: function(){
            var variables = { search_label: "My Search" };
            var template = _.template( $("#search_template").html(), variables );
            $(this.el).html(template);
        }
    });

    var search_view = new SearchView({ el: $("#search_container") });

    <!-- language: html -->
    <div id="search_container"></div>

    <script type="text/template" id="search_template">
        <label><%= search_label %></label>
        <input type="text" id="search_input" />
        <input type="button" id="search_button" value="Search" />
    </script>

路由router
===========

## 简述
使用`hash tags(#)`路由到application的URL，router也用于，application/feature需要URL的routing / history的功能，类似 [__Sammy.js__](http://sammyjs.org/)。

1. 功能由url#hash来表示，而DOM元素，比如a标签，href指向该url#hash，或在button的click事件中调用router.navigate()；
1. 当url#hash变化时，将调用相应的回调函数，通过调用模型中方法，则功能被调用；
1. 功能的使用能形成记录存入history

定义router时，至少要定义一个route以及它映射的function，路由解释url中`#`后面的部分，所有application中的link必须类似"#/action"或"#action" `"http://example.com/#/user/help"`

<!-- language: web -->

    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    var AppRouter = Backbone.Router.extend({
        routes: {
            "*actions": "defaultRoute" // matches http://example.com/#anything-here
        },
        defaultRoute: function( actions ){
            // The variable passed in matches the variable in the route definition "actions"
            alert( actions );
        }
    });
    // Initiate the router
    var app_router = new AppRouter;
    // Start Backbone history a neccesary step for bookmarkable URL's
    Backbone.history.start();

    $('input[type=button]').click(function(){
        //  If you wish to also call the route function, set the trigger option to true. To update the URL without creating an entry in the browser's history, set the replace option to true.
        app_router.navigate("help/troubleshooting", {trigger: true});
        // app_router.navigate("help/troubleshooting", {trigger: true, replace: true});
    });

    <!-- language: html -->
    <a href="#action">#action</a><br/>
    <a href="#/route/action">#/route/action</a><br/>
    <a href="#help/readme">help</a><br/>
    <input type="button" value="click"/>


需要通过调用Backbone.history.start()方法来初始化这个Router。
在v0.5以前，router被称作controller，为了避免困惑而改的名。

## 动态路由及传参
大多数框架允许在定义路由时混合使用静态和动态路由，比如需要根据友好URL中的一个变量来获取一个post，`"http://example.com/#/posts/12"`

<!-- language: web -->

    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    var AppRouter = Backbone.Router.extend({
        routes: {
            "posts/:id": "getPost",
            "*actions": "defaultRoute" // Backbone will try match the route above first
        },
        getPost: function( id ) {
            // Note the variable in the route definition being passed in here
            alert( "Get post number " + id );
        },
        defaultRoute: function( actions ){
            alert( actions );
        }
    });
    var app_router = new AppRouter;
    Backbone.history.start();

    <!-- language: html -->
    <a href="#posts/120">120</a><br/>
    <a href="#/posts/130">130</a>

## 动态路由之":params"和"*splats"
- ":params"匹配URL中两`/`中的内容

`"search/:query/p:page"`将匹配`#search/obama/p2`，并将`"obama"`和`"2"`传给action

- "*splats"匹配任何数量的URL片断

`"file/*path"`将匹配`#file/nested/folder/file.txt`，并将`"nested/folder/file.txt"`传给action

- 可选匹配"(/:optional)"

`"docs/:section(/:subsection)"`将匹配`#docs/faq` 和 `#docs/faq/installing`，在第一种情况下将 `"faq"`传给action，在第二种情况下将`"faq"` 和 `"installing"` 传给action

<!-- language: web -->

    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0
    var AppRouter = Backbone.Router.extend({
        routes: {
            "posts/:id": "getPost",
            "docs/:section(/:subsection)": "docs",
            "download/*path": "downloadFile",
            ":route/:action": "loadView",

        },
        getPost: function( id ){
            alert("getPost: "+id); // 121
        },
        docs: function(sec,sub){
            alert("docs: "+sec+"_"+sub);
        },
        downloadFile: function( path ){
            alert("downloadFile: "+path); // user/images/hey.gif
        },
        loadView: function( route, action ){
            alert("loadView: "+route + "_" + action); // dashboard_graph
        }
    });

    var app_router = new AppRouter;
    Backbone.history.start();

    <!-- language: html -->
    <a href="#posts/121">Example</a><br/>
    <a href="#docs/faq">Faq</a><br/>
    <a href="#docs/faq/installing">Faq Installing</a><br/>
    <a href="#download/user/images/hey.gif">Download</a><br/>
    <a href="#dashboard/graph">Load Route/Action View</a>


示例
====

## ToDo List
[__App__](http://backbonejs.org/examples/todos/index.html)
[__Doc__](http://backbonejs.org/docs/todos.html)
[__Src__](https://github.com/documentcloud/backbone)
[__更多TODO的例子__](http://todomvc.com)

### 单个模型Todo
主要包含默认属性及toggle方法

<!-- language: js -->

    var Todo = Backbone.Model.extend({
        // Default attributes for the todo item.
        defaults: function () {
            return {
                title: "empty todo...",
                order: Todos.nextOrder(), //使用集合对象的方法来生成排序
                done: false
            };
        },

        // Toggle the `done` state of this todo item.
        toggle: function () {
            this.save({ done: !this.get("done") });
        }
    });

### 集合类型TodoList

<!-- language: js -->

    var TodoList = Backbone.Collection.extend({
        // Reference to this collection's model.
        model: Todo,

        // Save all of the todo items under the `"todos-backbone"` namespace.
        localStorage: new Backbone.LocalStorage("todos-backbone"),

        // Filter down the list of all todo items that are finished.
        done: function () {
            return this.where({ done: true });  //backbone及underscore定义了许多操作集合的方法
        },

        // Filter down the list to only todo items that are still not finished.
        remaining: function () {
            return this.without.apply(this, this.done()); //通过apply将this.done()返回的集合展开调用，以符合`_.without(array, [*values])`的定义
        },

        // We keep the Todos in sequential order, despite being saved by unordered
        // GUID in the database. This generates the next order number for new items.
        nextOrder: function () {
            if (!this.length) return 1;
            return this.last().get("order") + 1;
        },

        // Todos are sorted by their original insertion order.
        comparator: "order"
    });

- 定义集合的相关自定义方法：done、remaining是过滤（基本上通过backbone及underscore来完成）；
- [localStorage](https://github.com/jeromegn/Backbone.localStorage)，第三方插件，一个替换 `Backbone.Sync()`，保存到本地存储的adapter
- nextOrder是给Todo模型使用的；
- comparator定义集合项的排序规则，本例中以Todo的order方法作为排序规则的回调，当集合add或sort时会被调用；

### 模型Todo对应的视图TodoView

<!-- language: js -->

    var TodoView = Backbone.View.extend({
        //... is a list tag.
        tagName: "li",

        // Cache the template function for a single item.
        template: _.template($("#item-template").html()),

        // The DOM events specific to an item.
        events: {
            "click .toggle": "toggleDone",
            "dblclick .view": "edit",
            "click a.destroy": "clear",
            "keypress .edit": "updateOnEnter",
            "blur .edit": "close"
        },

        // The TodoView listens for changes to its model, re-rendering. Since there's
        // a one-to-one correspondence between a **Todo** and a **TodoView** in this
        // app, we set a direct reference on the model for convenience.
        initialize: function () {
            this.listenTo(this.model, 'change', this.render);
            this.listenTo(this.model, 'destroy', this.remove);//view的方法，清除页面DOM
        },

        // Re-render the titles of the todo item.
        render: function () {
            this.$el.html(this.template(this.model.toJSON()));
            this.$el.toggleClass("done", this.model.get("done"));
            this.input = this.$(".edit");
            return this;
        },

        // Toggle the `"done"` state of the model.
        toggleDone: function () {
            this.model.toggle();
        },

        // Switch this view into `"editing"` mode, displaying the input field.
        edit: function () {
            this.$el.addClass("editing");
            this.input.focus();
        },

        // Close the `"editing"` mode, saving changes to the todo.
        close: function () {
            var value = this.input.val();
            if (!value) {
                this.clear();
            } else {
                this.model.save({ title: value });
                this.$el.removeClass("editing");
            }
        },

        // If you hit `enter`, we"re through editing the item.
        updateOnEnter: function (e) {
            if (e.keyCode == 13) this.close();
        },

        // Remove the item, destroy the model.
        clear: function () {
            this.model.destroy();
        }
    });

- 定义view的tagName，便于产生el元素；
- 定义template，作为render模板时的cache;
- 定义view监听的事件及事件的响应方法，该事件一般为DOM元素支持的事件，__主要是改变模型状态__
- 重写initialize，监听模型的事件（与DOM事件无关），__主要为了view改变__
- 重写render

### AppView，高层View

<!-- language: js -->

    var AppView = Backbone.View.extend({
        // Instead of generating a new element, bind to the existing skeleton of
        // the App already present in the HTML.
        el: $("#todoapp"),

        // Our template for the line of statistics at the bottom of the app.
        statsTemplate: _.template($("#stats-template").html()),

        // Delegated events for creating new items, and clearing completed ones.
        events: {
            "keypress #new-todo": "createOnEnter",
            "click #clear-completed": "clearCompleted",
            "click #toggle-all": "toggleAllComplete"
        },

        // At initialization we bind to the relevant events on the `Todos`
        // collection, when items are added or changed. Kick things off by
        // loading any preexisting todos that might be saved in *localStorage*.
        initialize: function () {

            this.input = this.$("#new-todo");
            this.allCheckbox = this.$("#toggle-all")[0];

            this.listenTo(Todos, "add", this.addOne);
            this.listenTo(Todos, "reset", this.addAll);
            this.listenTo(Todos, "all", this.render);

            this.footer = this.$("footer");
            this.main = $("#main");

            Todos.fetch();
        },

        // Re-rendering the App just means refreshing the statistics -- the rest
        // of the app doesn't change.
        render: function () {
            var done = Todos.done().length;
            var remaining = Todos.remaining().length;

            if (Todos.length) {
                this.main.show();
                this.footer.show();
                this.footer.html(this.statsTemplate({ done: done, remaining: remaining }));
            } else {
                this.main.hide();
                this.footer.hide();
            }

            this.allCheckbox.checked = !remaining;
        },

        // Add a single todo item to the list by creating a view for it, and
        // appending its element to the `<ul>`.
        addOne: function (todo) {
            var view = new TodoView({ model: todo });
            this.$("#todo-list").append(view.render().el);
        },

        // Add all items in the **Todos** collection at once.
        addAll: function () {
            Todos.each(this.addOne, this);
        },

        // If you hit return in the main input field, create new **Todo** model,
        // persisting it to *localStorage*.
        createOnEnter: function (e) {
            if (e.keyCode != 13) return;
            if (!this.input.val()) return;

            Todos.create({ title: this.input.val() });
            this.input.val("");
        },

        // Clear all done todo items, destroying their models.
        clearCompleted: function () {
            _.invoke(Todos.done(), "destroy");
            return false;
        },

        toggleAllComplete: function () {
            var done = this.allCheckbox.checked;
            Todos.each(function (todo) { todo.save({ "done": done }); });
        }
    });

- 定义el属性
- Cache相应的模板
- 定义view事件，因为高层view，此时selector多为id选择器，而上面的多为class选择器
- 重写initialize方法，监听TodoList集合对象Todos的事件，并获取该集合对象
- 重写render方法，渲染集合对象Todos的相关统计
- addOne方法，将Todo单模型、TodoView单模型的视图、AppView高层视图关联起来

小结
=======
如何合理使用这类框架（以下观点属于个人见解）：

- 能否区分 __业务逻辑__ 与 __交互需求__？
    1. 前者主要用于 __问题域建模__，基本上是POJO对象（不依赖像jq之类的库，能单独测试）
    1. 后者用于构建 __视图与模板__（模板中的逻辑应为与显式相关的 __表现逻辑__ 而不能是业务逻辑）
    1. 后者的复杂有可能会创建出新的模型/状态（视图模型）来支持视图或模板的构建

- 定义出的模型，如果需要依赖UI去做某些响应，该怎么做的？
    1. 只存在视图依赖模型，不存在模型依赖视图，由视图监听模型变化（listenTo），再由视图的响应方法去渲染UI响应，有两种操作方法：
        - __优先使用__ 依靠模型相联系的template重新生成HTML，template中表现逻辑依赖于模型状态，(特别当UI状态复杂，依赖状态机来表述时，建议以产生的HTML不响应UI事件（如disabled掉，或不创建某些标签）而不是将能否操作的判断放到视图方法中)
        - 当上述方式不能满足时，或重绘UI成本太大时，或需要动画时，可使用由视图中方法来依据模型状态，使用jq之类的直接操作UI元素，优先使用css，次之为操作DOM。
    1. 一个模型被多个视图观察，则当它变化时，多个UI产生变化
    1. 一个模型的改变引起其它模型的改变，产生其它模型对应的UI也发生变化
    1. 但一个UI元素最好被一个视图（它可监听多个模型）来修改，否则容易因时序产生不一致

- UI响应事件应该怎么写？
    1. __MV*模式__：由视图来监听UI响应事件，再由视图的响应方法做简单处理后，调用模型中的逻辑，view的职责相对复杂（监听模型变化，监听UI事件），可再分解成下面模式；
    1. __MVC/MVR模式__：由路由器来响应UI请求的url#hash（业务功能要表达成url#hash），路由到对应响应方法，做简单处理后，调用模型中的逻辑

- 做什么、怎么做、怎么用
    1. 做什么，属于分析范畴，关注 __问题域建模，职责划分__
    1. 怎么做，属于设计与实现范畴，关注 __职责与协作，抽象与解耦__
        1. [SOLID](http://zh.wikipedia.org/wiki/SOLID_(%E9%9D%A2%E5%90%91%E5%AF%B9%E8%B1%A1%E8%AE%BE%E8%AE%A1))，函数式编程
        1. 采用合理框架，优化编程思维，提高开发效率
        1. 必须采用库时，以设计成依赖注入或回调的方式
        1. 提高代码质量，考虑可测性
    1. 怎么用，属于实现与配置范畴，主要是应用层的实现
        1. 如是web开发，jq的使用建议放在这一层


参考
====
## 官网
- [http://backbonejs.org](http://backbonejs.org)
- [http://underscorejs.org](http://underscorejs.org)

## 其它
- [http://backbonetutorials.com](http://backbonetutorials.com) 版本过早
- [http://www.csser.com/tools/backbone/backbone.js.html](http://www.csser.com/tools/backbone/backbone.js.html) 版本过早
- [http://www.the5fire.com/backbone-tutorials-catalogue.html](http://www.the5fire.com/backbone-tutorials-catalogue.html)
