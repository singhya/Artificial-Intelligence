#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <limits>
#include <stack>
using namespace std;
stack<string> operations;
struct state
{
	vector<int> player1;
	vector<int> player2;
	int mancala1;
	int mancala2; 
	int pits;
	int depth;
	int alpha;
	int beta;
	int value;
	state* next_state;
	string operation;
	bool is_repeat;
};
struct program
{
	state initial_state;
	int strategy;
	int cutting_depth;
	int player;
};
struct result
{
	state s;
	bool explored;
	bool repeat;
};
void log(string s)
{
	ofstream myOutfile;
	string white_space;
	myOutfile.open("output.txt", std::ios_base::app);
	myOutfile << s;
	myOutfile.close();
}
void logTraversal(string s)
{
	ofstream myOutfile;
	myOutfile.open("traversal.txt", std::ios_base::app);
	myOutfile << s;
	myOutfile.close();
}
void log_next_move(state s)
{
	ofstream myOutfile;
	string str;
	for(int i=0;i<s.pits;i++)
	{
		str+=to_string(s.player2[i]);
		str+=" ";
	}
	str+="\n";
	for(int i=0;i<s.pits;i++)
	{
		str+=to_string(s.player1[i]);
		str+=" ";
	}
	str+="\n";
	str+=to_string(s.mancala2);
	str+="\n";
	str+=to_string(s.mancala1);
	myOutfile.open("next_move.txt");
	myOutfile << str;
	myOutfile.close();
}
void initialize_logs(int strategy)
{
	ofstream myOutfile;
	myOutfile.open("output.txt");
	myOutfile << "";
	myOutfile.close();
	myOutfile.open("traversal.txt");
	if(strategy == 3)
	{
		myOutfile << "Node,Depth,Value,Alpha,Beta";
		myOutfile << "\n";
	}
	if(strategy == 2)
	{
		myOutfile << "Node,Depth,Value";
		myOutfile << "\n";
	}
	myOutfile.close();
}
program init(string file)
{
	ifstream myInfile(file);
	program p;
	if (myInfile.is_open())
  	{
  		string line;
  		getline(myInfile,line);
		p.strategy = stoi(line);
    	
		getline(myInfile,line);
		p.player = stoi(line);
		
		getline(myInfile,line);
		p.cutting_depth = stoi(line);
		
		getline(myInfile,line);
		istringstream iss(line);
		vector<int> b{istream_iterator<int>{iss},
    	                istream_iterator<int>{}}; 
    	p.initial_state.player2 = b;
		
		getline(myInfile,line);
		istringstream iss1(line);
    	vector<int>a{istream_iterator<int>{iss1},
    	                istream_iterator<int>{}};
		p.initial_state.player1 = a;
			
		getline(myInfile,line);
		p.initial_state.mancala2 = stoi(line);
		
		getline(myInfile,line);
		p.initial_state.mancala1 = stoi(line);
		int N=0;
		for(vector<int>::iterator i = p.initial_state.player1.begin();i!=p.initial_state.player1.end();i++)
		{
			N++;
		}
		p.initial_state.pits = N;
		if(p.strategy==1)
		{
			p.cutting_depth = 1;
		}
		initialize_logs(p.strategy);	
		myInfile.close();
	}
    else cout << "Unable to open file"; 
  	return p;
}

