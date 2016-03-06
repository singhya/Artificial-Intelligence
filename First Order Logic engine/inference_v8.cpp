#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <limits>
#include <stack>
#include <map>
#include <queue>
using namespace std;
const bool Logging = true;		
void log(string s, ofstream &myOutfile)
{
	myOutfile << s;
}
void log_output(string s, ofstream &myOutfile)
{
	myOutfile << s;
}
void initialize_logs()
{
	ofstream myLogfile;
	ofstream myOutfile;
	myLogfile.open("log.txt");
	myLogfile << "";
	myLogfile.close();
	myOutfile.open("output.txt");
	myOutfile << "";
	myOutfile.close();
}
class Predicate
{
	public:
	string name;
	vector<string> attributes;
	int attribute_count;
	bool isNegated;	
	void displayFact(string s, ofstream &myOutfile)
	{
		string output = "";
		output += s + "Name : ";
		output += this->name;
		output += " | ";
		output += "Attribute count : ";
		output += to_string(this->attribute_count);
		output += " | ";
		output += "Is negated : ";
		output += to_string(this->isNegated);
		output += " | ";
		for(int i=0;i<this->attribute_count;i++)
		{
			output += this->attributes[i];
			output += " ";
		}
		log(output, myOutfile);
	}
};
struct Map
{
	string right_predicate_name;
	int right_predicate_index;
	string left_predicate_name;
	int left_predicate_index;
};
class Rule
{
	public:
	string rule_str;
	Predicate right;
	vector<Predicate> left;
	int left_predicate_count;
	//vector<Map> mapping;
	map<string, string> substitution;
	//int mapping_count;
	void displayRule(string whitespace, ofstream &myOutfile)
	{
		string output = "";
		output += whitespace+"Right predicate : ";
		output += "\n";
		log(output,myOutfile);
		output = "";
		this->right.displayFact(whitespace,myOutfile);
		output += "\n";
		output += whitespace+"Left Predicates : ";
		output += "\n";
		log(output,myOutfile);
		output = "";
		for(vector<Predicate>::iterator it = this->left.begin();it != this->left.end();it++)
		{
			(*it).displayFact(whitespace,myOutfile);
			log("\n",myOutfile);
		}
		output = "";
		output += whitespace+"Mapping : ";
		output += "\n";
		//Logging substitution list
		for(map<string, string>::iterator i = this->substitution.begin();i !=this->substitution.end(); i++)
		{
			output += whitespace;
			output += i->first;
			output += ":";
			output += i->second;
			output += "\n";	
		}
		log(output, myOutfile);
	}	
	void substitute()
	{
		for(int i = 0;i<this->right.attribute_count;i++)
		{
			if((this->right.attributes[i][0]>='a' && this->right.attributes[i][0]<='z')  
				&& (this->substitution.find(this->right.attributes[i])!=this->substitution.end()) 
				&& this->substitution[this->right.attributes[i]].compare("")!=0)
			{
				this->right.attributes[i] = this->substitution[this->right.attributes[i]];
			}	
		}
		//Todo: check if original rule is modified or not
		for(vector<Predicate>::iterator it = this->left.begin();it!=this->left.end();it++)
		{
			for(int i = 0;i<(*it).attribute_count;i++)
			{
				if((*it).attributes[i][0]>='a' && (*it).attributes[i][0]<='z' 
				&& (this->substitution.find((*it).attributes[i])!=this->substitution.end()) 
				&& this->substitution[(*it).attributes[i]].compare("")!=0)
				{
					(*it).attributes[i] = this->substitution[(*it).attributes[i]];
				}	
			}	
		}	
	}	
};
struct PredicateMatchResult
{
	vector<Predicate> preds;
	bool isFound;
};
class Program
{
	public:
	static map<string, vector<Predicate>> facts;
	static map<string, vector<Rule>> rules;
	static bool isPredicateSame(Predicate p1, Predicate p2)
	{
		if(p1.name.compare(p2.name)!=0)
			return false;
		if(p1.isNegated!=p2.isNegated)
			return false;
		for(int i=0;i<p1.attribute_count;i++)
			if((p2.attributes[i][0]>='A' && p2.attributes[i][0]<='Z') && p1.attributes[i].compare(p2.attributes[i])!=0)
				return false;
		return true;
	}
	static bool isFactSame(Predicate p1, Predicate p2)
	{
		if(p1.name.compare(p2.name)!=0)
			return false;
		if(p1.isNegated!=p2.isNegated)
			return false;
		for(int i=0;i<p1.attribute_count;i++)
			if((p2.attributes[i][0]>='A' && p2.attributes[i][0]<='Z') && 
			(p2.attributes[i][0]>='A' && p2.attributes[i][0]<='Z') && p1.attributes[i].compare(p2.attributes[i])!=0)
				return false;
		return true;
	}
	static bool checkForVisitedPredicate(Predicate already_visited, Predicate to_check)
	{
		if(already_visited.name.compare(to_check.name)!=0)
			return false;
		if(already_visited.isNegated!=to_check.isNegated)
			return false;
		for(int i=0;i<already_visited.attribute_count;i++)
			if((already_visited.attributes[i][0]>='A' && already_visited.attributes[i][0]<='Z') &&
			(to_check.attributes[i][0]>='A' && to_check.attributes[i][0]<='Z') && 
			already_visited.attributes[i].compare(to_check.attributes[i])!=0)
				return false;
			else if((already_visited.attributes[i][0]>='A' && already_visited.attributes[i][0]<='Z') &&
			(to_check.attributes[i][0]>='a' && to_check.attributes[i][0]<='z'))
				return false;
			else if((already_visited.attributes[i][0]>='a' && already_visited.attributes[i][0]<='z') &&
			(to_check.attributes[i][0]>='A' && to_check.attributes[i][0]<='Z'))
				return false;
		return true;
	} 
	static PredicateMatchResult checkFacts(Predicate pred)
	{
		PredicateMatchResult p;
		bool predFound = false;
		if(facts.find(pred.name)!=facts.end())
		{
			for(vector<Predicate>::iterator i = facts[pred.name].begin(); i!=facts[pred.name].end();i++)
			{
				if(isPredicateSame((*i),pred))
				{
					p.preds.push_back(*i);
					predFound = true;
				}	
			}
		}
		p.isFound = predFound;
		return p;
	}
	static Predicate createPredicate(string str, bool isFact)
	{
		Predicate pred;
			int index_of_starting_argument = str.find('(')+1;
			int index_of_right_paranthesis = str.find(')');
			string args_str = str.substr(index_of_starting_argument,index_of_right_paranthesis-index_of_starting_argument);
			istringstream ss(args_str);
			string token;
			pred.attribute_count = 0;
			while(getline(ss, token, ',')) {
	    		pred.attributes.push_back(token);
	    		pred.attribute_count++;
			}
			if(str[0]=='~')
			{
				pred.isNegated = true;
				pred.name = str.substr(1,index_of_starting_argument-2);
			}
			else
			{
				pred.isNegated = false;
				pred.name = str.substr(0,index_of_starting_argument-1);
			}
		return pred;
	}
	static bool isFactAlreadyExisting(Predicate pred)
	{
		if(Program::facts.find(pred.name) == Program::facts.end())
		{
			return false;
		}
		else
		{
			for(vector<Predicate>::iterator i = Program::facts[pred.name].begin(); i!=Program::facts[pred.name].end();i++)
			{
				if(Program::isFactSame((*i),pred))
					return true;
			}
		}
		return false;
	}
	static void insertFact(string str)
	{
		Predicate new_pred = createPredicate(str,true);
		if(!isFactAlreadyExisting(new_pred))
		{
			if(Program::facts.find(new_pred.name) == Program::facts.end())
			{
				vector<Predicate> predicates;
				predicates.push_back(new_pred);
				Program::facts[new_pred.name] = predicates;
			}
			else
			{
				Program::facts[new_pred.name].push_back(new_pred);
			}
		}
	}
	static bool isRuleAlreadyExisting(Rule rule)
	{
		for(vector<Rule>::iterator r = Program::rules[rule.right.name].begin();r!=Program::rules[rule.right.name].end();r++)
		{
			if(rule.rule_str.compare((*r).rule_str)==0)
				return true;
		}
		return false;
	}
	static void insertRule(string str, ofstream &myLogfile, bool isLogging)
	{
		string output = "---------------------";
		output += "\n";
		output += "Rule : ";
		output += str + "\n";
		Predicate right_pred;
		vector<Predicate> left_preds;
		Rule rule;
		rule.rule_str = str;
		rule.left_predicate_count = 0;
		output += "---------------------";
		output += "\n";
		string left_pred_str = str.substr(0,str.find("=>"));
		
		output += "Tokenized Left Hand side : ";
		string delimiter = "^ ";
		size_t pos = 0;
		string token;
		Predicate left_pred;
		while ((pos = left_pred_str.find(delimiter)) != string::npos) {
			token = left_pred_str.substr(0, pos);
    		output += token;
			output += " ";
	    	left_pred = createPredicate(token,false);
	    	left_preds.push_back(left_pred);
	    	rule.left_predicate_count++;
	    	//Inserting keys in substitution list if not present
	    	for(vector<string>::iterator iter = left_pred.attributes.begin(); iter!=left_pred.attributes.end(); iter++ )
			{	
				if(((*iter)[0]>='a' && (*iter)[0]<='z') && rule.substitution.find((*iter))==rule.substitution.end())
				{
					rule.substitution[(*iter)] = "";
				}
			}
    		left_pred_str.erase(0, pos + delimiter.length());
		}
		token = left_pred_str.substr(0, pos);
    	output += token;
		output += " ";
	    left_pred = createPredicate(token,false);
	    left_preds.push_back(left_pred);
	    rule.left_predicate_count++;
	    //Inserting keys in substitution list if not present
	    for(vector<string>::iterator iter = left_pred.attributes.begin(); iter!=left_pred.attributes.end(); iter++ )
		{	
			if(((*iter)[0]>='a' && (*iter)[0]<='z') && rule.substitution.find((*iter))==rule.substitution.end())
			{
				rule.substitution[(*iter)] = "";
			}
		}
    	left_pred_str.erase(0, pos + delimiter.length());
		
		output += "\n";
		if(isLogging)
			log(output,myLogfile);
		output = "";
		rule.left = left_preds;	
		
		
		output = "Tokenized right predicate : ";
		output += "\n";
		if(isLogging)
			log(output,myLogfile); 
		output = "";
		string right_str = str.substr(str.find("=>")+3);
		right_pred = createPredicate(right_str,false);
		rule.right = right_pred;
		for(vector<string>::iterator iter = right_pred.attributes.begin(); iter!=right_pred.attributes.end(); iter++ )
		{
			if(((*iter)[0]>='a' && (*iter)[0]<='z') && rule.substitution.find((*iter))==rule.substitution.end())
			{
				rule.substitution[(*iter)] = "";
			}
		}
		//create mapping
		{
			for(vector<Predicate>::iterator i = rule.left.begin();i!=rule.left.end();i++)
			{
				for(vector<string>::iterator it = (*i).attributes.begin();it!=(*i).attributes.end();it++)
				{
					for(vector<string>::iterator ite = rule.right.attributes.begin(); ite !=rule.right.attributes.end();ite++)
					{
						if((*it)==(*ite))
						{
							Map m;
							m.left_predicate_index = distance((*i).attributes.begin(), it);
							m.left_predicate_name = (*i).name;
							m.right_predicate_index = distance(rule.right.attributes.begin(), ite);
							m.right_predicate_name = rule.right.name;
						}
					}
				}
			}
		}
		//Add condition for already existing rule
		if(!isRuleAlreadyExisting(rule))
		{
			if(Program::rules.find(rule.right.name) == Program::rules.end())
			{
				vector<Rule> rules_list;
				rules_list.push_back(rule);
				Program::rules[rule.right.name] = rules_list;
			}
			else
			{
				Program::rules[rule.right.name].push_back(rule);
			}
			if(isLogging)
			{
				log(output, myLogfile);
				rule.displayRule("",myLogfile);
			}
		}
	}
	static void displayFacts(ofstream &myOutfile)
	{
		log("Facts : ",myOutfile);
		log("\n",myOutfile);
		for(map<string, vector<Predicate>>::iterator it = Program::facts.begin(); it!=Program::facts.end(); it++)
		{
			for(vector<Predicate>::iterator i = it->second.begin(); i!=it->second.end(); i++)
			{
				(*i).displayFact("",myOutfile);
				log("\n",myOutfile);
			}
		}
	}
	static void displayRules(ofstream &myOutfile)
	{
		log("Rules : ",myOutfile);
		log("\n",myOutfile);
		int count = 1;
		for(map<string, vector<Rule>>::iterator it = Program::rules.begin(); it!=Program::rules.end(); it++)
		{
			for(vector<Rule>::iterator r = it->second.begin(); r!=it->second.end(); r++)
			{
				log("Rule " + to_string(count++) + " : " +"\n",myOutfile);
				(*r).displayRule("",myOutfile);
				log("\n",myOutfile);
			}
		}
	}
	static map<string, Predicate> initializePredicate()
	{
		Predicate p;
		p.name = "initial";
		p.attributes;
		p.attribute_count = 0;
		map<string, Predicate> predicates;
		predicates["initial"] = p;
		return predicates;
	} 
	static map<string, vector<Predicate>> initializeFacts()
	{
		Predicate p;
		vector<Predicate> f;
		p.name = "initial";
		p.attributes;
		p.attribute_count = 0;
		f.push_back(p);
		map<string, vector<Predicate>> facts;
		facts["initial"] = f;
		return facts;
	} 
	static map<string, vector<Rule>> initializeRules()
	{
		Predicate dummy;
		dummy.name = "initial";
		dummy.attribute_count = 0;
		vector<Rule> rules_list;
		Rule r;
		r.right = dummy;
		r.left_predicate_count = 0;
		rules_list.push_back(r);
		map<string, vector<Rule>> rules;
		rules["initial"] = rules_list;
		return rules;
	} 
};
map<string, vector<Predicate>> Program::facts = Program::initializeFacts();
map<string, vector<Rule>> Program::rules = Program::initializeRules();
void parsePredicate(string line, ofstream &myLogfile, bool isLogging)
{
	if (line.find("=>")==-1)
	{
		Program::insertFact(line);
    }
    else
	{
		Program::insertRule(line, myLogfile, isLogging);
    }
}
Predicate assign(Predicate p1, Predicate p2, vector<Map> m,ofstream &myLogfile)
{
	Predicate assigned = p1;
	for(vector<Map>::iterator i = m.begin();i!=m.end();i++)
	{
		if((*i).right_predicate_name.compare(p2.name)==0 && (*i).left_predicate_name.compare(p1.name)==0)
		{
			assigned.attributes[(*i).left_predicate_index] = p2.attributes[(*i).right_predicate_index];
		}
	}
	return assigned;
}
bool isAlreadyVisited(Predicate pred, map<string, vector<Predicate>> visitedPredicate)
{
	if(visitedPredicate.find(pred.name)==visitedPredicate.end())
		return false;
	else
	{
		for(vector<Predicate>::iterator i = visitedPredicate[pred.name].begin(); i!= visitedPredicate[pred.name].end(); i++)
		{
			if(Program::checkForVisitedPredicate((*i),pred))
				return true;
		}
	}
	return false;
}
struct UnifyStruct
{
	Rule r;
	bool can_be_unified;
};
UnifyStruct unify(Rule r, Predicate pred, ofstream &myLogfile, bool isRight,int left_pred_index, string whitespace, bool is_logging)
{
	if(is_logging)
	{
		log("\n",myLogfile);
		pred.displayFact(whitespace,myLogfile);
		log("\n",myLogfile);
	}
	Rule rule = r;
	Predicate pred_to_be_unified;
	UnifyStruct result;
	result.can_be_unified = true;
	//Right hand predicate is unified
	if(isRight)
	{
		pred_to_be_unified = r.right;
	}
	else
	{
		for(int i = left_pred_index;i<r.left_predicate_count;i++)
		{
			if(r.left[i].name.compare(pred.name)==0)
			{
				pred_to_be_unified = r.left[i];
				break;
			}
		}
	}
	//Unify next recurrent predicate 
	for(int i=0;i<pred_to_be_unified.attribute_count;i++)
	{
		if((pred_to_be_unified.attributes[i][0]>='a'&& pred_to_be_unified.attributes[i][0]<='z') 
		//&& pred.attributes[i][0]>='A' && pred.attributes[i][0]<='Z'
		)
		{
			if(rule.substitution[pred_to_be_unified.attributes[i]].compare("")==0 ||
			 (rule.substitution[pred_to_be_unified.attributes[i]][0]>='a' && rule.substitution[pred_to_be_unified.attributes[i]][0]<='z') ||
			 rule.substitution[pred_to_be_unified.attributes[i]].compare(pred.attributes[i])==0)
			{
				rule.substitution[pred_to_be_unified.attributes[i]] = pred.attributes[i];
				rule.substitute();
			}
			else
				result.can_be_unified=false; 
			
		}
	}
	rule.substitute();
	result.r = rule;
	return result;	
}
struct QueueNode
{
	Rule r;
	int level;
};
void display(queue<QueueNode> q, string whitespace, ofstream &myLogfile)
{
	string output = whitespace;
	while(!q.empty())
	{
		QueueNode node = q.front();
		q.pop();
		output += node.r.right.name + " - ";
		for(map<string, string>::iterator i = node.r.substitution.begin();i !=node.r.substitution.end(); i++)
		{
			output += i->first;
			output += ":";
			output += i->second;
			output += " , ";	
		}
		output += " | ";
	}
	log(output,myLogfile);
}
bool checkRHS(Predicate RHS, Predicate passed_value)
{
	if(RHS.name.compare(passed_value.name)!=0)
		return false;
	for(int i=0;i<RHS.attribute_count;i++)
	{
		if(RHS.attributes[i][0]>='A' && RHS.attributes[i][0]<='Z' && 
		passed_value.attributes[i][0]>='A' && passed_value.attributes[i][0]<='Z' &&
		RHS.attributes[i].compare(passed_value.attributes[i])!=0)
			return false;
	}
	return true;
}
void logVisitedLog(string whitespace,map<string, vector<Predicate>> &visitedPredicate, ofstream &myLogfile)
{
	log(whitespace+"\n||||||||||||||||| Visited nodes |||||||||||||||||",myLogfile);
	for(map<string, vector<Predicate>>::iterator i= visitedPredicate.begin();i!=visitedPredicate.end();i++)
	{
		for(vector<Predicate>::iterator it = i->second.begin();it!=i->second.end();it++)
		{
			(*it).displayFact("\n"+whitespace, myLogfile);
		}
	}
	log(whitespace+"\n||||||||||||||||| Visited nodes |||||||||||||||||\n",myLogfile);
}
PredicateMatchResult backTrack(Predicate pred, ofstream &myLogfile,int level, map<string, vector<Predicate>> &visitedPredicate,bool isLogging)
{
	/*if(level>2)
	{
		PredicateMatchResult p;
		vector<Predicate> preds;
		preds.push_back(pred);
		p.preds = preds;
		p.isFound = false;
		return p;
	}*/	
	map<string, vector<Predicate>> temp_visited_nodes = visitedPredicate;
	string whitespace = "";
	if(isLogging)
	{
		for(int i=0;i<level;i++)
			whitespace+="\t";
		pred.displayFact(whitespace, myLogfile);
		log("\n",myLogfile);
	}
	PredicateMatchResult matched_fact = Program::checkFacts(pred);
	
	if(matched_fact.isFound)
	{
		if(isLogging)
		{
			log(whitespace+"-------------------------\n",myLogfile);
			for(vector<Predicate>::iterator it = matched_fact.preds.begin(); it!=matched_fact.preds.end(); it++)
			{		
				log(whitespace+"Found\n",myLogfile);
				(*it).displayFact(whitespace,myLogfile);
				log("\n",myLogfile);
			}
			log(whitespace+"-------------------------\n",myLogfile);
		}
		//To do : do not return check for substitutions as well
		//return matched_fact;
	}
	
	if(isAlreadyVisited(pred, visitedPredicate))
	{
		if(isLogging)
		{
			//pred.displayFact(whitespace, myLogfile);
			log("\nAlready visited\n",myLogfile);
			logVisitedLog(whitespace,visitedPredicate, myLogfile);
		}
		return matched_fact;
	}
	else
	{
		if(visitedPredicate.find(pred.name)==visitedPredicate.end())
		{
			vector<Predicate> preds;
			preds.push_back(pred);
			visitedPredicate[pred.name] = preds;
		}
		else
		{
			visitedPredicate[pred.name].push_back(pred);
		}
		if(isLogging)
		{
			logVisitedLog(whitespace,visitedPredicate, myLogfile);
		}
	}
	
	if(Program::rules.find(pred.name)!=Program::rules.end())
	{
		for(vector<Rule>::iterator it = Program::rules[pred.name].begin();it!=Program::rules[pred.name].end();it++)
		{
			if((*it).right.isNegated==pred.isNegated && checkRHS((*it).right,pred))
			{
				if(isLogging)
				log("\n"+whitespace+"************Unify "+pred.name+" ************\n",myLogfile);
				UnifyStruct right_result = unify((*it),pred,myLogfile,true,0,whitespace,isLogging);
				if(right_result.can_be_unified)
				{
					Rule r = right_result.r;
					queue<QueueNode> q;
					QueueNode node;
					node.r = r;
					node.level = 0;
					q.push(node);
					if(isLogging)
					r.displayRule(whitespace,myLogfile);
					bool istrue = false;
					while(!q.empty())
					{
						QueueNode front = q.front();
						q.pop();
						int i = front.level;
						if(i<(*it).left_predicate_count)
						{
							PredicateMatchResult assigned_backtrack = backTrack(front.r.left[i],myLogfile,level+1,visitedPredicate,isLogging);
							if(assigned_backtrack.isFound)
							{
								for(vector<Predicate>::iterator iter = assigned_backtrack.preds.begin(); iter!=assigned_backtrack.preds.end();iter++)
								{
									UnifyStruct result = unify(front.r,(*iter),myLogfile,false,front.level,whitespace,isLogging);
									if(result.can_be_unified)
									{
										Rule new_rule = result.r;
										if(isLogging)
										{
											log("\n"+whitespace+"************Unify "+(*iter).name+" level "+to_string(front.level)+" ************\n",myLogfile);
											new_rule.displayRule(whitespace,myLogfile);
										}
										QueueNode node;
										node.r = new_rule;
										node.level = front.level+1;
										q.push(node);
									}
								}
							}
						}
						else
						{
							istrue = true;
							q.push(front);
							break;
						}
					}
					//to do: check for this
					if(istrue)
					{
						vector<Predicate> preds;
						while(!q.empty())
						{
							QueueNode node = q.front();
							preds.push_back(node.r.right);
							q.pop();
						}
						matched_fact.isFound = true;
						for(vector<Predicate>::iterator it = preds.begin();it!=preds.end();it++)
						{
							matched_fact.preds.push_back(*it);
						}
					}
				}
			}
		}
	}
	if(isLogging)
	log(whitespace+"value returned : "+to_string(matched_fact.isFound)+"\n",myLogfile);
	visitedPredicate = temp_visited_nodes;
	return matched_fact;
}
void check(string query, ofstream &myLogfile, ofstream &myOutfile,bool isLogging,bool isFirstCase)
{
	if(isLogging)
	log("*************** checking for "+query+" ***********\n",myLogfile);
	Predicate pred = Program::createPredicate(query,true);
	map<string, vector<Predicate>> visitedPredicates;
	if(!isFirstCase)
		log_output("\n",myOutfile);
	try
	{
		if(backTrack(pred,myLogfile,0,visitedPredicates,isLogging).isFound)
		{
			log_output("TRUE",myOutfile);
			Program::insertFact(query);
		}
		else
			log_output("FALSE",myOutfile);
	}
	catch(const exception& e)
	{
		log_output("FALSE",myOutfile);
	}
}
void init(string file, ofstream &myLogfile, ofstream &myOutfile, bool isLogging)
{
	ifstream myInfile(file);
	string line;
	if (myInfile.is_open())
  	{
  		getline (myInfile,line);
  		int queries_count = stoi(line);
  		string queries[queries_count];
  		int i=0;
  		while(i<queries_count && getline (myInfile,line))
  		{
  			queries[i++]=line;
  		}
		getline (myInfile,line);
		i=0;
		int rules = stoi(line);
		while(i<rules && getline (myInfile,line))
  		{
  			parsePredicate(line, myLogfile, isLogging);
  			i++;
  		}
		myInfile.close();
		if(isLogging)
		{
			Program::displayFacts(myLogfile);
			Program::displayRules(myLogfile);
		}
		for(int j=0;j<queries_count;j++)
		{
			if(j==0)
			check(queries[j],myLogfile,myOutfile,isLogging,true);
			else
			check(queries[j],myLogfile,myOutfile,isLogging,false);
		}
	}
	else cout << "Unable to open file"; 
}
int main (int argc, char* argv[])
{
  	initialize_logs();
	ofstream myLogfile;
	ofstream myOutfile;
	myLogfile.open("log.txt", std::ios_base::app);
	myOutfile.open("output.txt", std::ios_base::app);
	init(argv[2],myLogfile,myOutfile,Logging);
	log("\n",myOutfile);
	myLogfile.close();
	myOutfile.close();
}
