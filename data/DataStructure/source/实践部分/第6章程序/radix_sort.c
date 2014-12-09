#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<malloc.h>
#include"common.h"
#include"radix_sort.h"
#define N 10
int main()
{
	int arr1[N],arr2[50000];
	set_arr(arr1,N);
	print_arr(arr1,N);
	radix_sort(arr1,N);
    print_arr(arr1,N);
    set_arr(arr2,50000);
	set_start();
	radix_sort(arr2,50000);
	printf("πÈ≤¢≈≈–Úª®∑—%d√Î\n",end_time());
}