vector <int> reverseB(vector<int> b, int n)
{
	for(int i=0;i<n/2;i++)
	{
		int temp = b[i];
		b[i] = b[n-1-i];
		b[n-1-i] = temp;
	}
	return b;
}
state playerSwitch(state s)
{
	vector<int> p = s.player1;
	s.player1 = s.player2;
	s.player2 = p;
	int temp = s.mancala1;
	s.mancala1 = s.mancala2;
	s.mancala2 = temp;
	return s;
}
string Display(state s)
{
	int N = s.player2.size();
	string white_space;
	for(int i=0;i<s.depth*5;i++)
	{
		 white_space += " ";
	}
	
	string output = "--- Player 2 ---";
	output+="\n"+white_space;
	output += "  ";
	for(int i=0;i<N;i++)
	{
		output+=to_string(s.player2[i])+" ";
	}
	output+="\n"+white_space;
	output+=to_string(s.mancala2);
	for(int i=0;i<N;i++)
	{
		output+="  ";
	}
	output+=" ";
	output+=to_string(s.mancala1);
	output+="\n"+white_space;
	output+="  ";
	for(int i=0;i<N;i++)
	{
		output+=to_string(s.player1[i])+" ";
	}
	output+="\n"+white_space;
	output+="--- Player 1 ---";
	output+="\n"+white_space;
	return output;
}
result Move(state s, int index, int player, bool isRepeat)
{
	int n = s.pits;
	if(!isRepeat)
	{
		s.depth = s.depth+1;
	}
	result r;
	r.repeat = false;
	if(player==1)
	{
		int stones = s.player1[index];
		s.player1[index] = 0;
		int i = index+1;
		while(stones)
		{
			for(;i<n&&stones;i++)
			{
				//Check for bonus case
				if(s.player1[i]==0 && s.player2[i]!=0 && stones==1)	
				{
					s.mancala1 = s.mancala1 + s.player2[i] + stones;
					s.player2[i] = 0;
				}
				else
				{
					s.player1[i] = s.player1[i]+1;
				}
				stones--;
			}
			if(stones)
			{
				s.mancala1++;
				stones--;
				if(!stones)
				{
					r.repeat = true;
				}
			}
			for(int j=n-1;j>=0&&stones;j--)
			{
				s.player2[j]= s.player2[j]+1;
				stones--;
			}
			i=0;
		}
	}
	else if(player==2)
	{
		int stones = s.player2[index];
		s.player2[index] = 0;
		int i = index-1;
		for(;i>=0&&stones;i--)
		{
			//Check for bonus case
			if(s.player2[i]==0 && s.player1[i]!=0 && stones==1)	
			{
				s.mancala2 = s.mancala2 + s.player1[i] + stones;
				s.player1[i] = 0;
			}
			else
			{
				s.player2[i] = s.player2[i]+1;
			}
			stones--;
		}
		if(stones)
		{
			s.mancala2++;
			stones--;
			if(!stones)
			{
				r.repeat = true;
			}
		}
		for(int j=0;j<n&&stones;j++)
		{
			s.player1[j] = s.player1[j]+1;
			stones--;
		}
		i = n-1;
	}
	r.s = s;
	return r;
}
result Greedy(state s, int player)
{
	int N = s.pits;
	result branches[N];
	for(int i=0;i<N;i++)
	{
		if(s.player1[i]!=0)
		{
			branches[i] = Move(s,i,player,false);
			branches[i].explored = true;
			if(branches[i].repeat)
			{
				branches[i] = Greedy(branches[i].s,player);
			}
		}
		else
		{
			branches[i].explored = false;
		}
	}
	int max = branches[0].s.mancala1;
	int max_index = 0;
	for(int i=1;i<N;i++)
	{
		if(branches[i].explored)
		{	
			if(branches[i].s.mancala1>max)
			{
				max = branches[i].s.mancala1;
				max_index=i;
			}
		}
	}
	return branches[max_index];
}

