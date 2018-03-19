#include "CExpressionParser.h"
namespace CParser
{
	CExpressionParser::CExpressionParser()
	{
		_str_expression = "";
		_vec_postfix_ptr = make_shared<NodeVec>();
		_vec_infix_ptr = make_shared<NodeVec>();
	}


	void CExpressionParser::clear()
	{
		_str_expression.clear();
		_vec_postfix_ptr->clear();
		_vec_infix_ptr->clear();
	}

	bool CExpressionParser::preprocess(string exp)
	{
		//clear all member variables
		clear();

		_str_expression = exp;

		if (_str_expression.empty())
		{
			_error_msg = error::EMPTY_EXP;
			_bool_valid = false;
			return false;
		}

		if (!legal(_str_expression))
		{
			_error_msg = error::ILLEGAL_EXP;
			_bool_valid = false;
			return false;
		}

		if (!strtoexp())
		{
			_error_msg = error::UNKNOWN;
			_bool_valid = false;
			return false;
		}

		if (!legal_exp(_vec_infix_ptr))
		{
			_error_msg = error::ILLEGAL_EXP;
			_bool_valid = false;
			return false;
		}

		_error_msg = error::SUCCESS;
		return true;
	}

	bool CExpressionParser::strtoexp()
	{
		if (!_bool_valid)
		{
			return _bool_valid;
		}

		PNode node;
		vector<char> tmpVec(255);
		tmpVec.clear();
		elementType curType = elementType::NONE;
		elementType lastType = elementType::NONE;
		char lastChar = ' ';
		string tmpStr;

		for (auto iter : _str_expression)
		{
			char tmpChar = iter;
			curType = get_element_type(tmpChar);

			if ((curType == lastType || (lastChar == 43 || lastChar == 45) && curType == elementType::NUM) && 
				(curType == elementType::NUM || curType == elementType::FUNC))
			{
				tmpVec.push_back(tmpChar);
			}
			else
			{
				if (!tmpVec.empty())
				{
					node = make_shared<Node>();
					tmpStr.insert(tmpStr.begin(), tmpVec.begin(), tmpVec.end());
					node->_ele = tmpStr;
					node->_type = lastType;
					tmpVec.clear();
					tmpStr.clear();

					tmpVec.push_back(tmpChar);

					_vec_infix_ptr->push_back(node);
				}
				else
				{
					tmpVec.push_back(tmpChar);
				}
			}

			lastType = curType;
			lastChar = tmpChar;
		}

		//The last node
		node = make_shared<Node>();
		tmpStr.insert(tmpStr.begin(), tmpVec.begin(), tmpVec.end());
		node->_ele = tmpStr;
		node->_type = curType;
		tmpVec.clear();
		tmpStr.clear();

		_vec_infix_ptr->push_back(node);
		return true;
	}

	void CExpressionParser::itop()
	{
		if (!_bool_valid)
		{
			return;
		}

		stack<PNode> tmpStack;
		PNode tmpNode;

		for (auto iter : (*_vec_infix_ptr))
		{
			switch (iter->_type)
			{
			case elementType::OPERTOR:
				if (parenthesis_left(iter->_ele))
				{
					tmpStack.push(iter);
				}
				else if (parenthesis_right(iter->_ele))
				{
					while (!parenthesis_left(tmpStack.top()->_ele))
					{
						tmpNode = tmpStack.top();
						tmpStack.pop();
						_vec_postfix_ptr->push_back(tmpNode);
					}

					tmpStack.pop();
				}
				else //General Operator
				{
					while (!tmpStack.empty() && less_equal(iter->_ele, tmpStack.top()->_ele) && !parenthesis_left(tmpStack.top()->_ele))
					{
						tmpNode = tmpStack.top();
						tmpStack.pop();
						_vec_postfix_ptr->push_back(tmpNode);
					}
					
					tmpStack.push(iter);
				}
	
				break;
			case elementType::NUM:
				_vec_postfix_ptr->push_back(iter);
				break;
			default:
				break;
			}
		}

		while (!tmpStack.empty())
		{
			tmpNode = tmpStack.top();
			tmpStack.pop();
			_vec_postfix_ptr->push_back(tmpNode);
		}
	}

