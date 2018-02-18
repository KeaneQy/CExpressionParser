#include "CExpressionParser.h"
namespace CParser
{
	CExpressionParser::CExpressionParser()
	{
		_str_expression = "";
		_vec_postfix_ptr = make_shared<NodeVec>();
		_vec_infix_ptr = make_shared<NodeVec>();
	}

	bool CExpressionParser::preprocess(string exp)
	{
		_str_expression = exp;

		if (_str_expression.empty())
		{
			_error_msg = error::EMPTY_EXP;
			return false;
		}

		if (!isLegal(_str_expression))
		{
			_error_msg = error::ILLEGAL_EXP;
			return false;
		}

		if (!getElements())
		{
			_error_msg = error::UNKNOWN;
			return false;
		}

		_error_msg = error::SUCCESS;
		return true;
	}

	bool CExpressionParser::getElements()
	{
		PNode node;
		vector<char> tmpVec(255);
		tmpVec.clear();
		elementType curType = elementType::NONE;
		elementType lastType = elementType::NONE;
		string tmpStr;

		for (auto iter : _str_expression)
		{
			char tmpChar = iter;
			curType = get_element_type(tmpChar);

			if (curType == lastType && (curType == elementType::NUM || curType == elementType::FUNC))
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
		stack<PNode> tmpStack;
		PNode tmpNode;

		for (auto iter : (*_vec_infix_ptr))
		{
			switch (iter->_type)
			{
			case elementType::OPERTOR:
				if (isParenthesis_left(iter->_ele))
				{
					tmpStack.push(iter);
				}
				else if (isParenthesis_right(iter->_ele))
				{
					while (!isParenthesis_left(tmpStack.top()->_ele))
					{
						tmpNode = tmpStack.top();
						tmpStack.pop();
						_vec_postfix_ptr->push_back(tmpNode);
					}

					tmpStack.pop();
				}
				else //General Operator
				{
					while (!tmpStack.empty() && isLessEqual(iter->_ele, tmpStack.top()->_ele) && !isParenthesis_left(tmpStack.top()->_ele))
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
		stack<double> tmpStack;
		PNode tmpNode;
		double result = -9999999;

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
			break;
		case '-':
			result = x - y;
			break;
		case '*':
			result = x * y;
			break;
		case '/':
			if (y != 0)
			{
				result = x / y;
			}
			else
			{
				_error_msg = error::DIVIDE_BY_ZERO;
			}
			break;
		default:
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

	bool CExpressionParser::isParenthesis_left(string ele)
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

	bool CExpressionParser::isParenthesis_right(string ele)
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

	bool CExpressionParser::isLessEqual(string ele, string current)
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

	bool CExpressionParser::isLegal(string exp)
	{
		return true;
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