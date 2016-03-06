#include <iostream>
#include <vector>
#include <string>
using namespace std;
class test
{
	public:
		vector<string> str;
	test()
	{
		str.push_back("chk1");
		str.push_back("chk2");
	}
	void change()
	{
		for(vector<string>::iterator i= this->str.begin();i!=this->str.end();i++)
		{
			(*i) = "changed";
		}
	}
	
};
void change(vector<string> s)
{
	for(vector<string>::iterator i= s.begin();i!=s.end();i++)
	{
		(*i) = "changed";
	}
}
int main()
{
	vector<string> s;
	s.push_back("a");
	s.push_back("b");
	test t;
	change(s);
	for(vector<string>::iterator i= s.begin();i!=s.end();i++)
	{
		cout<<(*i)<<endl;
	}
	t.change();
	for(vector<string>::iterator i= t.str.begin();i!=t.str.end();i++)
	{
		cout<<(*i)<<endl;
	}
}