	double CExpressionParser::calc_exp_by_postfix()
	{
		double result = -9999999;
		if (!_bool_valid)
		{
			return result;
		}

		stack<double> tmpStack;
		PNode tmpNode;
		

		for (auto iter : (*_vec_postfix_ptr))
		{
			if (iter->_type == elementType::NUM)
			{
				tmpStack.push(std::atof(iter->_ele.c_str()));
			}
			else if (iter->_type == elementType::OPERTOR && !tmpStack.empty())
			{
				double y = tmpStack.top();
				tmpStack.pop();
				double x = tmpStack.top();
				tmpStack.pop();
				char op = iter->_ele.at(0);

				double tmp = calc_with_operator(x, y, op);
				tmpStack.push(tmp);
			}
		}

		if (tmpStack.size() == 1)
		{
			result = tmpStack.top();
			tmpStack.pop();
		}

		return result;
	}

	double CExpressionParser::calc_with_operator(double x, double y, char op)
	{
		double result = 0;
		switch (op)
		{
		case '+':
			result = x + y;
			_error_msg = error::SUCCESS;
			break;
		case '-':
			result = x - y;
			_error_msg = error::SUCCESS;
			break;
		case '*':
			result = x * y;
			_error_msg = error::SUCCESS;
			break;
		case '/':
			if (y != 0)
			{
				result = x / y;
				_error_msg = error::SUCCESS;
			}
			else
			{
				_error_msg = error::DIVIDE_BY_ZERO;
			}
			break;
		default:
			_error_msg = error::UNKNOWN;
			break;
		}

		return result;
	}

	elementType CExpressionParser::get_element_type(char c)
	{
		if (c <= 57 && c >= 48)
		{
			return elementType::NUM;
		}
		else if (c == 46)
		{
			return elementType::NUM;
		}
		else if (c <= 43 && c >= 40)
		{
			return elementType::OPERTOR;
		}
		else if (c == 45 || c == 47)
		{
			return elementType::OPERTOR;
		}
		else
		{
			return elementType::NONE;
		}
	}

	bool CExpressionParser::parenthesis_left(string ele)
	{
		if (ele.length() == 1 && ele.at(0) == 40)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CExpressionParser::parenthesis_right(string ele)
	{
		if (ele.length() == 1 && ele.at(0) == 41)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CExpressionParser::less_equal(string ele, string current)
	{
		int s = _priority[ele];
		int t = _priority[current];

		if (s <= t)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CExpressionParser::legal(string exp)
	{
		bool tmp = true;
		regex basic_reg(R"(^[\d\+\-\*\/\(\)\.]+$)");		//match expression (only inlcude numbers and .+-*\())
		tmp = regex_match(exp, basic_reg);
		
		regex op_reg(R"([\+\-]{3,}|[\*\/]{2,}|\.{2,})");		//match ilegal ops (+-*/.)

#ifdef MY_DEBUG

		cout << tmp << " this is the first matching." << endl;
#endif
		tmp = !regex_search(exp, op_reg);

#ifdef MY_DEBUG

		cout << tmp << " this is the second matching." << endl;
#endif
		_bool_valid = tmp;
		return tmp;
	}

	bool CExpressionParser::legal_exp(PNodeVec infix)
	{
		regex num_reg(R"(^[-+]?\d+\.?\d+$)");		//match numbers (both of integer and decimal)
		bool tmp = true;
		int tmpCount = 0;
		for (auto iter : (*infix))
		{
			elementType tmpType = iter->_type;

			if (tmpType == elementType::OPERTOR)
			{
				if (iter->_ele.at(0) == 40 || iter->_ele.at(0) == 41)
				{
					++tmpCount;
				}
			}

			if (tmpType == elementType::NUM)
			{
				tmp = regex_match(iter->_ele, num_reg);
#ifdef MY_DEBUG

				cout << tmp << " this is the third num matching." << endl;
#endif
			}
			else
			{
				tmp = true;
			}
		}

		if (tmpCount % 2 == 0)
		{
			tmp = true;
		}
		else
		{
			tmp = false;
		}
		_bool_valid = tmp;
		return tmp;
	}

#ifdef MY_DEBUG
	string CExpressionParser::getExp()
	{
		return _str_expression;
	}

	void CExpressionParser::showStack()
	{
		for (auto iter : (*_vec_infix_ptr))
		{
			cout << (*iter)._ele << " ";
		}

		cout << endl;

		for (auto iter : (*_vec_postfix_ptr))
		{
			cout << (*iter)._ele << " ";;
		}

		cout << endl;
	}
#endif // DEBUG
}