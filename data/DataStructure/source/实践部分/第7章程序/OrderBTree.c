#include<stdio.h>
#include<malloc.h>
#include"OrderBTree.h"

int less(OBT_DataType a,OBT_DataType b)
{
	return a<b;
}

int greater(OBT_DataType a,OBT_DataType b)
{
	return a>b;
}

int main()
{
	OBT_DataType arr[16]={8,4,12,2,3,1,6,7,5,11,10,9,13,15,16,14};
	int i;
	OTree t1,t2;
	OBT_init(&t1,less);
	OBT_init(&t2,greater);

	for(i=0;i<16;i++)
	{
		OBT_insert(&t1,arr[i]);
		OBT_insert(&t2,arr[i]);
	}
	OBT_inPrint(t1);
	OBT_inPrint(t2);
	printf("%d,%d\n",OBT_find(t1,4),OBT_find(t2,4));
	printf("%d,%d\n",OBT_remove(&t1,4),OBT_remove(&t2,4));
	printf("%d,%d\n",OBT_find(t1,4),OBT_find(t2,4));
   	OBT_inPrint(t1);
	OBT_inPrint(t2);

	OBT_clear(&t1);
	OBT_clear(&t2);
}