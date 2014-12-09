/* ==================Program Description================== */
/* 程序名称：QuickSort.cpp                                    */
/* 程序目的：快速排序               */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/  
# include <conio.h>
# define MAXSIZE 20
typedef int RedType;
typedef struct                    /*定义 structure */
{   RedType    r[MAXSIZE+1];
    int length;
}SqList;

int  Partition(SqList *L,int low,int high)    /*Partition() 子函数 */
{  int pivotkey;
   L->r[0]=L->r[low];            /*用子表的第一个记录作"枢纽"记录*/
   pivotkey=L->r[low];        /*pivotkey是枢纽的记录关键字*/
   while(low<high)            /*从表的两端交替地向中间扫描*/
      {  while(low<high&&L->r[high]>=pivotkey)
        --high;
     L->r[low]=L->r[high];    /*比"枢纽"小的记录移到底端*/
     while(low<high&&L->r[low]<=pivotkey)
        ++low;                
     L->r[high]=L->r[low];    /*比"枢纽"大的记录移到高端*/
      }
   L->r[low]=L->r[0];            /*"枢纽"记录到位*/
   return (low);            /*返回"枢纽"位置*/
} /*Partition() end */

void Qsort(SqList *L,int low,int high)    /*Qsort() sub-function */
{  int pivotloc;
   if(low<high)                    /*长度大于1*/
   {  pivotloc=Partition(L,low,high);
      Qsort(L,low,pivotloc-1);
      Qsort(L,pivotloc+1,high);
   }
}

void QuickSort(SqList *L)        /*QuickSort() 子函数*/
{  Qsort(L,1,L->length);            /*调用 Qsort()*/
}

void main()                        /*main() 函数*/
{  int i;
   SqList L;
  printf("QuickSort.cpp \n");
  printf("============= \n");
  printf("Please input the length of SqList (eg,5): ");
   scanf("%d" ,&L.length);
   for(i=1;i<=L.length;++i)
   {   printf("Please input the %d th element of SqList (eg,58):",i );
       scanf("%d",&L.r[i]);
   }
   printf("\n");
   printf("The disordered : ");         /*排序之前的线性表*/
   for(i=1;i<=L.length;i++)
       printf("%d  ", L.r[i]);
   QuickSort(&L);                /*调用 QuickSort()*/
   printf("\n");
   printf("The sorted     : ");        /*排序之后的线性表*/
   for(i=1;i<=L.length;i++)
     printf("%d  ",L.r[i]);
   getch();
}                               /*main() end */

