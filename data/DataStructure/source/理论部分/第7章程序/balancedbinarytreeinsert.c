
/* ==================Program Description================== */
/* 程序名称：balancedbinarytreeinsert.c                                       */
/* 程序目的：创建平衡二叉树                */
/* Written By Mao Yifei                                  */
/* ========================================================*/
    #include "stdio.h"
    typedef struct bst/*定义二叉排序树结构*/
    {
      int key; /*关键字字段，可以是整形、字符串型、结构类型等*/
      struct bst *leftchild; /*左子树*/
      struct bst *rightchild;/*右子树*/
      int bf;/*平衡因子*/
    } bstnode;
    void L_Rotate(bstnode **p)/*对以*p为根节点的子树做顺时针旋转，处理后，*p指向的节点为子树的新根*/
    {
      bstnode *lc;
      lc=(*p)->leftchild/*lc指向*p左子树的根节点*/;
      (*p)->leftchild=lc->rightchild;/*p的左子树挂接lc的右子树*/
      lc->rightchild=*p;
      *p=lc;/* *p指向新节点*/
    }
    void R_Rotate(bstnode **p) /*对以*p为根节点的子树做逆时针旋转，处理后，*p指向的节点为子树的新根*/
    {
      bstnode *lc;
      lc=(*p)->rightchild; /*lc指向*p右子树的根节点*/;
      (*p)->rightchild=lc->leftchild; /*p的右子树挂接lc的左子树*/
      lc->leftchild=*p;
      *p=lc; /* *p指向新节点*/
    }
    void LeftBalance(bstnode **parent,bstnode **p,bstnode **root,int kx)
    /*将关键字值为kx的插入到平衡二叉树*root中，并将离插入节点最近的非平衡节点*p为根节点的子树做左平衡旋转，处理之后，*p指向的节点为子树的新的根,*parent指向*p的父节点*/
    {
      int flag=0;
      bstnode *lc;
      bstnode *lrc;
      if(*p==*root)/*如果调整平衡的为根节点*/
      flag=1;/*设置标记*/
      lc=(*p)->leftchild;/*lc指向*p的左子树根节点*/
      if(lc->key>kx)/*新节点插入在p的左子树的左子树上*/
      {
        (*p)->bf=0;/*修改根节点与左子树的平衡因子*/
        lc->bf=0;
        L_Rotate(p);/*做顺时针旋转*/
      }
      else /*新节点插入在p的左子树的右子树上*/
      /*先做逆时针旋转再做顺时针旋转*/
      {
        lrc=lc->rightchild;/*lrc指向lc的右子树根节点*/
        if(lrc->bf==1)/*修正*p及其左孩子的平衡因子*/
        {
          (*p)->bf=0;
          lc->bf=1;
        }
        if(lrc->bf==0) /*修正*p及其左孩子的平衡因子*/
        {
          (*p)->bf=0;
          lc->bf=0;
        }
        if(lrc->bf==-1) /*修正*p及其左孩子的平衡因子*/
        {
          (*p)->bf=-1;
          lc->bf=0;
        }
        lrc->bf=0;
        R_Rotate(&((*p)->leftchild));/*先做逆时针旋转*/
        L_Rotate(p);/*再做顺指针旋转*/
      }
      if(flag==1)/*如果修正的是根节点的平衡*/
        *root=*p;/*将当前旋转后的节点设置为根节点*/
      else/**parent指向旋转后的节点*p*/
      {
        if((*parent)->key>(*p)->key)
          (*parent)->rightchild=*p;
        else
          (*parent)->leftchild=*p;
        }
      }
    void RightBalance(bstnode **parent,bstnode **p,bstnode **root,int kx)
    /*将关键字值为kx的插入到平衡二叉树*root中，并将离插入节点最近的非平衡节点*p为根节点的子树做左平衡旋转，处理之后，*p指向的节点为子树的新的根,*parent指向*p的父节点*/
    {
      int flag;
      bstnode *lc;
      bstnode *lrc;
      if(*p==*root)
        flag=1;
      lc=(*p)->rightchild;
      if(lc->key<kx)/*新节点插入在p的右子树的右子树上*/
      {
        (*p)->bf=0;/*修改根节点与右子树的平衡因子*/
        lc->bf=0;
        R_Rotate(p);/*做顺时针旋转*/
      }
      else /*新节点插入在p的右子树的左子树上*/
         /*先做逆时针旋转再做顺时针旋转*/
      {
        lrc=lc->leftchild;
        if(lrc->bf==1) /*修正*p及其右孩子的平衡因子*/
        {
          (*p)->bf=-1;
          lc->bf=0;
        }
        if(lrc->bf==0) /*修正*p及其右孩子的平衡因子*/
        {
          (*p)->bf=0;
          lc->bf=0;
        }
        if(lrc->bf==-1) /*修正*p及其右孩子的平衡因子*/
        {
          (*p)->bf=0;
          lc->bf=1;
        }
        lrc->bf=0;
        L_Rotate(&((*p)->rightchild)); /*先做顺时针旋转*/
        R_Rotate(p); /*再做逆时针旋转*/
      }
      if(flag==1) /*如果修正的是根节点的平衡*/
      *root=*p;
      else /**parent指向旋转后的节点*p*/
      {
        if((*parent)->key<(*p)->key)
          (*parent)->rightchild=*p;
        else
          (*parent)->leftchild=*p;
      }
    }
    void BTreeinsert(bstnode **root,int kx)
    {
      bstnode *point;/*定义指针point，初始指向根结点root;*/
      bstnode *s; /*定义指针s，指向新结点*/
      bstnode *a;/*定义指针a，指向离插入结点最近的平衡因子不等于0的结点*/
      bstnode *f;/*定义指针f，指向a的父结点root;*/
      bstnode *parent;/*定义指针parent，用来指向point结点的父结点*/
      bstnode *b;
      int d;/*标记平衡因子*/
      s=(bstnode *)malloc(sizeof(bstnode));/*为新插入kx分配空间并初始化*/
      s->key=kx;
      s->leftchild=NULL;
      s->rightchild=NULL;
      s->bf=0;
      if (*root==NULL)/*平衡二叉树为空*/
      {
        *root=s;
      }
      /*平衡二叉树不为空*/
      else
      {
        point=*root;
        parent=NULL;
        a=*root;
        f=NULL;
        while(point!=NULL)/*若p指向二叉树不为空*/
        {
          if(point->bf!=0)/*若s平衡因子不为0*/
          {
            a=point;
            f=parent;
         }/*endif*/
          parent=point;
          if(point->key>s->key)/*新结点值小于当前结点point的值*/
            point=point->leftchild;
          else
            point=point->rightchild;
        }
        if(s->key<parent->key)/*新结点值小于找到的平衡二叉树叶子结点parent的值，s作为parent的左子结点*/
          parent->leftchild=s;
        else
          parent->rightchild=s;
         /*修改从a到s各节点的平衡因子*/
        if(s->key<a->key)
          point=a->leftchild;
        else
          point=a->rightchild;
        while(point!=s)  /*修改从a子树开始至新节点s各节点的平衡因子*/
        {
          if(s->key<point->key)
          {
            point->bf=1;
            point=point->leftchild;
          }
          else
          {
            point->bf=-1;
            point=point->rightchild;
          }
        }
        if(s->key<a->key)/*如果新节点在离它最近的a节点的左子树上插入*/
        {
          if(a->bf==0)    /*原左右子树等高*/
            a->bf=1;
          else if(a->bf==-1) /*原右子树高1*/
            a->bf=0;
          else if(a->bf==1)  /*原左子树高1*/
            LeftBalance(&f,&a,root,kx);/*需做左平衡旋转*/
        }
        else
          if(s->key>a->key)/*如果新节点在离它最近的a节点的右子树上插入*/
          {
            if(a->bf==0)    /*原左右子树等高*/
              a->bf=-1;
          else if(a->bf==-1) /*原右子树高1*/
            RightBalance(&f,&a,root,kx); /*需做右平衡旋转*/
          else if(a->bf==1)  /*原左子树高1*/
            a->bf=0;
        }
      }
    }/* BTreeinsert函数结束*/
    void inorder(bstnode *root) /*中序遍历以root为根结点的二叉排序树*/
    {
      bstnode *point;
      point=root;
      if(point!=NULL)
      {
        inorder(point->leftchild);
        printf("%d ",point->key);
        inorder(point->rightchild);
      }/*endif*/
    }/*inorder函数结束*/
    main()
    {
      bstnode *root;
      int kx,flag;
      root=NULL;
      while(1)
      {
        printf("please input the data you want to insert:(zero will exit)\n");
        scanf("%d",&kx);  /*输入待查找的值kx*/
        if(kx==0) /*输入值为0，程序结束*/
          break;
        BTreeinsert(&root,kx); /*将关键字为kx值插入到平衡二叉树中*/
        printf("the inorder traversal of the tree is:");
        inorder(root);/*中序遍历输出二叉排序树*/
        printf("\n");
      }
    }

