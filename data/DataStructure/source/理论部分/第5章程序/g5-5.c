/* 深度优先搜索遍历图，图以邻接表存储*/
#include "Stdio.h"
#include "Conio.h"

#define MaxNum 50          /*图的最大顶点数为50*/
int visited[MaxNum];
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
void DFSAL(ALGraph *G,int i)
{
/*从顶点Vi出发，对以邻接表存储的图G进行深度优先遍历DFS*/
       EdgeNode *p;
       printf("访问顶点：%s\n",G->adjlist[i].vertex);/*访问顶点Vi*/
       visited[i]=1;                  /*标记Vi已访问*/
       p=G->adjlist[i].firstedge;        /*读取Vi邻接表的头指针*/
       while(p)                     /*在邻接表中依次搜索Vi的邻接点Vj，j=p->adjvex*/
       {if (!visited[p->adjvex])        /*如果顶点Vj未曾访问，则以Vj为出发点DFS*/
           DFSAL(G,p->adjvex);
        p=p->next;                  /*搜索Vi的下一个邻接点*/
        }
}

void DFSTraAL(ALGraph *G)
{/*深度优先搜索遍历图G,该图采用邻接表存储*/
       int i;

       for (i=0;i<G->n;i++)
         visited[i]=0;                 /*初始化访问标志数组*/
       for (i=0;i<G->n;i++)
         if (!visited[i]) DFSAL(G,i);      /*若当前顶点vi未被访问过，则从vi开始进行深度优先遍历DFS*/
}




main()
{
 /* 此处添加你自己的代码 */
 ALGraph alg;
 CreateALGraph(&alg);
 /*PrintGraph(&alg);*/
 printf("深度优先遍历搜索图：\n");
 DFSTraAL(&alg);
getch();
}
