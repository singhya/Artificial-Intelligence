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
const string master_pred_rgx = "(~?)([A-Z][a-z]*)\\(([A-Za-z]+)(?:,([A-Za-z]+))*\\)";
const string master_fact_rgx = "(~?)([A-Z][a-z]*)\\(([A-Z][A-Za-z]*)(?:,([A-Z][A-Za-z]*))*\\)";
const string master_rule_rgx = "~?([A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) (?:\\^ ~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) )*)=> (~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\))";
const string master_lhs_rule_rgx = "(~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\)) (?:\\^ (~?[A-Z][a-z]*\\([A-Za-z]+(?:,[A-Za-z]+)*\\) ))*";

int main()
{

    string input = "abc,def,ghi";
	istringstream ss(input);
	string token;

	while(getline(ss, token, ',')) {
	    cout << token << '\n';
	}
	regex rgx(master_pred_rgx);
	regex factrgx(master_fact_rgx);
	regex rule_rgx(master_rule_rgx);
	smatch match;
	smatch fact_match;
	smatch rule_match;
	string str = "Hero(A)";
	if (regex_search(str, fact_match, factrgx))
	{
		cout<<fact_match[0];
		string s = fact_match[0];
	}
}
