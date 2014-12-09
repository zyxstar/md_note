#include<stdio.h>
#include<string.h>
char *  Strncat(char * a, char * b, unsigned n)
{
	char* old_a=a;
	unsigned i=0;
	while(*a)
		a++;
	for(;i<n;i++)
		if(!(*a++=*b++)) break;
	if(i==n)
		*a='\0';
	return old_a;
}

int main()
{
	char p[10]="123";
	char q[10]="123";	
	printf("%s\n",Strncat(p,"abcd",3));
	printf("%s\n",Strncat(q,"abcd",6));
	return 0;
}
