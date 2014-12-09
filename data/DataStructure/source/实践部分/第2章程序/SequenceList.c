#include<stdio.h>
#include<malloc.h>
#include"SequenceList.h"
void print(SeqList sl)
{
	size_t i;
	for(i=0;i<sl.len;++i)
		printf("%d ",sl.ptr[i]);
	printf("\n");
}
int main()
{
	SL_DataType i;
	SeqList sl;
	SL_init(&sl,5);
	printf("长度：%d\n",SL_getLen(sl));
	for(i=0;i<8;++i)
		SL_insert(&sl,i,0);
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	SL_insert(&sl,5,6);
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	SL_insert(&sl,5,1);
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	printf("%d\n",SL_delete(&sl,100));
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	printf("%d\n",SL_delete(&sl,5));
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	printf("%d\n",SL_remove(&sl,100));
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
	printf("%d\n",SL_remove(&sl,5));
	print(sl);
	printf("长度：%d\n",SL_getLen(sl));
    SL_clear(&sl);
	return 0;
}

