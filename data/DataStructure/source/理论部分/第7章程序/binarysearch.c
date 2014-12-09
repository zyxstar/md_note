
/* ==================Program Description================== */
/* 程序名称：binarysearch.c                                       */
/* 程序目的：在r[n]数组中查找关键字等于kx的记录                */
/* Written By Mao Yifei                                  */
/* ========================================================*/
    #include "stdio.h"
    #define max 13
    int searchlist[max]={0,5,7,12,25,34,37,43,46,58,80,92,105};
    int binary_search(int r[],int n, int kx)
    /*在r[n]数组中查找关键字等于kx的记录，若找到返回该元素在数组中的下标，否则返回0*/
    {
      int mid,low,high;
      low=1;/*设置查找区间*/
      high=n-1;
      while(low<=high)/*查找表非空，进行折半查找*/
     {
       mid=(low+high)/2;    /*取中间位置*/
       if(kx<r[mid])     /*查找在表的左半区进行*/
       high=mid-1;
       else if(kx>r[mid])  /*查找在表的右半区进行*/
         low=mid+1;
       else return mid;
     }      /*查找成功，返回元素位置*/
     return 0;              /*查找失败，返回值－1*/
    }
    main()
    {
     int key;
     printf("please input the data you want to search:\n");
     scanf("%d",&key);/*输入要查找的关键字key*/
     if(binary_search(searchlist,max,key)!=0)/*如果返回值不等于0，即查找成功*/
      printf("the data is found ");
    else  /*查找失败*/
      printf("the data is not found");
    getch();
    }

