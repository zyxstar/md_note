#include<stdio.h>
#include<malloc.h>
#include"DLinkList.h"
#include"LGraph.h"
int main()
{
	int i;
	LGraph lg;
	LG_init(&lg,8);
	for(i=0;i<5;i++)
		LG_insertV(&lg,'A'+i);
	LG_insertE(&lg,'A','B');
	LG_insertE(&lg,'A','C');
	LG_insertE(&lg,'A','D');
	LG_insertE(&lg,'A','E');
	LG_insertE(&lg,'C','E');
	LG_insertE(&lg,'D','A');
	LG_insertE(&lg,'D','B');
	LG_DFS_print(lg,'A');
	printf("\n");
	LG_DFS_print(lg,'D');
    printf("\n");
	LG_clear(&lg);
	return 0;
}