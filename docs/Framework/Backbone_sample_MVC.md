> 2013-11-08

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
            <a href="#default/<%= model.id%>" class="default">set default</a>
        <% }%>
      </td>
      <td>
        <a class="<%=editor.status=="normal"?"edit":"disabled edit_disabled"%>" <%=editor.status=="normal"?("href='#edit/" + model.id+"'"):""%> >edit</a>
        <a class="<%=model.id!=editor.cur_model.id ? "destroy" : "disabled" %>" <%=model.id!=editor.cur_model.id?("href='#destroy/" + model.id+"'"):""%> >destroy</a>
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
    //= require json2
    //= require jquery.1.9.0
    //= require underscore.1.4.3
    //= require backbone.1.0.0

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
    var Deliver = Backbone.Model.extend({
        urlRoot: "/delivers",
        defaults: {
            id: null,
            postcode: "",
            telephone: "",
            mobile: "",
            is_default: false
        },
        validate: function(attr){
            if(attr.telephone == "" && attr.mobile == ""){
                return "must set telephone or mobile";
            }
        },
        initialize: function(){
            this.bind("invalid", function(model, error){
                alert( error );
            });
        }
    });


    var DeliverList = Backbone.Collection.extend({
        model: Deliver,
        url: "/delivers",

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
        m_get_deliver: function(id){
            var arr=this.where({id:id});
            return arr.length==0 ? null : arr[0];
        },


        _try_save: function(deliver,success){
            var me=this;
            deliver.save({},{success:function(){
                if(deliver.get("is_default"))
                    me.m_defalut(deliver);
                if(success) success();
            }});
        }
    });

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

    var deliver_list = new DeliverList();

    //路由，将界面需要响应的动作设计成路由规则
    //由它监听UI响应，则视图的主要职责就是负责模型的监听(可以不再持有events属性以及响应方法)
    //对应的模板可能需要修改，比如a标签的href需要编写#hash
    var AppRouter = Backbone.Router.extend({
        routes: {
            "new": "r_new",
            "create": "r_create",
            "update": "r_update",
            "cancel": "r_cancel",

            "edit/:id": "r_edit",
            "destroy/:id": "r_destroy",
            "default/:id": "r_default"
        },
        r_new: function(){
            editor_mode.set("status","new");
        },
        r_create: function(){
            var deliver=this._build_deliver();
            deliver_list.m_append(deliver,function(){
                editor_mode.normal();
            });
        },
        r_update: function(){
            var deliver=this._build_deliver();
            deliver_list.m_modify(deliver,function(){
                editor_mode.normal();
            });
        },
        r_cancel: function(){
            editor_mode.normal();
        },

        r_edit: function(id) {
            //可以做判断，防止直接输入url#hash时被修改
            if(editor_mode.get("status")!="normal"){
                alert("can't edit");
                return;
            }
            editor_mode.set("status","edit");
            editor_mode.set("cur_model",deliver_list.m_get_deliver(id));
        },
        r_destroy: function(id){
            var deliver = deliver_list.m_get_deliver(id);
            if(editor_mode.get("cur_model")==deliver){
                alert("this record is be editing, can't destroy");
                return;
            }
            if(confirm("really delete?"))
                deliver.destroy();
        },
        r_default:function(id){
            deliver_list.m_defalut(deliver_list.m_get_deliver(id));
        },

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
    var app_router = new AppRouter;
    Backbone.history.start();

    var DeliverView = Backbone.View.extend({
        tagName: "tr",
        template: _.template($('#item-template').html()),
        initialize: function() {
            this.listenTo(this.model, 'change', this.e_render);
            this.listenTo(editor_mode, 'change', this.e_render);
            this.listenTo(this.model, 'destroy', this.remove);
        },
        e_render: function() {
            this.$el.html(this.template({
                editor: editor_mode.toJSON(),
                model: this.model.toJSON()
            }));
            return this;
        }
    });

    var DeliverListView = Backbone.View.extend({
        initialize: function() {
            this.listenTo(deliver_list, 'add', this.e_addOne);
            this.listenTo(deliver_list, 'reset', this.e_addAll);
            deliver_list.fetch();
        },

        e_addOne: function(deliver){
            var view = new DeliverView({
                model: deliver
            });
            $("#deliver-list").append(view.e_render().el);
        },
        e_addAll: function() {
            deliver_list.each(this.e_addOne, this);
        }

    });

    var EditorView = Backbone.View.extend({
        el: $("#editor"),
        editor_template: _.template($('#editor-template').html()),
        initialize: function() {
            this.listenTo(editor_mode, 'change', this.e_render);
            this.e_render();
        },
        e_render: function(){
            $("#editor").html(this.editor_template(editor_mode.toJSON()));
        },

        //因为是按钮，不能编写#hash，仍由视图来监听UI，只不过响应方法调用路由的跳转
        events: {
            "click #btn_new": function(){app_router.navigate("#new", {trigger: true});},
            "click #btn_create": function(){app_router.navigate("#create", {trigger: true});},
            "click #btn_update": function(){app_router.navigate("#update", {trigger: true});},
            "click #btn_cancel": function(){app_router.navigate("#cancel", {trigger: true});}
        }
    });

    new DeliverListView();
    new EditorView();

    //Backbone.sync=Backbone.ajaxSync;


