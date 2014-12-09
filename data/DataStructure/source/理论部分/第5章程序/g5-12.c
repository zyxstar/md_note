/*迪杰斯特拉算法，有向网使用邻接矩阵存储 */

#include "Stdio.h"
#include "Conio.h"

#define MaxNum 50          /*图的最大顶点数为50*/
#define MAX 32767          /*表示计算机中最大整数,假设为边上权值不能取的无穷大*/
#define TRUE 1
#define FALSE 0

typedef char VexType[3];        /*图的顶点类型设为字符数组，2个字符长度*/
typedef struct {
    VexType V[MaxNum];          /*一维数组存储顶点信息*/
    int E[MaxNum][MaxNum]; /*二维数组表示邻接矩阵，存储边的信息*/
    int n,e;                    /*顶点数和边数*/
}MGraph;                        /*图类型定义*/

int Locate(MGraph *G,char vex[])  /*确定顶点在图G中的位置，即在G->V中的序号*/
{
int i;
for(i=0;i<G->n;i++)
if(strcmp(G->V[i],vex)==0)
return i;
}

void CreateGraph(MGraph *G)
      {
       int i,j,k,cost;
       char vex1[3],vex2[3];/*保存输入的顶点*/
       printf("\n输入图的顶点数和边数(用逗号分隔):");
       scanf("%d,%d",&(G->n),&(G->e));/*输入顶点数和边数*/
       printf("输入图的顶点信息(最长2个字符):");
for (i=0;i<G->n;i++)
{

 scanf("\n%s",G->V[i]); /*输入n个顶点的标识信息，建立顶点数组*/
}
       for (i=0;i<G->n;i++)
   for (j=0;j<G->n;j++)
   {if(j==i)  G->E[i][j]=0;
    else G->E[i][j]=MAX;  /*对邻接矩阵元素进行初始化*/
   }
       printf("\n输入图中每条边所依附的两个顶点的标识信息:");
       for (k=0;k<G->e;k++)         /*输入e条边*/
        {
        printf("\n输入第%d条边的第1个顶点：",k+1);
        scanf("%s",vex1);
        printf("输入第%d条边的第2个顶点：",k+1);
        scanf("%s",vex2);
        /* scanf("%s\n%s",vex1,vex2);*/
        i=Locate(G,vex1);j=Locate(G,vex2);
   /*     scanf("\n%d,%d",&i,&j); */   /*输入每条边所依附的顶点在二维数组中的下标，建立邻接矩阵*/
        printf("输入第%d条边的权值：",k+1);
        scanf("%d",&cost);
         G->E[i][j]=cost;             /*如果同时输入G->E[j][i]=cost;，则是建立无向图的邻接矩阵*/
        }
}
void PrintGraph(MGraph *G)/*打印图的顶点和边的信息*/
{
int i,j;
printf("\n图中顶点个数为：%d",G->n);
printf("\n图中边数为：%d",G->e);
printf("\n图中顶点为：");
for (i=0;i<G->n;i++)
printf("%c%c ",G->V[i][0],G->V[i][1]);
printf("\n图中边为：");
for(i=0;i<G->n;i++)
   for(j=0;j<G->n;j++)
    if(G->E[i][j])
        printf("[%c%c,%c%c] ",G->V[i][0], G->V[i][1], G->V[j][0], G->V[j][1]);
}


void Dijkstra(MGraph *G)
    { /*用Dijkstra算法求有向网G的v0顶点到其余顶点v的最短路径P[v]及其路径长度D[v]

    v0为源点在一维数组中的序号，p[n][n]为二维数组，第v行中为1的元素表示了从源点到v的最短路径上所经过的顶点。
      若P[v][w]为TRUE，则w是从v0到 v当前求得最短路径上的顶点*/
      /*final[v] 为TRUE当且仅当v∈S, ，即已经求得从v0到v的最短路径*/

   int final[MaxNum];/*为TRUE表示表示该元素属于最短路径的终点集合S，为FALSE表示还不是*/
   int P[MaxNum][MaxNum];/*P[w][v]为TRUE，表示从源点到顶点w的最短路径经过顶点v*/
   int D[MaxNum];/*源点到顶点v的路径长度D[v]*/
   int v0,v,w;/*顶点序号变量,v0为源点*/
   int i,j,min; /*i,j是循环变量，min保存当前的路径最小值*/
   char vex[3];

   printf("\n输入源点：");
   scanf("%s",vex);
   v0=Locate(G,vex);

   for (v=0;v<G->n;v++)
   {
    final[v]=FALSE; /*S集合初始为空*/
    D[v]=G->E[v0][v];/*源点到各终点的最短路径的初值*/

    for (w=0; w<G->n;w++)  P[v][w]=FALSE;  /*设空路径*/

    if(D[v]<MAX) {P[v][v0]=TRUE; }/*从v0到v的当前的最短路径*/
    }

    D[v0]=0; final[v0]=TRUE;        /*初始化，v0顶点属于S集*/

    /*开始主循环，每次求得v0到某个v 顶点的最短路径，并加v到S集*/
      for(i=1; i<G->n; i++)          /*其余n-1个顶点*/
      {
      min=MAX;                       /*min为当前所知离v0顶点的最近距离*/
      for (w=0;w<G->n;w++)
        if (!final[w])               /*w顶点在V－S中*/
            if (D[w]<min)
             {v=w; min=D[w];}        /*v保存最短路径的终点的序号*/

      final[v]=TRUE;                 /*离v0顶点最近的v加入S集合*/

      for(w=0;w<G->n;w++)                      /*更新当前最短路径*/
         if(!final[w]&&(min+G->E[v][w]<D[w])&&(G->E[v][w]!=MAX))  /*修改D[w]和P[w],w∈V-S*/
            {
             D[w]=min+G->E[v][w];

             for(j=0;j<G->n;j++)
                P[w][j]=P[v][j];     /*P[w]=P[v]; */
             P[w][v]=TRUE;              /*源点到w的最短路径是源点到v的最短路径加上v到w的弧。*/
            }
      }

for(w=0;w<G->n;w++)
{
if(w!=v0)
{
printf("\n源点%s到终点%s的最短路径长度为：%d\n",G->V[v0],G->V[w],D[w]);

printf("经过的顶点为：");
printf("%s  ",G->V[v0]);
for(v=0;v<G->n;v++)
    if(v!=v0 && P[w][v])
        printf("%s  ",G->V[v]);
printf("%s  ",G->V[w]);

}

}

}


main()
{
 /* 此处添加你自己的代码 */
 MGraph mg;
 CreateGraph(&mg);
 Dijkstra(&mg);

getch();
}
