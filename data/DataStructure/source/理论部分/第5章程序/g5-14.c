/* AOE网的关键路径,图以带入度的邻接表表示,顶点表示事件,弧表示活动,弧上的权值表示活动持续的时间*/

#include "Stdio.h"
#include "Conio.h"

#define MaxNum 50          /*图的最大顶点数为50*/

typedef struct
{
int elem[MaxNum];
int top;
}sqstacktp;

sqstacktp T;/*全局变量，用于存储拓扑逆序的栈*/
int ve[MaxNum];/*全局变量，存储顶点事件的最早发生时间*/

void ini_sqstack(sqstacktp *s)/*初始化栈*/
{
    s->top=0;
}

int empty_sqstack(sqstacktp *s)/*判断栈是否为空*/
{
    if((*s).top>0)
    {
        return(0);
    }
    else
    {
        return(1);
    }
}

void push_sqstack(sqstacktp *s,int x)/*入栈*/
{
 if((*s).top==MaxNum)/*若栈s未满，将元素x压入栈中；否则，栈的状态不变并给出出错信息*/
    printf("Overflow");
 else
          {
           (*s).elem[(*s).top]=x;     /*x进栈*/
           (*s).top++;      /*栈顶指针加1*/
           }
}


int pop_sqstack(sqstacktp *s)/*出栈*/
    {
        if((*s).top==0)/*若栈s不空，则删去栈顶元素并返回元素值，否则返回空元素NULL*/
          return(NULL);
        else{
           (*s).top--;          /* 栈顶指针减1*/
           return((*s).elem[(*s).top]);   /*返回原栈顶元素值*/
        }
}

typedef struct enode{               /*边结点结构定义*/
          int adjvex;               /*邻接点域*/
          int cost;                 /*存储弧上的权值,即活动持续的时间*/
          struct enode  * next;     /*指针域，指向下一个边结点*/

         }EdgeNode;        
    typedef struct vnode{             /*顶点结点结构定义*/
          char vertex[3];             /*顶点域，类型为字符数组，2个字符长度*/
          int indegree;               /*保存顶点入度的数据域*/
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
       int i,j,k,cost;
       char vex1[3],vex2[3];
       EdgeNode * s;
       printf("输入图的顶点数和边数(用逗号分隔)：");
       scanf("%d,%d",&(G->n),&(G->e));  /*保存输入的顶点数和边数*/
       printf("\n输入顶点信息(不超过2个字符)：");
       for (i=0;i<G->n;i++)              /*建立n个顶点的顶点数组*/
      {scanf("\n%s",G->adjlist[i].vertex);    /*输入每个顶点的标识信息*/
      G->adjlist[i].indegree=0;           /*初始化顶点的入度为0*/
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
        printf("输入第%d条边的权值：",k+1);
        scanf("%d",&cost);
        
      s=(EdgeNode*)malloc(sizeof(EdgeNode));/*生成新的边结点s*/
      s->adjvex=j;                          /*邻接点序号为j*/
      s->cost=cost;
      s->next=G->adjlist[i].firstedge;      /*将新的边结点s插入到顶点Vi的邻接表头部*/
      G->adjlist[i].firstedge=s;
      G->adjlist[j].indegree++;             /*弧头顶点vj的入度增1*/
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

int TopoSort(ALGraph *G)
{
int i,j,k,count;
EdgeNode * p;
sqstacktp stack;  /*存储入度为0的顶点*/

ini_sqstack(&stack);
ini_sqstack(&T);

for(i=0;i<G->n;i++)
{
ve[i]=0; /*初始化ve数组*/
if(G->adjlist[i].indegree==0)
    push_sqstack(&stack,i);
}

count=0;  /*对输出顶点计数*/
printf("\n拓扑排序序列为: ");
while(!empty_sqstack(&stack))
{
j=pop_sqstack(&stack);
push_sqstack(&T,j); /*将拓扑排序输出的顶点压入堆栈T*/
printf("%s ",G->adjlist[j].vertex);
count++;
p=G->adjlist[j].firstedge;
while(p!=NULL)
{
k=p->adjvex;
G->adjlist[k].indegree--;
if(G->adjlist[k].indegree==0)
    push_sqstack(&stack,k);
if(ve[j]+p->cost>ve[k])
    ve[k]=ve[j]+p->cost; /*修改事件k的最早发生时间*/
p=p->next;
}
}

return(count);

}

void CriticalPath(ALGraph *G)
{
int i,j,k;
int ee,el;/*弧活动的最早开始时间和最晚开始时间*/
EdgeNode * p;
int vl[MaxNum]; /*存储事件的最晚开始时间*/

int count;   /*存储拓扑排序的返回值,若小于顶点个数,则AOE网存在环*/
count=TopoSort(G);
if(count<G->n)
{
printf("\n提示:该有向图存在回路!");
return;
}

for(i=0;i<G->n;i++)
    vl[i]=ve[G->n-1];/*初始化vl数组*/

while(!empty_sqstack(&T))
{
j=pop_sqstack(&T);
p=G->adjlist[j].firstedge;
while(p!=NULL)
{
k=p->adjvex;
if(vl[k]-p->cost<vl[j])
    vl[j]=vl[k]-p->cost; /*修改事件j的最晚发生时间*/
p=p->next;
}
}

for(j=0;j<G->n;j++)/*求关键路径，显示关键顶点事件*/
{
 p=G->adjlist[j].firstedge;
while(p!=NULL)
{
k=p->adjvex;
ee=ve[j]; /*活动<vj,vk>的最早开始时间*/
el=vl[k]-p->cost; /*活动<vj,vk>的最晚开始时间*/
if(ee==el)
printf("\n关键活动：<%s,%s> 持续时间：%d, 最早开始时间：%d, 最晚开始时间：%d",
    G->adjlist[j].vertex,G->adjlist[k].vertex,p->cost,ee,el);
else
printf("\n活动：<%s,%s> 持续时间：%d, 最早开始时间：%d, 最晚开始时间：%d",
    G->adjlist[j].vertex,G->adjlist[k].vertex,p->cost,ee,el);
p=p->next;
}
}

}

main()
{
 /* 此处添加你自己的代码 */
 ALGraph alg;
 CreateALGraph(&alg);
 CriticalPath(&alg);
getch();
}
