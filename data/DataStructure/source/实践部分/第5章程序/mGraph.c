#include<stdio.h>
#include<malloc.h>
#include"queue.h"
#include"mGraph.h"
int main()
{
	int i;
	MGraph mg;
	MG_init(&mg,10);
	for(i=0;i<6;i++)
		MG_insertV(&mg,'A'+i);
	MG_insertE(&mg,'A','B');
	MG_insertE(&mg,'A','C');
	MG_insertE(&mg,'A','D');
	MG_insertE(&mg,'B','D');
	MG_insertE(&mg,'C','E');
	MG_insertE(&mg,'C','F');
	MG_insertE(&mg,'E','F');
	MG_BFS_print(mg,'A');
	printf("\n");
	MG_BFS_print(mg,'F');
	printf("\n");
	MG_clear(&mg);
	return 0;
}