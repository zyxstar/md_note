typedef char AtomType;
struct Lists;
typedef struct Node
{
	int tag;
	union 
	{
		AtomType atom;
		struct Lists* lp;
	};
	struct Node* next;
} Lnode;
typedef struct Lists
{
	Lnode* head,*tail;
} Lists;

Lnode* L_makeNode(int t,AtomType value,Lists *lp)
{
	Lnode* new_ptr= (Lnode*)malloc(sizeof(Lnode));
	new_ptr->tag=t;
	if(t==0)
		new_ptr->atom=value;
	else
		new_ptr->lp=lp;
	new_ptr->next=0;
	return new_ptr;
}



void L_init(Lists * p_l)
{
	p_l->head=p_l->tail=0;
}
void L_clear(Lists * p_l)
{
	Lnode* cur,*del;
	if(p_l==0)
		return ;
	cur=p_l->head;
	while(cur)
	   if(cur->tag==0)
		{
			del=cur;
			cur=cur->next;
			free(del);
		}
		else
		{
			L_clear(cur->lp);
			free(cur->lp);
			del=cur;
			cur=cur->next;
			free(del);
		}	
}

void L_insertAtTail(Lists * p_l,int t,AtomType value,Lists *lp)
{
	Lnode* new_ptr=L_makeNode(t,value,lp);
	if(p_l->tail)
	{
	   p_l->tail->next=new_ptr;
	   p_l->tail=new_ptr;
	}
    else
    	p_l->tail=p_l->head=new_ptr;
}


void L_create(Lists * p_l, char *  str)
{
	static int index=0;
	for(;str[index];index++)
	{
		if(str[index]!='('&&str[index]!=')'&&!(str[index]>='a'&&str[index]<='z'))
		    continue;
		if(str[index]>='a'&&str[index]<='z')
			L_insertAtTail( p_l,0,str[index],0);
		if(str[index]=='(')
		{
			Lists * new_p_l=(Lists *)malloc(sizeof(Lists));
			L_init(new_p_l);
			L_insertAtTail(p_l,1,0,new_p_l);
			index++;
			L_create(new_p_l,str);
			continue;			
		}
		if(str[index]==')')
			return;
	}
}

void L_print(Lists *p_l)
{
	Lnode* cur;
	if(p_l==0)
		return ;
	printf("(");
	cur=p_l->head;
	while(cur)
	{
		if(cur->tag==0)
			printf("%c",cur->atom);
		else
			L_print(cur->lp);		
		cur=cur->next;
		if(cur)
			printf(",");
	}
	printf(")");
}
