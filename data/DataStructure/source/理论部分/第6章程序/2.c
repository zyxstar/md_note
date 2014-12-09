/* ==================Program Description================== */
/* 程序名称：Shellinert.cpp                                     */
/* 程序目的：希尔排序                */
/* Written By Cheng Zhuo                                  */
/* ========================================================*/  

# include <conio.h>
# define MAXSIZE 20
# define OK 1
# define ERROR 0
typedef int RedType;

typedef struct                        /*定义 SqList*/
{   RedType    r[MAXSIZE+1];
    int length;
}SqList;

void Shellinsert(SqList *L,int dk)     /*Shellinsert() 子函数*/
/*对顺序表L作一趟希尔插入排序.排序中,记录增量为dk,L.r[0]是暂存元素*/
{  int i,j;
   for(i=dk+1;i<=L->length;++i)
       if(L->r[i]<L->r[i-dk])       /*需要将L.r[i]插入到有序增量子表*/
       {    L->r[0]=L->r[i];            /*暂存在L.r[0]*/
            for(j=i-dk;j>0&&(L->r[0]<L->r[j]);j-=dk)
                L->r[j+dk]=L->r[j];    /*记录后移 */
            L->r[j+dk]=L->r[0];        /*插入到正确的位置*/
       }
}

void main()                            /*main() 函数*/
{  int i,dk=5;
   SqList L;/*={{0,49,38,65,97,76,13,27,49,55,4},10};*/
  printf("Shellinsert.cpp \n");
   printf("=============== \n");
   printf("Please input the length of SqList (eg, 5):  ");
   scanf("%d",  &L.length);                       /*输入线性表长*/
   printf("\n");
   for(i=1;i<=L.length;++i)
   {   printf("Please input the %d th integer (eg,58): ", i);
       scanf("%d", &L.r[i]);            /*输入线性表元素,其中L.r[0]是哨兵*/
   }
   printf("The disordered : ");
   for(i=1;i<=L.length;i++)
       printf("%d ",L.r[i]);
   Shellinsert(&L,dk);                   /*调用 Shellinsert()*/
   printf("\n");
   printf("The once ShellSorted sorted: ");
   for(i=1;i<=L.length;i++)
     printf("%d ",L.r[i]);
   printf("\n");
   getch();
} /*main() end */

