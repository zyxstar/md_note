/* ==================Program Description================== */
/* 程序名称：search.c                                       */
/* 程序目的：在r[n]数组中查找关键字等于kx的记录                */
/* Written By Mao Yifei                                  */
/* ========================================================*/

    #include "stdio.h"
    #define max 7
    int searchlist[max]={0,13,25,16,23,57,66};
    int s_search(int r[],int n,int kx)
    /*在r[n]数组中查找关键字等于kx的记录，若找到返回该元素在数组中的下标，否则返回0*/
        {
           int i;
           r[0]=kx;/*在下标为0处设置监视哨*/
           i=n-1;
      while(r[i]!=kx)
        i--;
      return i;/*返回查找的位置*/
    }
    main()
    {
      int key;
      printf("please input the data you want to search:\n");
      scanf("%d",&key);/*输入要查找的关键字key*/
      if(s_search( searchlist,max, key)!=0)/*如果返回值不等于0，即查找成功*/
         printf("the data is found ");
      else  /*查找失败*/
         printf("the data is not found");
    }

