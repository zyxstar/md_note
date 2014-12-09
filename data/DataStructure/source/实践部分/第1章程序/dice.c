/*
   1  2  3  4  5  6  
---------------------
1| 2  3  4  5  6  7
2| 3  4  5  6  7  8
3| 4  5  6  7  8  9
4| 5  6  7  8  9  10
5| 6  7  8  9  10 11
6| 7  8  9  10 11 12
*/
//掷两个骰子360000次，统计各点数出现的次数，并显示
#include<stdio.h>
#include<stdlib.h>

unsigned two_dice()
{
	return (rand()%6+1)+(rand()%6+1);
}

void diceAndPrint()
{
	unsigned arr[11]={0},i;
	for(i=0;i<360000;++i)
		arr[two_dice()-2]++;
	for(i=0;i<11;++i)
		printf("点数%d出现：%d次\n",i+2,arr[i]);
}

int main()
{
	diceAndPrint();
	return 0;
}