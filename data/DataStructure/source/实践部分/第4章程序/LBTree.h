#ifndef LBTDATATYPE
#define LBTDATATYPE
typedef int LBT_DataType;
#endif

typedef struct Node
{
	LBT_DataType data;
	struct Node* left,*right;
}LBT_Node;

typedef LBT_Node* LBTree;
#include"Queue.h"

//创建新结点
LBT_Node* LBT_makeNode(LBT_DataType value)
{
	LBT_Node* new_ptr=(LBT_Node*)malloc(sizeof(LBT_Node));
	new_ptr->data=value;
	new_ptr->left=new_ptr->right=0;
	return new_ptr;
}

//初始化空树
void LBT_init(LBTree* p_t)
{
	*p_t=0;
}

void LBT_clear(LBTree* p_t )
{
	if(*p_t)
	{
		LBT_clear(&((*p_t)->left));
		LBT_clear(&((*p_t)->right));
		free(*p_t);
		*p_t=0;
	}
}

//插入成功返回1，失败返回0
int LBT_append(LBTree* p_t,LBT_DataType value,unsigned floor,unsigned index)
{
	int parents_floor,parents_index;
	int i;
	LBT_Node* new_ptr;
	LBT_Node* par_ptr=*p_t; 
	if(floor==0||index==0||index>(1<<(floor-1))) 
		return 0;
	//判断插入位置的双亲是否存在
	parents_floor=floor-1;
	parents_index=(index+1)/2;
	for(i=parents_floor-2>=0?(1<<(parents_floor-2)):0;i;i>>=1)
		if(par_ptr)
			if((parents_index-1)&i)
				par_ptr=par_ptr->right;
			else
				par_ptr=par_ptr->left;
		else
			return 0;
	if(par_ptr==0&&floor!=1)
		return 0;
	//判断插入位置是否被占
	if(par_ptr&&index%2&&par_ptr->left) 
		return 0;
	else if(par_ptr&&index%2==0&&par_ptr->right)
		return 0;
	new_ptr=LBT_makeNode(value);
	if(par_ptr==0)
	{
		*p_t=new_ptr;
		return 1;
	}
	if(index%2)
		par_ptr->left=new_ptr;
	else
		par_ptr->right=new_ptr;
	return 1;
}

//删除子树成功返回1，失败返回0
int LBT_delete(LBTree* p_t,unsigned floor,unsigned index)
{
	int i;
	LBTree* cur_ptr=p_t; 
	if(*cur_ptr==0||floor==0||index==0||index>(1<<(floor-1))) 
		return 0;
	//判断待删除子树是否存在
	
	for(i=floor-2>=0?(1<<(floor-2)):0;i;i>>=1)
		if(*cur_ptr)
			if((index-1)&i)
				cur_ptr=&((*cur_ptr)->right);
			else
				cur_ptr=&((*cur_ptr)->left);
		else
			return 0;
	if(*cur_ptr==0)
		return 0;
	LBT_clear(cur_ptr);
	return 1;

}

void  LBT_floorPrint(LBTree t)
{
	Queue q;
	if(t==0) return;
	Q_init(&q);
    Q_push(&q,t);
	while(!Q_isEmpty(q))
	{
		Q_getFront(q,&t);
		Q_pop(&q);
		printf("%d ",t->data);
		if(t->left)
			Q_push(&q,t->left);		
		if(t->right)
			Q_push(&q,t->right);		
	}
	Q_clear(&q);
}	

void  LBT_prePrint(LBTree t)
{
	if(t)
	{	
		printf("%d ",t->data);
		LBT_prePrint(t->left);	    
		LBT_prePrint(t->right);
	}
}
void  LBT_inPrint(LBTree t)
{
	if(t)
	{	
		LBT_inPrint(t->left);
	    printf("%d ",t->data);
		LBT_inPrint(t->right);
	}
}
void  LBT_postPrint(LBTree t)
{
	if(t)
	{	
		LBT_postPrint(t->left);
	    LBT_postPrint(t->right);
		printf("%d ",t->data);
	}
}

unsigned LBT_getDepth(LBTree t)
{
    unsigned l,r;
	if(t==0)
		return 0;
	l=LBT_getDepth(t->left);
	r=LBT_getDepth(t->right);
	return (l>r?l:r)+1;
}

unsigned LBT_getCountLeaf(LBTree t)
{
	unsigned l_count=0,r_count=0;
	if(t==0) return 0;
	if(t->left==0&&t->right==0)
		return 1;
	l_count=LBT_getCountLeaf(t->left);
	r_count=LBT_getCountLeaf(t->right);
	return l_count+r_count;
}

//按层次遍历查找结点，存在等值结点返回层数，否则返回0，层次遍历中使用空指针0作为分层标记入队列
unsigned LBT_getFloor(LBTree t,LBT_DataType value)
{
	Queue q;
	unsigned floor=1;
	if(t==0) return 0;
	Q_init(&q);
    Q_push(&q,t);
	Q_push(&q,0);
	while(!Q_isEmpty(q))
	{
		Q_getFront(q,&t);
		Q_pop(&q);
		if(t)
		{
		    if(t->data==value)
				break;
		    if(t->left)
			    Q_push(&q,t->left);		
		    if(t->right)
			    Q_push(&q,t->right);
		}
		else 
		{
			floor++;
			Q_push(&q,0);
			Q_getFront(q,&t);
			if(t==0) {floor=0;break;}
		}    	
	}
	Q_clear(&q);
	return floor;
}

