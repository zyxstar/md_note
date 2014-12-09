#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"common.h"
#include"insert_sort.h"
#define N 10
int main()
{
	int arr1[N],arr2[50000],arr3[50000];
	set_arr(arr1,N);
	print_arr(arr1,N);
	insert_sort(arr1,N,less);
    print_arr(arr1,N);
	insert_sort(arr1,N,greater);
    print_arr(arr1,N);
    shell_sort(arr1,N,less);
	print_arr(arr1,N);
	set_arr(arr2,50000);
	set_arr(arr3,50000);
	set_start();
	insert_sort(arr2,50000,less);
	printf("直接插入排序花费%d秒\n",end_time());
	set_start();
	shell_sort(arr3,50000,less);
	printf("shell排序花费%d秒\n",end_time());
}

