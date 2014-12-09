//动态存储的顺序表
typedef int SL_DataType;

typedef struct
{
	unsigned max;
	unsigned len;
	SL_DataType* ptr;    
} SeqList;

//初始化
void SL_init(SeqList* p_sl,unsigned n)
{
	if(n==0) n=1;
	p_sl->max=n;
	p_sl->len=0;
	p_sl->ptr=(SL_DataType*)malloc(sizeof(SL_DataType)*n);
}

//清空
void SL_clear(SeqList* p_sl)
{
	p_sl->max=p_sl->len=0;
	if(p_sl->ptr) 
		free(p_sl->ptr);
	p_sl->ptr=0;
}

//测试是否空
int SL_isEmpty(SeqList sl)
{
	return sl.len==0;
}

//求长度
unsigned SL_getLen(SeqList sl)
{
	return sl.len;
}

//插入元素
void SL_insert(SeqList* p_sl,SL_DataType value,unsigned pos)
{
	unsigned i;
	pos=pos>p_sl->len?p_sl->len:pos;
	if(p_sl->max==p_sl->len)
	{
		SL_DataType* new_ptr=(SL_DataType*)malloc(sizeof(SL_DataType)*p_sl->len*2);
		for(i=0;i<p_sl->len;++i)
			new_ptr[i]=p_sl->ptr[i];
		free(p_sl->ptr);
		p_sl->max*=2;
		p_sl->ptr=new_ptr;
	}
	for(i=p_sl->len;i>pos;--i)
		p_sl->ptr[i]=p_sl->ptr[i-1];
	p_sl->ptr[pos]=value;
	++p_sl->len;
}

//按位置删除元素，返回值表示删除是否成功
int SL_delete(SeqList* p_sl,unsigned pos)
{
	unsigned i;
	if(pos>=p_sl->len)
		return 0;
    for(i=pos;i<p_sl->len-1;++i)
		p_sl->ptr[i]=p_sl->ptr[i+1];
	--p_sl->len;
	return 1;
}

//按值删除元素，返回值表示删除元素个数
unsigned SL_remove(SeqList* p_sl,SL_DataType value)
{
	unsigned count=0;
	unsigned write=0,read=0;
	while(read<p_sl->len)
		if(p_sl->ptr[read]!=value)
		{
			p_sl->ptr[write]=p_sl->ptr[read];
		    ++write;
			++read;
		}
		else
		{
			++count;
			++read;
		}
	p_sl->len-=count;
	return count;
}





