#define R 10
void radix_sort(unsigned * arr,unsigned len)
{	 
	unsigned *arr2d[R];
	unsigned n=1,i,j;
	unsigned index;
	unsigned indexs[R];

	for( i=0;i<R;++i)
		arr2d[i]=(unsigned *)malloc(sizeof(unsigned)*len);
	while(1)
	{
		index=0;
		for( i=0;i<R;++i)
			indexs[i]=0;
		for(i=0;i<len;++i)
			arr2d[arr[i]/n%R][indexs[arr[i]/n%R]++]=arr[i];
		for(i=0;i<R;++i)
			for( j=0;j<indexs[i];++j)
				arr[index++]=arr2d[i][j];
		n*=R;
		for(i=0;arr[i]<n&&i<len;++i);
		if(i==len) break;
	}
	for(i=0;i<R;++i)
		free(arr2d[i]);
}

