#include "stdio.h"
main()
{
  int select;
  int stack[5];
  int i,value;

  printf("\n(1)Input a stack data");
  printf("\n(2)Onput a stack data");
  printf("\n(3)Exit");
  printf("\nPlease select one=>");
  scanf("%d",&select);
  do
  {
    switch(select)
    {
     case 1:printf(\nPlease input the data=>");
            scanf("%d",&value);
            push(value);
            break;
     case 2:value=pop();
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
getch();
}