string stackValue()
{
	string output;
	stack<string> reverse;
	for (stack<string> dump = operations; !dump.empty(); dump.pop())
    {
    	reverse.push(dump.top());
	}
	for (stack<string> dump = reverse; !dump.empty(); dump.pop())
    {
    	output+= dump.top();
        output+=",";
	}
	output+="\n";
	return output;
}
void generateLog(string op,state s, int strategy)
{
	string white_space;
	for(int i=0;i<s.depth*5;i++)
	{
		 white_space += " ";
	}
	string value;
	if(s.value==numeric_limits<int>::min())
	{
		value = "-Infinity";	
	}
	else if(s.value==numeric_limits<int>::max())
	{
		value = "Infinity";
	}
	else
	{
		value = to_string(s.value);
	}
	string alpha_value;
	string beta_value;
	if(strategy==3)
	{
		if(s.alpha==numeric_limits<int>::min())
		{
			alpha_value = "-Infinity";	
		}
		else if(s.alpha==numeric_limits<int>::max())
		{
			alpha_value = "Infinity";
		}
		else
		{
			alpha_value = to_string(s.alpha);
		}
		if(s.beta==numeric_limits<int>::min())
		{
			beta_value = "-Infinity";	
		}
		else if(s.beta==numeric_limits<int>::max())
		{
			beta_value = "Infinity";
		}
		else
		{
			beta_value = to_string(s.beta);
		}
	}
	
	string output;
	output = white_space+op + ":" + s.operation + "," + to_string(s.depth) + ",";
	//output = s.operation + "," + to_string(s.depth) + ",";
	output+=" value = ";
	/*if(s.depth%2==0)
	{
		output += to_string(s.alpha);
	}
	else
	{
		output += to_string(s.beta);
	}*/
	output += value;
	output += ", alpha = ";
	output += alpha_value;
	output += ", beta = ";
	output += beta_value;
	output += "\n"+white_space;
	output += stackValue()+white_space;
	output+=Display(s);
	output+="\n";
	//cout<<output;
  	log(output);
  	string traversal;
  	traversal = s.operation + "," + to_string(s.depth) + ",";
  	traversal += value;
	if(strategy==3)
	{
		traversal+=",";
		traversal+=alpha_value;
		traversal+=",";
		traversal+=beta_value;
	}
  	traversal +="\n";
  	logTraversal(traversal);
}
state MaxValue(state s, int cutoff, int player, int strategy);
state MinValue(state s, int cutoff, int player, int strategy);
state repeatEvaluationForMin(state s, int cutoff, int player,int strategy)
{
	int N = s.pits;
	result branches[N];
	for(int i=0;i<N;i++)
	{
		if((player==1 && s.player1[i]!=0) || (player==2 && s.player2[i]!=0))
		{
			string operation;
			if(player==1)
			{
				operation = "B"+to_string(i+2);
			}
			else
			{
				operation = "A"+to_string(i+2);
			}
			branches[i] = Move(s,i,player,true);	
			branches[i].s.operation = operation;
			operations.push(operation);
			int output;
			state next_state;
			if(branches[i].repeat)
			{
				branches[i].s.value = numeric_limits<int>::max();
				if(branches[i].s.depth==cutoff)
				{
					branches[i].s.value = branches[i].s.mancala1-branches[i].s.mancala2;
				}
				generateLog("Max",branches[i].s, strategy);
				next_state = repeatEvaluationForMin(branches[i].s, cutoff, player, strategy);
				next_state.is_repeat = true;
				output = next_state.value;
				
			}
			else
			{
				branches[i].s.value = numeric_limits<int>::min();
				if(branches[i].s.depth==cutoff)
				{
					branches[i].s.value = branches[i].s.mancala1-branches[i].s.mancala2;
				}
		
				generateLog("Max",branches[i].s, strategy);
				next_state = MaxValue(branches[i].s, cutoff, player%2+1, strategy);
				next_state.is_repeat = false;
				output = next_state.value;	
			}
			operations.pop();
			if(output<s.beta)
			{		
				s.value = output;	
				s.next_state = next_state;
			}				
			if(s.alpha>=s.value)//prune
			{
				generateLog("Max",s, strategy);
				return s;
			}
			else if(output<s.beta)
			{
				s.beta = output;
			}
			generateLog("Max",s, strategy);
		}
	}
	return s;
}
state repeatEvaluationForMax(state s, int cutoff, int player, int strategy)
{
	int N = s.pits;
	result branches[N];
	state final_state = s;
	//s.value = numeric_limits<int>::min();
	for(int i=0;i<N;i++)
	{
		if((player==1 && s.player1[i]!=0) || (player==2 && s.player2[i]!=0))
		{
			string operation;
			if(player==1)
			{
				operation = "B"+to_string(i+2);
			}
			else
			{
				operation = "A"+to_string(i+2);
			}
			branches[i] = Move(s,i,player,true);	
			branches[i].s.operation = operation;
			operations.push(operation);
			int output;
			state next_state;
			if(branches[i].repeat)
			{
				branches[i].s.value = numeric_limits<int>::min();
				if(branches[i].s.depth==cutoff)
				{
					branches[i].s.value = branches[i].s.mancala1-branches[i].s.mancala2;
				}
				generateLog("Min",branches[i].s,strategy);
			
				next_state = repeatEvaluationForMax(branches[i].s, cutoff, player, strategy);
				next_state.repeat = true;
				output = next_state.value;
			}
			else
			{
			    branches[i].s.value = numeric_limits<int>::max();
				if(branches[i].s.depth==cutoff)
				{
					branches[i].s.value = branches[i].s.mancala1 - branches[i].s.mancala2;
				}
				generateLog("Min",branches[i].s,strategy);
				next_state = MinValue(branches[i].s, cutoff, player%2+1, strategy);
				next_state.repeat = false;
			    output = next_state.value;
			}
			operations.pop();
			if(output>s.alpha)
			{		
				s.value = output;
				s.next_state = next_state;
			}
			if(s.value>=s.beta)//prune
			{
				generateLog("Min",s, strategy);
				return s;
			}
			else if(output>s.alpha)
			{
				s.alpha = output;
			}
			s.value = s.alpha;
			generateLog("Min",s, strategy);
		}
	}
	//return s.beta;
	return s;
}

