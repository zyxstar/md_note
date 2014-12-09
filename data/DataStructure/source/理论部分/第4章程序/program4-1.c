/*====================================================================*/
/*程序名称：traverse.c                                                */
/*程序目的：建立表达式二叉树并计算其结果                              */
/* ===================================================================*/

#include "stdio.h"
#include "stdlib.h"
int GetValue(int oper,int oper1,int oper2) ;

typedef char TElemType;             /*定义元素类型为字符型*/
typedef struct BTNode               /*定义二叉树的二叉链表结构*/
{TElemType data;                    /*存放结点的数据，字符类型*/
struct BTNode *lchild, *rchild;     /*存放左、右子树的指针*/
}BTNode;

/*--------------------------------------------------------------------*/
/*函数名：CreateBTree                                                 */
/*功能：使用递归建立二叉树                                            */
/*参数说明：nodelist是字符串类型，其中每个字符作为二叉树的一个结点    */
/*          i是整型，表示结点的位置，即字符在nodelist中的位置         */
/*返回值：返回指向二叉树根结点的指针，                                */
/*        如果输入0或者结点个数大于7，则返回NULL                      */
/*--------------------------------------------------------------------*/
BTNode * CreateBTree(char *nodelist,int i)
{
    BTNode *newnode; /*声明新结点指针*/
    if(nodelist[i]==0||i>7)/*递归的终止条件*/
        return NULL;
    else
    {
        /*为新结点分配内存空间*/
        newnode=(BTNode *)malloc(sizeof(BTNode));
        /*将字符存入新结点*/
        newnode->data=nodelist[i];
        /*递归建立左子树*/
        newnode->lchild=CreateBTree(nodelist,2*i);
        /*递归建立右子树*/
        newnode->rchild=CreateBTree(nodelist,2*i+1);
        return newnode;/*返回复制树的位置*/
    }
}
/*--------------------------------------------------------------------*/
/*函数名：PreTraverse                                                 */
/*功能：二叉树前序遍历的递归算法                                      */
/*参数说明：bt是指向结点的指针，指向二叉树的根结点，表示一棵二叉树    */
/*返回值：没有返回值                                                  */
/*--------------------------------------------------------------------*/

void PreTraverse(BTNode *bt)
{
    if(bt!=NULL)                    /*遍历的终止条件*/
    {
        printf("%c",bt->data);      /*打印结点内容*/
        PreTraverse(bt->lchild);      /*递归遍历左子树*/
        PreTraverse(bt->rchild);     /*递归遍历右子树*/
    }
}
/*--------------------------------------------------------------------*/
/*函数名：InTraverse                                                  */
/*功能：二叉树中序遍历的递归算法                                      */
/*参数说明：bt是指向结点的指针，指向二叉树的根结点，表示一棵二叉树    */
/*返回值：没有返回值                                                  */
/*--------------------------------------------------------------------*/
void  InTraverse(BTNode * bt)
{
    if(bt!=NULL)                    /*遍历的终止条件*/
    {
        InTraverse(bt->lchild);       /*递归遍历左子树*/
        printf("%c",bt->data);      /*打印结点内容*/  
        InTraverse(bt->rchild);      /*递归遍历右子树*/
    }
}
/*--------------------------------------------------------------------*/
/*函数名：PostTraverse                                                */
/*功能：二叉树后序遍历的递归算法                                      */
/*参数说明：bt是指向结点的指针，指向二叉树的根结点，表示一棵二叉树    */
/*返回值：没有返回值                                                  */
/*--------------------------------------------------------------------*/
void  PostTraverse(BTNode * bt)
{
    if(bt!=NULL)                    /*遍历的终止条件*/
    {
        PostTraverse(bt->lchild);     /*递归遍历左子树*/
        PostTraverse(bt->rchild);    /*递归遍历右子树*/
        printf("%c",bt->data);      /*打印结点内容*/
    }
}
/*--------------------------------------------------------------------*/
/*函数名：Calculate                                                   */
/*功能：计算表达式结果                                                */
/*参数说明：bt是指向结点的指针，指向二叉树的根结点，表示一棵二叉树    */
/*返回值：返回表达式计算的结果                                        */
/*--------------------------------------------------------------------*/
int Calculate(BTNode * bt)
{
    int oper1=0;/*前操作数*/
    int oper2=0;/*后操作数*/
    if(bt->lchild==NULL && bt->rchild==NULL)
        return bt->data-48;
    else
    { 
        oper1=Calculate(bt->lchild);/*左子数*/
        oper2=Calculate(bt->rchild);/*右子数*/
        return (GetValue(bt->data,oper1,oper2));
    }
}
/*--------------------------------------------------------------------*/
/*函数名：GetValue                                                    */
/*功能：给定操作符和操作数，计算结果                                  */
/*参数说明：oper是操作符，oper1和oper2是两个操作数                    */
/*返回值：没有返回值                                                  */
/*--------------------------------------------------------------------*/
int GetValue(int oper,int oper1,int oper2)
{
    int result=0;
    switch((char)oper)
    { 
        case '*': result=oper1*oper2;return(result);
        case '/':result=oper1/oper2;return(result);
        case '+':result=oper1+oper2;return(result);
        case '-':result=oper1-oper2;return(result);
    }
}
/*--------------------------------------------------------------------*/
/*主函数：main                                                        */
/*功能：建立表达式二叉树，并计算结果                                  */
/*参数说明：无参数                                                    */
/*返回值：没有返回值                                                  */
/*--------------------------------------------------------------------*/
void main()
{
    BTNode * root=NULL;                         /*声明表达式二叉树指针*/
    int result;                                 /*声明结算结果变量*/
    /*声明二叉树数组结点数据*/
    char nodelist[8]={'-','+','*','/','4','8','6','2'};
    root=CreateBTree(nodelist,1);              /*建立表达式二叉树*/
    /*输出前缀表达式*/
    printf("\n 前缀表达式PreTraverse expression:[");
    PreTraverse(root);
    printf("]\n");
    /*输出中缀表达式*/
    printf("\n InTraverse expression:[");
    InTraverse(root);
    printf("]\n");
    /*输出后缀表达式*/
    printf("\n PostTraverse expression:[");
    PostTraverse(root);
    printf("]\n");
    /*计算表达式结果*/
    result=Calculate(root);
    printf("\n calculate result is [%2d]\n",result);
    getchar();
}
