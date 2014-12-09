#include<stdio.h>
#include<string.h>

int  Substr(char * str, unsigned i, unsigned n,char * sub)
{
	unsigned str_len=strlen(str);
	if(i>=str_len) return 0;
    n=i+n>str_len?str_len-i:n;
	strncpy(sub,str+i,n);
	sub[n]='\0';
	return 1;
}

int main()
{
	char p[10]="123456789";
	char q[10];

	if(Substr(p,2,5,q))
		printf("%s\n",q);
	if(Substr(p,2,10,q))
		printf("%s\n",q);
	if(!Substr(p,10,5,q))
		printf("参数太大，求子串操作失败\n");
	
	return 0;
}