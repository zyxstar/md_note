//邻接链表实现的有向图
typedef char G_Datatype;
typedef struct
{
	unsigned size,count;
	G_Datatype* d_ptr;
	unsigned* in_ptr;
	DList * l_ptr;
}LGraph;

void LG_init(LGraph* p_lg, unsigned n)
{
    unsigned i;
	p_lg->count=0;
	p_lg->size=n>0?n:1;
	p_lg->d_ptr=(G_Datatype*)malloc(sizeof(G_Datatype)*p_lg->size);
	p_lg->in_ptr=(unsigned*)malloc(sizeof(unsigned)*p_lg->size);
	p_lg->l_ptr=(DList *)malloc(sizeof(DList )*p_lg->size);
	for(i=0;i<p_lg->size;i++)
		p_lg->in_ptr[i]=0;
	for(i=0;i<p_lg->size;i++)
		DLL_init(&p_lg->l_ptr[i]);		
}
void LG_clear(LGraph* p_lg)
{
	unsigned i;
	for(i=0;i<p_lg->size;i++)
		DLL_clear(&p_lg->l_ptr[i]);
	free(p_lg->l_ptr);
	free(p_lg->in_ptr);
	free(p_lg->d_ptr);
	p_lg->size=p_lg->count=0;
}
//插入结点成功返回1，插入重复值失败返回0
int LG_insertV(LGraph* p_lg,G_Datatype value)
{
	unsigned i;
	for(i=0;i<p_lg->count&&value!=p_lg->d_ptr[i];++i)
		;
	if(i<p_lg->count)
		return 0;
	if(p_lg->count==p_lg->size)
	{
		G_Datatype* new_d_ptr=(G_Datatype*)malloc(sizeof(G_Datatype)*p_lg->size*2);
		unsigned* new_in_ptr=(unsigned*)malloc(sizeof(unsigned)*p_lg->size*2);
		DList * new_l_ptr=(DList *)malloc(sizeof(DList)*p_lg->size*2);
		
		for(i=0;i<p_lg->count;++i)
			new_d_ptr[i]=p_lg->d_ptr[i];
		free(p_lg->d_ptr);
		p_lg->d_ptr=new_d_ptr;

		for(i=0;i<p_lg->count;++i)
			new_in_ptr[i]=p_lg->in_ptr[i];
		for(i=0;i<p_lg->count;++i)
		    new_in_ptr[i+p_lg->count]=0;
		free(p_lg->in_ptr);
		p_lg->in_ptr=new_in_ptr;
		

		for(i=0;i<p_lg->count;++i)
			new_l_ptr[i]=p_lg->l_ptr[i];
		for(i=0;i<p_lg->size;i++)
		    DLL_init(&new_l_ptr[i+p_lg->size]);
		free(p_lg->l_ptr);
		p_lg->l_ptr=new_l_ptr;
		p_lg->size*=2;
	}
	p_lg->d_ptr[p_lg->count++]=value;
	return 1;
}

//插入结点成功返回1，失败返回0
int LG_insertE(LGraph* p_lg,G_Datatype start,G_Datatype end)
{
	unsigned i,j;
	for(i=0;i<p_lg->count&&p_lg->d_ptr[i]!=start;i++);
	if(i==p_lg->count)
		return 0;
	for(j=0;j<p_lg->count&&p_lg->d_ptr[j]!=end;j++);
	if(j==p_lg->count)
		return 0;
	if(i==j)
		return 0;
	p_lg->in_ptr[j]++;
	DLL_insertAtTail(&p_lg->l_ptr[i],j);
	return 1;
}

#include"stack.h"

void LG_DFS_print(LGraph lg,G_Datatype start)
{
	unsigned i,j;
	int *set;
	Stack s;
	DLL_Node* lnode_ptr;
	for(i=0;i<lg.count&&lg.d_ptr[i]!=start;i++);
	if(i==lg.count)
		return ;
	set=(int*)malloc(sizeof(int)*lg.count);
	for(j=0;j<lg.count;j++)
		set[j]=0;
	S_init(&s);
   	set[i]=1;
	printf("%c ",lg.d_ptr[i]);
	lnode_ptr=lg.l_ptr[i].head;
	while(lnode_ptr||!S_isEmpty(s))
	{
		if(lnode_ptr&&!set[lnode_ptr->data])
		{				
			S_push(&s,lnode_ptr);
			i=lnode_ptr->data;
			set[i]=1;
		    printf("%c ",lg.d_ptr[i]);
		    lnode_ptr=lg.l_ptr[i].head;
			continue;
		}
		if(lnode_ptr)
            lnode_ptr=lnode_ptr->next;
		else if(S_getTop(s,&lnode_ptr))
		{
		    S_pop(&s);
		    lnode_ptr=lnode_ptr->next;
		}
	}
	S_clear(&s);
	free(set);
}
