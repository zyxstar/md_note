/* ==================Program Description================== */
/* 程序名称：blocksearch.c                                       */
/* 程序目的：分块索引表slist长为n，查找表为r[]，查找关键字等于kx的数据元素*/
/* Written By Mao Yifei                                  */
/* ========================================================*/
    #include "stdio.h"
    #define max 14
    #define m 3
    int searchlist[max]={14,31,8,22,18,43,62,49,35,52,88,78,71,83};
    int slist[m][2]={31,0,62,5,88,10};
    int block_search(int r[],int slist[][2],int kx,int n)
    /*分块索引表slist长为n，查找表为r[]，查找关键字等于kx的数据元素，数据元素均从下标为0处存储*/
    {
      int i,flag;
    int lowlim,highlim;/*定义分块查找的上下区间*/
    i=0;
    while((slist[i][0]<kx)&&(i<n))
    i++;
    if(slist[i][0]<kx)/*最后一个分块最大的值比要查找的kx还小*/
    return 0;
    if(i==2)/*在最后一个分块中查找关键字kx*/
    {
      lowlim=slist[2][1];
      highlim=13;
    }
    else
    {
      lowlim=slist[i][1];
      highlim=slist[++i][1]-1;
    }
    i=lowlim;
    while(i<=highlim)/*在分块中查找数据元素*/
    {
      if(r[i]==kx) /*找到了*/
        return i;
      else
        i++;
    }
    /*未找到了，返回值0*/
    return 0;
    }
    main()
    {
      int key;
      printf("please input the data you want to search:\n");
      scanf("%d",&key);/*输入要查找的关键字key*/
      if(block_search(searchlist,slist,key,m)!=0)/*如果返回值不等于0，即查找成功*/
        printf("the data is found at %d",block_search(searchlist,slist,key,m));
      else  /*查找失败*/
        printf("the data is not found");
      getch();
    }

