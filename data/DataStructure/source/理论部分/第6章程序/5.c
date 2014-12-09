/* ==================Program Description================== */
/* 程序名称：SelectSort.cpp                                   */
/* 程序目的：直接选择排序               */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/  
# include <conio.h>
# define MAXSIZE 20
typedef int RedType;

typedef struct            /*定义 SqList*/
{   RedType    r[MAXSIZE+1];
    int length;
}SqList;

void SelectSort(SqList *L)        /*SelectSort() 子函数*/
{   int i,j,k,temp;
    for(i=1;i<L->length;++i)    /*选择第i个小的记录,并交换到位*/
    {   k=i;
        for(j=i+1;j<=L->length;++j) /*在L.r[i..length]中选择值最小的记录*/
            if(L->r[j]<L->r[k])
                k=j;
        if(i!=k)
        {   temp=L->r[i];        /*与第i个记录交换*/
            L->r[i]=L->r[k];
            L->r[k]=temp;
        }
    }
}/*SelectSort() end */

void main()            /*main() 函数*/ 
{  int i;
   SqList L;
   printf("SelectSort.cpp \n");
   printf("==============\n");
   printf("Please input the length of SqList (eg,5): ");
   scanf("%d",&L.length);
   for(i=1;i<=L.length;++i)
   {   printf("Please input the %d th element of SqList (eg,58): ",i);
      scanf("%d",&L.r[i]);
   }
   printf("\n");
   printf("The disordered : ");        /*未排序的线性表*/
   for(i=1;i<=L.length;i++)
       printf("%d  ", L.r[i]);
   SelectSort(&L);                       /*调用 SelectSort()*/
    printf("\n");
   printf("The sorted     : ");        /*排序后的线性表*/
   for(i=1;i<=L.length;i++)
     printf("%d  ",L.r[i]);
   getch();
} /*main() end*/ 

