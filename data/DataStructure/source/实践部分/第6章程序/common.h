typedef int DataType;
void swap_by_address(DataType* p,DataType *q)
{
	DataType t=*p;
	*p=*q;
	*q=t;
}

void print_arr(DataType* arr,unsigned len)
{
	unsigned i;
	for(i=0;i<len;++i)
	{
		printf("%d ",arr[i]);
		if((i+1)%10==0)
			printf("\n");
	}
}

void set_arr(DataType* arr,unsigned len)
{
	unsigned i;
	for(i=0;i<len;++i)
		arr[i]=rand()%100;
}

 int less(DataType a,DataType b)
{
	return a<b;
}
 int greater(DataType a,DataType b)
{
	return a>b;
}

int start =0;
void set_start()
{
	start=time(0);
}
int end_time()
{
	return time(0)-start;
}