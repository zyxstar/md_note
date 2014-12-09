#include<stdio.h>
#include<malloc.h>

unsigned sel_min(int *arr_w,unsigned n)
{
	unsigned i,min;
	for(i=0;i<n;i++)
		if(arr_w[i]>=0)
			break;
	min=i++;
	for(;i<n;i++)
		if(arr_w[i]>=0&&arr_w[i]<arr_w[min])
			min=i;
	return min;
}

void get_huffmanCode(int * arr_w,unsigned * res,unsigned n)
{
	unsigned i,j,k;
	unsigned *set;
	if(n==0)
		return;
	set=(unsigned *)malloc(sizeof(unsigned)*n);
	for(i=0;i<n;++i)
		res[i]=1;
	for(i=0;i<n;++i)
	{
		set[i]=1;
		set[i]<<=i;
	}

	for(i=0;i<n-1;++i)
	{
		unsigned left,right;
		left=sel_min(arr_w, n);
		arr_w[left]=-arr_w[left];
		right=sel_min(arr_w, n);
		arr_w[right]=-arr_w[right];
		arr_w[left]=-arr_w[left]-arr_w[right];
		for(k=1,j=0;j<n;k<<=1,j++)
		{
			if(set[left]&k) res[j]<<=1;
			if(set[right]&k)
			{
				res[j]<<=1;
			    res[j]++;
			}
		}
		set[left]|=set[right];
		set[right]=0;	
	}
	free(set);
	
}

void print_code(char* arr_c,unsigned* res,unsigned n)
{
	unsigned i,j,k;
	for(i=0;i<n;i++)
	{
		printf("%c:",arr_c[i]);
		k=1<<31;
		while(!(res[i]&k))
			k>>=1;
		for(j=1;j!=k;j<<=1)
			if(res[i]&j) printf("1");
			else    printf("0");

		printf("\n");
	}
}
int main()
{
	char c[]={'a','b','c','d','e','f','g'};
	int w[]={13,15,25,10,27,1,9};
	unsigned r[7];
	get_huffmanCode(w,r,7);
	print_code(c,r,7);
}