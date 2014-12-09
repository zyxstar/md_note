#include<stdio.h>
#include<string.h>
#include<malloc.h>

typedef struct
{
	char* str;
}String;

void init(String *pstr)
{
	pstr->str=0;
}

void clear(String *pstr)
{
	if(pstr->str) 
		free(pstr->str);
	pstr->str=0;
}

int get_len(String *pstr)
{
	if(pstr->str==0) return -1;
	return strlen(pstr->str);
}

int cmp(String str1,String str2)
{
	if(str1.str==0||str2.str==0)
		return -2;
	return strcmp(str1.str,str2.str);
}

String * cstr_to_String(String *pstr,char * s)
{
	if(s==0) 
		return 0;
	if(pstr->str==s)
		return pstr;
	clear(pstr);
	pstr->str=(char*)malloc(strlen(s)+1);
	strcpy(pstr->str,s);
	return pstr;
}

String * assign(String *pstr1,String *pstr2)
{
	if(pstr2->str==0) 
		return 0;
	if(pstr1==pstr2)
		return pstr1;
	clear(pstr1);
	pstr1->str=(char*)malloc(get_len(pstr2)+1);
	strcpy(pstr1->str,pstr2->str);
	return pstr1;
}

String * append(String *pstr1,String *pstr2)
{
	char* new_s;
	if(pstr2->str==0) 
		return 0;
	if(pstr1->str==0)
		return assign(pstr1,pstr2);
	new_s=(char*) malloc(get_len(pstr1)+get_len(pstr2)+1);
	strcpy(new_s,pstr1->str);
	strcat(new_s,pstr2->str);
	clear(pstr1);
	pstr1->str=new_s;
	return pstr1;
}
 
String* insert(String *pstr1,unsigned index,String* pstr2)
{
	char* new_s;
	if(pstr2->str==0) 
		return 0;
	if(pstr1->str==0)
		return assign(pstr1,pstr2);
	if((int)index>get_len(pstr1))
		return append(pstr1,pstr2);
	new_s=(char*) malloc(get_len(pstr1)+get_len(pstr2)+1);
	strncpy(new_s,pstr1->str,index);
	strcpy(new_s+index,pstr2->str);
	strcat(new_s,pstr1->str+index);
	clear(pstr1);
	pstr1->str=new_s;
	return pstr1;
}

String* del(String *pstr,unsigned index,unsigned len)
{
	char* new_s;
	unsigned str_len;
	if(pstr->str==0) 
		return 0;
	str_len=get_len(pstr);
	if(index>str_len)
		return pstr;
	len=index+len>str_len?str_len-index:len;
	new_s=(char*) malloc(str_len-len+1);
	strncpy(new_s,pstr->str,index);
	new_s[index]='\0';
	strcat(new_s,pstr->str+index+len);
	clear(pstr);
	pstr->str=new_s;
	return pstr;
}

String* replace(String *pstr1,unsigned index,unsigned len,String* pstr2)
{
	char* new_s;
	unsigned str_len1,str_len2;
	if(pstr1->str==0||pstr2->str==0) 
		return 0;
	str_len1=get_len(pstr1);
	str_len2=get_len(pstr1);
	if(index>str_len1)
		return pstr1;
	len=index+len>str_len1?str_len1-index:len;
	new_s=(char*) malloc(str_len1-len+str_len2+1);
	strncpy(new_s,pstr1->str,index);
	new_s[index]='\0';
	strcat(new_s,pstr2->str);
	strcat(new_s,pstr1->str+index+len);
	clear(pstr1);
	pstr1->str=new_s;
	return pstr1;
}

void print(String str)
{
	if(str.str)
		printf("%s\n",str.str);
}
int main()
{
	String s1,s2;
	init(&s1);
	init(&s2);

	cstr_to_String(&s1,"abcdefgh");
	print(s1);

	assign(&s2,&s1);
	print(s2);

	printf("%d\n",cmp(s1,s2));

	append(&s2,&s1);
	print(s2);

	del(&s2,3,2);
	print(s2);

	insert(&s2,3,&s1);
	print(s2);

	replace(&s1,4,2,&s1);
	print(s1);

	clear(&s1);
	clear(&s2);
	
	return 0;
}

