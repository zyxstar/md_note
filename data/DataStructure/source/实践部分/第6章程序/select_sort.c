#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"common.h"
#include"select_sort.h"
#define N 10
int main()
{
	int arr1[N],arr2[50000],arr3[50000];
	set_arr(arr1,N);
	print_arr(arr1,N);
	select_sort(arr1,N,less);
    print_arr(arr1,N);
    set_arr(arr1,N);
	print_arr(arr1,N);
	make_heap(arr1,N,less);
	print_arr(arr1,N);
	sort_heap(arr1,N,less);
	print_arr(arr1,N);
	set_arr(arr1,N);
	print_arr(arr1,N);
	make_heap(arr1,N,greater);
	print_arr(arr1,N);
	sort_heap(arr1,N,greater);
	print_arr(arr1,N);
	set_arr(arr2,50000);
	set_arr(arr3,50000);
	set_start();
	select_sort(arr2,50000,less);
	printf("简单选择插入排序花费%d秒\n",end_time());
	set_start();
	make_heap(arr3,50000,less);
	sort_heap(arr3,50000,less);
	printf("堆排序花费%d秒\n",end_time());
}

