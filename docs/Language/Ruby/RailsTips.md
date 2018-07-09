## Rails 开发建议
- 不在 Model 以外的地方做数据库查询，函数组合模式

```ruby
# app/models/user.rb
class User < ActiveRecord::Base

  scope :active, -> { where(active: true) }
  scope :by_last_login_at, -> { order(:lasst_login_at) }

end

# app/controllers/users_controller.rb
class UsersController < ApplicationController

  def index
    @users = User.active.by_last_login_at
  end

end
```

- 不在 View 里有业务逻辑处理，放到helper，或使用https://github.com/drapergem/draper

```ruby
# app/helpers/comments_helper.rb
module CommentsHelper
  def comments_count(comments)
    "#{comments.count} Comment#{comments.count == 1 ? '' : 's'}"
  end
end

<!-- app/views/posts/show.html.erb -->
<h2>
  <%= comments_count(@comments) %>
</h2>
```


- 不使用 Unless 或 否定表达式 在判断条件里，unless将会给代码增加些一点复杂度，因为必须要反响思考

```ruby
# app/models/user.rb
class User < ActiveRecord::Base

  def disabled?
    !enabled?
  end

end

# app/services/charge_user.rb
class ChargeUser

  def self.perform(user, amount)
    #return false unless user.enabled?
    return false if user.disabled?

    PaymentGateway.charge(user.account_id, amount)
  end

end
```


- Rails里面改变validate错误信息默认的属性名字

```ruby
class User < ActiveRecord::Base
  validates :name, :presence => true

  HUMANIZED_ATTRIBUTES = {
    :name => "User Name"
  }

  def self.human_attribute_name(attr, options={})
    HUMANIZED_ATTRIBUTES[attr.to_sym] || super
  end

end
```


- 寻找Array中满足条件的第一个值

```ruby
[1,2,3].select {|x| x > 1 }.first #X
[1,2,3].find {|x| x > 1 } #V
```

- 使用reverse循环的时候应该使用reverse_each方法

```ruby
[1,2,3].reverse.each {|x| x+1 } #X
[1,2,3].reverse_each {|x| x+1 } #V
```

- 计算数组满足条件的值的长度

```ruby
[1,2,3].select {|x| x > 1 }.size #X
[1,2,3].count {|x| x > 1 } #V
```

- map后面做flatten操作

```ruby
[1,2,3].map {|x| [x,x+1] }.flatten #X
[1,2,3].flat_map {|x| [x, x+1]} #V
```


- Ruby 2.0 新特性 Keyword Argument

```ruby
def log(msg, level: "ERROR", time: Time.now)
  puts "#{ time.ctime } [#{ level }] #{ msg }"
end

#1.9
def log(*msgs)
  opt = msgs.last.is_a?(Hash) ? msgs.pop : {}
  level = opt.key?(:level) ? opt.delete(:level) : "ERROR"
  time  = opt.key?(:time ) ? opt.delete(:time ) : Time.now
  raise "unknown keyword: #{ opt.keys.first }" if !opt.empty?
  msgs.each {|msg| puts "#{ time.ctime } [#{ level }] #{ msg }" }
end
```


- `gem env`查看gem的环境信息，如版本、gem包路径

- rubygems覆盖了ruby的require方法来实现加载gem的功能，RubyGems首先使用Ruby的require方法从loadpath里去找，通过require找不到的话,rubygems会从Gems文件里面找，如果找到的话，将会激活这个gem，然后把这个gem加到load path里面去

```shell
2.2.6 :001 > spec = Gem::Specification.find_by_path('active_support')
 => #<Gem::Specification:0x76bec0 activesupport-4.2.0>
2.2.6 :002 > $LOAD_PATH
 => ["/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby/2.2.0/x86_64-linux", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby/2.2.0/x86_64-linux", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/2.2.0/x86_64-linux"]
2.2.6 :003 > spec.activate
 => true
2.2.6 :004 > $LOAD_PATH
 => ["/home/zyx/.rvm/gems/ruby-2.2.6@r4.2/gems/activesupport-4.2.0/lib", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby/2.2.0/x86_64-linux", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/site_ruby", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby/2.2.0/x86_64-linux", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/vendor_ruby", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/2.2.0", "/home/zyx/.rvm/rubies/ruby-2.2.6/lib/ruby/2.2.0/x86_64-linux"]

```

- 使用symbol排序的话，默认会带上表名，这是一个非常好的习惯。如果不带上表名，在多个表连接查询的时候，另外的表也有同样的字段的话，这个排序将会出现混淆

```ruby
Motel.order(:name).to_sql=> SELECT "motels".* FROM "motels" ORDER BY "motels"."name" ASC
Motel.order(name: :desc).to_sql=> SELECT "motels".* FROM "motels" ORDER BY "motels"."name" DESC
```

- 占位符

```ruby
variables = {:animal => 'fox', :action => 'jumps'}
template = "The quick brown %{animal} %{action} over the lazy dog"
puts template % variables
```

- 字符串替换

```ruby
replacements = {'dog' => 'pig', 'fox' => 'cat'}
replacements.default_proc = ->(h, k) { k }
puts quote.gsub(/\w+/, replacements)
```

- 定义非ActiveRecord子类

```ruby
class Error
  include ActiveModel::Validations
  include ActiveModel::Conversion
  extend ActiveModel::Naming

  attr_accessor :text, :status, :url, :method

  def initialize(attributes = {})
    attributes.each do |name, value|
      send("#{name}=", value)
    end
  end
end

#usage
@error = Error.new(text: "404 Not found",
                   status: 404,
                   url: request.url,
                   method: request.method)
```

- 定义redis存储类

```ruby
require 'ohm'
require 'ohm/contrib'

class Filter < Ohm::Model
  include Ohm::DataTypes
  include Ohm::Versioned
  include Ohm::Timestamps

  attribute :filter_id
  attribute :message
  attribute :location
  attribute :status
  attribute :code
  index :filter_id


  def serialize
    {
      filter: { id: filter_id, location: location },
      response: { message: message, code: code.to_i },
      status: status.to_i
    }
  end
end
```



