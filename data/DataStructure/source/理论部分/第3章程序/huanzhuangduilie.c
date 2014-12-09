/*  HELLO.C -- Hello, world */

#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#define MAXSIZE 10


typedef  struct     { 
    int data[MAXSIZE];  /*数据的存储区*/
    int front,rear;  /*队头队尾指针*/
  }c_SeQueue;    /*循环队*/



c_SeQueue*  Init_SeQueue()  /*创建一个带头结点的空队*/
 {
  c_SeQueue *q;
  q=malloc(sizeof(c_SeQueue));
  q->front=q->rear=0;
  return q;
 }

int In_SeQueue(c_SeQueue *q ,int x) /*入队列*/
{
         if ( (q->rear+1) % MAXSIZE==q->front)
        {
          printf("队满");
          return -1;   /*队满不能入队*/
        }
        else
        {
         q->rear=(q->rear+1) % MAXSIZE;
         q->data[q->rear]=x;
         return 1;    /*入队完成*/
        }
}


int Out_SeQueue(c_SeQueue *q ,int *x) /*出队列*/
{
      if(q->rear==q->front)
      {
        printf("队空");
        return -1;   /*队空不能出队*/
      }
     else
      {
       q->front=(q->front+1)%MAXSIZE;
       *x=q->data[q->front]; /*读出队头元素*/
       return 1;    /*出队完成*/
       }
}


int Empty_SeQueue(c_SeQueue  *q)    /*判队空*/
{  
  if  (q->rear == q->front)  return 1;
       else return 0;
}

int Full_SeQueue(c_SeQueue  *q)    /*判队空*/
{  
  if  ((q->rear+1) % MAXSIZE== q->front)  return 1;
  else return 0;
}

main()
{
    c_SeQueue *cq;
    int select,i;
    int y,*z;
    cq=Init_SeQueue();

    printf("\n(1)Input a stack data");
    printf("\n(2)Onput a stack data");
    printf("\n(3)Exit");
    printf("\nPlease select one=>");
    scanf("%d",&select);
    do
    {
      switch(select)
      {
        case 1:printf("\nPlease input the data=>");
               scanf("%d",&y);
               In_SeQueue(cq ,y);

               break;
        case 2:Out_SeQueue(cq,z);
               printf("the elements are:\n ");
               for(i=cq->front+1;i!=(cq->rear+1)%MAXSIZE;i=(i+1)%MAXSIZE)
                 printf("%d ",cq->data[i]);
               break;
      }
    printf("\n(1)Input a stack data");
    printf("\n(2)Onput a stack data");
    printf("\n(3)Exit");
    printf("\nPlease select one=>");
    scanf("%d",&select);
    printf("\n");
   }
    while(select!=3);
}
