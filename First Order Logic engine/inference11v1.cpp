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
using namespace std;
void log(string s, ofstream &myOutfile)
{
	//myOutfile << s;
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
	void displayPredicate(string s, ofstream &myOutfile)
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
		log(output, myOutfile);
	}	
	void displayFact(string s, ofstream &myOutfile)
	{
		displayPredicate(s,myOutfile);
		string output = "";
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
	vector<Map> mapping;
	int mapping_count;
	void displayRule(ofstream &myOutfile)
	{
		string output = "";
		output += "\tRight predicate : ";
		output += "\n";
		log(output,myOutfile);
		output = "";
		this->right.displayPredicate("\t",myOutfile);
		output += "\n";
		output += "\tLeft Predicates : ";
		output += "\n";
		log(output,myOutfile);
		output = "";
		for(vector<Predicate>::iterator it = this->left.begin();it != this->left.end();it++)
		{
			(*it).displayPredicate("\t",myOutfile);
			log("\n",myOutfile);
		}
		output = "";
		output += "\tMapping : ";
		output += "\n";
		for(vector<Map>::iterator m = this->mapping.begin();m != this->mapping.end();m++)
		{
			output += "\t";
			output += to_string((*m).left_predicate_index);
			output += " ";
			output += (*m).left_predicate_name;
			output += " ";
			output += to_string((*m).right_predicate_index);
			output += " ";
			output += (*m).right_predicate_name;
			output += " ";
			output += "\n";
		}
		log(output, myOutfile);
	}		
};

