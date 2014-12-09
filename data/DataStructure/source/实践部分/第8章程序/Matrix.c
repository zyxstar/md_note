#include<stdio.h>
#include<malloc.h>
#include"Matrix.h"

int main()
{
	Matrix m;
	M_init(&m,8,4,3);
	M_setij(&m,1.9,0,0);
	M_setij(&m,0.9,2,1);
	M_setij(&m,0.19,1,2);
	M_setij(&m,0.3,3,2);
	M_print(m);
	M_clear(&m);
}
