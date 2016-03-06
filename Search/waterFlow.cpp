#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <queue>
#include <algorithm>
using namespace std;

struct TimeSlot
{
	int start;
	int end;
};

struct Edge
{
	string NodeTo;
	int length;
	vector<TimeSlot> slots;
};

struct QueueNode
{
	string name;
	int level;
};

bool is_number(const string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool BFS(map<string, vector<Edge> > graph, map<string, bool> goals, string root, int time, string & output)
{
	//cout<<"BFS called";
	//cout<<endl;
	map<string, bool> visited_nodes;
	queue<QueueNode> q;
	int level = time;
	QueueNode node;
	node.name = root;
	node.level = time;
	q.push(node);
	bool goalAttained = false;
	visited_nodes[root] = true;
	while(!q.empty() && !goalAttained)
	{
		// Checking current node
		string selected_node = q.front().name;
		int selected_node_level = q.front().level;
		if(goals[selected_node])
		{	
			output.append(selected_node + " " + to_string(q.front().level%24));
   			return true;
		}
		
		// Moving to next level
		vector<Edge> edges = graph[selected_node];
		int new_level = selected_node_level + 1;
		q.pop();
		//cout<<q.front().name<<" ";
		for(vector<Edge>::iterator i=edges.begin(); i!=edges.end() && !goalAttained; ++i){
			string nodeTo = (*i).NodeTo;
			if(visited_nodes.find(nodeTo) == visited_nodes.end())
			{
				QueueNode node;
				node.name = nodeTo;
				node.level = new_level; 
				q.push(node);
   				visited_nodes[nodeTo] = true;
			}	
		}
		//cout<<selected_node<<" ";
	}
	return false;
}

bool DFS(map<string, vector<Edge> > graph, map<string, bool> goals, string node, map<string, bool> visited_nodes, int level, string & output)
{
	visited_nodes[node] = true;
	bool goalFound = false;
	//cout<<node;
	//cout<<" ";
	//cout<<level;
	vector<Edge> edges = graph[node];
	if(goals[node])
	{
		output.append(node + " " + to_string(level%24));
		goalFound = true;
		return goalFound;
	}
	for(vector<Edge>::iterator i=edges.begin(); i!=edges.end() && !goalFound; ++i){
			string nodeTo = (*i).NodeTo; 
			if(visited_nodes.find(nodeTo) == visited_nodes.end())
			{
   				visited_nodes[nodeTo] = true;
   				goalFound = DFS(graph, goals, (*i).NodeTo, visited_nodes, level+1, output);
			}	
	}
	return goalFound;
}

struct NodeByDistance
{
	string name;
	int distance;
};

class LessThanByDistance {
    public:
    bool operator()(NodeByDistance& lhs, NodeByDistance& rhs){
    	if(lhs.distance!=rhs.distance)
    	{
		   return lhs.distance > rhs.distance;
    	}
		else
    	{
    		return lhs.name > rhs.name;	
		}
    }
};

priority_queue<NodeByDistance, std::vector<NodeByDistance>, LessThanByDistance> checkAndInsert(
				priority_queue<NodeByDistance, std::vector<NodeByDistance>, LessThanByDistance> pq,
				NodeByDistance node)
{
	priority_queue<NodeByDistance, std::vector<NodeByDistance>, LessThanByDistance> new_pq;
	bool new_node_inserted = false;	
	while(!pq.empty())
	{
	
		NodeByDistance n = pq.top();
	
		if(n.name != node.name)
		{
			new_pq.push(n);
		}
		else if(n.name == node.name && node.distance<n.distance)
		{
			new_pq.push(node);
			new_node_inserted = true;
		}
		else if(n.name == node.name && node.distance>=n.distance)
		{
			new_pq.push(n);
			new_node_inserted = true;
		}
		pq.pop();
	}
	if(!new_node_inserted)
	{
		new_pq.push(node);
	}
	return new_pq;
}
bool UCS(map<string, vector<Edge> > graph, map<string, bool> goals, string root, map<string, bool> visited_nodes, int time, string & output)
{
	priority_queue<NodeByDistance, std::vector<NodeByDistance>, LessThanByDistance> pq;
	NodeByDistance n;
	n.distance = time;
	n.name = root;
	pq.push(n);
	int distance = 0;
	bool goalAttained = false;
	while(!pq.empty() && !goalAttained)
	{
		vector<Edge> edges = graph[pq.top().name];
		string selected_node = pq.top().name;
		int selected_node_distance = pq.top().distance;
		visited_nodes[selected_node] = true;
		pq.pop();
		if(goals[selected_node])
   		{
   			output.append(selected_node + " "+ to_string((selected_node_distance)%24));
   			return true;
   			//break;
   		}
   		int current_time = (selected_node_distance)%24;
		//cout<<q.front().name<<" ";
		for(vector<Edge>::iterator i=edges.begin(); i!=edges.end() && !goalAttained; ++i){
			string nodeTo = (*i).NodeTo;
			vector<TimeSlot> blockedTimes = (*i).slots;
			/*cout<<"Current time : ";
			cout<<current_time;
			cout<<" Node from ";
			cout<<selected_node;
			cout<<" to ";
			cout<<(*i).NodeTo;
			cout<<" slot count :";
			cout<<(*i).slots.size();
			cout<<endl;*/
			bool isBlocked = false;
			for(vector<TimeSlot>::iterator j=blockedTimes.begin(); j!=blockedTimes.end() && !goalAttained; ++j)
			{
				if((*j).start<=current_time && (*j).end>=current_time)
				{
					isBlocked = true;
				}
			}
			if(!isBlocked)
			{
				int distanceFromRoot = (*i).length + selected_node_distance;
				if(visited_nodes.find(nodeTo) == visited_nodes.end())
				{
					NodeByDistance node;
					node.name = nodeTo;
					node.distance = distanceFromRoot;
					// check if queue already contains that node
					pq = checkAndInsert(pq, node); 
				}
			}
		}
		//cout<<selected_node<<" ";
	}
	return false;
}

bool compareByNodeName(const Edge &a, const Edge &b)
{
    return a.NodeTo < b.NodeTo;
}

void prepareGraph(vector<string> test_case, string & output)
{
	map<string, vector<Edge> > graph;
	map<string, bool> goals;
	graph[test_case[1]] = {};
	goals[test_case[1]] = false;
	istringstream iss(test_case[2]);
    vector<string> nodes{istream_iterator<string>{iss},
                      istream_iterator<string>{}};
                     
    for(vector<string>::iterator i=nodes.begin(); i!=nodes.end(); ++i){
		graph[(*i)] = {};
		goals[(*i)] = true;
	}
	
	istringstream iss1(test_case[3]);
	nodes = {istream_iterator<string>{iss1},
     istream_iterator<string>{}};
    
    for(vector<string>::iterator i=nodes.begin(); i!=nodes.end(); ++i){
		graph[(*i)] = {};
		goals[(*i)] = false;
	}
	
	/*for(map<string, vector<Edge>>::iterator i=graph.begin(); i!=graph.end(); ++i){
		cout<<i->first<<endl;
	}   
	
	for(map<string, bool>::iterator i=goals.begin(); i!=goals.end(); ++i){
		cout<<i->first;
		cout<<" ";
		cout<<i->second;
		cout<<endl;
	}   
	cout<<endl;*/
	
	int edgeCount = stoi(test_case[4]);
	for(int i = 0;i<edgeCount;i++)
	{
		string edgeString = test_case[5+i];
		istringstream edgeStream(edgeString);
		vector<string> tokens{istream_iterator<string>{edgeStream},
                      istream_iterator<string>{}};
        string node1 = tokens[0];
		string node2 = tokens[1];
		int length = stoi(tokens[2]);
		int numberOfOffTimes = stoi(tokens[3]);
		vector<TimeSlot> time_slots;
        for(int j=0;j<numberOfOffTimes;j++)
        {
        	string slot = tokens[4+j];
        	int index = slot.find("-");
        	TimeSlot ts;
        	ts.start = stoi(slot.substr(0,index));
        	ts.end = stoi(slot.substr(index+1));
        	time_slots.push_back(ts);
		}
		Edge e;
		e.NodeTo = node2;
		e.length = length;
		e.slots = time_slots;
		graph[node1].push_back(e);
		//e.NodeTo = node1;
		//graph[node2].push_back(e);
	}
	for(map<string, vector<Edge> >::iterator i=graph.begin(); i!=graph.end(); ++i){
		/*cout<<"Edge details for ";
		cout<<i->first;
		cout<<" (if exists) :"<<endl;*/
		sort(i->second.begin(), i->second.end(), compareByNodeName);
		/*if(i->second.size())
		{
			for(vector<Edge>::iterator k=i->second.begin(); k!=i->second.end(); ++k)
			{
				cout<<(*k).NodeTo;
				cout<<" ";
				//cout<<(*k).length;
				//cout<<" Off times count:";
				//cout<<(*k).slots.size()<<endl;
			} 
		}
		cout<<endl;*/
	}   
	//cout<<endl;
	
	string operation = test_case[0];
	int lineOfWaterFLowTime = test_case.size();
	int time = stoi(test_case[lineOfWaterFLowTime-1]);
	map<string, bool> visited_nodes;
	if(operation=="BFS")
	{
		if(BFS(graph, goals,test_case[1],time, output))
		{
			
		}
		else
		{
			output.append("None");
		}
		output.append("\n");
	}
	else if(operation=="DFS")
	{
		if(DFS(graph, goals,test_case[1],visited_nodes,time, output))
		{
			
		}
		else
		{
			output.append("None");
		}
		output.append("\n");
	}
	else
	{
		if(UCS(graph, goals,test_case[1],visited_nodes,time,output))
		{
			
		}
		else
		{
			output.append("None");
		}
		output.append("\n");
	}
	/*cout<<test_case[0]<<endl;
	cout<<test_case[1]<<endl;
	cout<<test_case[2]<<endl;
	cout<<test_case[3]<<endl;
	cout<<test_case[4]<<endl;*/
	/*for(i=test_case.begin(); i!=test_case.end(); ++i){
		cout<<(*i)<<endl;
	}*/
}

int main (int argc, char* argv[]) {
  ofstream myOutfile;
  myOutfile.open ("output.txt");
  string output="";
  int testCase = 0;
  string line;
  //cout<<argv[0]<<endl;
  //cout<<argv[1]<<endl;
  //cout<<argv[2]<<endl;
  ifstream myInfile(argv[2]);
  if (myInfile.is_open())
  {
  	getline(myInfile,line);
    int testCases = stoi(line);
    
	vector<string> testCase;
	bool Probable_end_of_test_case = false;
    while(getline (myInfile,line))
    {
    	if(line.size() == 0 && Probable_end_of_test_case)
    	{
    		prepareGraph(testCase, output);
    		testCase={};
    	}
    	else
    	{
    		testCase.push_back(line);
    	}
    	if(is_number(line))
    	{
    		Probable_end_of_test_case = true;
		}
		else
		{
			Probable_end_of_test_case = false;
		}
	}
	if(testCase.size()>0)
    	prepareGraph(testCase, output);
	
	myInfile.close();
  }

  else cout << "Unable to open file"; 
  cout<<output;
  myOutfile << output;
  myOutfile.close();
  return 0;
}
