#include<stdio.h>
#include<malloc.h>
#include"OrderList.h"

void print(OList ol)
{
	OL_Node* cur_ptr=ol.head;
	while(cur_ptr)
	{
		printf("%d ",cur_ptr->data);
		cur_ptr=cur_ptr->next;
	}
	printf("\n");
}

int less1(OL_DataType a,OL_DataType b)
{
	return a<b;
}
int less2(OL_DataType a,OL_DataType b)
{
	return a%10<b%10;
}
int greater(OL_DataType a,OL_DataType b)
{
	return a>b;
}

int main()
{
	OList ol1,ol2,ol3;
	OL_init(&ol1,less1);
	OL_init(&ol2,greater);
	OL_init(&ol3,less2);

	OL_insert(&ol1,2);
	print(ol1);
	OL_insert(&ol1,1);
	print(ol1);
	OL_insert(&ol1,4);
	print(ol1);
	OL_insert(&ol1,3);
	print(ol1);
	if(!OL_insert(&ol1,3))
		printf("存在重复值\n");
	OL_remove(&ol1,4);
	print(ol1);
	if(!OL_remove(&ol1,4))
		printf("不存在要删除的值\n");	
	OL_remove(&ol1,1);
	print(ol1);
	OL_remove(&ol1,3);
	print(ol1);
	OL_remove(&ol1,2);
	print(ol1);
	OL_insert(&ol2,12);
	OL_insert(&ol2,13);
	OL_insert(&ol2,11);
	print(ol2);
	OL_insert(&ol3,32);
	OL_insert(&ol3,13);
	OL_insert(&ol3,11);
	OL_insert(&ol3,22);
	print(ol3);
	OL_clear(&ol1);
	OL_clear(&ol2);
	OL_clear(&ol3);
	return 0;
}