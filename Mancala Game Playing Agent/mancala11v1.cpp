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
class state
{
	public:
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
		bool is_end;
	state* copy()
	{
		
		state* new_state = new state();
		new_state->mancala1 = this->mancala1;
		new_state->mancala2 = this->mancala2;
		new_state->pits = this->pits;
		new_state->depth = this->depth;
		new_state->alpha = this->alpha;
		new_state->beta = this->beta;
		new_state->value = this->value;
		new_state->next_state = this->next_state;
		new_state->operation = this->operation;
		new_state->is_repeat = this->is_repeat;
		new_state->player1 = this->player1;
		new_state->player2 = this->player2;
		return new_state;
	}
};
struct program
{
	state* initial_state;
	int strategy;
	int cutting_depth;
	int player;
};
struct result
{
	state* s;
	bool explored;
	bool repeat;
};
void log(string s, ofstream &myOutfile)
{
	myOutfile << s;
}
void logTraversal(string s, ofstream &myOutfile)
{
	myOutfile << s;
}
void log_next_move(state* s)
{
	ofstream myOutfile;
	string str;
	for(int i=0;i<s->pits;i++)
	{
		str+=to_string(s->player2[i]);
		str+=" ";
	}
	str+="\n";
	for(int i=0;i<s->pits;i++)
	{
		str+=to_string(s->player1[i]);
		str+=" ";
	}
	str+="\n";
	str+=to_string(s->mancala2);
	str+="\n";
	str+=to_string(s->mancala1);
	myOutfile.open("next_state.txt");
	myOutfile << str;
	myOutfile.close();
}
void initialize_logs(int strategy)
{
	ofstream myOutfile;
	myOutfile.open("output.txt");
	myOutfile << "";
	myOutfile.close();
	if(strategy!=1)
	{
	myOutfile.open("traverse_log.txt");
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
}

bool check_goal(state *st,int p)
{
	bool isFinalState = false;
	int n = st->pits;
	bool is_player1_empty = true;
	for(int i=0;i<n;i++)
	{
		if(st->player1[i]!=0)
		{
			
			is_player1_empty = false;
			break;
		}
	}
	if(is_player1_empty)
	{
		for(int i=0;i<n;i++)
		{
			st->mancala2 += st->player2[i];
			st->player2[i] = 0;
		}
		st->value = (st->mancala1-st->mancala2)*(3-(2*p));
		st->is_end = true;
		st->next_state = NULL;
		return true;
	}
	bool is_player2_empty = true;
	for(int i=0;i<n;i++)
	{
		if(st->player2[i]!=0)
		{
			is_player2_empty = false;
			break;
		}
	}
	if(is_player2_empty)
	{
		for(int i=0;i<n;i++)
		{
			st->mancala1 += st->player1[i];
			st->player1[i] = 0;
		}
		st->value = (st->mancala1-st->mancala2)*(3-(2*p));
		st->is_end = true;
		st->next_state = NULL;
		return true;
	}	
	return false;
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
		state* initial_state = new state();
		vector<int> b{istream_iterator<int>{iss},
    	                istream_iterator<int>{}}; 
    	initial_state->player2 = b;
		
		getline(myInfile,line);
		istringstream iss1(line);
		vector<int>a{istream_iterator<int>{iss1},
    	                istream_iterator<int>{}};
    	initial_state->player1 = a;
		//p.initial_state.player1 = a;
			
		getline(myInfile,line);
		initial_state->mancala2 = stoi(line);
		
		getline(myInfile,line);
		initial_state->mancala1 = stoi(line);
		initial_state->is_end = false;
		check_goal(initial_state,p.player);
		int N=0;
		for(vector<int>::iterator i = initial_state->player1.begin();i!=initial_state->player1.end();i++)
		{
			N++;
		}
		initial_state->pits = N;
		if(p.strategy==1)
		{
			p.cutting_depth = 1;
		}
		p.initial_state = initial_state;
		initialize_logs(p.strategy);	
		myInfile.close();
	}
    else cout << "Unable to open file"; 
  	return p;
}

