#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include"SBTree.h"

int main()
{
	SBTree t;int i,j;
	SBT_init(&t);
	for(i=1;i<6;i++)
		for(j=1;j<10;j++)
	SBT_append(&t,i*10+j,i,j);
	SBT_floorPrint(t);
	printf("\n");
	SBT_prePrint(t);
	printf("\n");
	SBT_inPrint(t);
	printf("\n");
	SBT_postPrint(t);
	printf("\n");

	printf("%d\n",SBT_getDepth(t));
	printf("%d\n",SBT_getCountLeaf(t));
	printf("%d\n",SBT_getFloor(t,47));
	
	SBT_delete(&t,2,1);
	SBT_floorPrint(t);
	printf("\n");
	SBT_clear(&t);

	return 0;
}

