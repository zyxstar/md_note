#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"common.h"
#include"swap_sort.h"
#define N 10
int main()
{
	int arr1[N],arr2[50000],arr3[50000];
	set_arr(arr1,N);
	print_arr(arr1,N);
	bubble_sort(arr1,N,less);
    print_arr(arr1,N);
	bubble_sort(arr1,N,greater);
    print_arr(arr1,N);
	quick_sort(arr1,N,less);
	print_arr(arr1,N);
	set_arr(arr2,50000);
	set_arr(arr3,50000);
	set_start();
	bubble_sort(arr2,50000,less);
	printf("冒泡排序花费%d秒\n",end_time());
	set_start();
	quick_sort(arr3,50000,less);
	printf("快速排序花费%d秒\n",end_time());
}

