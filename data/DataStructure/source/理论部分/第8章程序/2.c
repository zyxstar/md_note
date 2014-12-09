/* ==================Program Description================== */
/* 程序名称：triangle_compress.c                              */
/* 程序目的：三角矩阵压缩                               */
/* Written By Yang yonghu                                 */
/* ========================================================*/  
#include "stdio.h"
#define Max 30
#define n 6
void main()
 {
   int a[n][n]={1,2, 3, 4, 5, 6,
		        0,7, 8, 9,10,11,
		        0,0,12,13,14,15,
		        0,0, 0,16,17,18,
		        0,0, 0, 0,19,20,
                0,0, 0, 0, 0,21};

   int Data[Max];
   int Index;
   int i,j;

   Index=0;

   printf("two dimensional sparse array:\n");
   for(i=0;i<n;i++)

    { 
      for(j=0;j<6;j++)
         printf("%3d",a[i][j]);
      printf("\n");
    }
  
   for(i=0;i<n;i++)
      for(j=0;j<n;j++)
         if(i<=j)
            {
  	 	
		Index=(n+(n-i+1))*i/2+(j-i);
                Data[Index]=a[i][j];
 	     }

   printf("\n");
   printf("two dimensionsal compress array : \n");
    
   for(i=0;i<=Index;i++)
     {
       printf("%3d,",Data[i]);
     }

}


