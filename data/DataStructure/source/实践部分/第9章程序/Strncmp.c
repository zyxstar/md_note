#include<stdio.h>

int  Strncmp(char * a, char * b, unsigned n)
{
	unsigned i=0;
	for(;i<n;i++,a++,b++)
		if(*a<*b) return -1;
		else if(*a>*b) return 1;
		else if(*a=='\0') return 0;	   
	return 0;
}

int main()
{
	char p1[10]="12345abcd";
	char q1[10]="12345efgh";
	char p2[11]="12345\0abcd";
	char q2[11]="12345\0efgh";
	printf("%d\n",Strncmp(p1,q1,5));
	printf("%d\n",Strncmp(p1,q1,9));
	printf("%d\n",Strncmp(q1,p1,9));
	printf("%d\n",Strncmp(p2,q2,9));
	
	return 0;
}

