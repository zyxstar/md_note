#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<malloc.h>
#include"common.h"
#include"swap_sort.h"
#include"merge_sort.h"

int my_less(DataType a, DataType b)
{
	return a%10<b%10;
}

#define N 10
int main()
{
	DataType arr1[N],arr2[50000];
	DataType arr3[N]={327,47,768,98,8,56,5636,5,435,533};
	DataType arr4[N]={327,47,768,98,8,56,5636,5,435,533};
	set_arr(arr1,N);
	print_arr(arr1,N);
	merge_sort(arr1,N,less);
    print_arr(arr1,N);
	merge_sort(arr1,N,greater);
    print_arr(arr1,N);
	set_arr(arr2,50000);
	set_start();
	merge_sort(arr2,50000,less);
	printf("归并排序花费%d秒\n",end_time());	
	print_arr(arr3,N);
	printf("快速排序结果：");
	quick_sort(arr3,N,my_less);
	print_arr(arr3,N);
	printf("归并排序结果：");
	merge_sort(arr4,N,my_less);
	print_arr(arr4,N);
}

