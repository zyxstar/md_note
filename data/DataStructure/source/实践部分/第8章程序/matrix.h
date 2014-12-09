typedef struct
{
	unsigned i,j;
	double data;
}Triple;
typedef struct
{
	Triple * d_ptr;
	unsigned size;
	unsigned count;
	unsigned row,col;	
} Matrix;

int Triple_less(Triple a,Triple b)
{
	return a.i<b.i||a.i==b.i&&a.j<b.j;
}
typedef Triple DataType;

#include"insert_sort.h"
#include"binary_search.h"


void M_init(Matrix* p_m,unsigned s,unsigned r,unsigned c)
{
    p_m->size=s>0?s:16;
	p_m->row=r>0?r:1;
	p_m->col=c>0?c:1;
	p_m->count=0;
	p_m->d_ptr=(Triple *)malloc(sizeof(Triple)*p_m->size);
}

void M_clear(Matrix* p_m)
{
  if(p_m->d_ptr)
    free(p_m->d_ptr);
}

int M_setij(Matrix* p_m,double d,unsigned i,unsigned j)
{
	Triple tri;
	if(i>=p_m->row||j>=p_m->col)
		return 0;
	tri.data=d;
	tri.i=i;
	tri.j=j;
	if(binary_search(p_m->d_ptr,p_m->count,tri,Triple_less))
		return 0;
	if(p_m->size==p_m->count)
	{
		unsigned i;
		Triple *new_d_ptr=(Triple *)malloc(sizeof(Triple)*p_m->size*2);
		for(i=0;i<p_m->size;i++)
			new_d_ptr[i]=p_m->d_ptr[i];
		free(p_m->d_ptr);
		p_m->d_ptr=new_d_ptr;
		p_m->size*=2;
	}
	p_m->d_ptr[p_m->count++]=tri;
	insert_sort(p_m->d_ptr,p_m->count,Triple_less);
	return 1;
}

int M_getij(Matrix* p_m,double* p_d,unsigned i,unsigned j)
{
	Triple *p_tri,tri;
	if(i>=p_m->row||j>=p_m->col)
		return 0;
	tri.i=i;
	tri.j=j;
	*p_d=0.0;
	p_tri=binary_search(p_m->d_ptr,p_m->count,tri,Triple_less);
	if(p_tri)
		*p_d=p_tri->data;
	return 1;	
}

void M_print(Matrix m)
{
	unsigned i,j;
	for(i=0;i<m.row;i++)
	{
		double d;
		for(j=0;j<m.col;j++)
		{
			M_getij(&m,&d,i,j);
			printf("%lf ",d);
		}
		printf("\n");
	}
}
