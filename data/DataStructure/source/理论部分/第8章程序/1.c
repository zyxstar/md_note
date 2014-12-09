/* ==================Program Description================== */
/* 程序名称：array_compress.c                              */
/* 程序目的：三元组表法压缩稀疏数组                       */
/* Written By Yang yonghu                                 */
/* ========================================================*/  
#include "stdio.h"
#define Max 10
typedef struct{
 int i,j;
 int v;
}node;
typedef struct{
int mu,nu,tu;
node data[Max];
}mat;

void main()
 {
   int Data[5][6]={0,0,2,0,5,0,
		   1,0,0,0,0,0,
		   0,8,0,0,0,0,
		   0,0,0,0,0,0,
		   0,0,0,0,3,0,
		  };
   mat CompressData;
   int Index;
   int i,j;

   Index=0;

   printf("two dimensional sparse array:\n");
   for(i=0;i<5;i++)
    { 
      for(j=0;j<6;j++)
         printf("%3d",Data[i][j]);
      printf("\n");
    }
  
   for(i=0;i<5;i++)
      for(j=0;j<6;j++)
         if(Data[i][j]!=0)
            {
  	 	
		CompressData.data[Index].i=i;
 		CompressData.data[Index].j=j;
		CompressData.data[Index].v=Data[i][j];
		Index++;
 	     }

   CompressData.mu=5;
   CompressData.nu=6;
   CompressData.tu=Index;

   printf("two dimensionsal compress array : \n");
   printf("%3d,%3d,%3d",CompressData.mu,CompressData.nu,CompressData.tu);
   printf("\n");
   for(i=0;i<Index;i++)
     {
       printf("%3d,%3d,%3d",CompressData.data[i].i,CompressData.data[i].j,CompressData.data[i].v);
       printf("\n");
     }

}


