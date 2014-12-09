/* 深度优先遍历非连通图，建立生成森林，图使用邻接表存储，生成森林使用孩子兄弟链表存储*/

#include "Stdio.h"
#include "Conio.h"
#include "string.h"
#define MaxNum 50     /*图的最大顶点数为50*/
int visited[MaxNum];  /*标记顶点遍历数组,元素值为0表示该顶点未遍历,为1表示该顶点已遍历*/

typedef struct CSNode{ /*生成森林的孩子兄弟链表存储结构*/
      char data[3];/*数据域，类型为字符数组，最大2个字符长度*/
      struct CSNode *firstchild, *nextsibling;
      } CSNode, *CSTree;

typedef struct enode{               /*边结点结构定义*/
          int adjvex;                   /*邻接点域*/
          struct enode  * next;         /*指针域，指向下一个边结点*/
          /*对于网图，则应增加一个表示权值的域weight*/
         }EdgeNode;        
    typedef struct vnode{              /*顶点结点结构定义*/
          char vertex[3];          /*顶点域，类型为字符数组，2个字符长度*/
          EdgeNode  * firstedge;      /*指针域，指向第一个边结点*/
         }VexNode;       
    typedef VexNode AdjList[MaxNum];  /*邻接表类型定义*/
    typedef struct{  
         AdjList adjlist;                /*邻接表*/
         int n,e;                     /*顶点数和边数*/
        }ALGraph;                   /*ALGraph是以邻接表方式存储的图类型*/

int Locate(ALGraph *G,char vex[])  /*确定顶点在图G中的位置，即在G->V中的序号*/
{
int i;
for(i=0;i<G->n;i++)
if(strcmp(G->adjlist[i].vertex,vex)==0)
return i;
}

void CreateALGraph(ALGraph *G)
     {/*建立有向图的邻接表存储*/
       int i,j,k;
       char vex1[3],vex2[3];
       EdgeNode * s;
       printf("输入图的顶点数和边数(用逗号分隔)：\n");
       scanf("%d,%d",&(G->n),&(G->e));  /*保存输入的顶点数和边数*/
       printf("输入顶点信息(不超过2个字符)：\n");
       for (i=0;i<G->n;i++)              /*建立n个顶点的顶点数组*/
      {scanf("\n%s",G->adjlist[i].vertex);    /*输入每个顶点的标识信息*/
      G->adjlist[i].firstedge=NULL;       /*顶点的邻接表头指针设为空*/
       }
       printf("\n输入图中每条边所依附的两个顶点的的标识信息: ");
       for (k=0;k<G->e;k++)         /*输入e条边建立邻接表*/
        {
        printf("\n输入第%d条边的第1个顶点：",k+1);
        scanf("%s",vex1);
        printf("输入第%d条边的第2个顶点：",k+1);
        scanf("%s",vex2);
        i=Locate(G,vex1);j=Locate(G,vex2); /*i,j为顶点在顶点数组中的下标  */

      s=(EdgeNode*)malloc(sizeof(EdgeNode));  /*生成新的边结点s*/
      s->adjvex=j;                          /*邻接点序号为j*/
      s->next=G->adjlist[i].firstedge;           /*将新的边结点s插入到顶点Vi的邻接表头部*/
      G->adjlist[i].firstedge=s;
       }
}

void PrintGraph(ALGraph *G)/*打印图的顶点和边的信息*/
{
 int i,j;
 EdgeNode * s;
printf("\n图中顶点个数为：%d ",G->n);
printf("\n图中边数为：%d ",G->e);
printf("\n图中顶点为：");
for (i=0;i<G->n;i++)
printf("%s ",G->adjlist[i].vertex);

printf("\n图中顶点的邻接顶点为：");
for(i=0;i<G->n;i++)
{printf("\n%s: ",G->adjlist[i].vertex);
s=G->adjlist[i].firstedge;
while(s)
{printf("%s ",G->adjlist[s->adjvex].vertex);
s=s->next;
}
}
}

