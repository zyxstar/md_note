> 2013-07-01

<!-- language: web -->

    <!-- language: html -->
    <div id="todoapp">

      <header>
        <h1>Todos</h1>
        <input id="new-todo" type="text" placeholder="What needs to be done?">
      </header>

      <section id="main">
        <input id="toggle-all" type="checkbox">
        <label for="toggle-all">Mark all as complete</label>
        <ul id="todo-list"></ul>
      </section>

      <footer>
        <a id="clear-completed">Clear completed</a>
        <div id="todo-count"></div>
      </footer>

    </div>

    <div id="instructions">
      Double-click to edit a todo.
    </div>

    <div id="credits">
      Created by
      <br />
      <a href="http://jgn.me/">J&eacute;r&ocirc;me Gravel-Niquet</a>.
      <br />Rewritten by: <a href="http://addyosmani.github.com/todomvc">TodoMVC</a>.
    </div>


    <!-- Templates -->

    <script type="text/template" id="item-template">
      <div class="view">
        <input class="toggle" type="checkbox" <%= done ? 'checked="checked"' : '' %> />
        <label><%- title %></label>
        <a class="destroy"></a>
      </div>
      <input class="edit" type="text" value="<%- title %>" />
    </script>

    <script type="text/template" id="stats-template">
      <% if (done) { %>
        <a id="clear-completed">Clear <%= done %> completed <%= done == 1 ? 'item' : 'items' %></a>
      <% } %>
      <div class="todo-count"><b><%= remaining %></b> <%= remaining == 1 ? 'item' : 'items' %> left</div>
    </script>

    <!-- language: css -->
    html, body {
        margin: 0;
        padding: 0;
    }

    body {
        font: 14px "Helvetica Neue", Helvetica, Arial, sans-serif;
        line-height: 1.4em;
        background: #eeeeee;
        color: #333333;
        width: 520px;
        margin: 0 auto;
        -webkit-font-smoothing: antialiased;
    }

    #todoapp {
        background: #fff;
        padding: 20px;
        margin-bottom: 40px;
        -webkit-box-shadow: rgba(0, 0, 0, 0.2) 0 2px 6px 0;
        -moz-box-shadow: rgba(0, 0, 0, 0.2) 0 2px 6px 0;
        -ms-box-shadow: rgba(0, 0, 0, 0.2) 0 2px 6px 0;
        -o-box-shadow: rgba(0, 0, 0, 0.2) 0 2px 6px 0;
        box-shadow: rgba(0, 0, 0, 0.2) 0 2px 6px 0;
        -webkit-border-radius: 0 0 5px 5px;
        -moz-border-radius: 0 0 5px 5px;
        -ms-border-radius: 0 0 5px 5px;
        -o-border-radius: 0 0 5px 5px;
        border-radius: 0 0 5px 5px;
    }

    #todoapp h1 {
        font-size: 36px;
        font-weight: bold;
        text-align: center;
        padding: 0 0 10px 0;
    }

    #todoapp input[type="text"] {
        width: 466px;
        font-size: 24px;
        font-family: inherit;
        line-height: 1.4em;
        border: 0;
        outline: none;
        padding: 6px;
        border: 1px solid #999999;
        -webkit-box-shadow: rgba(0, 0, 0, 0.2) 0 1px 2px 0 inset;
        -moz-box-shadow: rgba(0, 0, 0, 0.2) 0 1px 2px 0 inset;
        -ms-box-shadow: rgba(0, 0, 0, 0.2) 0 1px 2px 0 inset;
        -o-box-shadow: rgba(0, 0, 0, 0.2) 0 1px 2px 0 inset;
        box-shadow: rgba(0, 0, 0, 0.2) 0 1px 2px 0 inset;
    }

    #todoapp input::-webkit-input-placeholder {
        font-style: italic;
    }

    #main {
        display: none;
    }

    #todo-list {
        margin: 10px 0;
        padding: 0;
        list-style: none;
    }

    #todo-list li {
        padding: 18px 20px 18px 0;
        position: relative;
        font-size: 24px;
        border-bottom: 1px solid #cccccc;
    }

    #todo-list li:last-child {
        border-bottom: none;
    }

    #todo-list li.done label {
        color: #777777;
        text-decoration: line-through;
    }

    #todo-list .destroy {
        position: absolute;
        right: 5px;
        top: 20px;
        display: none;
        cursor: pointer;
        width: 20px;
        height: 20px;
        background: url(destroy.png) no-repeat;
    }

    #todo-list li:hover .destroy {
        display: block;
    }

    #todo-list .destroy:hover {
        background-position: 0 -20px;
    }

    #todo-list li.editing {
        border-bottom: none;
        margin-top: -1px;
        padding: 0;
    }

    #todo-list li.editing:last-child {
        margin-bottom: -1px;
    }

    #todo-list li.editing .edit {
        display: block;
        width: 444px;
        padding: 13px 15px 14px 20px;
        margin: 0;
    }

    #todo-list li.editing .view {
        display: none;
    }

    #todo-list li .view label {
        word-break: break-word;
    }

    #todo-list li .edit {
        display: none;
    }

    #todoapp footer {
        display: none;
        margin: 0 -20px -20px -20px;
        overflow: hidden;
        color: #555555;
        background: #f4fce8;
        border-top: 1px solid #ededed;
        padding: 0 20px;
        line-height: 37px;
        -webkit-border-radius: 0 0 5px 5px;
        -moz-border-radius: 0 0 5px 5px;
        -ms-border-radius: 0 0 5px 5px;
        -o-border-radius: 0 0 5px 5px;
        border-radius: 0 0 5px 5px;
    }

    #clear-completed {
        float: right;
        line-height: 20px;
        text-decoration: none;
        background: rgba(0, 0, 0, 0.1);
        color: #555555;
        font-size: 11px;
        margin-top: 8px;
        margin-bottom: 8px;
        padding: 0 10px 1px;
        cursor: pointer;
        -webkit-border-radius: 12px;
        -moz-border-radius: 12px;
        -ms-border-radius: 12px;
        -o-border-radius: 12px;
        border-radius: 12px;
        -webkit-box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 0 0;
        -moz-box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 0 0;
        -ms-box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 0 0;
        -o-box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 0 0;
        box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 0 0;
    }

    #clear-completed:hover {
        background: rgba(0, 0, 0, 0.15);
        -webkit-box-shadow: rgba(0, 0, 0, 0.3) 0 -1px 0 0;
        -moz-box-shadow: rgba(0, 0, 0, 0.3) 0 -1px 0 0;
        -ms-box-shadow: rgba(0, 0, 0, 0.3) 0 -1px 0 0;
        -o-box-shadow: rgba(0, 0, 0, 0.3) 0 -1px 0 0;
        box-shadow: rgba(0, 0, 0, 0.3) 0 -1px 0 0;
    }

    #clear-completed:active {
        position: relative;
        top: 1px;
    }

    #todo-count span {
        font-weight: bold;
    }

    #instructions {
        margin: 10px auto;
        color: #777777;
        text-shadow: rgba(255, 255, 255, 0.8) 0 1px 0;
        text-align: center;
    }

    #instructions a {
        color: #336699;
    }

    #credits {
        margin: 30px auto;
        color: #999;
        text-shadow: rgba(255, 255, 255, 0.8) 0 1px 0;
        text-align: center;
    }

    #credits a {
        color: #888;
    }

    <!-- language: js -->
    //= require json2
    //= require jquery.1.9.0
    //= require underscore.1.4.3
    //= require backbone.1.0.0
    //= require backbone_localStorage
    $(function() {

        // Todo Model
        // ----------

        // Our basic **Todo** model has `title`, `order`, and `done` attributes.
        var Todo = Backbone.Model.extend({

            // Default attributes for the todo item.
            defaults: function() {
                return {
                    title: "empty todo...",
                    order: Todos.nextOrder(),
                    done: false
                };
            },

            // Toggle the `done` state of this todo item.
            toggle: function() {
                this.save({
                    done: !this.get("done")
                });
            }

        });

        // Todo Collection
        // ---------------

        // The collection of todos is backed by *localStorage* instead of a remote
        // server.
        var TodoList = Backbone.Collection.extend({

            // Reference to this collection‘s model.
            model: Todo,

            // Save all of the todo items under the `"todos-backbone"` namespace.
            localStorage: new Backbone.LocalStorage("todos-backbone"),

            // Filter down the list of all todo items that are finished.
            done: function() {
                return this.where({
                    done: true
                });
            },

            // Filter down the list to only todo items that are still not finished.
            remaining: function() {
                return this.without.apply(this, this.done());
            },

            // We keep the Todos in sequential order, despite being saved by unordered
            // GUID in the database. This generates the next order number for new items.
            nextOrder: function() {
                if (!this.length) return 1;
                return this.last().get('order') + 1;
            },

            // Todos are sorted by their original insertion order.
            comparator: 'order'

        });

        // Create our global collection of **Todos**.
        var Todos = new TodoList;

        // Todo Item View
        // --------------

        // The DOM element for a todo item...
        var TodoView = Backbone.View.extend({

            //... is a list tag.
            tagName: "li",

            // Cache the template function for a single item.
            template: _.template($('#item-template').html()),

            // The DOM events specific to an item.
            events: {
                "click .toggle": "toggleDone",
                "dblclick .view": "edit",
                "click a.destroy": "clear",
                "keypress .edit": "updateOnEnter",
                "blur .edit": "close"
            },

            // The TodoView listens for changes to its model, re-rendering. Since there is
            // a one-to-one correspondence between a **Todo** and a **TodoView** in this
            // app, we set a direct reference on the model for convenience.
            initialize: function() {
                this.listenTo(this.model, 'change', this.render);
                this.listenTo(this.model, 'destroy', this.remove);
            },

            // Re-render the titles of the todo item.
            render: function() {
                this.$el.html(this.template(this.model.toJSON()));
                this.$el.toggleClass('done', this.model.get('done'));
                this.input = this.$('.edit');
                return this;
            },

            // Toggle the `"done"` state of the model.
            toggleDone: function() {
                this.model.toggle();
            },

            // Switch this view into `"editing"` mode, displaying the input field.
            edit: function() {
                this.$el.addClass("editing");
                this.input.focus();
            },

            // Close the `"editing"` mode, saving changes to the todo.
            close: function() {
                var value = this.input.val();
                if (!value) {
                    this.clear();
                } else {
                    this.model.save({
                        title: value
                    });
                    this.$el.removeClass("editing");
                }
            },

            // If you hit `enter`, we are through editing the item.
            updateOnEnter: function(e) {
                if (e.keyCode == 13) this.close();
            },

            // Remove the item, destroy the model.
            clear: function() {
                this.model.destroy();
            }

        });

        // The Application
        // ---------------

        // Our overall **AppView** is the top-level piece of UI.
        var AppView = Backbone.View.extend({

            // Instead of generating a new element, bind to the existing skeleton of
            // the App already present in the HTML.
            el: $("#todoapp"),

            // Our template for the line of statistics at the bottom of the app.
            statsTemplate: _.template($('#stats-template').html()),

            // Delegated events for creating new items, and clearing completed ones.
            events: {
                "keypress #new-todo": "createOnEnter",
                "click #clear-completed": "clearCompleted",
                "click #toggle-all": "toggleAllComplete"
            },

            // At initialization we bind to the relevant events on the `Todos`
            // collection, when items are added or changed. Kick things off by
            // loading any preexisting todos that might be saved in *localStorage*.
            initialize: function() {

                this.input = this.$("#new-todo");
                this.allCheckbox = this.$("#toggle-all")[0];

                this.listenTo(Todos, 'add', this.addOne);
                this.listenTo(Todos, 'reset', this.addAll);
                this.listenTo(Todos, 'all', this.render);

                this.footer = this.$('footer');
                this.main = $('#main');

                Todos.fetch();
            },

            // Re-rendering the App just means refreshing the statistics -- the rest
            // of the app does not change.
            render: function() {
                var done = Todos.done().length;
                var remaining = Todos.remaining().length;

                if (Todos.length) {
                    this.main.show();
                    this.footer.show();
                    this.footer.html(this.statsTemplate({
                        done: done,
                        remaining: remaining
                    }));
                } else {
                    this.main.hide();
                    this.footer.hide();
                }

                this.allCheckbox.checked = !remaining;
            },

            // Add a single todo item to the list by creating a view for it, and
            // appending its element to the `ul`.
            addOne: function(todo) {
                var view = new TodoView({
                    model: todo
                });
                this.$("#todo-list").append(view.render().el);
            },

            // Add all items in the **Todos** collection at once.
            addAll: function() {
                Todos.each(this.addOne, this);
            },

            // If you hit return in the main input field, create new **Todo** model,
            // persisting it to *localStorage*.
            createOnEnter: function(e) {
                if (e.keyCode != 13) return;
                if (!this.input.val()) return;

                Todos.create({
                    title: this.input.val()
                });
                this.input.val('');
            },

            // Clear all done todo items, destroying their models.
            clearCompleted: function() {
                _.invoke(Todos.done(), 'destroy');
                return false;
            },

            toggleAllComplete: function() {
                var done = this.allCheckbox.checked;
                Todos.each(function(todo) {
                    todo.save({
                        'done': done
                    });
                });
            }

        });

        // Finally, we kick things off by creating the **App**.
        var App = new AppView;

    });
