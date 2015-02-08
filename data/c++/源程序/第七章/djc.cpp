//7.11 多继承
#include<iostream>
using std::cout;
using std::endl;
//using namespace std;

class A1
{
public:
	int x;
	A1(int x1):x(x1)
 	{
		cout<<"A1的构造函数被执行"<<endl;
	}
	~A1()
	{
		cout<<"A1的析构函数被执行"<<endl;
	}
	void f()const
	{
		cout<<"A1::f()被执行"<<endl;
		cout<<"调用者地址："<<this<<endl;
		cout<<"x的地址："<<&x<<endl;
	}
};
class A2
{
public:
	int x;
	A2(int x1):x(x1)
	{
		cout<<"A2的构造函数被执行"<<endl;
	}
	~A2()
	{
		cout<<"A2的析构函数被执行"<<endl;
	}
	void f()const
	{
		cout<<"A2::f()被执行"<<endl;
		cout<<"调用者地址："<<this<<endl;
		cout<<"x的地址："<<&x<<endl;
	}
};
class B:public A1,public A2
{
public:
	int x;
	B(int x1,int x2,int x3):A2(x2),A1(x1),x(x3)
	{
		cout<<"B的构造函数被执行"<<endl;
	}
	~B()
	{
		cout<<"B的析构函数被执行"<<endl;
	}
	void f()const
	{
		cout<<"B::f()被执行"<<endl;
		cout<<"调用者地址："<<this<<endl;
		cout<<"x的地址："<<&x<<endl;
	}
};
int main()
{
	B b(1,2,3);
	cout<<"b.A1::x="<<b.A1::x<<endl;
	cout<<"b.A2::x="<<b.A2::x<<endl;
	cout<<"b.x="<<b.x<<endl;
	cout<<"-------------"<<endl;

	b.A1::f();
	b.A2::f();
	b.f();
	cout<<"-------------"<<endl;

	return 0;
}
