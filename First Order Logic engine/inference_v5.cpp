#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <limits>
#include <stack>
#include <regex>
#include <map>
#include <queue>
using namespace std;
const string master_pred_rgx = "(~?)([A-Z][a-z]*)\\(([A-Za-z]+)(?:,([A-Za-z]+))*\\)";
const string master_fact_rgx = "(~?)([A-Z][a-z]*)\\(([A-Z][A-Za-z]*)(?:,([A-Z][A-Za-z]*))*\\)";
const string master_rule_rgx = "~?([A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) (?:\\^ ~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) )*)=> (~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\))";
const string master_lhs_rule_rgx = "(~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\)) (?:\\^ (~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) ))*";
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
			if((this->right.attributes[i][0]>='a' && this->right.attributes[i][0]<='z') && 
				this->substitution[this->right.attributes[i]].compare("")!=0)
			{
				this->right.attributes[i] = this->substitution[this->right.attributes[i]];
			}	
		}
		//Todo: check if original rule is modified or not
		for(vector<Predicate>::iterator it = this->left.begin();it!=this->left.end();it++)
		{
			for(int i = 0;i<(*it).attribute_count;i++)
			{
				if((*it).attributes[i][0]>='a' && (*it).attributes[i][0]<='z' && this->substitution[(*it).attributes[i]].compare("")!=0)
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
		regex rgx;
		if(!isFact)
		rgx.assign(master_pred_rgx, regex::ECMAScript);
		else
		rgx.assign(master_fact_rgx, regex::ECMAScript);
		smatch match;
		Predicate pred;
		if (regex_search(str, match, rgx))
		{
			pred.name = match[2];
			int index_of_left_paranthesis = str.find('(')+1;
			int index_of_right_paranthesis = str.find(')');
			string args_str = str.substr(index_of_left_paranthesis,index_of_right_paranthesis-index_of_left_paranthesis);
			istringstream ss(args_str);
			string token;
			pred.attribute_count = 0;
			while(getline(ss, token, ',')) {
	    		pred.attributes.push_back(token);
	    		pred.attribute_count++;
			}
			if(match[1].length()>0)
			{
				pred.isNegated = true;
			}
			else
			{
				pred.isNegated = false;
			}
		}
		return pred;
	}
	
	static void insertFact(string str)
	{
		Predicate new_pred = createPredicate(str,true);
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
	static void insertRule(string str, ofstream &myLogfile, bool isLogging)
	{
		string output = "---------------------";
		output += "\n";
		output += "Rule : ";
		output += str + "\n";
		regex rgx(master_rule_rgx);
		regex left_regex(master_lhs_rule_rgx);
		regex left_pred_regex(master_pred_rgx);
		regex right_rgx(master_pred_rgx);
		smatch match;
		Predicate right_pred;
		vector<Predicate> left_preds;
		Rule rule;
		rule.left_predicate_count = 0;
		output += "---------------------";
		output += "\n";
		if (regex_search(str, match, rgx))
		{
			string left_str = match[1];
			smatch left_match;
			if (regex_search(left_str, left_match, left_regex))
			{
				output += "Tokenized Left Hand side : ";
				string left_pred_str = left_match[0];
				istringstream ss(left_pred_str);
				string token;
				Predicate left_pred;
				while(getline(ss, token, '^')) {
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
				}
				output += "\n";
				if(isLogging)
					log(output,myLogfile);
				output = "";
				rule.left = left_preds;	
			}
			
			output = "Tokenized right predicate : ";
			output += "\n";
			if(isLogging)
				log(output,myLogfile); 
			output = "";
			string right_str = match[2];
			smatch right_match;
			if (regex_search(right_str, right_match, right_rgx))
			{
				right_pred = createPredicate(right_str,false);
				rule.right = right_pred;
			}
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
			if(Program::rules.find(rule.right.name) == Program::rules.end())
			{
				vector<Rule> rules_list;
				rules_list.push_back(rule);
				string s = match[1];	
				Program::rules[rule.right.name] = rules_list;
			}
			else
			{
				Program::rules[rule.right.name].push_back(rule);
			}
		}
		if(isLogging)
		{
			log(output, myLogfile);
			rule.displayRule("",myLogfile);
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
	regex factrgx(master_fact_rgx);
	regex rule_rgx(master_rule_rgx);
	smatch fact_match;
	smatch rule_match;
	if (regex_search(line, fact_match, factrgx) && line.find("=>")==-1)
	{
		string s = fact_match[0];
		if(line.find(s)==0)
		Program::insertFact(s);
    }
    if(regex_search(line, rule_match, rule_rgx))
	{
		string s = rule_match[0];
		Program::insertRule(s, myLogfile, isLogging);
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
UnifyStruct unify(Rule r, Predicate pred, ofstream &myLogfile, bool isRight,int left_pred_index)
{
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
		for(vector<Predicate>::iterator i = r.left.begin();i!=r.left.end();i++)
		{
			if((*i).name.compare(pred.name)==0)
			{
				pred_to_be_unified = (*i);
				break;
			}
		}
	}
	//Unify next recurrent predicate 
	for(int i=left_pred_index;i<pred_to_be_unified.attribute_count;i++)
	{
		if((pred_to_be_unified.attributes[i][0]>='a'&& pred_to_be_unified.attributes[i][0]<='z') &&
			pred.attributes[i][0]>='A' && pred.attributes[i][0]<='Z')
		{
			if(rule.substitution[pred_to_be_unified.attributes[i]].compare("")==0 ||
			 rule.substitution[pred_to_be_unified.attributes[i]].compare(pred.attributes[i])==0)
				rule.substitution[pred_to_be_unified.attributes[i]] = pred.attributes[i];
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
	log(whitespace+"\n||||||||||||||||| Visited nodes |||||||||||||||||\n",myLogfile);
	for(map<string, vector<Predicate>>::iterator i= visitedPredicate.begin();i!=visitedPredicate.end();i++)
	{
		for(vector<Predicate>::iterator it = i->second.begin();it!=i->second.end();it++)
		{
			(*it).displayFact(whitespace, myLogfile);
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
		}
		logVisitedLog(whitespace,visitedPredicate, myLogfile);
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
		logVisitedLog(whitespace,visitedPredicate, myLogfile);
	}
	
	if(Program::rules.find(pred.name)!=Program::rules.end())
	{
		for(vector<Rule>::iterator it = Program::rules[pred.name].begin();it!=Program::rules[pred.name].end();it++)
		{
			if((*it).right.isNegated==pred.isNegated && checkRHS((*it).right,pred))
			{
				if(isLogging)
				log("\n"+whitespace+"************Unify "+pred.name+" ************\n",myLogfile);
				UnifyStruct right_result = unify((*it),pred,myLogfile,true,0);
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
									UnifyStruct result = unify(front.r,(*iter),myLogfile,false,front.level);
									if(result.can_be_unified)
									{
										Rule new_rule = result.r;
										if(isLogging)
										{
											log("\n"+whitespace+"************Unify "+(*iter).name+" ************\n",myLogfile);
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
	return matched_fact;
}
void check(string query, ofstream &myLogfile, ofstream &myOutfile,bool isLogging)
{
	if(isLogging)
	log("*************** checking for "+query+" ***********\n",myLogfile);
	Predicate pred = Program::createPredicate(query,true);
	map<string, vector<Predicate>> visitedPredicates;
	if(backTrack(pred,myLogfile,0,visitedPredicates,isLogging).isFound)
	{
		log_output("TRUE\n",myOutfile);
		Program::insertFact(query);
	}
	else
		log_output("FALSE\n",myOutfile);
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
			check(queries[j],myLogfile,myOutfile,isLogging);
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
