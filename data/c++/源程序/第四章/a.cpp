//4.3 成员函数
#include<iostream>
using std::cout;
using std::endl;

class A
{
public: 
	int x;
	void set_x(int n)
 	{   
		x=n;//写成员变量
		cout<<x<<endl;//输出成员变量
	}
};

int main()
{
   A a,b; 
   
   cout<<"a.x=";
   a.set_x(1); 
   
   cout<<"b.x=";
   b.set_x(2); 
   return 0;
}
