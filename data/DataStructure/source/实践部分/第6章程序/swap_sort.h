void bubble_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	unsigned i,j;
	int flag;
        if(len<=1) 
		return ;
	for(i=0;i<len-1;++i)
	{
		flag=1;
		for(j=0;j<len-1-i;++j)
			if(comp(arr[j+1],arr[j]))
			{
				swap_by_address(arr+j+1,arr+j);
				flag=0;
			}
		if(flag)
			break;
	}
}

void quick_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	unsigned low=0;
	unsigned high=len;
	if(len<=1) 
		return ;
	while(1)
	{
		while(++low<len&&!comp(arr[0],arr[low]));
		while(--high>0&&!comp(arr[high],arr[0]));
		if(low<high)
			swap_by_address(arr+low,arr+high);
		else
			break;
	}
	swap_by_address(arr,arr+high);
	quick_sort(arr,high,comp);
	quick_sort(arr+high+1,len-high-1,comp);
}