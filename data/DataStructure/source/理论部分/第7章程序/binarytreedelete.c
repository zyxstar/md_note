/* ==================Program Description================== */
/* 程序名称：binarytreedelete.c                                       */
/* 程序目的：在以*root为根结点的二叉排序树上删除关键字等于kx的记录*/
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
    void inorder(bstnode *root) /*中序遍历以*root为根结点的二叉排序树*/
    {
      bstnode *point;
      point=root;
      if(point!=NULL)/*二叉树不为空*/
      {
        inorder(point->leftchild);/*中序遍历左子树*/
        printf("%d ",point->key);
        inorder(point->rightchild); /*中序遍历右子树*/
      }/*endif*/
    }/*inorder函数结束*/
    int bstdelete(bstnode **root,int kx)/*删除二叉排序树中的结点*/
    {
      bstnode *point,*left;
      if(*root==NULL)/*二叉排序树为空，返回值0*/
      {
        return 0;
      }
      if(kx<(*root)->key)/*待删除值小于根结点的值，在左子树上删除*/
        return bstdelete(&(*root)->leftchild,kx);
      if(kx>(*root)->key) /*待删除值大于根结点的值，在右子树上删除*/
        return bstdelete(&(*root)->rightchild,kx);
      point=*root;
      if((point->leftchild==NULL)&&(point->rightchild==NULL))/*待删除结点是叶子结点*/
      {
        *root=NULL;
        free(point);
        return 1;
      }
      if(point->leftchild==NULL)/*待删除值等于根结点的值，且根结点无左子树*/
      {
        *root=(*root)->rightchild;
        free(point);
        return 1;
      }
      if(point->rightchild==NULL) /*待删除值等于根结点的值，且根结点无右子树*/
      {
        *root=(*root)->leftchild;
        free(point);
        return 1;
      }
      else/*待删除值等于根结点的值，且根结点既有左子树，又有右子树*/
      {
        if((*root)->leftchild->rightchild==NULL)
        {
          (*root)->key=(*root)->leftchild->key;
          return bstdelete(&point->leftchild,(*root)->key);
        }
        left=point->leftchild;
        while(left->rightchild!=NULL)/*寻找待删除根结点左子树最右端的结点*/
        {
          point=left;
          left=left->rightchild;
        }
        (*root)->key=left->key;
        return bstdelete(&point->rightchild,(*root)->key);
      }/*endelse*/
    }
    main()
    {
      bstnode *root;
      int kx,flag,value,key;
      root=NULL;
      while(1)/*建立二叉排序树*/
      {
        printf("please input the data to create the binary tree:(zero will exit)\n");
        scanf("%d",&kx);  /*输入待查找的值kx*/
        if(kx==0) /*输入值为0，程序结束*/
          break;
        flag=bstinsert(&root,kx); /*查找二叉排序树root是否存在等于kx值的节点*/
        printf("the inorder traversal of the tree is:");
        inorder(root);/*中序遍历输出二叉排序树*/
        printf("\n");
      }
      while(1)
      {
        printf("please input the data you want to delete:(zero will exit)\n");
        scanf("%d",&key);  /*输入待查找的值kx*/
        if(key==0) /*输入值为0，程序结束*/
          break;
        flag=bstdelete(&root,key);
        if(flag==0)
         printf("the data is not exist\n");
        if(flag==1)
        {
          printf("the data is be deleted\n");
          printf("the inorder traversal of the tree is:");
          inorder(root);/*中序遍历输出二叉排序树*/
          printf("\n");
        }
      }
    }/*主函数结束*/

