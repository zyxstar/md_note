#include<stdio.h>
#include<stdlib.h>
//返回[0,1)间的均匀分布小数
double rand01()
{
	return rand()/32768.0;
}

//mu是数学期望，omg是方差
double normal(double mu,double omg)
{
	double sum=0.0;
	int i;
	for(i=0;i<12;i++)
		sum+=rand01();
	sum-=6.0;
	return sum*omg+mu;
}

//测试，产生200000个数学期望为10.0,方差为参数值的随机数，分别统计各个区间随机数出现的次数
void test(double omg)
{
	int arr[21]={0},i,index;
	for(i=0;i<200000;++i)
	{
		index=(int)(normal(10.0,omg)+0.5);
		if(index<0) index=0;
		if(index>20) index=20;
		arr[index]++;
	}
	for(i=0;i<21;++i)
		printf("%d附近的小数出现：%d次\n",i,arr[i]);
}
int main()
{
	test(0.1);
	printf("\n");
	test(1);
	printf("\n");
	test(5);
	printf("\n");

	
	return 0;
}