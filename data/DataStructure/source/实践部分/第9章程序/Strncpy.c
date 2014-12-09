#include<stdio.h>

char *  Strncpy(char * a, char * b, unsigned n)
{
	char* old_a=a;
	unsigned i=0;
	for(;i<n;i++)
		if(*a++=*b) b++;
	return old_a;
}

int main()
{
	char p[10]="*********";
	char q[10]="*********";
	
	Strncpy(p,"12345",3);
	printf("%s\n",p);
	
	Strncpy(q,"12345",7);
	printf("%s\n",q);
	printf("q[5]=%d\n",q[5]);
	printf("q[6]=%d\n",q[6]);
	printf("q[7]=%d\n",q[7]);
	printf("q[8]=%d\n",q[8]);
	printf("q[9]=%d\n",q[9]);

	return 0;
}