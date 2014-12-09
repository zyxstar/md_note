void insert_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	int i,j,k;
	DataType tmp;
	for(i=1;i<(int)len;++i)
	{
		for(j=i-1;j>=0&&comp(arr[i],arr[j]);--j);
		tmp=arr[i];
		for(k=i;k>j+1;--k)
			arr[k]=arr[k-1];
		arr[j+1]=tmp;
	}
}

void insert_sort1(DataType* arr,unsigned len,int (*comp)(DataType,DataType),unsigned span)
{
	int i,j,k;
	DataType tmp;
	for(i=span;i<(int)len;i+=span)
	{
		for(j=i-span;j>=0&&comp(arr[i],arr[j]);j-=span);
		tmp=arr[i];
		for(k=i;k>j+(int)span;k-=(int)span)
			arr[k]=arr[k-span];
		arr[j+span]=tmp;
	}
}

void shell_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
    #define D 2
	int span=len/D;
	while(span)
	{
		int i;
		for(i=0;i<span;++i)
			insert_sort1(arr+i,len-i,comp,span);
		span/=2;
	}
}