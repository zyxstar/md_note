/* ==================Program Description================== */
/* 程序名称：InsertSort.cpp                                      */
/* 程序目的：直接插入排序                 */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/  
/* # include <iostream.h> */ 
# include <conio.h>
# define MAXSIZE 20
# define MAX_LENGTH 100
typedef int RedType;

typedef struct                             /*定义 SqList*/
{   RedType    r[MAXSIZE+1];
    int length;
}SqList;

void InsertSort(SqList *L)                /*InsertSort() 子函数*/
{  int i,j;
   for(i=2;i<=L->length;++i)
       if(L->r[i]<L->r[i-1])
       {    L->r[0]=L->r[i];              /*复制为哨兵*/
            for(j=i-1;L->r[0]<L->r[j];--j)
                L->r[j+1]=L->r[j];        /*记录后移*/
            L->r[j+1]=L->r[0];            /*插入到正确的位置*/
       }                                   
}                                         /*InsertSort() 结束*/

void main()                               /*main() 函数*/
{  int i;
   SqList L;
 printf("InsertSort.cpp \n");
   printf("============== \n");
   printf("Please input the length of SqList (eg, 5): ");
  scanf("%d",&L.length);                 /*输入线性表长*/

   for(i=1;i<=L.length;++i)
   {  printf("Please input the %d th integer (eg,58): ",i);
     scanf("%d",&L.r[i]);             /*输入线性表元素,其中L.r[0]是哨兵*/
   }
   printf("The disordered : ");        /*输出未排序之前的线性表*/
   for(i=1;i<=L.length;i++)
       printf("%d  ", L.r[i]);
   InsertSort(&L);                      /*调用 InsertSort()   */
   printf("\n");
   printf("The ordered    : ");        /*输出排序之后的线性表*/
   for(i=1;i<=L.length;i++)
    printf("%d  ",L.r[i]);
   getch();
}                                         /*main() end*/

