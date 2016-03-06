struct state
{
	vector<int> player1;
	vector<int> player2;
	int mancala1;
	int mancala2; 
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
	bool repeat;
};

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
		
		myInfile.close();
  	}

  	else cout << "Unable to open file"; 
  	return p;
}
