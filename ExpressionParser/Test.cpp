#include <iostream>
#include <string>

#include "CExpressionParser.h"

using std::cout;
using std::endl;
using std::string;


int main()
{
	CParser::CExpressionParser parser;
	string exp = "(1.345+(+2.22))*(3.5678-5.33/6.765)";
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