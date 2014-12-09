#include<stdio.h>
#include<malloc.h>
#include"Stack.h"

int grade(int ch)
{
	if(ch=='*'||ch=='/'||ch=='%')
		return 100;
	if(ch=='+'||ch=='-')
		return 10;
	if(ch=='(')
		return 1;
	return 0;
}

void in_to_post(char* in,char* post)
{
	Stack s;
	char ch;
	S_init(&s);
	S_push(&s,'(');
	while(*in)
	{
		if(*in>='0'&&*in<='9')
		{
			*post++=*in++;
			continue;
		}
		else
		   *post++=' ';
		if(*in=='(')
		{
			S_push(&s,'(');
			in++;
			continue;
		}
		if(*in=='*'||*in=='/'||*in=='%'||*in=='+'||*in=='-')
		{
			while(S_getTop(s,&ch)&&grade(*in)<=grade(ch))
			{
				*post++=ch;
				S_pop(&s);				
			}
			S_push(&s,*in);
			in++;
			continue;
		}
		if(*in==')')
		{
			while(S_getTop(s,&ch)&&ch!='(')
			{
				*post++=ch;
				S_pop(&s);				
			}
			S_pop(&s);
			in++;
			continue;
		}
		in++;
	}
	while(S_getTop(s,&ch)&&ch!='(')
	{
		*post++=ch;
		S_pop(&s);				
	}
	*post='\0';
	S_clear(&s);
}
int compute(char* post)
{
	Stack s;
	int left=0,right=0;
	int res;
	S_init(&s);
	while(*post)
	{
		if(*post==' ')
		{
			post++;
			continue;
		}
		if(*post>='0'&&*post<='9')
		{
			right=*post-'0';
			left*=10;
			left+=right;
			if(*(++post)==' ')	
			{
				S_push(&s,left);
				left=0;
			}
			continue;
		}
		if(*post=='*'||*post=='/'||*post=='%'||*post=='+'||*post=='-')
		{
			S_getTop(s,&right);
			S_pop(&s);
			S_getTop(s,&left);
			S_pop(&s);
			switch (*post)
			{
			    case '*':res=left*right;break;
				case '/':res=left/right;break;
				case '%':res=left%right;break;
				case '+':res=left+right;break;
				case '-':res=left-right;break;
			}
		
			S_push(&s,res);
			left=0;
			post++;
			continue;
		}
	}
    S_getTop(s,&res);
	S_clear(&s);
	return res;
}

int main()
{
	char p[100];
	in_to_post("123+21*(322-933)/(37+13*23)",p);
	printf("%s\n",p);
	printf("%d\n",compute(p));
}