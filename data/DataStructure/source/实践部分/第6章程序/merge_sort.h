void merge(DataType* arr1,int len1,DataType* arr2,int len2,DataType* arr,int (*comp)(DataType,DataType))
{
	int i1=0,i2=0,i=0;
	while(i1<len1||i2<len2)
		if(i2>=len2||i1<len1&&!comp(arr2[i2],arr1[i1]))
			arr[i++]=arr1[i1++];
		else
			arr[i++]=arr2[i2++];
}

void merge_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	DataType* tmp_arr;
	int int_len=(int)len;
	int n=2,i;
	if(len<=1) 
		return;
	tmp_arr=(DataType*)malloc(sizeof(DataType)*len);
	while(n<=int_len)
	{
		for(i=0;i<int_len;i+=n)
			merge(arr+i,int_len-i<n/2?int_len-i:n/2,arr+i+n/2,int_len-i-n/2<n/2?int_len-i-n/2:n/2,tmp_arr+i,comp);
		n*=2;
		for(i=0;i<int_len;i+=n)
			merge(tmp_arr+i,int_len-i<n/2?int_len-i:n/2,tmp_arr+i+n/2,int_len-i-n/2<n/2?int_len-i-n/2:n/2,arr+i,comp);
		n*=2;
	}	
	free(tmp_arr);
}
