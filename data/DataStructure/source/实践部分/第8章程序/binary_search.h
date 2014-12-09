DataType *binary_search(DataType *arr,int len,DataType value,int(* comp)(DataType,DataType))
{
	int low=0;
	int high=len-1;
	int mid;
	while(low<=high)
	{
		mid=(low+high)/2;
		if(comp(value,arr[mid]))
			high=mid-1;
		else if(comp(arr[mid],value))
			low=mid+1;
		else
			return arr+mid;
	}
	return 0;
}