string Display(state* s)
{
	int N = s->player2.size();
	string white_space;
	for(int i=0;i<s->depth*5;i++)
	{
		 white_space += " ";
	}
	
	string output = "--- Player 2 ---";
	output+="\n"+white_space;
	output += "  ";
	for(int i=0;i<N;i++)
	{
		output+=to_string(s->player2[i])+" ";
	}
	output+="\n"+white_space;
	output+=to_string(s->mancala2);
	for(int i=0;i<N;i++)
	{
		output+="  ";
	}
	output+=" ";
	output+=to_string(s->mancala1);
	output+="\n"+white_space;
	output+="  ";
	for(int i=0;i<N;i++)
	{
		output+=to_string(s->player1[i])+" ";
	}
	output+="\n"+white_space;
	output+="--- Player 1 ---";
	output+="\n"+white_space;
	return output;
}
result Move(state* st, int index, int player, bool isRepeat, ofstream &myOutfile, int p)
{
	int n = st->pits;
		
	state* new_state = st->copy();
	
	if(!isRepeat)
	{
		new_state->depth = new_state->depth+1;
	}
	result r;
	r.repeat = false;
	if(player==1)
	{
		int stones = new_state->player1[index];
		new_state->player1[index] = 0;
		int i = index+1;
		while(stones)
		{
			for(;i<n&&stones;i++)
			{
				//Check for bonus case
				if(new_state->player1[i]==0 && stones==1)	
				{
					new_state->mancala1 = new_state->mancala1 + new_state->player2[i] + stones;
					new_state->player2[i] = 0;
				}
				else
				{
					new_state->player1[i] = new_state->player1[i]+1;
				}
				stones--;
			}
			if(stones)
			{
				new_state->mancala1++;
				stones--;
				if(!stones)
				{
					r.repeat = true;
				}
			}
			for(int j=n-1;j>=0&&stones;j--)
			{
				new_state->player2[j]= new_state->player2[j]+1;
				stones--;
			}
			i=0;
		}
	}
	else if(player==2)
	{
		int stones = new_state->player2[index];
		new_state->player2[index] = 0;
		int i = index-1;
		while(stones)
		{
			for(;i>=0&&stones;i--)
			{
				//Check for bonus case
				if(new_state->player2[i]==0 && stones==1)	
				{
					new_state->mancala2 = new_state->mancala2 + new_state->player1[i] + stones;
					new_state->player1[i] = 0;
				}
				else
				{
					new_state->player2[i] = new_state->player2[i]+1;
				}
				stones--;
			}
			if(stones)
			{
				new_state->mancala2++;
				stones--;
				if(!stones)
				{
					r.repeat = true;
				}
			}
			for(int j=0;j<n&&stones;j++)
			{
				new_state->player1[j] = new_state->player1[j]+1;
				stones--;
			}
			i = n-1;
		}
	}
	check_goal(new_state,p);
	//check_goal(new_state);
	r.s = new_state;
	return r;
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

void generateLog(string op,state* s, int strategy, ofstream &myOutfile, ofstream &traversalOutfile)
{
	string white_space;
	for(int i=0;i<s->depth*5;i++)
	{
		 white_space += " ";
	}
	string value;
	if(s->value==numeric_limits<int>::min())
	{
		value = "-Infinity";	
	}
	else if(s->value==numeric_limits<int>::max())
	{
		value = "Infinity";
	}
	else
	{
		value = to_string(s->value);
	}
	string alpha_value;
	string beta_value;
	if(strategy==3 || strategy ==2)
	{
		if(s->alpha==numeric_limits<int>::min())
		{
			alpha_value = "-Infinity";	
		}
		else if(s->alpha==numeric_limits<int>::max())
		{
			alpha_value = "Infinity";
		}
		else
		{
			alpha_value = to_string(s->alpha);
		}
		if(s->beta==numeric_limits<int>::min())
		{
			beta_value = "-Infinity";	
		}
		else if(s->beta==numeric_limits<int>::max())
		{
			beta_value = "Infinity";
		}
		else
		{
			beta_value = to_string(s->beta);
		}
	}
	
	string output;
	output = white_space+op + ":" + s->operation + "," + to_string(s->depth) + ",";
	output+=" value = ";
	output += value;
	output += ", alpha = ";
	output += alpha_value;
	output += ", beta = ";
	output += beta_value;
	output += "\n"+white_space;
	output += stackValue()+white_space;
	output+=Display(s);
	output+="\n";
	log(output, myOutfile);
  	string traversal;
  	traversal = s->operation + "," + to_string(s->depth) + ",";
  	traversal += value;
	if(strategy==3)
	{
		traversal+=",";
		traversal+=alpha_value;
		traversal+=",";
		traversal+=beta_value;
	}
  	traversal +="\n";
  	if(strategy!=1)
  	{
  		logTraversal(traversal, traversalOutfile);
  	}
}
state* MaxValue(state* s, int cutoff, int player, int strategy, int p, bool is_repeat, ofstream &myOutfile, ofstream &traversalOutfile);
state* MinValue(state* s, int cutoff, int player, int strategy, int p, bool is_repeat, ofstream &myOutfile, ofstream &traversalOutfile);

//Depth two
state* MinValue(state* s, int cutoff, int player, int strategy, int p, bool is_repeat, ofstream &myOutfile, ofstream &traversalOutfile)
{
	int N = s->pits;
	result branches[N];
	if(s->depth==cutoff && !is_repeat)
	{
		s->value = (s->mancala1-s->mancala2)*(3-(2*p));
		return s;
	}
	for(int i=0;i<N;i++)
	{
		if((player==1 && s->player1[i]!=0) || (player==2 && s->player2[i]!=0))
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
			branches[i] = Move(s,i,player,is_repeat,myOutfile, p);	
			branches[i].s->operation = operation;
			operations.push(operation);
			int output;
			state* next_state;
			if(branches[i].s->is_end)
			{
				if(branches[i].repeat)
					generateLog("Min",branches[i].s,strategy,myOutfile,traversalOutfile);
				else
					generateLog("Max",branches[i].s,strategy,myOutfile,traversalOutfile);
				output = branches[i].s->value;
				next_state = branches[i].s;
			}
			else if(branches[i].repeat)
			{
				branches[i].s->value = numeric_limits<int>::max();
				generateLog("Min",branches[i].s, strategy, myOutfile, traversalOutfile);
				next_state = MinValue(branches[i].s, cutoff, player, strategy,p, true, myOutfile, traversalOutfile);
				next_state->is_repeat = true;
				output = next_state->value;
			}
			else
			{
				branches[i].s->value = numeric_limits<int>::min();
				if(branches[i].s->depth==cutoff)
				{
					branches[i].s->value = (branches[i].s->mancala1-branches[i].s->mancala2)*(3-(2*p));
				}
			
				generateLog("Max",branches[i].s, strategy, myOutfile, traversalOutfile);
				next_state = MaxValue(branches[i].s, cutoff, player%2+1, strategy,p, false, myOutfile, traversalOutfile);
				next_state->is_repeat = false;
				output = next_state->value;
			}
			operations.pop();
			if(output<s->value)   // this is correct - proof 17th line of traversal
			{		
				s->value = output;
				s->next_state = next_state;
				if(s->alpha>=s->value && strategy==3)//prune
				{
					generateLog("Min",s,strategy, myOutfile, traversalOutfile);
					return s;
				}
			
				s->beta = output;
			}
			generateLog("Min",s,strategy, myOutfile, traversalOutfile);
		}
	}
	return s;
}
//Depth one
state* MaxValue(state *s, int cutoff, int player,int strategy, int p, bool is_repeat, ofstream &myOutfile, ofstream &traversalOutfile)
{
	int N = s->pits;
	result branches[N];
	string next_move;
	
	if(s->depth==cutoff && !is_repeat)
	{
		s->value = (s->mancala1-s->mancala2)*(3-(2*p));
		return s; 
	}
	
	for(int i=0;i<N;i++)
	{
		if((player==1 && s->player1[i]!=0) || (player==2 && s->player2[i]!=0))
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
			
			branches[i] = Move(s,i,player,is_repeat,myOutfile,p);
			branches[i].s->operation = operation;
			operations.push(operation);
			int output;
			state* next_state;
			if(branches[i].s->is_end)
			{
				if(branches[i].repeat)
					generateLog("Max",branches[i].s,strategy,myOutfile,traversalOutfile);
				else
					generateLog("Min",branches[i].s,strategy,myOutfile,traversalOutfile);
				output = branches[i].s->value;
				next_state = branches[i].s;
			}
			else if(branches[i].repeat)
			{
				branches[i].s->value = numeric_limits<int>::min();
				generateLog("Max",branches[i].s,strategy,myOutfile,traversalOutfile);
				next_state = MaxValue(branches[i].s, cutoff, player,strategy, p, true,myOutfile,traversalOutfile);
				output = next_state->value;
				next_state->is_repeat = true;
			}
			else
			{
				branches[i].s->value = numeric_limits<int>::max();
				if(branches[i].s->depth==cutoff)
				{
					branches[i].s->value = (branches[i].s->mancala1 - branches[i].s->mancala2)*(3-(2*p));
				}
				generateLog("Min",branches[i].s,strategy,myOutfile,traversalOutfile);
				next_state = MinValue(branches[i].s, cutoff, player%2+1,strategy,p, false,myOutfile,traversalOutfile);
				output = next_state->value;
				next_state->is_repeat = false;
			}
			operations.pop();
			if(output>s->value)   // this is correct - proof 17th line of traversal
			{		
				s->value = output;
				next_move = operation;
				s->next_state = next_state;
				if(s->beta<=s->value && strategy==3)//prune
				{
					generateLog("Max",s, strategy,myOutfile,traversalOutfile);
					return s;
				}
			
				s->alpha = output;
			}
			generateLog("Max",s,strategy,myOutfile,traversalOutfile);
		}
	}
	return s;
}
int main (int argc, char* argv[])
{
  	program p = init(argv[2]);
  	state* final_next_state;
  	p.initial_state->depth = 0;
	p.initial_state->alpha = numeric_limits<int>::min();
	p.initial_state->beta = numeric_limits<int>::max();
	p.initial_state->value = numeric_limits<int>::min();
	p.initial_state->operation = "root";
	ofstream myOutfile;
	myOutfile.open("output.txt", std::ios_base::app);
	ofstream traversalOutfile;
	traversalOutfile.open("traverse_log.txt", std::ios_base::app);
	generateLog("Max",p.initial_state,p.strategy, myOutfile,traversalOutfile);
	if(p.strategy == 4)
	{
		p.strategy = 3;
	}
	if(p.strategy==1)
	{
		final_next_state = MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy,p.player, false, myOutfile,traversalOutfile);
	}
	else if(p.strategy==2)
	{
		final_next_state = MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy,p.player, false, myOutfile,traversalOutfile);
	}
	else if(p.strategy==3)
	{
		final_next_state = MaxValue(p.initial_state,p.cutting_depth,p.player,p.strategy,p.player, false, myOutfile,traversalOutfile);
	}
	final_next_state = final_next_state->next_state;
	while(final_next_state->is_repeat && final_next_state->next_state!=NULL)
	{
		final_next_state = final_next_state->next_state; 
	}
	log_next_move(final_next_state);
	myOutfile.close();
	traversalOutfile.close();
  return 0;
}
