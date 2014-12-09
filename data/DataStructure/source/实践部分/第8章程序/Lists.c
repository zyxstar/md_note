#include<stdio.h>
#include<malloc.h>
#include"lists.h"

int main()
{
	Lists l;
	L_init(&l);
	L_create(&l,"a,((b,c),(d,e),f),(g,h),i");
	L_print(&l);
	printf("\n");
	L_clear(&l);
	return 0;
}



