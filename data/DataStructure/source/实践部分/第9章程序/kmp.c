#include<stdio.h>
#include<string.h>
#include<malloc.h>

void set_next(char* t,int * next,unsigned len)
{
    int j=-1;
	int i=1;
	next[0]=-1;
	for(;i<len;i++)
	{
		while(j!=-1&&t[i-1]!=t[j]) j=next[j];
		++j;
		if(t[i]==t[j]) next[i]=next[j];
		else next[i]=j;
	}
}
int kmp(char* s,char* t,unsigned pos)
{
	int j=0;
	int s_len=strlen(s);
	int t_len=strlen(t);	
	int * next;
	if(t_len==0) return -1;
	next=(int*)malloc(sizeof(int)*t_len);	
	set_next(t,next,t_len);
	while(s_len-pos>=t_len)
	{
		while(j<t_len&&s[pos]==t[j]) 
		{
			++pos;
			++j;
		}
		if(j==t_len) 
		{
			free(next);
			return pos-t_len;
		}
		j=next[j];
		if(j==-1)
		{
			++pos;
			j=0;
		}
	}
	free(next);
	return -1;
}
int main()
{
	printf("%d\n",kmp("aabaabaacaabaab","aab",0));
	printf("%d\n",kmp("aabaabaacaabaab","aab",10));
	printf("%d\n",kmp("aabaabaacaabaab","aac",0));
	printf("%d\n",kmp("aabaabaacaabaab","aac",10));
	
	return 0;
}

