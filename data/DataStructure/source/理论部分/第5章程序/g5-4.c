/* 无向图的邻接多重链表表示*/

#include "Stdio.h"
#include "Conio.h"
#define MaxNum 50
    typedef struct EdgeNode{     /*定义边结点类型，假设不考虑边的访问标记和边上的权值等与边相关的信息*/
    int ivex,jvex;                /*该边所依附的两个顶点的位置*/
    struct EdgeNode *ilink, *jlink;  /*指针域，分别指向依附这两个顶点的下一条边*/
    } EdgeNode ;
    typedef struct VexNode{  /*定义顶点结点类型*/
    char vertex[3];          /*顶点值域，类型为字符数组，2个字符长度*/
    EdgeNode *firstedge;    /*指向第一条依附该顶点的边*/
    } VexNode ;
    typedef struct{          /*定义多重链表存储的图类型*/
    VexNode adjmulist[MaxNum];
    int vexnum,edgenum;     /*无向图中的顶点数和边数*/
}AMLGraph;

int Locate(AMLGraph *G,char vex[])  /*确定顶点在图G中的位置，即在G->V中的序号*/
{
int i;
for(i=0;i<G->vexnum;i++)
if(strcmp(G->adjmulist[i].vertex,vex)==0)
return i;
}

void CreateAMLG(AMLGraph *G) /*采用多重链表表示，构造无向图G*/
{
int i,j,k;
char vex1[3],vex2[3];
EdgeNode * s;
printf("输入无向图的顶点数和边数（用逗号分隔：）\n");
scanf ("%d,%d",&(G->vexnum),&(G->edgenum)); 
printf("输入顶点信息(不超过2个字符)：\n");
for (i=0;i<G->vexnum;i++)             /*建立顶点表*/
{ 
scanf("\n%s",G->adjmulist[i].vertex);    /*输入每个顶点的标识信息*/
G-> adjmulist[i].firstedge=NULL;         /*初始化指针*/
}
for(k=0;k<G->edgenum; k++)                    /*输入e条边，构造多重链表*/
{
        printf("\n输入第%d条边的第1个顶点：",k+1);
        scanf("%s",vex1);
        printf("输入第%d条边的第2个顶点：",k+1);
        scanf("%s",vex2);
        i=Locate(G,vex1);j=Locate(G,vex2); /*i,j为边（Vi,Vj）的顶点对应的数组下标 */

    s=(EdgeNode*) malloc (sizeof(EdgeNode));         /*生成新的边结点s */
    s->ivex=i;s->jvex=j;
    s->ilink= G-> adjmulist[i].firstedge; 
    s->jlink= G-> adjmulist[j].firstedge;        /*对边结点进行赋值*/
    G-> adjmulist[i].firstedge=s; 
    G-> adjmulist[j].firstedge=s;              /*完成在多重链表链头的插入*/
}
} 

void PrintGraph(AMLGraph *G)/*打印图的顶点和边的信息*/
{
 int i,j;
 EdgeNode * s;
printf("\n图中顶点个数为：%d ",G->vexnum);
printf("\n图中边数为：%d ",G->edgenum);
printf("\n图中顶点为：");
for (i=0;i<G->vexnum;i++)
printf("%s ",G->adjmulist[i].vertex);

printf("\n图中顶点的邻接顶点为：");
for(i=0;i<G->vexnum;i++)
{printf("\n%s: ",G->adjmulist[i].vertex);
s=G->adjmulist[i].firstedge;
while(s)
{
if((s->ivex)==i)
{printf("%s ",G->adjmulist[s->jvex].vertex);
s=s->ilink;
}
else
{printf("%s ",G->adjmulist[s->ivex].vertex);
s=s->jlink;
}
}
}

}

main()
{
 /* 此处添加你自己的代码 */
 AMLGraph amlg;
 CreateAMLG(&amlg);
 PrintGraph(&amlg);
getch();
}
