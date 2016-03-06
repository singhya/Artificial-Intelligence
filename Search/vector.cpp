#include <iostream>
#include <map>
#include <vector>
#include <string>
using namespace std;
struct TimeSlot
{
	int start;
	int end;
};
struct Edge
{
	int NodeTo;
	int length;
	vector<TimeSlot> slots;
};
void display(vector<Edge> v)
{
	vector<Edge>::iterator i;
	for(i=v.begin(); i!=v.end(); ++i){
		cout<<(*i).length<<endl;
	}
}
int main()
{
	vector<Edge> v;
	map<string, vector<Edge>> graph;
	
	map<string, vector<Edge>>::iterator i;
	Edge e1;
	e1.length = 10;
	Edge e2;
	e2.length = 20;
	v.push_back(e1);
	v.push_back(e2);
	//v.push_back(3);
	//v.push_back(4);
	//v.push_back(5);
	graph["AA"] = v;
	for(i=graph.begin(); i!=graph.end(); ++i){
		cout<<i->first<<endl;
		display(i->second);
	}
	return 0;
}
