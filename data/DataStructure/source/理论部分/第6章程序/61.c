      /* ==================Program Description================== */
/* 程序名称：HeapSort.cpp                                  */
/* 程序目的：堆排序               */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/
# include <conio.h>
# define MAXSIZE 20
typedef int RedType;

typedef struct                        /*定义 SqList*/
{   RedType    r[MAXSIZE+1];
    int length;
}SqList;
typedef SqList HeapType;

void HeapAdjust(sqlist r,int l,int m) /*排列元素r[1]~r[n]*/
{  int i,j;
   struct rec x;
   i=1;
   j=2*i;
   x=r[i];
   while (j<=m)
 {
    if (j<m && r[j].key<r[j+1].key) j++;
    if (x.key<r[j].key)
    {
      r[i]=r[j];
      i=j;
      j=2*i;
     }
     elst j=m+1;
    }
    r[i]=x;
} /*HeapAdjust() end*/

void HeapSort(sqlist r, int n)          /*HeapSort() 子函数 */
{
   int i;
   struct rec m;
   for(i=n/2;i>=1;i--) HeapAdjust(r,i,n);
      for (i=n;i>=2;i--)
      {
        m=r[i];
        r[i]=r[1];
        r[1]=m;
        HeapAdjust((r,1,i-1);
       }
 }



void main()                           /*main() 函数*/
{  int i;
   HeapType r;
   printf("HeapSort.cpp\n");
   printf("============\n");
   printf("Please input the length of SqList (eg,5): ");
   scanf("%d",&r.length);
   for(i=1;i<=r.length;++i)
   {   printf("Please input the %d th element of SqList (eg,58): ",i);
       scanf("%d",&r.r[i]);
   }
   printf("The disordered : ");    /*未排序之前 */
   for(i=1;i<=r.length;i++)
       printf("%d  ",r.r[i]);
   HeapSort(&r);                        /*调用 HeapSort()*/
  printf("\n");
  printf("The sorted     : ");    /*排序之后*/
   for(i=1;i<=H.length;i++)
       printf("%d  ",H.r[i]);
   getch();
} /*main() end*/

