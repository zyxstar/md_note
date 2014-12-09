//打印[2,n)内的所有质数
#include<stdio.h>
#include<malloc.h>
#include<math.h>

void print_prime(unsigned n)
{
	unsigned * arr,i,j;
	if(n<=2)
		return;
	arr=(unsigned *)malloc(sizeof(unsigned )*n);
	for(i=2;i<n;i++)
		arr[i]=1;
	for(i=2;i<sqrt(n);i++)
		if(arr[i])
		    for(j=2*i;j<n;j+=i)
			    arr[j]=0;
	for(i=2,j=0;i<n;i++)
		if(arr[i])
		{
			printf("%d ",i);
			++j;
			if(j%10==0) 
				printf("\n");
		}
	printf("\n");
	free(arr);
}
//同样方法，使用压缩存储，练习位运算

void print_prime1(unsigned n)
{
	unsigned i,j,len;
	char* arr;
	if(n<=2)
		return;
    len=ceil(n/8.0);
	arr=(char*)malloc(len);
	for(i=0;i<len;i++)
		arr[i]=255;
	for(i=2;i<sqrt(n);i++)
		if(arr[i/8]&(1<<(i%8)))
		    for(j=2*i;j<n;j+=i)
			    arr[j/8]&=~(1<<(j%8));
	for(i=2,j=0;i<n;i++)
		if(arr[i/8]&(1<<(i%8)))
		{
			printf("%d ",i);
			++j;
			if(j%10==0) 
				printf("\n");
		}
	printf("\n");
	free(arr);
}

int main()
{
	print_prime(265);
	printf("----------------------------------------------\n");
	print_prime1(265);
	return 0;
}


