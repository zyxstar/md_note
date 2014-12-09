//mGraph.h 邻接矩阵实现的无向图
typedef char G_Datatype;
typedef struct
{
	unsigned size,count;
	G_Datatype* d_ptr;
	int** m_ptr;
}MGraph;

void MG_init(MGraph* p_mg, unsigned n)
{
    unsigned i,j;
	p_mg->count=0;
	p_mg->size=n>0?n:1;
	p_mg->d_ptr=(G_Datatype*)malloc(sizeof(G_Datatype)*p_mg->size);
	p_mg->m_ptr=(int**)malloc(sizeof(int*)*p_mg->size);
	for(i=0;i<p_mg->size;i++)
		p_mg->m_ptr[i]=(int*)malloc(sizeof(int)*p_mg->size);
	for(i=0;i<p_mg->size;i++)
		for(j=0;j<p_mg->size;j++)
		      p_mg->m_ptr[i][j]=0;
}
void MG_clear(MGraph* p_mg)
{
	unsigned i;
	for(i=0;i<p_mg->size;i++)
		free(p_mg->m_ptr[i]);
	free(p_mg->m_ptr);
	free(p_mg->d_ptr);
	p_mg->size=p_mg->count=0;
}
//插入结点成功返回1，插入重复值失败返回0
int MG_insertV(MGraph* p_mg,G_Datatype value)
{
	unsigned i,j;
	for(i=0;i<p_mg->count&&value!=p_mg->d_ptr[i];++i)
		;
	if(i<p_mg->count)
		return 0;
	if(p_mg->count==p_mg->size)
	{
		G_Datatype* new_d_ptr=(G_Datatype*)malloc(sizeof(G_Datatype)*p_mg->size*2);
		int** new_m_ptr=(int**)malloc(sizeof(int*)*p_mg->size*2);
		for(i=0;i<p_mg->size*2;i++)
		    new_m_ptr[i]=(int*)malloc(sizeof(int)*p_mg->size*2);
		for(i=0;i<p_mg->count;++i)
			new_d_ptr[i]=p_mg->d_ptr[i];
		free(p_mg->d_ptr);
		p_mg->d_ptr=new_d_ptr;
		for(i=0;i<p_mg->size;i++)
		    for(j=0;j<p_mg->size;j++)
		      new_m_ptr[i][j]=p_mg->m_ptr[i][j];
		for(i=0;i<p_mg->size;i++)
		    free(p_mg->m_ptr[i]);
		free(p_mg->m_ptr);
		p_mg->m_ptr=new_m_ptr;
		p_mg->size*=2;
	}
	p_mg->d_ptr[p_mg->count++]=value;
	return 1;
}

//插入结点成功返回1，失败返回0
int MG_insertE(MGraph* p_mg,G_Datatype v1,G_Datatype v2)
{
	unsigned i,j;
	for(i=0;i<p_mg->count&&p_mg->d_ptr[i]!=v1;i++);
	if(i==p_mg->count)
		return 0;
	for(j=0;j<p_mg->count&&p_mg->d_ptr[j]!=v2;j++);
	if(j==p_mg->count)
		return 0;
	if(i==j)
		return 0;
	p_mg->m_ptr[i][j]=p_mg->m_ptr[j][i]=1;
	return 1;
}

void MG_BFS_print(MGraph mg,G_Datatype start)
{
	unsigned i,j;
	int *set;

	Queue q;
	for(i=0;i<mg.count&&mg.d_ptr[i]!=start;i++);
	if(i==mg.count)
		return ;
	set=(int *)malloc(sizeof(int)*mg.count);
	for(j=0;j<mg.count;j++)
		set[j]=0;
	Q_init(&q);
	set[i]=1;
	Q_push(&q,i);
	while(!Q_isEmpty(q))
	{
		Q_getFront(q,&i);
		Q_pop(&q);
		printf("%c ",mg.d_ptr[i]);
		for(j=0;j<mg.count;j++)
			if(mg.m_ptr[i][j]&&!set[j])
			{
				
				Q_push(&q,j);
				set[j]=1;
			}
	}
	Q_clear(&q);
    free(set);
}
