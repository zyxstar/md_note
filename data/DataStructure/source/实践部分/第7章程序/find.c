#include<stdio.h>
typedef int DataType;
int find(DataType *arr,int len,DataType value)
{
	DataType tmp;
	int i;
	if(len==0)
		return -1;
	tmp=arr[len-1];
	arr[len-1]=value;
	for(i=0;arr[i]!=value;i++);
	arr[len-1]=tmp;
	if(i<len-1)
		return i;
	return tmp==value?len-1:-1;
}

int main()
{
	DataType arr[10]={5,1,2,3,4,6,7,8,0,9};

    printf("%d\n",find(arr,10,7));
	printf("%d\n",find(arr,10,5));
	printf("%d\n",find(arr,10,9));
	printf("%d\n",find(arr,10,10));
	return 0;
}