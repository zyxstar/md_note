#include<stdio.h>
#include<malloc.h>
typedef struct
{
	int expn;
	double coef;
} Polynomial;
#define OLDT
typedef Polynomial OL_DataType;
#include"OrderList.h"

void print(OList ol)
{
	OL_Node* cur_ptr=ol.head;
	while(cur_ptr)
	{
		if(cur_ptr!=ol.head&&cur_ptr->data.coef>=0.0)
			printf("+");
		printf("%lfx^%d",cur_ptr->data.coef,cur_ptr->data.expn);
		cur_ptr=cur_ptr->next;
	}
	printf("\n");
}

int  greater(OL_DataType a,OL_DataType b)
{
	return a.expn>b.expn;
}

void P_add(OList a,OList b,OList* sum)
{
	OList a1,b1;	
	OL_Node* cur_ptra=a.head;
	OL_Node* cur_ptrb=b.head;
	OL_DataType tmp;
	OL_init(&a1,a.rel);
	OL_init(&b1,b.rel);
	if(sum->head==a.head)
	{
		for(;a.head;a.head=a.head->next)
			OL_insert(&a1,a.head->data);
		cur_ptra=a1.head;
	}
	if(sum->head==b.head)
	{
		for(;b.head;b.head=b.head->next)
			OL_insert(&b1,b.head->data);
		cur_ptrb=b1.head;
	}   
    OL_clear(sum);
    OL_init(sum,greater);

	while(cur_ptra||cur_ptrb)
	{
		if(cur_ptrb==0||sum->rel(cur_ptra->data,cur_ptrb->data))
		{
			OL_insert(sum,cur_ptra->data);
			cur_ptra=cur_ptra->next;
			continue;
		}
		if(cur_ptra==0||sum->rel(cur_ptrb->data,cur_ptra->data))
		{
			OL_insert(sum,cur_ptrb->data);
			cur_ptrb=cur_ptrb->next;
			continue;
		}
		tmp.expn=cur_ptra->data.expn;
		tmp.coef=cur_ptra->data.coef+cur_ptrb->data.coef;
		cur_ptra=cur_ptra->next;
		cur_ptrb=cur_ptrb->next;
		if(tmp.coef)
			OL_insert(sum,tmp);		
	}
	OL_clear(&a1);
	OL_clear(&b1);
}

void P_mul(OList a,OList b,OList* prod)
{
	OList a1,b1,sum;	
	OL_Node* cur_ptra=a.head;
	OL_Node* cur_ptrb=b.head;
	OL_DataType tmp;
	if(OL_isEmpty(a)||OL_isEmpty(b))
	{
		OL_clear(prod);
		return;
	}
	OL_init(&a1,a.rel);
	OL_init(&b1,b.rel);
	OL_init(&sum,prod->rel);
	if(prod->head==a.head)
	{
		for(;a.head;a.head=a.head->next)
			OL_insert(&a1,a.head->data);
		cur_ptra=a1.head;
	}
	if(prod->head==b.head)
	{
		for(;b.head;b.head=b.head->next)
			OL_insert(&b1,b.head->data);
		cur_ptrb=b1.head;
	}   
    OL_clear(prod);
	while(cur_ptra)
	{
        OL_Node* cur_ptrb1=cur_ptrb;
		while(cur_ptrb1)
		{
			tmp.coef=cur_ptra->data.coef*cur_ptrb1->data.coef;
			tmp.expn=cur_ptra->data.expn+cur_ptrb1->data.expn;
			OL_insert(&sum,tmp);
			cur_ptrb1=cur_ptrb1->next;
		}
		P_add(sum,*prod,prod);
		OL_clear(&sum);
		cur_ptra=cur_ptra->next;
	}
	OL_clear(&a1);
	OL_clear(&b1);

}

int main()
{
	OList ol1,ol2,ol3;
	OL_DataType d1,d2,d3,d4;
	d1.expn=1;
	d1.coef=1.1;
	d2.expn=2;
	d2.coef=-2.2;
	d3.expn=3;
	d3.coef=-3.3;
	d4.expn=4;
	d4.coef=4.4;	
	OL_init(&ol1,greater);
	OL_init(&ol2,greater);
	OL_init(&ol3,greater);
	OL_insert(&ol1,d1);
	OL_insert(&ol1,d2);
	OL_insert(&ol1,d3);
	OL_insert(&ol1,d4);
	d2.coef=2.2;
	OL_insert(&ol2,d1);
	OL_insert(&ol2,d2);
	OL_insert(&ol2,d3);
	P_add(ol1,ol2,&ol3);
	print(ol1);
	print(ol2);
	print(ol3);
	P_mul(ol1,ol2,&ol3);
	print(ol3);
	OL_clear(&ol1);
	OL_clear(&ol2);
	OL_clear(&ol3);
}