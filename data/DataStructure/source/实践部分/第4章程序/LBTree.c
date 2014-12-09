#include<stdio.h>
#include<malloc.h>
#include"LBTree.h"

int main()
{
	LBTree t;int i,j;
	LBT_init(&t);
	for(i=1;i<6;i++)
		for(j=1;j<10;j++)
	LBT_append(&t,i*10+j,i,j);
	LBT_floorPrint(t);
	printf("\n");
	LBT_prePrint(t);
	printf("\n");
	LBT_inPrint(t);
	printf("\n");
	LBT_postPrint(t);
	printf("\n");

	printf("%d\n",LBT_getDepth(t));
	printf("%d\n",LBT_getCountLeaf(t));
	printf("%d\n",LBT_getFloor(t,57));
	
	LBT_delete(&t,2,1);
	LBT_floorPrint(t);

	printf("\n");
	LBT_clear(&t);

	return 0;
}

