
/* ==================Program Description================== */
/* 程序名称：binarytreeinsert.c                                       */
/* 程序目的：在以*root为根结点的二叉排序树上查找关键字等于kx的记录*/
/* Written By Mao Yifei                                  */
/* ========================================================*/
    #include "stdio.h"
    typedef struct bst/*定义二叉排序树结构*/
    {
      int key; /*关键字字段，可以是整形、字符串型、结构类型等*/
      struct bst *leftchild; /*左子树*/
      struct bst *rightchild;/*右子树*/
    } bstnode;
    int bstinsert(bstnode **root,int kx)
    /*在以*root为根结点的二叉排序树上查找关键字等于kx的记录，指针value记录在二叉排序树中是否存在待查找的kx值，如存在，返回值1，否则返回值0，函数返回根结点指针*/
   {
      bstnode *point;/*定义指针point，初始指向根结点root;*/
      bstnode *parent;/*定义指针parent，用来指向point结点的根结点*/
      int flag;/*标记变量，用于标记point是parent的左子或右子结点。如为左子结点flag值记为0，如为右子结点flag值记为1*/
      point=*root;
      parent=NULL;
      while(point!=NULL)/*若p指向二叉树不为空*/
      {
        if(point->key==kx)/*若根结点关键字等于给定值kx，查找成功*/
        {
          return 1;
        }/*endif*/
        if(point->key<kx) /*若根结点关键字小于给定值kx*/
        {
          parent=point;
          point=point->rightchild;/*在右子树上继续进行查找*/
          flag=1;/*标记point为parent的右子结点*/
        }/*endif*/
        else
        {
          parent=point;
          point=point->leftchild; /*若根结点关键字大于给定值kx，在左子树上继续进行查找*/
          flag=0; /*标记point为parent的左子结点*/
        }/*endif*/
      }/*查找失败，插入关键字为kx的新结点在point指针处*/
      point=(bstnode *)malloc(sizeof(bstnode));/*为新结点分配空间*/
      point->key=kx;/*结点关键字值为kx*/
      point->leftchild=NULL;/*新结点为叶子结点*/
      point->rightchild=NULL;
      if(parent==NULL)/*如果原二叉排序树为空*/
        *root=point;/*新结点成为根结点*/
      else
      {
        if(flag==0) /*新结点为上层结点的左子结点*/
          parent->leftchild=point;
        else /*新结点为上层结点的右子结点*/
          parent->rightchild=point;
      }/*endelse*/
      return 0;/*返回新结点的指针*/
    }/* bstinsert函数结束*/
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
        printf("please input the data you want to search:(zero will exit)\n");
        scanf("%d",&kx);  /*输入待查找的值kx*/
        if(kx==0) /*输入值为0，程序结束*/
           break;
        flag=bstinsert(&root,kx); /*查找二叉排序树root是否存在等于kx值的节点*/
        if(flag==1)  /*找到了*/
          printf("the data is found\n");
        else     /*未找到*/
          printf("the data is not found and it has been inserted into the tree\n");
        printf("the inorder traversal of the tree is:");
        inorder(root);/*中序遍历输出二叉排序树*/
        printf("\n");
      }
    }

