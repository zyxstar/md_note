#include <stdio.h>
#include<malloc.h>
#include"Stack.h"

int main()
{
	int i;
	Stack s;
	S_init(&s);
	for(i=0;i<10;i++)
		S_push(&s,i);
	while(!S_isEmpty(s))
	{
		S_getTop(s,&i);
		printf("%d",i);
		S_pop(&s);
	}
	printf("\n");
	S_clear(&s);
}


