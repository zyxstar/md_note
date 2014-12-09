#include<stdio.h>
#include<malloc.h>
#include"DLinkList.h"
#include"LGraph.h"
#include"Queue.h"

void LG_topPrint(LGraph lg)
{
	unsigned *indegree;
	unsigned i,count=0;
	DLL_Node* node_ptr;
	Queue q;
	if(lg.count==0)
		return ;
	indegree=(unsigned *)malloc(sizeof(unsigned)*lg.count);
	for(i=0;i<lg.count;i++)
		indegree[i]=lg.in_ptr[i];

	Q_init(&q);
	for(i=0;i<lg.count;i++)
		if(indegree[i]==0)
			Q_push(&q,i);
	while(!Q_isEmpty(q))
	{
		Q_getFront(q,&i);
		Q_pop(&q);
		printf("%c ",lg.d_ptr[i]);
		++count;
		for(node_ptr=lg.l_ptr[i].head;node_ptr;node_ptr=node_ptr->next)
		{
			i=node_ptr->data;
			if(--indegree[i]==0)
				Q_push(&q,i);
		}
	}
	if(count<lg.count)
		printf("图中有回路，排序失败\n");
	else
		printf("\n");
	Q_clear(&q);
	free(indegree);
	
}


int main()
{
	int i;
	LGraph lg;
	LG_init(&lg,10);
	for(i=0;i<14;i++)
		LG_insertV(&lg,'A'+i);
	LG_insertE(&lg,'A','I');
	LG_insertE(&lg,'B','C');
	LG_insertE(&lg,'B','D');
	LG_insertE(&lg,'C','D');
	LG_insertE(&lg,'C','H');
	LG_insertE(&lg,'D','E');
	LG_insertE(&lg,'D','F');
	LG_insertE(&lg,'E','L');
	LG_insertE(&lg,'G','F');
	LG_insertE(&lg,'H','J');
	LG_insertE(&lg,'H','K');
	LG_insertE(&lg,'H','M');
	LG_insertE(&lg,'H','N');
    LG_insertE(&lg,'J','K');
	LG_insertE(&lg,'J','N');
	LG_insertE(&lg,'K','N');
	LG_insertE(&lg,'L','N');
	LG_insertE(&lg,'M','N');
	LG_topPrint(lg);
	LG_clear(&lg);
	return 0;
}

