typedef LBT_Node* Q_DataType;
typedef struct
{
	unsigned max;
	unsigned len;
	unsigned front,rear;
	Q_DataType *ptr;
}Queue;

void Q_init(Queue* pq)
{
	pq->max=8;
	pq->len=pq->front=pq->rear=0;
	pq->ptr=(Q_DataType*)malloc(sizeof(Q_DataType)*pq->max);
}

void Q_clear(Queue* pq)
{
	pq->max=pq->len=pq->front=pq->rear=0;
	if(pq->ptr) 
		free(pq->ptr);
	pq->ptr=0;
}

//²âÊÔÊÇ·ñ¿Õ
int Q_isEmpty(Queue q)
{
	return q.len==0;
}

void Q_push(Queue* pq,Q_DataType value)
{
	unsigned i;
	if(pq->len==pq->max)
	{
		Q_DataType* new_ptr=(Q_DataType*)malloc(sizeof(Q_DataType)*pq->max*2);
		for(i=0;i<pq->max;++i)
			new_ptr[i]=pq->ptr[(pq->front+i)%pq->max];
		pq->max*=2;
		free(pq->ptr);
		pq->ptr=new_ptr;
		pq->front=0;
		pq->rear=pq->len;
	}
	pq->ptr[pq->rear++]=value;
	pq->len++;
	if(pq->rear==pq->max)pq->rear=0;
}

int Q_pop(Queue* pq)
{
	if(pq->len==0) 
		return 0;
	pq->front++;
	if(pq->front==pq->max) pq->front=0;
	--pq->len;
	return 1;
}

int Q_getFront(Queue q,Q_DataType* p_value)
{
	if(q.len==0) 
		return 0;
	*p_value=q.ptr[q.front];
	return 1;
}
