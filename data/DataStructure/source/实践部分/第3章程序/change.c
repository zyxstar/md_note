#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include"Stack.h"
#include"Queue.h"

//精确转换返回1，近似转换返回0，参数错误返回-1
int change(double d,unsigned r,Stack *ps,Queue* pq)
{
    #define N 6 
	int j;
	int i=fabs(d);
	int res;
	d=fabs(d)-i;
	if(r<=1) return -1;
	do
	{
		res=i%r;
		S_push(ps,res);
		i/=r;
	}while(i);
	for(j=0;d&&j<N;++j)
	{
		res=d*r;
		Q_push(pq,res);
		d=d*r-res;
	}
	if(d==0) 
		return 1;
	return 0;
}

void S_printAndPop(Stack* ps)
{
	int i;
	while(!S_isEmpty(*ps))
	{
		S_getTop(*ps,&i);
		printf("%d ",i);
        S_pop(ps);
	}
}

void Q_printAndPop(Queue* pq)
{
	int i;
	while(!Q_isEmpty(*pq))
	{
		Q_getFront(*pq,&i);
		printf("%d ",i);
        Q_pop(pq);
	}
}

int main()
{
	Stack s;
	Queue q;
	S_init(&s);
	Q_init(&q);

	if(change(200.53125,2,&s,&q)==1)
	{
		printf("%lf的绝对值精确转换为%d进制\n",200.53125,2);
		S_printAndPop(&s);
	    printf(".");
	    Q_printAndPop(&q);
	    printf("\n");
	}
	if(change(-200.53125,8,&s,&q)==1)
	{
		printf("%lf的绝对值精确转换为%d进制\n",-200.53125,8);
		S_printAndPop(&s);
	    printf(".");
	    Q_printAndPop(&q);
	    printf("\n");
	}
	if(change(-200.53125,3,&s,&q)==0)
	{
		printf("%lf的绝对值近似转换为%d进制\n",-200.53125,3);
		S_printAndPop(&s);
	    printf(".");
	    Q_printAndPop(&q);
	    printf("\n");
	}
	if(change(-200.53125,1,&s,&q)==-1)
		printf("参数错误\n");		

	S_clear(&s);
	Q_clear(&q);
	return 0;
}