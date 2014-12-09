/*  HELLO.C -- Hello, world */

#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#define MAXSIZE 10


typedef  struct
  {int  data[MAXSIZE];
   int  top;
  }SeqStack;


SeqStack  *Init_SeqStack()  /*ÖÃ¿ÕÕ»*/
{
  SeqStack  *s;
  s=malloc(sizeof(SeqStack));
  s->top=0;  return s;
}


int Empty_SeqStack(SeqStack *s) /*ÅÐ¿ÕÕ»*/
{
   if (s->top==0)  return 1;
   else  return 0;
}


int Push_SeqStack (SeqStack *s, int  x)    /* ÈëÕ»*/
{
  if (s->top==MAXSIZE)  return 0; /*Õ»Âú²»ÄÜÈëÕ»*/
  s->data[s->top]=x;
  s->top++;

  return 1;

}


int  Pop_SeqStack(SeqStack *s, int *x)
{  if  (Empty_SeqStack ( s ) )  return 0; /*Õ»¿Õ²»ÄÜ³öÕ» */
   else  {
          s->top--;
          *x=s->data[s->top-1];

          return 1; }        /*Õ»¶¥ÔªËØ´æÈë*x£¬·µ»Ø*/
}


int  Top_SeqStack(SeqStack *s)
{
      if ( Empty_SeqStack ( s ))  return 0;  /*Õ»¿Õ*/
      else return (s->data[s->top]);
}


main()
{
    SeqStack *ss;
    int select,i;
    int y,*z;
    ss=Init_SeqStack();

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
               Push_SeqStack(ss,y);
               printf("\nthe content:");
               for(i=0;i<ss->top;i++)
               printf("%d ",ss->data[i]);
               break;
        case 2:Pop_SeqStack(ss,z);
               printf("the elements are:\n ");
               for(i=0;i<ss->top;i++)
               printf("%d ",ss->data[i]);
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
