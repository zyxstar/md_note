/*
abcdefghijklmnopqrstuvwxwz
etarkmilswgqhojxuzwpnbfdvc
cvzxa
*/
//按照以上对照表为字符串进行加密和解密

#include<stdio.h>

char code[]="etarkmilswgqhojxuzwpnbfdvc";

void encode(char*str)
{
	while(*str)
	{
		if(*str>='a'&&*str<='z')
			*str=code[*str-'a'];
		if(*str>='A'&&*str<='Z')
			*str=code[*str-'A']-32;
		str++;
	}
}

void decode(char*str)
{
	char _code[26];
	int i;
	for(i=0;i<26;i++)
		_code[code[i]-'a']='a'+i;
	while(*str)
	{
		if(*str>='a'&&*str<='z')
			*str=_code[*str-'a'];
		if(*str>='A'&&*str<='Z')
			*str=_code[*str-'A']-32;
		str++;
	}
}

int main()
{
	char str[]="Hello C!";
	encode(str);
	printf("加密后：%s\n",str);
	decode(str);
	printf("解密后：%s\n",str);
}