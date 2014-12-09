#include <stdio.h>
#include<malloc.h>
#include"Queue.h"

int main()
{
	int i;
	Queue q;
	Q_init(&q);
	for(i=0;i<10;i++)
		Q_push(&q,i);
	while(!Q_isEmpty(q))
	{
		Q_getFront(q,&i);
		printf("%d",i);
		Q_pop(&q);
	}
	
	printf("\n");
	Q_clear(&q);
}
