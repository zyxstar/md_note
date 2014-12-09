#include<stdio.h>
#include<malloc.h>
#include"DLinkList.h"
void print(DList dl)
{
	DLL_Node* cur_ptr=dl.head;
	while(cur_ptr)
	{
		printf("%d ",cur_ptr->data);
		cur_ptr=cur_ptr->next;
	}
	printf("\n");
}
int main()
{
	DLL_DataType i;
	DList dl;
	DLL_init(&dl);

	for(i=0;i<3;++i)
	   DLL_insertAtHead(&dl,i);
	print(dl);

	for(i=0;i<3;++i)
	   DLL_insertAtTail(&dl,i);
	print(dl);

	DLL_deleteFromHead(&dl,&i);
	printf("删除头部的%d：",i);
	print(dl);

	DLL_deleteFromTail(&dl,&i);
	printf("删除尾部的%d：",i);
	print(dl);

	DLL_clear(&dl);
	return 0;
}