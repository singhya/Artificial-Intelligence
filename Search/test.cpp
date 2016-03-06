#include <iostream>
#include <fstream>
#include <string>
using namespace std;
struct TreeNode
{
	std::string title;
};
class A
{
	public:
	TreeNode* t;
	A(const string & p)
	{
		TreeNode *new_node;
		new_node = (TreeNode*)malloc(sizeof(TreeNode));
		new_node->title=p;
		t = new_node;
	}
};
int main()
{
	A a("Aa");
	//cout<<a.t->title;
}
