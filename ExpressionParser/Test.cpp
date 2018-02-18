#include <iostream>
#include <string>

#include "CExpressionParser.h"

using std::cout;
using std::endl;
using std::string;


int main()
{
	CParser::CExpressionParser parser;
	//string exp = "(23+19)*50+71.3/28.6";
	string exp = "(1+2)*3*(4-5/6)";
	parser.preprocess(exp);

	cout << parser.getExp() << endl;
	
	parser.showStack();

	cout << "itop" << endl;
	parser.itop();

	parser.showStack();

	cout << parser.calc_exp_by_postfix() << endl;

	return 0;
}