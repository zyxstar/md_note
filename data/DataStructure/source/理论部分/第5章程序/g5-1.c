/*图的邻接矩阵表示*/

#include "Stdio.h"
#include "Conio.h"
#include"stdio.h"
       #define MaxNum 50        /*图的最大顶点数设为50*/
    typedef char VexType[3];           /*图的顶点类型设为字符数组，2个字符长度*/
    typedef int EdgeType;              /*弧（边）的权值设为整型*/
    typedef struct {
       VexType V[MaxNum]; /*一维数组存储顶点信息*/
       EdgeType E[MaxNum][MaxNum]; /*二维数组表示邻接矩阵，存储边的信息*/
       int n,e;                       /*顶点数和边数*/
      }MGraph;                      /*图类型定义*/

int Locate(MGraph *G,char vex[])  /*确定顶点在图G中的位置，即在G->V中的序号*/
{
int i;
for(i=0;i<G->n;i++)
if(strcmp(G->V[i],vex)==0)
return i;
}

void CreateGraph(MGraph *G)
      {
       int i,j,k;
       char vex1[3],vex2[3];/*保存输入的顶点*/
       printf("\n输入图的顶点数和边数(用逗号分隔):");
       scanf("%d,%d",&(G->n),&(G->e));/*输入顶点数和边数*/
       printf("输入图的顶点信息(最长2个字符):");
for (i=0;i<G->n;i++)
{
 /*G->V[i][0]=G->V[i][1]=0;*/
 scanf("\n%s",G->V[i]); /*输入n个顶点的标识信息，建立顶点数组*/
}
       for (i=0;i<G->n;i++)
   for (j=0;j<G->n;j++)  
G->E[i][j]=0;  /*对邻接矩阵元素进行初始化*/
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
         G->E[i][j]=1;             /*如果同时输入G->E[j][i]=1;，则是建立无向图的邻接矩阵*/
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


main()
{
MGraph mg;

CreateGraph(&mg);
PrintGraph(&mg);

/*printf("图中顶点个数为：%d,图中边数为：%d",mg.n,mg.e);
printf("\n图中顶点为：");
for (i=0;i<mg.n;i++)
printf("%c%c ",mg.V[i][0],mg.V[i][1]); 输入n个顶点的标识信息，建立顶点数组
printf("\n图中边为：");
for(i=0;i<mg.n;i++)
   for(j=0;j<mg.n;j++)
    if(mg.E[i][j])
        printf("[%c%c,%c%c] ",mg.V[i][0],mg.V[i][1],mg.V[j][0],mg.V[j][1]);*/

getch();
}
