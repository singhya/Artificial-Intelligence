#include <iostream>
using namespace std;
class B
{
	public:
	int c;
};
class A
{
	public:
	int a;
	int b;
	B Bval;
};
int main()
{
	A p;
	p.a=1;
	p.b=2;
	p.Bval.c = 3;
	cout<<p.a;
	cout<<p.b;
	cout<<p.Bval.c<<endl;
	A q = p;
	cout<<q.a;
	cout<<q.b;
	cout<<q.Bval.c<<endl;
	q.a++;
	q.b++;
	q.Bval.c++;
	cout<<p.a;
	cout<<p.b;
	cout<<p.Bval.c<<endl;
	cout<<q.a;
	cout<<q.b;
	cout<<q.Bval.c<<endl;
	return 0;
}
