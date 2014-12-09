#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
typedef int HDataType;
typedef struct
{
	unsigned size;
	unsigned D;
	HDataType *d_ptr;
	int *f_ptr;
}HashTable;

void H_init(HashTable * p_h,unsigned s,unsigned d)
{
	unsigned i;
	if(s==0) s=17;
	if(d==0||d>s) d=s;
	p_h->size=s;
	p_h->D=d;
    p_h->d_ptr=(HDataType*)malloc(sizeof(HDataType)*s);
	p_h->f_ptr=(int*)malloc(sizeof(int)*s);
	for(i=0;i<s;++i)
		p_h->f_ptr[i]=0;
}

void H_clear(HashTable * p_h)
{
	if(p_h)
	{
		free(p_h->d_ptr);
		free(p_h->f_ptr);
	}
}
//插入成功返回1，否则返回0
int H_insert(HashTable * p_h,HDataType value)
{
	unsigned pos=value%p_h->D;
	unsigned old_pos=pos;
	while(p_h->f_ptr[pos])
	{
		pos=(pos+1)%p_h->size;
		if(pos==old_pos)
			return 0;
	}
    p_h->f_ptr[pos]=1;
	p_h->d_ptr[pos]=value;
	return 1;
}
//查找成功返回下标，否则返回-1
int H_find(HashTable h,HDataType value)
{
	unsigned pos=value%h.D;
	unsigned old_pos=pos;
	
	while(h.f_ptr[pos]&&h.d_ptr[pos]!=value)
	{
		pos=(pos+1)%h.size;
		if(pos==old_pos)
			return -1;
	} 
	if(!h.f_ptr[pos])
		return -1;  
	return pos;
}


void print(HashTable h)
{
	unsigned i;
	for(i=0;i<h.size;i++)
		printf("%-8d",i);
	printf("\n");
	for(i=0;i<h.size;i++)
		if(h.f_ptr[i])
			printf("%-8d",h.d_ptr[i]);
		else
			printf("        ");
	printf("\n");
	printf("--------------------------------\n");	
}
int main()
{
	HashTable h;
	unsigned  i;
	H_init(&h,7,5);
	for(i=0;i<4;i++)
		H_insert(&h,rand()%100);
	H_insert(&h,9);
	H_insert(&h,19);
	print(h);
	printf("The postion is %d\n",H_find(h,67));
	printf("The postion is %d\n",H_find(h,29));
	return 0;
}

