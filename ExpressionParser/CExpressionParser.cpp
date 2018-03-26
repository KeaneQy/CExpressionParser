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
		elementType curType = elementType::NONE;
		stack<PNode> tmpStack;
		
		for (auto iter : _str_expression)
		{
			char tmpChar = iter;
			curType = get_element_type(tmpChar);
			
			node.reset();

			if (_vec_infix_ptr->empty())
			{
				node = make_shared<Node>();
				node->_ele = tmpChar;
				if (curType == elementType::POINT)
				{
					curType = elementType::NUM;
				}
				node->_type = curType;
				_vec_infix_ptr->push_back(node);

				continue;
			}
			
			switch (curType)
			{
			case CParser::elementType::FUNC:
				break;
			case CParser::elementType::NUM:

				node = _vec_infix_ptr->back();
				if (node->_type == elementType::NUM)
				{
					node->_ele += tmpChar;
				}
				else
				{
					node = make_shared<Node>();
					node->_ele = tmpChar;
					node->_type = elementType::NUM;
					_vec_infix_ptr->push_back(node);
				}

				break;
			case CParser::elementType::OPERTOR:

				node = _vec_infix_ptr->back();
				if (node->_type == elementType::PARENTHSIS_LEFT)
				{
					node = make_shared<Node>();
					node->_ele = tmpChar;
					node->_type = elementType::NUM;
					_vec_infix_ptr->push_back(node);
				}
				else
				{
					node = make_shared<Node>();
					node->_ele = tmpChar;
					node->_type = elementType::OPERTOR;
					_vec_infix_ptr->push_back(node);
				}

				break;
			case CParser::elementType::POINT:

				node = _vec_infix_ptr->back();
				if (node->_type == elementType::NUM)
				{
					node->_ele += tmpChar;
					node->_type = elementType::NUM;
				}
				else
				{
					node = make_shared<Node>();
					node->_ele = tmpChar;
					node->_type = elementType::NUM;
					_vec_infix_ptr->push_back(node);
				}

				break;
			case CParser::elementType::NONE:
				break;
			case CParser::elementType::PARENTHSIS_LEFT:
				node = make_shared<Node>();
				node->_ele = tmpChar;
				node->_type = elementType::PARENTHSIS_LEFT;
				_vec_infix_ptr->push_back(node);
				break;
			case CParser::elementType::PARENTHSIS_RIGHT:
				node = make_shared<Node>();
				node->_ele = tmpChar;
				node->_type = elementType::PARENTHSIS_RIGHT;
				_vec_infix_ptr->push_back(node);
				break;
			default:
				break;
			}
		}
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
			case CParser::elementType::FUNC:
				break;
			case CParser::elementType::NUM:
				_vec_postfix_ptr->push_back(iter);
				break;
			case CParser::elementType::OPERTOR:
				while (!tmpStack.empty() && less_equal(iter->_ele, tmpStack.top()->_ele) && tmpStack.top()->_type != elementType::PARENTHSIS_LEFT)
				{
					tmpNode = tmpStack.top();
					tmpStack.pop();
					_vec_postfix_ptr->push_back(tmpNode);
				}

				tmpStack.push(iter);
				break;
			case CParser::elementType::PARENTHSIS_LEFT:
				tmpStack.push(iter);
				break;
			case CParser::elementType::PARENTHSIS_RIGHT:
				while (tmpStack.top()->_type != elementType::PARENTHSIS_LEFT)
				{
					tmpNode = tmpStack.top();
					tmpStack.pop();
					_vec_postfix_ptr->push_back(tmpNode);
				}

				tmpStack.pop();
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
			return elementType::POINT;
		}
		else if (c == 43 || c == 42 || c == 45 || c == 47)
		{
			return elementType::OPERTOR;
		}
		else if (c == 40)
		{
			return elementType::PARENTHSIS_LEFT;
		}
		else if (c == 41)
		{
			return elementType::PARENTHSIS_RIGHT;
		}
		else
		{
			return elementType::NONE;
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
		
		regex op_reg(R"([\+\-]{2,}|[\*\/]{2,}|\.{2,})");		//match ilegal ops (+-*/.)

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

			if (tmpType == elementType::PARENTHSIS_LEFT || tmpType == elementType::PARENTHSIS_RIGHT)
			{
				++tmpCount;
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