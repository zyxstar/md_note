#include<stdio.h>
#include<malloc.h>
#include"Matrix.h"

void M_transpose(Matrix*p_m)
{
	unsigned t,i;
	t=p_m->row;
	p_m->row=p_m->col;
	p_m->col=t;
	for(i=0;i<p_m->count;i++)
	{
		t=p_m->d_ptr[i].i;
		p_m->d_ptr[i].i=p_m->d_ptr[i].j;
		p_m->d_ptr[i].j=t;
	}
	insert_sort(p_m->d_ptr,p_m->count,Triple_less);
}

int M_mul(Matrix m1,Matrix m2,Matrix *p_m)
{
	unsigned i,j,k;
	double d,d1,d2;
	if(&m1==p_m||&m2==p_m)
		return 0;
	if(m1.col!=m2.row)
		return 0;
	M_clear(p_m);
	M_init(p_m,8,m1.row,m2.col);
	for(i=0;i<m1.row;i++)
	  for(j=0;j<m2.col;j++)
		{
		  d=0.0;
		  for(k=0;k<m1.col;k++)
		  {
			M_getij(&m1,&d1,i,k);
			M_getij(&m2,&d2,k,j);
			d+=d1*d2;
		  }
		  M_setij(p_m,d,i,j);
		}
	return 1;
}
int main()
{
	Matrix m1,m2,m3;
	unsigned i,j;
	M_init(&m1,8,5,3);
	M_init(&m2,8,5,3);
	M_init(&m3,1,1,1);
    for(i=0;i<5;i++)
		for(j=0;j<3;j++)
		{
			M_setij(&m1,i*10+j,i,j);
			M_setij(&m2,i*10+j,i,j);
		}
	M_print(m1);
	printf("-----------------------\n");
	M_transpose(&m1);
	M_print(m1);
	printf("-----------------------\n");
	M_print(m2);
	printf("-----------------------\n");
	M_mul(m1,m2,&m3);
	M_print(m3);

	M_clear(&m1);
	M_clear(&m2);
	M_clear(&m3);
}