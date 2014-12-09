#include<stdio.h>
#include<malloc.h>
#include"Stack.h"

void print(int** arr,int n)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
			printf("%d ",arr[i][j]);
		printf("\n");
	}
}

int test(int**arr,int n,int i,int j)
{
	int k;
	for(k=0;k<i;k++)
		if(arr[k][j]==1||k-i+j>=0&&arr[k][k-i+j]==1||i+j-k<n&&arr[k][i+j-k]==1)
			return 0;
	return 1;
}

void Queen(int n)
{
	int** arr;
	int i,j;
	int count=0;
	Stack s;
	if(n<4) return;
	S_init(&s);
	arr=(int**)malloc(sizeof(int*)*n);
	for(i=0;i<n;++i)
		arr[i]=(int*)malloc(sizeof(int)*n);
	for(i=0;i<n;++i)
		for(j=0;j<n;++j)
			arr[i][j]=0;
	i=j=0;
	while(1)
	{
		for(;j<n;++j)
			if(test(arr,n,i,j))
			{
				arr[i][j]=1;
			    S_push(&s,j);
				++i;
			    break;
			}
		if(j==n||i==n)
		{
			if(i==n)
			{
				printf("µÚ%d½â£º\n",++count);
				print(arr,n);
			}
			--i;
			if(i<0) break;
			S_getTop(s,&j);
			S_pop(&s);
			arr[i][j]=0;
			++j;
		}
		else
			j=0;
	}	
	S_clear(&s);
	for(i=0;i<n;++i)
		free(arr[i]);
	free(arr);
}
int main()
{
	Queen(4);
	//Queen(8);
	return 0;
}