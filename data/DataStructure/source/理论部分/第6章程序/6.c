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

void HeapAdjust(HeapType *H,int s,int m) /*HeapAdjust() 子函数*/
{  int temp,j;
   temp=H->r[s];
   for(j=2*s;j<=m;j*=2)
   {  if(j<m && (H->r[j]<H->r[j+1]))
        ++j;
      if(!(temp<H->r[j]))
        break;
      H->r[s]=H->r[j];
      s=j;
   }
   H->r[s]=temp;
} /*HeapAdjust() end*/

void HeapSort(HeapType *H)          /*HeapSort() 子函数 */
{  int i,temp;
   for(i=H->length/2;i>=1;--i)
       HeapAdjust(H,i,H->length);        /*把H.r[1..H.length]构造成堆*/
   for(i=H->length;i>=1;--i)
   {  temp=H->r[1];                    /*把堆顶记录和未经排序的子序列*/
      H->r[1]=H->r[i];                /*H.r[1..i]中的最后一个记录相交换*/
      H->r[i]=temp;
      HeapAdjust(H,1,i-1);            /*将H.r[1,i-1]重新调整为堆*/
   }
}/*HeapSort() end */

void main()                           /*main() 函数*/
{  int i;
   HeapType H;
   printf("HeapSort.cpp\n");
   printf("============\n");
   printf("Please input the length of SqList (eg,5): ");
   scanf("%d",&H.length);
   for(i=1;i<=H.length;++i)
   {   printf("Please input the %d th element of SqList (eg,58): ",i);
       scanf("%d",&H.r[i]);
   }
   printf("\n");
   printf("The disordered : ");    /*未排序之前 */
   for(i=1;i<=H.length;i++)
       printf("%d  ",H.r[i]);
   HeapSort(&H);                        /*调用 HeapSort()*/
  printf("\n");
  printf("The sorted     : ");    /*排序之后*/
   for(i=1;i<=H.length;i++)
       printf("%d  ",H.r[i]);
   getch();
} /*main() end*/