//Depth two
state MinValue(state s, int cutoff, int player, int strategy)
{
	int N = s.pits;
	result branches[N];
	state final_state = s;
	if(s.depth==cutoff)
	{
		s.value = s.mancala1-s.mancala2;
		return s;
	}
	for(int i=0;i<N;i++)
	{
		string operation;
		if(player==1)
		{
			operation = "B"+to_string(i+2);
		}
		else
		{
			operation = "A"+to_string(i+2);
		}
		branches[i] = Move(s,i,player,false);	
		branches[i].s.operation = operation;
		operations.push(operation);
		int output;
		state next_state;
		//branches[i].s.value = branches[i].s.alpha;
		if(branches[i].repeat)
		{
			branches[i].s.value = numeric_limits<int>::max();
			if(branches[i].s.depth==cutoff)
			{
				branches[i].s.value = branches[i].s.mancala1-branches[i].s.mancala2;
			}
		
			generateLog("Max",branches[i].s, strategy);
			next_state = repeatEvaluationForMin(branches[i].s, cutoff, player, strategy);
			next_state.is_repeat = true;
			output = next_state.value;
		}
		else
		{
			branches[i].s.value = numeric_limits<int>::min();
			if(branches[i].s.depth==cutoff)
			{
				branches[i].s.value = branches[i].s.mancala1-branches[i].s.mancala2;
			}
		
			generateLog("Max",branches[i].s, strategy);
			next_state = MaxValue(branches[i].s, cutoff, player%2+1, strategy);
			next_state.is_repeat = false;
			output = next_state.value;
		}
		operations.pop();
		if(output<s.beta)   // this is correct - proof 17th line of traversal
		{		
			final_state = next_state;
			s.value = output;
			s.next_state = next_state;
		}
		if(s.alpha>=s.value)//prune
		{
			generateLog("Min",s,strategy);
			return s;
		}
		if(output<s.beta)   // this is correct - proof 17th line of traversal
		{		
			s.beta = output;
		}
		generateLog("Min",s,strategy);
	}
	s.value = s.beta;
	return s;
}
//Depth one
state MaxValue(state s, int cutoff, int player,int strategy)
{
	int N = s.pits;
	result branches[N];
	string next_move;
	state final_state = s;
	if(s.depth==cutoff)
	{
		s.value = s.mancala1-s.mancala2;
		return s; 
	}
	for(int i=0;i<N;i++)
	{
		string operation;
		if(player==1)
		{
			operation = "B"+to_string(i+2);
		}
		else
		{
			operation = "A"+to_string(i+2);
		}
		
		branches[i] = Move(s,i,player,false);
		branches[i].s.operation = operation;
		operations.push(operation);
		log("Yash");
		int output;
		state next_state;
		if(branches[i].repeat)
		{
			branches[i].s.value = numeric_limits<int>::min();
			if(branches[i].s.depth==cutoff)
			{
				branches[i].s.value = branches[i].s.mancala1 - branches[i].s.mancala2;
			}
		
			generateLog("Min",branches[i].s,strategy);
			next_state = repeatEvaluationForMax(branches[i].s, cutoff, player,strategy);
			next_state.is_repeat = true;
			Display(next_state);
		}
		else
		{
			branches[i].s.value = numeric_limits<int>::max();
			if(branches[i].s.depth==cutoff)
			{
				branches[i].s.value = branches[i].s.mancala1 - branches[i].s.mancala2;
			}
			generateLog("Min",branches[i].s,strategy);
			log(to_string(s.beta));
			next_state = MinValue(branches[i].s, cutoff, player%2+1,strategy);
			next_state.is_repeat = false;
		}
		output = next_state.value;
		operations.pop();
		
		if(output>s.alpha)   // this is correct - proof 17th line of traversal
		{		
			s.value = output;
			next_move = operation;
			s.next_state = next_state;
		}
		if(s.beta<=s.value)//prune
		{
			generateLog("Max",s, strategy);
			return s;
		}
		else if(output>s.alpha)   // this is correct - proof 17th line of traversal
		{		
			s.alpha = output;
		}
		generateLog("Max",s,strategy);
	}
	s.value = s.alpha;
	state final_next_state = s.next_state;
	while(final_state.is_repeat)
	{
		final_state = final_state.next_state; 
	}
	log_next_move(final_state);
	return final_state;
}
int main (int argc, char* argv[])
{
  	program p = init(argv[2]);
  	/*if(p.strategy==1)
	{
		if(p.player==1)
		{
			p.initial_state.player2 = reverseB(p.initial_state.player2, p.initial_state.pits);
			result r = Greedy(p.initial_state,p.player);
			r.s.player2 = reverseB(r.s.player2, r.s.pits);
			Display(r.s);
		}
		else if (p.player==2)
		{
			p.initial_state.player2 = reverseB(p.initial_state.player2, p.initial_state.pits);
			vector<int> temp = p.initial_state.player1;
			p.initial_state.player1 = p.initial_state.player2;
			p.initial_state.player2 = temp;
			int t = p.initial_state.mancala1;
			p.initial_state.mancala1 = p.initial_state.mancala2;
			p.initial_state.mancala2 = t;
			
			result r = Greedy(p.initial_state,p.player);
			temp = r.s.player1;
			r.s.player1 = r.s.player2;
			r.s.player2 = temp;
			t = r.s.mancala1;
			r.s.mancala1 = r.s.mancala2;
			r.s.mancala2 = t;
			r.s.player2 = reverseB(r.s.player2, r.s.pits);
			Display(r.s);
		}
	}*/
	if(p.strategy==1)
	{
		p.initial_state.depth = 0;
		p.initial_state.alpha = numeric_limits<int>::min();
		p.initial_state.beta = numeric_limits<int>::max();
		p.initial_state.value = numeric_limits<int>::min();
		p.initial_state.operation = "root";
		generateLog("Max",p.initial_state,p.strategy);
		MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy);
	}
	else if(p.strategy==2)
	{
		p.initial_state.depth = 0;
		p.initial_state.alpha = numeric_limits<int>::min();
		p.initial_state.beta = numeric_limits<int>::max();
		p.initial_state.value = numeric_limits<int>::min();
		p.initial_state.operation = "root";
		generateLog("Max",p.initial_state,p.strategy);
		MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy);
	}
	else if(p.strategy==3)
	{
		p.initial_state.depth = 0;
		p.initial_state.alpha = numeric_limits<int>::min();
		p.initial_state.beta = numeric_limits<int>::max();
		p.initial_state.value = numeric_limits<int>::min();
		p.initial_state.operation = "root";
		generateLog("Max",p.initial_state,p.strategy);
		MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy);
	}
	/*else if(TestCase==3)
	{
		//AlphaBeta(p.initial_state,p.cutting_depth,p.player);
	}
	else if(TestCase==4)
	{
		if(Player==1)
		{
			//Competition(p.initial_state,p.cutting_depth,p.player);
		}
		else
		{
			//Competition(p.initial_state,p.cutting_depth,p.player);
		}
	}*/		
  return 0;
}
