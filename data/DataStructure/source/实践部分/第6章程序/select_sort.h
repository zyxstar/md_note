unsigned select(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	unsigned i,min=0;
	for(i=1;i<len;++i)
		if(comp(arr[i],arr[min]))
			min=i;
	return min;
}

void select_sort(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	unsigned i;
	for(i=0;i<len;++i)
	{
		unsigned min=select(arr+i,len-i,comp)+i;
		swap_by_address(arr+i,arr+min);
	}
}

void make_heap(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	int i,cur;
	for(i=1;i<(int)len;++i)
	{
		cur=i;
		while(comp(arr[(cur-1)/2],arr[cur]))
		{
			swap_by_address(arr+cur,arr+(cur-1)/2);
			cur=(cur-1)/2;
		}
	}
}

void sort_heap(DataType* arr,unsigned len,int (*comp)(DataType,DataType))
{
	int i;
	int cur,left,right;
	for(i=(int)len-1;i>0;--i)
	{
		swap_by_address(arr,arr+i);
        cur=0;
		left=1;
		right=2;
		while(right<i&&comp(arr[cur],arr[right])||left<i&&comp(arr[cur],arr[left]))
		{
			if(right<i&&comp(arr[left],arr[right]))
			{
				swap_by_address(arr+cur,arr+right);
				cur=right;
			}
			else
			{
				swap_by_address(arr+cur,arr+left);
				cur=left;
			}
			left=2*cur+1;
		    right=2*cur+2;
		}
	}
}

