typedef int S_DataType;
typedef struct
{
	unsigned max;
	unsigned top;
	S_DataType *ptr;
}Stack;

void S_init(Stack* ps)
{
	ps->max=8;
	ps->top=0;
	ps->ptr=(S_DataType*)malloc(sizeof(S_DataType)*ps->max);
}

void S_clear(Stack* ps)
{
	ps->max=ps->top=0;
	if(ps->ptr) 
		free(ps->ptr);
	ps->ptr=0;
}

//²âÊÔÊÇ·ñ¿Õ
int S_isEmpty(Stack s)
{
	return s.top==0;
}

void S_push(Stack* ps,S_DataType value)
{
	unsigned i;
	if(ps->top==ps->max)
	{
		S_DataType* new_ptr=(S_DataType*)malloc(sizeof(S_DataType)*ps->max*2);
		for(i=0;i<ps->top;++i)
			new_ptr[i]=ps->ptr[i];
		ps->max*=2;
		free(ps->ptr);
		ps->ptr=new_ptr;
	}
	ps->ptr[ps->top++]=value;
}

int S_pop(Stack* ps)
{
	if(ps->top==0) 
		return 0;
	--ps->top;
	return 1;
}

int S_getTop(Stack s,S_DataType* p_value)
{
	if(s.top==0) 
		return 0;
	*p_value=s.ptr[s.top-1];
	return 1;
}
