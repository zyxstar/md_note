/* ==================Program Description================== */
/* 程序名称：MergeSort.cpp                                 */
/* 程序目的：归并排序               */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/


#include <conio.h>

#define MAXSIZE 20
#define LENGTH 7
typedef int RedType;

typedef struct                /*定义 SqList*/
{   RedType    r[MAXSIZE+1];    
    int length;
}SqList;
typedef SqList RcdType;

void Merge(RcdType *SR,RcdType *TR,int i,int m,int n)/*Merge() 子函数*/
/*将有序的SR[i..m]和有序的SR[m+1..n]归并为有序的TR[i..n]*/
{   int j,k;
    for(j=m+1,k=i;i<=m&&j<=n;++k)    /*将SR中的记录由小到大地并入TR*/
    {   if(SR->r[i]<=SR->r[j])
            TR->r[k]=SR->r[i++];
        else
            TR->r[k]=SR->r[j++];
    }
    while(i<=m)               /*将剩余的SR[i..m]复制到TR*/
       TR->r[k++]=SR->r[i++];
    while(j<=n)                /*将剩余的SR[j..n]复制到TR*/
       TR->r[k++]=SR->r[j++];
}/*end of Merge() function*/

void MSort(RcdType *SR,RcdType *TR1,int s, int t)/*MSort() 子函数*/
/*将SR[s..t]归并排序为TR1[s..t]*/
{  int m;
   RcdType TR2;
   if(s==t)
     TR1->r[s]=SR->r[t];
   else
   {  m=(s+t)/2;            /*将SR[s..t]平分为SR[s..m]和SR[m+1..t]*/
      MSort(SR,&TR2,s,m);    /*递归地将SR[s..m]归并为有序的TR2[s..m]*/
      MSort(SR,&TR2,m+1,t);    /*递归地将SR[m+1..t]归并为有序的TR2[m+1..t]*/
      Merge(&TR2,TR1,s,m,t);   /*将TR2[s..m]和TR2[m+1..t]归并到TR1[s..t]*/
   }/*end of else */
}/*end of MSort() function*/

void MergeSort(SqList *L)    /*MergeSort() 函数*/
/* 对顺序表L作归并排序*/
{
   MSort(L,L,1,L->length);
}/*end of MergeSort() 函数*/

void main()                    /*main 函数 */
{  int i;
   SqList L;
   printf("MergeSort.cpp \n");
   printf("============= \n");
   printf("Please input the length of SqList L: <eg,8>:");
   scanf("%d",&L.length);
  /* printf("Please input the disordered array L.r: ");*/
   for(i=1;i<=L.length;++i)
        {   printf("Please input the %d th element of SqList (eg,58):",i);
            scanf("%d",&L.r[i]);
        }
   printf("\n");
   printf("The disordered : ");        /*未排序的线性表*/
   for(i=1;i<=L.length;i++)
       printf("%d ",L.r[i]);
   printf("\n");
   MergeSort(&L);                        /*调用 MergeSort()*/
   printf("The sorted     : ");        /*排序后的*/
   for(i=1;i<=L.length;i++)
       printf("%d ",L.r[i]);
   getch();
}                                     /*main() end */

