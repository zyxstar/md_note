class Authority::Resource

  # def register_res(res_name, op_list)

  # end

  #used for backend
  def register_ctrl(ctrl, action_list)
    # add sym_res
    #   res_name = ctrl
    #   res_type = 'ctrl'
    # each action_list
    #   add sym_res_op
    #     op_name = action_name
  end

  #used for backend
  def register_app(app_name)
    # add sym_res
    #   res_name = app_name
    #   res_type = 'app'
    # add sym_res_op
    #   1 op_name = 'open'
    #   2 op_name = 'access'
    # add syc_role
    #   1 name = app_name + '_open'
    #   1 name = app_name + '_access'
    # add syt_auth
    #   role_id from syc_role
    #   res_op_id from sym_res_op
  end

  #used for backend
  def register_group(group_name)
    # add sym_res
    #   res_name = group_name
    #   res_type = 'group'
    # add sym_res_op
    #   1 op_name = 'join'
    #   2 op_name = 'access'
    # add syc_role
    #   1 name = group_name + '_join'
    #   1 name = group_name + '_access'
    # add syt_auth
    #   role_id from syc_role
    #   res_op_id from sym_res_op
  end

end

# http://bbs.tianya.cn/post-329-5884-2.shtml
