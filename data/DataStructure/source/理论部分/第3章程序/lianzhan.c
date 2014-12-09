/*  HELLO.C -- Hello, world */

#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#define MAXSIZE 10


typedef  struct node
{ int data;
  struct node *next;
}StackNode,* LinkStack;

LinkStack  Init_LinkStack()  /*ÖÃ¿ÕÕ»*/
{
  return  NULL;
}

int  Empty_LinkStack(LinkStack  top)/*ÅÐ¿ÕÕ»*/
{  if(top==NULL) return 1;
       else  return  0;        
}

LinkStack  Push_LinkStack(LinkStack  top, int x)/* ÈëÕ»*/
{
   StackNode  *s;
   s=malloc(sizeof(StackNode));
   s->data=x;
   s->next=top;
   top=s;
   return top;
}


LinkStack   Pop_LinkStack (LinkStack  top, int  *x)
{  StackNode  *p;
   if (top==NULL) return NULL;
   else
   { *x = top->data;
   p = top;
   top = top->next;
   free (p);
   return  top;
     }
}


main()
{
    LinkStack ls,p;
    int select;
    int y,*z;
    ls=Init_LinkStack();

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
               Push_LinkStack(ls,y);
               printf("***%d ",y);
               printf("\nthe content:");
               p=ls;
               while(p!=NULL)
                 {
                   printf("%d ",p->data);
                   p=p->next;
                  }
               break;
        case 2:Pop_LinkStack(ls,z);
               printf("the elements are:\n ");
               p=ls;
               while(p!=NULL)
                 printf("%d ",p->data);
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
