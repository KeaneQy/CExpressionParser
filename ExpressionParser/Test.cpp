#include <iostream>
#include <string>

#include "CExpressionParser.h"

using std::cout;
using std::endl;
using std::string;


int main()
{
	CParser::CExpressionParser parser;
	string exp = "(233.1+12.5))*250+-711.3333/2878.6";
	//string exp = "(1+2)*3*(4-5/6)";
	bool result = parser.preprocess(exp);

	cout << parser.getExp() << endl;
	
	parser.showStack();

	cout << "itop" << endl;
	parser.itop();

	parser.showStack();

	double tmp = parser.calc_exp_by_postfix();
	cout << tmp << endl;

	return 0;
}