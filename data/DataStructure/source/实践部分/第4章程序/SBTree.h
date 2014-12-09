typedef int SBT_DataType;

typedef struct 
{
    unsigned max;
    int * f_ptr;
    SBT_DataType * d_ptr;
}SBTree;

void SBT_init(SBTree* p_t)
{
	unsigned i;
    p_t->max=8;
	p_t->f_ptr=(int*)malloc(sizeof(int)*p_t->max);
	p_t->d_ptr=(SBT_DataType*)malloc(sizeof(SBT_DataType)*p_t->max);
	for(i=0;i<p_t->max;++i)
		p_t->f_ptr[i]=0;
}

void SBT_clear(SBTree* p_t )
{
	if(p_t)
	{
		p_t->max=0;
		free(p_t->f_ptr);
		free(p_t->d_ptr);
	}
}

//插入成功返回1，失败返回0
int SBT_append(SBTree* p_t,SBT_DataType value,unsigned floor,unsigned index)
{
	int parents_pos,insert_pos;
	
	int* new_f_ptr;
	SBT_DataType* new_d_ptr;
	if(floor==0||index==0||index>pow(2,floor-1)) 
		return 0;
	
	//插入根
	if(floor==1&&index==1&&p_t->f_ptr[0]==0)
	{
		p_t->f_ptr[0]=1;
		p_t->d_ptr[0]=value;
		return 1;
	}
	//判断插入位置的双亲是否存在
    parents_pos=(int)pow(2,floor-2)+(index+1)/2-2;
	if(parents_pos>=(int)p_t->max||parents_pos<0||p_t->f_ptr[parents_pos]==0)
		return 0;
	//判断插入位置是否被占
	insert_pos=(int)pow(2,floor-1)+index-2;
	if(insert_pos<0||insert_pos<(int)p_t->max&&p_t->f_ptr[insert_pos]==1)
		return 0;
	if(insert_pos>=(int)p_t->max)
	{   unsigned i;
		new_f_ptr=(int*)malloc(sizeof(int)*2*p_t->max);
		new_d_ptr=(SBT_DataType*)malloc(sizeof(SBT_DataType)*2*p_t->max);
		for(i=0;i<p_t->max;i++)
			new_f_ptr[i]=p_t->f_ptr[i];
		for(;i<p_t->max*2;i++)
			new_f_ptr[i]=0;
		for(i=0;i<p_t->max;i++)
			new_d_ptr[i]=p_t->d_ptr[i];
		p_t->max*=2;
		free(p_t->f_ptr);
		free(p_t->d_ptr);
		p_t->f_ptr=new_f_ptr;
		p_t->d_ptr=new_d_ptr;
	}
	p_t->f_ptr[insert_pos]=1;
	p_t->d_ptr[insert_pos]=value;	
	return 1;
}


//删除子树成功返回1，失败返回0
int SBT_delete(SBTree* p_t,unsigned floor,unsigned index)
{
	int del_pos;
	if(floor==0||index==0||index>pow(2,floor-1)) 
		return 0;
	//判断待删除子树是否存在
	del_pos=(int)pow(2,floor-1)+index-2;
	if(del_pos<0||del_pos>=(int)p_t->max||p_t->f_ptr[del_pos]==0)
		return 0;
	p_t->f_ptr[del_pos]=0;
	SBT_delete(p_t,floor+1,index*2-1);
	SBT_delete(p_t,floor+1,index*2);   
	return 1;

}

void  SBT_floorPrint(SBTree t)
{
	unsigned i;
	for(i=0;i<t.max;i++)
		if(t.f_ptr[i])
			printf("%d ",t.d_ptr[i]);
}	
#include"stack.h"
void  SBT_prePrint(SBTree t)
{
	Stack s;
	unsigned index=0;
	if(t.f_ptr[0]==0) return;
	S_init(&s);
	while(!S_isEmpty(s)||index<t.max&&t.f_ptr[index]==1)
    	if(index<t.max&&t.f_ptr[index]==1)
		{
			printf("%d ",t.d_ptr[index]);
			S_push(&s,index);
			index=index*2+1;
		}
		else if(!S_isEmpty(s))
		{
			S_getTop(s,&index);
			S_pop(&s);
			index=index*2+2;
		}	
				
	S_clear(&s);

}
void  SBT_inPrint(SBTree t)
{
	Stack s;
	unsigned index=0;
	if(t.f_ptr[0]==0) return;
	S_init(&s);
	while(!S_isEmpty(s)||index<t.max&&t.f_ptr[index]==1)
		if(index<t.max&&t.f_ptr[index]==1)
		{
			S_push(&s,index);
			index=index*2+1;
		}
		else if(!S_isEmpty(s))
		{
			S_getTop(s,&index);
			S_pop(&s);
			printf("%d ",t.d_ptr[index]);
			index=index*2+2;
		}	
	S_clear(&s);
}
void  SBT_postPrint(SBTree t)
{
	Stack s;
	unsigned pre=0;
	unsigned index=0;
	if(t.f_ptr[0]==0) return;
	S_init(&s);
	while(!S_isEmpty(s)||index<t.max&&t.f_ptr[index]==1)
		if(index<t.max&&t.f_ptr[index]==1)
		{
			S_push(&s,index);
			index=index*2+1;
		}
		else if(!S_isEmpty(s))
		{			
			while(S_getTop(s,&index)&&	(index*2+2>=t.max||t.f_ptr[index*2+2]==0||pre==2*index+2))
			{
				S_pop(&s);
				printf("%d ",t.d_ptr[index]);
				pre=index;
			}
			if(S_isEmpty(s)) break;
			index=index*2+2;		
		}	
	S_clear(&s);
}

unsigned SBT_getDepth(SBTree t)
{
    int i,j=1,floor=0;
    for(i=t.max-1;i>=0;i--)
		if(t.f_ptr[i]==1) break;
	while(i+1>=j)
	{
		j<<=1;
		++floor;
	}
	return floor;
}

unsigned SBT_getCountLeaf(SBTree t)
{
	unsigned count=0;
    unsigned i;
	if(t.f_ptr[0]==0) 
		return 0;
	for(i=0;i<t.max;i++)
		if(t.f_ptr[i]==1&&i*2+1>=t.max||i*2+2==t.max&&t.f_ptr[i*2+1]==0||t.f_ptr[i*2+1]==0&&t.f_ptr[i*2+2]==0)
			count++;	
	return count;
}

//按层次遍历查找结点，存在等值结点返回层数，否则返回0
unsigned SBT_getFloor(SBTree t,SBT_DataType value)
{
	unsigned i,j=1,floor=0;
	for(i=0;i<t.max;i++)
		if(t.f_ptr[i]&&t.d_ptr[i]==value)
			break;
	while(i+1>=j)
	{
		j<<=1;
		++floor;
	}
	return floor;
}

