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
