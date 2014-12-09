/* 有向图的十字链表表示*/

#include "Stdio.h"
#include "Conio.h"
#define  MaxNum 50
    typedef struct ArcNode {/*定义弧结点类型，假设不考虑弧上的权值等与弧相关的信息*/
    int tailvex,headvex;  /*弧尾顶点和弧头顶点的位置*/
    struct  ArcNode  * hlink, *tlink; /*指针域，指向相同弧头和弧尾的其他弧*/
    }ArcNode;
    typedef struct VexNode {/*定义顶点结点类型*/
    char vertex[3] ;      /*顶点值域，类型为字符数组，2个字符长度*/
    ArcNode  *firstin, *firstout;  /*分别指向该顶点第一条入弧和出弧*/
    }VexNode;
    typedef struct {/*定义十字链表存储的图类型*/
    VexNode vlist[MaxNum]; /*顶点表*/
    int  vexnum,arcnum;  /*有向图中的顶点数和弧数*/
}OLGraph;

int Locate(OLGraph *G,char vex[])  /*确定顶点在图G中的位置，即在G->V中的序号*/
{
int i;
for(i=0;i<G->vexnum;i++)
if(strcmp(G->vlist[i].vertex,vex)==0)
return i;
}

void CreateOLG(OLGraph *G) /*采用十字链表表示，构造有向图G*/
{  
int i,j,k;
char vex1[3],vex2[3];
ArcNode * s;
printf("输入有向图的顶点数和弧数（用逗号分隔：）\n");
scanf ("%d,%d",&(G->vexnum),&(G->arcnum)); 
        printf("输入顶点信息(不超过2个字符)：\n");
for (i=0;i<G->vexnum;i++)                       /*建立顶点表*/
        { 
scanf("\n%s",G->vlist[i].vertex);                 /*输入每个顶点的标识信息*/
     G->vlist[i].firstin=NULL;G->vlist[i].firstout =NULL;  /*初始化指针*/
        }
       for(k=0;k<G->arcnum; k++)                        /*输入e条弧，构造十字链表*/
        {
         printf("\n输入第%d条边的第1个顶点：",k+1);
        scanf("%s",vex1);
        printf("输入第%d条边的第2个顶点：",k+1);
        scanf("%s",vex2);
        i=Locate(G,vex1);j=Locate(G,vex2); /*i,j为弧<Vi,Vj>的顶点对应的数组下标 */

         s=(ArcNode*) malloc (sizeof(ArcNode));            /*生成新的弧结点s */
         s->tailvex=i;s->headvex=j;
         s->hlink=G->vlist[j].firstin;
         s->tlink=G->vlist[i].firstout;
         /*
         *s={ i,j,G->vlist[j].firstin,G->vlist[i].firstout};        对弧结点进行赋值*/
                                                     /*{tailvex,headvex,hlink,tlink}*/
         G->vlist[j].firstin=G->vlist[i].firstout=s;             /*完成在入弧和出弧链表链头的插入*/
        }
}
void PrintGraph(OLGraph *G)/*打印图的顶点和边的信息*/
{
 int i,j;
 ArcNode * s;
printf("\n图中顶点个数为：%d ",G->vexnum);
printf("\n图中边数为：%d ",G->arcnum);
printf("\n图中顶点为：");
for (i=0;i<G->vexnum;i++)
printf("%s ",G->vlist[i].vertex);

printf("\n图中顶点的邻接顶点为：");
for(i=0;i<G->vexnum;i++)
{printf("\n%s: ",G->vlist[i].vertex);
s=G->vlist[i].firstout;
while(s)
{printf("%s ",G->vlist[s->headvex].vertex);
s=s->tlink;
}
}

printf("\n图中顶点的逆邻接顶点为：");
for(i=0;i<G->vexnum;i++)
{printf("\n%s: ",G->vlist[i].vertex);
s=G->vlist[i].firstin;
while(s)
{printf("%s ",G->vlist[s->tailvex].vertex);
s=s->hlink;
}
}

}

main()
{
 /* 此处添加你自己的代码 */
 OLGraph olg;
 CreateOLG(&olg);
 PrintGraph(&olg);
getch();
}
