//求两非负整数的最大公约数
#include<stdio.h>
unsigned gcd(unsigned a,unsigned b)
{
	while(b)
	{
		unsigned old_a=a;
		a=b;
		b=old_a%b;
	}
	return a;
}

int main()
{
	printf("45和81的最大公约数是：%d\n",gcd(45,81));
	printf("121和77的最大公约数是：%d\n",gcd(121,77));

}