void  DFSTree(ALGraph *G,int v ,CSTree T)
{/*从第v个顶点出发深度优先遍历图G，建立以*T为根的生成树*/
 EdgeNode *w;
 CSTree p,q;
 int first;
 visited[v]=1; /*标记v已访问*/
 first=1; /*first是标记,为1表示建立根的左孩子,为0表示建立左孩子的右兄弟*/
 w=G->adjlist[v].firstedge;        /*读取v邻接表的头指针*/
 while(w)
 {
        if(!visited[w->adjvex])
         { p=(CSTree)malloc(sizeof(CSNode));       /*分配孩子结点*/
           strcpy(p->data,G->adjlist[(w->adjvex)].vertex);
           p->firstchild=NULL;
           p->nextsibling=NULL;
           if (first)            /*w是v的第一个未被访问的邻接顶点，作为根的左孩子结点*/
            { T->firstchild=p;
              first=0;
            }
           else { /*w是v的其它未被访问的邻接顶点，作为上一邻接顶点的右兄弟*/
                q->nextsibling=p;           
                }
           q=p;
           DFSTree(G,w->adjvex,q);   /*从第w个顶点出发深度优先遍历图G，建立生成子树*q*/
          }
w=w->next;

}
}

                                 
CSTree DFSForest(ALGraph *G)
{ /*建立无向图G的深度优先生成森林的孩子兄弟链表T*/
      int i;
      CSTree T,p,q;
      T=NULL;
      for (i=0;i<G->n;i++)
        visited[i]=0;                 /*初始化访问标志数组*/
      for(i=0;i<G->n;i++)
        if (!visited[i])                       /*顶点i为新的生成树的根结点*/
          { p=(CSTree )malloc(sizeof(CSNode)); /*分配根结点*/
           strcpy(p->data,G->adjlist[i].vertex);
           p->firstchild=p->nextsibling=NULL;
            /*给根结点赋值*/
            if (!T)  
                 T=p;                    /*T是第一棵生成树的根*/
            else  q->nextsibling=p;            /*前一棵的根的兄弟是其它生成树的根*/
            q=p;                            /*q指示当前生成树的根*/
            DFSTree(G,i,p);                 /*建立以p为根的生成树*/
          }
     return T;
}

void PrintL(CSTree T)
{
CSTree child;
for(child=T->firstchild;child;child=child->nextsibling)
{
printf("(%s, %s) ",T->data,child->data);
PrintL(child);
}
}


void PrintForest(CSTree T)  /*打印生成森林中结点之间的分支，即边*/
{
if(T)
{
printf("\n");
PrintL(T);
PrintForest(T->nextsibling);
}

}

void Print_CSTreeLeft(CSTree T,int i)
{

int j;
CSTree p;
for(j=1;j<=i;j++)printf("  ");/*留出i个空格以表现出层次*/
printf("%s\n",T->data);      /*打印元素，换行*/
for(p=T->firstchild;p;p=p->nextsibling)
    Print_CSTreeLeft(p,i+1);     /*打印子树*/
}

void Print_CSTree(CSTree T,int i)/*按凹入表形式打印输出生成森林的子树的元素，i表示结点所在层次，初次调用时i=0*/
{
if(T)
{
printf("\n");
Print_CSTree(T->nextsibling,i+1);
Print_CSTreeLeft(T,i);

}
}

void Print_BiTree(CSTree T,int i) /*打印生成森林的结点结构，以孩子兄弟链表存储*/
{
int j;

if(T->nextsibling)Print_BiTree(T->nextsibling,i+1);
for(j=1;j<=i;j++)printf("  ");
printf("%s\n",T->data);
if(T->firstchild)Print_BiTree(T->firstchild,i+1);
}

/*
void InOrderTraverse_Tree(CSTree T，void(*visit)( CSTree ))
　{
　　// 以孩子-兄弟链表作存储结构，中序遍历指针T所指森林
CSTree *p;　　
p=T; 　　　　　　　　　　　　　　　　　　　// 指针p指向第一棵树的根
　　while (p) {
　　　InOrderTraverse_Tree(p->firstchild, visit); // 中序遍历*p的子树森林
　　　visit(p); 　　　　　　　　　　　// 通过函数指针*visit 访问根结点*p
　　　p=p->nextsibling; 　　　　　　　　　　　　// 指针p移向下一棵树的根
　　} // while
　}
*/

main()
{
 /* 此处添加你自己的代码 */
 ALGraph alg;
 CSTree T;
 CreateALGraph(&alg);
 /*PrintGraph(&alg);*/
 T=DFSForest(&alg);
 printf("打印生成森林的结点结构图示：\n");
 Print_BiTree(T,0);
 printf("打印生成森林的子树：\n");
 Print_CSTree(T,0);
 printf("打印生成森林中的结点之间的分支：\n");
 PrintForest(T);
getch();
}