class Program
{
	public:
	static map<string, Predicate> predicates ;
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
	static bool checkFacts(Predicate pred)
	{
		if(facts.find(pred.name)!=facts.end())
		{
			for(vector<Predicate>::iterator i = facts[pred.name].begin(); i!=facts[pred.name].end();i++)
			{
				if(isPredicateSame((*i),pred))
					return true;
			}
		}
		return false;
	}
	static Predicate createPredicate(string str, ofstream &myOutfile, bool isFact)
	{
		regex rgx;
		if(!isFact)
		rgx.assign("(~?)([A-Z][a-z]*)\\(([a-z]+)(?:,([a-z]+))*\\)", std::regex::ECMAScript);
		else
		rgx.assign("(~?)([A-Z][a-z]*)\\(([A-Z][a-z]+)(?:,([A-Z][a-z]+))*\\)", std::regex::ECMAScript);
		smatch match;
		Predicate pred;
		string output = "";
		if (regex_search(str, match, rgx))
		{
			for(int i=0; i<match.length();i++)
			{
				output += match[i];
				output += " ";
			}
			output += "\n";
				
			pred.name = match[2];
			pred.attribute_count = 0;
			for(int i=3;i<match.size() && match[i].length()>0;i++)
			{
				pred.attributes.push_back(match[i]);
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
		log(output, myOutfile);
		return pred;
	}
	static void insertPredicate(string str, ofstream &myOutfile)
	{
		regex rgx("([A-Z][a-z]*)\\(([a-z]+)(,([a-z]+))*\\)");
		smatch match;
		if (regex_search(str, match, rgx))
		{
			Predicate new_pred = createPredicate(str,myOutfile,false);
			if(Program::predicates.find(new_pred.name) == Program::predicates.end())
			{
				Program::predicates[new_pred.name] = new_pred;
			}
		}
	}
	static void insertFact(string str, ofstream &myOutfile)
	{
		regex rgx("([A-Z][a-z]*)\\(([A-Z][a-z]*)(,([A-Z][a-z]*))*\\)");
		smatch match;
		if (regex_search(str, match, rgx))
		{
			Predicate new_pred = createPredicate(str,myOutfile,true);
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
	static void insertRule(string str, ofstream &myOutfile)
	{
		string output = "---------------------";
		output += "\n";
		output += "Rule : ";
		output += str + "\n";
		regex rgx("~?([A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\) (?:\\^ ~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\) )*)=> (~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\))");
		regex left_regex("(~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\)) (?:\\^ (~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\) ))*");
		regex left_pred_regex("(~?[A-Z][a-z]*)\\(([a-z]+)(?:,([a-z]+))*\\)");
		regex right_rgx("(~?[A-Z][a-z]*)\\(([a-z]+)(?:,([a-z]+))*\\)");
		smatch match;
		Predicate right_pred;
		vector<Predicate> left_preds;
		Rule rule;
		rule.left_predicate_count = 0;
		rule.mapping_count = 0;
		output += "---------------------";
		output += "\n";
		if (regex_search(str, match, rgx))
		{
			string left_str = match[1];
			smatch left_match;
			if (regex_search(left_str, left_match, left_regex))
			{
				output += "Tokenized Left Hand side : ";
				for(int i=0; i<left_match.length();i++)
				{
					output += left_match[i];
					output += " ";
				}
				output += "\n";
				output += "Tokenized left predicates : ";
				output += "\n";
				log(output,myOutfile); 
				output = "";
				for(int i=1;i<left_match.length();i++)
				{
					string left_pred_str = left_match[i];
					smatch left_pred_match;
					Predicate left_pred;
					if (regex_search(left_pred_str, left_pred_match, left_pred_regex))
					{
						left_pred = createPredicate(left_pred_str,myOutfile,false);
						left_preds.push_back(left_pred);
						rule.left_predicate_count++;
					}
					rule.left = left_preds;
				}
			}
			output = "Tokenized right predicate : ";
			output += "\n";
			log(output,myOutfile); 
			output = "";
			string right_str = match[2];
			smatch right_match;
			if (regex_search(right_str, right_match, right_rgx))
			{
				right_pred = createPredicate(right_str,myOutfile,false);
				rule.right = right_pred;
			}
			//if(!rule.left && !rule.right)
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
								rule.mapping.push_back(m);
								rule.mapping_count++;
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
		
		log(output, myOutfile);
	}
	static void displayPredicates(ofstream &myOutfile)
	{
		log("Predicates : ",myOutfile);
		log("\n",myOutfile);
		for(map<string, Predicate>::iterator it = Program::predicates.begin(); it!=Program::predicates.end(); it++)
		{
			it->second.displayPredicate("",myOutfile);
			log("\n",myOutfile);
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
				(*r).displayRule(myOutfile);
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
map<string, Predicate> Program::predicates = Program::initializePredicate();
map<string, vector<Predicate>> Program::facts = Program::initializeFacts();
map<string, vector<Rule>> Program::rules = Program::initializeRules();
void parsePredicate(string line, ofstream &myOutfile)
{
	regex rgx("[A-Z][a-z]*\\([a-z]+(,[a-z]+)*\\)");
	regex factrgx("[A-Z][a-z]*\\([A-Z][a-z]*(,[A-Z][a-z]*)*\\)");
	regex rule_rgx("~?([A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\) (?:\\^ ~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\) )*)=> (~?[A-Z][a-z]*\\([a-z]+(?:,[a-z]+)*\\))");
	smatch match;
	smatch fact_match;
	smatch rule_match;
	int index = 0;
	while (regex_search(line.substr(index), match, rgx))
	{
		string s = match[0];
		Program::insertPredicate(s, myOutfile);
        index = line.find(s) + s.length();
    }
    index = 0;
    while (regex_search(line.substr(index), fact_match, factrgx))
	{
		string s = fact_match[0];
		Program::insertFact(s, myOutfile);
        index = line.find(s) + s.length();
    }
    index = 0;
    while (regex_search(line.substr(index), rule_match, rule_rgx))
	{
		string s = rule_match[0];
		Program::insertRule(s, myOutfile);
        index = line.find(s) + s.length();
    }
}
Predicate assign(Predicate p1, Predicate p2, vector<Map> m)
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
			if(Program::isPredicateSame((*i),pred))
				return true;
		}
	}
	return false;
}
bool backTrack(Predicate pred, ofstream &myOutfile,int level, map<string, vector<Predicate>> &visitedPredicate)
{
	//if(level>4)
	//	return false;
	string whitespace = "";
	for(int i=0;i<level;i++)
		whitespace+="\t";
	pred.displayFact(whitespace, myOutfile);
	log("\n",myOutfile);
	if(Program::checkFacts(pred))
	{
		log("Found\n",myOutfile);
		return true;
	}
	
	if(isAlreadyVisited(pred, visitedPredicate))
	{
		log("Already visited\n",myOutfile);
		return false;
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
	}
	
	if(Program::rules.find(pred.name)!=Program::rules.end())
	{
		for(vector<Rule>::iterator it = Program::rules[pred.name].begin();it!=Program::rules[pred.name].end();it++)
		{
			if((*it).right.isNegated==pred.isNegated)
			{
				bool istrue = true;
				for(vector<Predicate>::iterator i = (*it).left.begin();i!=(*it).left.end();i++)
				{
					Predicate assigned = assign((*i),pred,(*it).mapping);
					if(!backTrack(assigned,myOutfile,level+1,visitedPredicate))
					{
						istrue = false;
						break;
					}
				}
				if(istrue)
				{
					return true;
				}
			}
		}
	}
	return false;
}
void check(string query, ofstream &myLogfile, ofstream &myOutfile)
{
	log("*************** checking for "+query+" ***********\n",myLogfile);
	Predicate pred = Program::createPredicate(query,myLogfile,true);
	map<string, vector<Predicate>> visitedPredicates;
	if(backTrack(pred,myLogfile,0,visitedPredicates))
		log_output("TRUE\n",myOutfile);
	else
		log_output("FALSE\n",myOutfile);
}
void init(string file, ofstream &myLogfile, ofstream &myOutfile)
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
		int rules = stoi(line);
		while(i<rules && getline (myInfile,line))
  		{
  			parsePredicate(line, myLogfile);
  		}
		myInfile.close();
		Program::displayPredicates(myLogfile);	
		Program::displayFacts(myLogfile);
		Program::displayRules(myLogfile);
		for(int i=0;i<queries_count;i++)
		{
			check(queries[i],myLogfile,myOutfile);
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
	init(argv[2],myLogfile,myOutfile);
	log("\n",myOutfile);
}
