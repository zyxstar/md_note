<!-- language: web -->

    <!-- language: html -->
    <div id="deliver-app">
      <section>
        <div id="editor"></div>
      </section>

      <section>
        <h1>List</h1>
        <table colspan='0' rowsapn>
        <thead>
        <tr>
            <th>postcode</th>
            <th>telephone</th>
            <th>mobile</th>
            <th style="width:100px">status</th>
            <th>operate</th>
        <tr>
        </thead>
        <tbody id="deliver-list">
        </tbody>
        </table>
      </section>
    </div>

    <script type="text/template" id="editor-template">
      <input id="btn_new" type="button" value="New" <%= status != "normal" ? "disabled" : "" %>/>
      <div class='<%= status == "normal" ? "hide" : "show" %>'>
       <h1>Editor</h1>
       <form name='frm1' id='frm1'>
         <input type='hidden' id="txt_id" name="id" value="<%= cur_model.id %>"/>
         <label><span>postcode:</span><input id="txt_postcode" name="postcode" type="text" placeholder="postcode" value="<%= cur_model.postcode %>"></label>
         <label><span>telephone:</span><input id="txt_tel" name="telephone" type="text" placeholder="telephone"value="<%= cur_model.telephone %>"/></label>
         <label><span>mobile:</span><input id="txt_mobile" name="mobile" type="text" placeholder="mobile" value="<%= cur_model.mobile %>"/></label>
         <label><span>is default:</span><input id="chk_default" name="is_default" type="checkbox" <%= cur_model.is_default ? "checked" : "" %>/></label>
         <input id="btn_create" type="button" value="Create" <%= status != "new" ? "disabled" : "" %>/>
         <input id="btn_update" type="button" value="Update" <%= status != "edit" ? "disabled" : "" %>/>
         <input id="btn_cancel" type="button" value="Cancel" <%= status == "normal" ? "disabled" : "" %>/>
       </form>
      </div>
    </script>

    <script type="text/template" id="item-template">
      <td><%= model.postcode %></td>
      <td><%= model.telephone %></td>
      <td><%= model.mobile %></td>
      <td>
        <% if (model.is_default) { %>
            default
        <% }else{%>
            <a class="default">set default</a>
        <% }%>
      </td>
      <td>
        <a class="<%=editor.status=="normal"?"edit":"disabled edit_disabled"%>">edit</a>
        <a class="<%=model.id!=editor.cur_model.id ? "destroy" : "disabled" %>">destroy</a>
      </td>
    </script>

    <!-- language: css -->
    label{
        display:block;
        width: 500px;
      }
    label span{
        display:inline-block;
        width: 100px;
    }
    label input[type=text]{
        width: 300px;
    }
    a{
        cursor: pointer ;
        color: blue;
        text-decoration:none;
    }
    table {
        border-collapse: collapse;
        border-spacing: 2px;
        overflow: auto;
        border-color: gray;
    }
    tbody {
        display: table-row-group;
        vertical-align: middle;
        border-color: inherit;
    }
    table tr {
        border-top: 1px solid #ccc;
        background-color: #fff;
        display: table-row;
        vertical-align: inherit;
        border-color: inherit;
    }
    table tr:nth-child(2n) {
        background-color: #f8f8f8;
    }

    table tr:hover td{
        background-color:#eee;
    }

    table td, table th {
        display: table-cell;
        vertical-align: inherit;
        border: 1px solid #ddd;
        padding: 3px 13px;
    }

    table th {
        font-weight: bold;
    }

    table tr a:hover{
        text-decoration:underline;
    }

    table tr a.edit{
        margin-right: 10px;
    }

    table tr a.default{
        display:none;
    }

    table tr:hover a.default{
        display:inline;
    }

    table tr a.disabled{
        cursor: default;
        color:gray;
        text-decoration:none;
    }
    table tr a.edit_disabled{
        margin-right: 10px;
    }

    .hide{
        display:none;
    }
    .show{
        display:block;
    }



    <!-- language: js -->
    //import json2
    //import jquery.1.9.0
    //import underscore.1.4.3
    //import backbone.1.0.0

    //改写默认的（ajaxSync）模式
    Backbone.sync = function(method,model,options){
        var store = {
          create: function(model) {
            model.id=parseInt((1+Math.random())*100).toString();
            alert("create "+model.id);
            return model;
          },

          update: function(model) {
            alert("update "+model.id);
            return model;
          },

          find: function(model) {
            alert("get "+model.id);
            return model;
          },

          findAll: function() {
            return [{
                id: "1",
                postcode: "100010",
                telephone: "010-84539852",
                mobile: "13612341234",
                is_default: true
              },{
                id: "2",
                postcode: "100020",
                telephone: "010-84539852",
                mobile: "13612341234",
                is_default: false
              },{
                id: "3",
                postcode: "100030",
                telephone: "010-84539852",
                mobile: "13612341234",
                is_default: false
             },{
                id: "4",
                postcode: "100040",
                telephone: "010-84539852",
                mobile: "13612341234",
                is_default: false
             },{
                id: "5",
                postcode: "100050",
                telephone: "010-84539852",
                mobile: "13612341234",
                is_default: false
             }];
          },

          destroy: function(model) {
            alert("delete "+model.id);
            return model;
          },
        };
        var resp, errorMessage, syncDfd = $.Deferred && $.Deferred();
        try {
            switch (method) {
              case "read":
                resp = model.id != undefined ? store.find(model) : store.findAll();
                break;
              case "create":
                resp = store.create(model);
                break;
              case "update":
                resp = store.update(model);
                break;
              case "delete":
                resp = store.destroy(model);
                break;
            }
        } catch(error) {
              errorMessage = error.message;
        }

        if (resp) {
        model.trigger("sync", model, resp, options);
        if (options && options.success)
          options.success(resp);
        if (syncDfd)
          syncDfd.resolve(resp);

        } else {
        errorMessage = errorMessage ? errorMessage
                                    : "Record Not Found";
        if (options && options.error)
          options.error(errorMessage);
        if (syncDfd)
          syncDfd.reject(errorMessage);
        }
        if (options && options.complete) options.complete(resp);
        return syncDfd && syncDfd.promise();
    }

    //=========================================================
    //代码开始
    //收货信息模型
    var Deliver = Backbone.Model.extend({
        //配置ajax时对应CRUD的url，以http method区分
        urlRoot: "/delivers",
        defaults: {
            id: null,
            postcode: "",
            telephone: "",
            mobile: "",
            is_default: false
        },
        //模型的验证规则
        validate: function(attr){
            if(attr.telephone == "" && attr.mobile == ""){
                return "must set telephone or mobile";
            }
        },
        //初始化器
        initialize: function(){
            //监听验证规则，挂上处理函数
            this.bind("invalid", function(model, error){
                alert( error );
            });
        }
    });

    //收货信息 集合 模型
    var DeliverList = Backbone.Collection.extend({
        model: Deliver,
        url: "/delivers",//fetch时的url

        //以下业务方法，随功能而定
        m_defalut: function(deliver){
            this.each(function(m){
                if(m.get("is_default") && m!==deliver){
                    m.set("is_default",false);
                    m.save();
                }
            });
            if(!deliver.get("is_default")){
                deliver.set("is_default",true);
                deliver.save();
            }
        },
        m_modify: function(deliver,success){
            var m=this.where({id:deliver.get("id")})[0];
            m.attributes=deliver.attributes;
            this._try_save(m,success);
        },
        m_append: function(deliver,success){
            var me=this;
            this._try_save(deliver,function(){
                me.add(deliver);
                if(success) success();
            });
        },

        //私有方法
        _try_save: function(deliver,success){
            var me=this;
            deliver.save({},{success:function(){
                if(deliver.get("is_default"))
                    me.m_defalut(deliver);
                if(success) success();
            }});
        }
    });

    //编辑器模式(视图模型[为视图而存在,与业务无关])：new，edit
    var editor_mode = new (Backbone.Model.extend({
        defaults: {
            status:"normal",
            cur_model:new Deliver()
        },
        toJSON: function(){
            return {
                status: this.get("status"),
                cur_model: this.get("cur_model").toJSON()
            }
        },
        normal: function(){
            this.set("status","normal");
            this.set("cur_model",new Deliver());
        }
    }));
    //收货集合对象
    var deliver_list = new DeliverList();


    //收货信息视图
    //只存在视图关联模型，不存在模型引用视图
    var DeliverView = Backbone.View.extend({
        //外围用tr来包装
        tagName: "tr",
        //cache对应的html模板
        template: _.template($('#item-template').html()),
        //初始化器
        initialize: function() {
            //监听属性变化时，重新渲染页面
            this.listenTo(this.model, 'change', this.e_render);
            //列表项的状态还依赖于编辑模式的状态
            this.listenTo(editor_mode, 'change', this.e_render);
            //当模型删除时，该视图也删除
            this.listenTo(this.model, 'destroy', this.remove);
        },
        e_render: function() {
            //根据模板直接重新生成新的html，this.$el是当前视图对应的DOM(jq对象)
            this.$el.html(this.template({
                editor: editor_mode.toJSON(),
                model: this.model.toJSON()
            }));
            return this;
        },

        //视图上可响应的动作
        //以下职责可以由router来完成
        events: {
            "click a.edit": "v_edit",
            "click a.destroy": "v_destroy",
            "click a.default": "v_default"
        },
        //以下为视图上动作响应，随交互需求而定，
        //该部分 只是简单的信息获取与界面赋值，然后 调用模型 来完成业务功能
        v_edit: function(){
            //最好不要在此判断能否操作，而应在UI上就不响应（但如果是使用路由导航到该功能时还是要的）
            //if(editor_mode.get("status")!="normal"){
            //  alert("can't edit");
            //  return;
            //}
            editor_mode.set("status","edit");
            editor_mode.set("cur_model",this.model);
        },
        v_destroy: function(){
            //if(editor_mode.get("cur_model")==this.model){
            //  alert("this record is be editing, can't destroy");
            //  return;
            //}
            if(confirm("really delete?"))
                this.model.destroy();
        },
        v_default:function(){
            deliver_list.m_defalut(this.model);
        }
    });

    var DeliverListView = Backbone.View.extend({
        initialize: function() {
            //监听集合add事件
            this.listenTo(deliver_list, 'add', this.e_addOne);
            //当集合fetch后（下行代码），reset事件会被激发，响应函数事先挂上
            this.listenTo(deliver_list, 'reset', this.e_addAll);
            //加载所有收货信息
            deliver_list.fetch();
        },

        //加载一项收货信息
        e_addOne: function(deliver){
            var view = new DeliverView({
                model: deliver
            });
            //需要依赖 收货信息视图 来完成
            $("#deliver-list").append(view.e_render().el);
        },
        e_addAll: function() {
            deliver_list.each(this.e_addOne, this);
        }

    });

    var EditorView = Backbone.View.extend({
        el: $("#editor"),//绑定的DOM
        editor_template: _.template($('#editor-template').html()),
        initialize: function() {
            //监听编辑模式，当编辑模式改变时，UI响应变化
            this.listenTo(editor_mode, 'change', this.e_render);
            this.e_render();
        },
        e_render: function(){
            $("#editor").html(this.editor_template(editor_mode.toJSON()));
        },

        //视图上可响应的动作,必须存在this.el,依据它来查找元素
        events: {
            "click #btn_new": "v_new",
            "click #btn_create": "v_create",
            "click #btn_update": "v_update",
            "click #btn_cancel": "v_cancel"
        },
        //以下是响应动作，视交互需要而定
        v_new: function(){
            editor_mode.set("status","new");
        },
        v_create: function(){
            var deliver=this._build_deliver();
            deliver_list.m_append(deliver,function(){
                editor_mode.normal();
            });
        },
        v_update: function(){
            var deliver=this._build_deliver();
            deliver_list.m_modify(deliver,function(){
                editor_mode.normal();
            });
        },
        v_cancel: function(){
            editor_mode.normal();
        },

        //私有方法
        _build_deliver:function(){
            var deliver = new Deliver();
            var txt_id=$("#txt_id").val();
            deliver.set({
                id: txt_id==""?null:txt_id,
                postcode: $("#txt_postcode").val(),
                telephone: $("#txt_tel").val(),
                mobile: $("#txt_mobile").val(),
                is_default: $("#chk_default")[0].checked
            });
            return deliver;
        }
    });

    new DeliverListView();
    new EditorView();

    //Backbone.sync=Backbone.ajaxSync;


