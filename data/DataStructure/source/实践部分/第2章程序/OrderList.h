//有序的双向链表
#ifndef OLDT
#define OLDT
typedef int OL_DataType;
#endif 

typedef struct Node
{
	OL_DataType data;
	struct Node* pre,*next;
}OL_Node;

typedef struct 
{
	OL_Node* head,*tail;
	int (*rel)(OL_DataType,OL_DataType);
}OList;

//创建新结点
OL_Node* OL_makeNode(OL_DataType value)
{
	OL_Node* new_ptr=(OL_Node*)malloc(sizeof(OL_Node));
	new_ptr->data=value;
	new_ptr->pre=new_ptr->next=0;
	return new_ptr;
}

//初始化空链表
void OL_init(OList* p_ol,int (*rel)(OL_DataType,OL_DataType))
{
	p_ol->head=p_ol->tail=0;
	p_ol->rel=rel;
}

//清空
void OL_clear(OList* p_ol)
{
	while(p_ol->head)
	{
		OL_Node* del_ptr=p_ol->head;
		p_ol->head=p_ol->head->next;
		free(del_ptr);
	}
	p_ol->tail=0;
}

//测试是否空
int OL_isEmpty(OList dl)
{
	return dl.head==0;
}

//插入，返回值表示插入是否成功，有重复值时失败，返回0，否则返回1
int OL_insert(OList*p_ol,OL_DataType value)
{
	OL_Node* new_ptr;
	OL_Node* cur_ptr=p_ol->head;
	while(cur_ptr&&p_ol->rel(cur_ptr->data,value))
		cur_ptr=cur_ptr->next;
	if(cur_ptr&&!p_ol->rel(cur_ptr->data,value)&&!p_ol->rel(value,cur_ptr->data))
		return 0;
	
	new_ptr=OL_makeNode(value);
	new_ptr->next=cur_ptr;
	if(cur_ptr)
	{
		new_ptr->pre=cur_ptr->pre;
		if(cur_ptr->pre)
			cur_ptr->pre->next=new_ptr;
		cur_ptr->pre=new_ptr;
	}
	else
	{
		new_ptr->pre=p_ol->tail;
		if(p_ol->tail)
			p_ol->tail->next=new_ptr;
		p_ol->tail=new_ptr;
	}
	if(cur_ptr==p_ol->head)
		p_ol->head=new_ptr;
	return 1;
}

//按值删除，有则删除返回1，无则返回0
int OL_remove(OList*p_ol,OL_DataType value)
{
	OL_Node* cur_ptr=p_ol->head;
	while(cur_ptr&&p_ol->rel(cur_ptr->data,value))
		cur_ptr=cur_ptr->next;
	if(cur_ptr==0||p_ol->rel(value,cur_ptr->data))
		return 0;
	if(cur_ptr==p_ol->head)
		p_ol->head=p_ol->head->next;
	else
		cur_ptr->pre->next=cur_ptr->next;
	if(cur_ptr==p_ol->tail)
		p_ol->tail=p_ol->tail->pre;
	else
		cur_ptr->next->pre=cur_ptr->pre;
	free(cur_ptr);
	return 1;	
}
