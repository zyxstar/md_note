/*  HELLO.C -- Hello, world */

#include "stdio.h"
#include "stdlib.h"
#define MAXSIZE 10


typedef struct node
 { int  data;
   struct  node *next;
 } QNode;     /*链队结点的类型*/
typedef struct 
 { QNode   *front,*rear;
 }LQueue;     /*将头尾指针封装在一起的链队*/


LQueue  *Init_LQueue()
{
  LQueue *q;
  QNode *p;
  q=malloc(sizeof(LQueue)); /*申请头尾指针结点*/
  p=malloc(sizeof(QNode));  /*申请链队头结点*/
  p->next=NULL;
  q->front=q->rear=p;
  return q;
}


void In_LQueue(LQueue *q , int  x)
{  QNode *p;
   p=malloc(sizeof(QNode));  /*申请新结点*/
   p->data=x;   p->next=NULL;
   q->rear->next=p;
   q->rear=p;
}


int  Empty_LQueue( LQueue *q)
{
   if (q->front==q->rear)   return 0;
   else  return 1;
}


int Out_LQueue(LQueue *q , int  *x)
{  QNode *p;
   if (Empty_LQueue(q) )
    {
      printf ("队空"); return 0;
    }   /*队空，出队失败*/
   else
         { p=q->front->next;
           q->front->next=p->next;
           *x=p->data;/*队头元素放x中*/
           free(p);
           if (q->front->next==NULL)
           q->rear=q->front;  
                /*只有一个元素时，出队后队空，此时还要要修改队尾指针参考图3.16(c)*/
           return 1;
         }
}


main()
{
    LQueue  *lq;
    int select,i;
    int y,*z;
    lq=Init_LQueue();

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
               In_LQueue(lq,y);
               break;
        case 2:Out_LQueue(lq,z);
               printf("the elements are:\n ");
               break;
      }
    printf("\n(1)Input  data");
    printf("\n(2)Onput  data");
    printf("\n(3)Exit");
    printf("\nPlease select one=>");
    scanf("%d",&select);
    printf("\n");
   }
    while(select!=3);
}
