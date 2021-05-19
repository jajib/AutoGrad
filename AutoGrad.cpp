#include <string>
#include <cctype>
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <cmath>
#include <cassert>
#include <algorithm>

std::unordered_map<std::string, double>constants({{"e", 2.7182818285}, {"pi", 3.14159265}});

std::unordered_map<std::string, double(*)(double)>func_one_arg ({{"sin", sin}, {"cos", cos},\
{"ln", log}});

std::unordered_map<std::string, double(*)(double, double)>op_two_arg ({{"+",\ 
[](double a, double b){return a+b;}},\ 
{"-", [](double a, double b){return a-b;}},\
{"*", [](double a, double b){return a*b;}},\ 
{"/", [](double a, double b){assert(b);return a/b;}},\
{"^", [](double a, double b){return pow(a, b);}}});

std::unordered_map<std::string, double(*)(double, double)>func_two_arg;

std::unordered_map<std::string, double>op_prior({{"+", 1}, {"-", 1}, {"*", 2},\ 
{"/", 2}, {"^", 3}}); 

std::vector<std::string>var ({"x", "y", "z"});

std::string GetFirstOperation (std::string str, int start = 0, bool spaces = 1)
{
	std::string ret_str;
	
	if (spaces == true)
	{
		for (int cnt = start; cnt < str.size() && str[cnt] != ' '; cnt++)
		{
			ret_str.push_back(str[cnt]);
		}
		
		return ret_str;
	}
	
	for(int i = start; i < str.size() && isalpha(str[i]); i++)
	{
		ret_str.push_back(str[i]);
	}
	
	if (!ret_str.empty())
	{
		return ret_str;
	}
	
	for(int i = start; i < str.size() && (isdigit(str[i]) || str[i] == '.'); i++)
	{
		ret_str.push_back(str[i]);
	}
	
	if (!ret_str.empty())
	{
		return ret_str;
	}
	
	ret_str.push_back(str[start]);
	return ret_str;
}

std::string InfixStrProc (std::string str)
{
	std::string ret (str);
	/*
	if (str[0] == '-')
	{
		ret.insert(0, "0");
	}
	*/
	for (int i = 0; i < str.size() - 1; i++)
	{
		if (str[i] == '(' && str[i+1] == '-')
		{
			ret.insert(i+1, "0");
		}
	}
	
	return ret;
}

std::string GetPrefix (std::string infix_str)
{
	std::string prefix_str;
	
	std::string temp_str;
	
	std::stack<std::string>buffer;
	
	for (int i = 0; i < infix_str.size();)
	{
		temp_str = GetFirstOperation(infix_str, i, false);
		
		if((isdigit(temp_str[0]) || temp_str[0] == '.') || var.end() != std::find(var.begin(), var.end(), temp_str))
		{
			prefix_str.append(temp_str);
			prefix_str.push_back(' ');
		}
		else if ((func_one_arg.end() != func_one_arg.find(temp_str)) ||\ 
		         (func_two_arg.end() != func_two_arg.find(temp_str)))
		{
			buffer.push(temp_str);
		}
		else if (!temp_str.compare(","))
		{
			while(buffer.top().compare("("))
			{
				prefix_str.append(buffer.top());
				prefix_str.push_back(' ');
				buffer.pop();
			}
		}
		else if (op_prior.end() != op_prior.find(temp_str))
		{
			while ((buffer.empty() == false && op_prior.end() != op_prior.find(buffer.top())) &&\
			       (op_prior[buffer.top()] >= op_prior[temp_str]))
			{
				prefix_str.append(buffer.top());
				prefix_str.push_back(' ');
				buffer.pop();
			}
			
			buffer.push(temp_str);
		}
		else if (!temp_str.compare("("))
		{
			buffer.push(temp_str);
		}
		else if (!temp_str.compare(")"))
		{
			while((buffer.empty() == false) && (buffer.top().compare("(")))
			{
				prefix_str.append(buffer.top());
				prefix_str.push_back(' ');
				buffer.pop();
			}
			
			buffer.pop();
			if ((buffer.empty() == 0) && ((func_one_arg.end() != func_one_arg.find(buffer.top())) ||\ 
			    (func_two_arg.end() != func_two_arg.find(buffer.top()))))
			{
				prefix_str.append(buffer.top());
				prefix_str.push_back(' ');
				buffer.pop();	
			}
		}
	
		i += temp_str.size();
	}
	
	while(!buffer.empty())
	{
		prefix_str.append(buffer.top());
		prefix_str.push_back(' ');
		buffer.pop();
	}
	
	return prefix_str;
}

struct Node
{
	std::string operation;
	
	Node* left;
	Node* right;
	
	Node(std::string new_op = "", Node* new_left = nullptr, Node* new_right = nullptr)
	{
		operation = new_op;
		left = new_left;
		right = new_right;
	}
	
	~Node()
	{
		operation = "";
		
		if (left != nullptr)
		{
			delete left;
		}
		
		if (right != nullptr)
		{
			delete right;
		}
		
		left = nullptr;
		right = nullptr;
	}
	
	int IsEqual(Node* root)
	{
		if ((left == nullptr && right == nullptr) && 
		(root->left == nullptr && root->right == nullptr))
		{
			return (!operation.compare(root->operation) ? 1 : 0);
		}
		
		if (left == nullptr)
		{
			if (root->left == nullptr)
			{
				return right->IsEqual(root->right);
			}
			else
			{
				return 0;
			}
		}
		
		if (right == nullptr)
		{
			if (root->right == nullptr)
			{
				return left->IsEqual(root->left);
			}
			else
			{
				return 0;
			}
		}
	
		if (root->left == nullptr || root->right == nullptr)
		{
			return 0;
		}
	
		return left->IsEqual(root->left) * right->IsEqual(root->right); 
	}
	
	int CheckNotContainsX()
	{
		if (!operation.compare("x"))
		{
			return 0;
		}
		else
		{
			return (left == nullptr ? 1 : left->CheckNotContainsX()) * (right == nullptr ? 1 : right->CheckNotContainsX());
		}
	}
	
	int CheckDivisionByVar()
	{
		if (!operation.compare("/"))
		{
			return 1 - right->CheckNotContainsX();
		}
		else if (!operation.compare("^"))
		{
			return 1 - left->CheckNotContainsX();
		}
		else 
		{
			
		}
	}
	
	Node* Copy()
	{	
		Node* copy_node = new Node(operation);
		
		if (left == nullptr && right == nullptr)
		{
			return copy_node;
		}
		
		if (left != nullptr)
		{
			copy_node->left = left->Copy();
		}
		
		if (right != nullptr)
		{
			copy_node->right = right->Copy();
		}
		
		return copy_node;
	}
		
	int print(int level = 0)
	{
		for (int i = 0; i < level; i++)
		{
			std::cout << "	";	
		}	
			
		std::cout << operation << "\n";
		
            	
    	if (right != nullptr)
		{
			right->print(level+1);       
		}
		else
		{
			for (int i = 0; i <= level; i++)
    		{
    			std::cout << "	";
    		}
			
			std::cout << "#\n";
		}
		
		if (left != nullptr)
		{
			left->print(level+1);        
    	}
    	else
    	{
    		for (int i = 0; i <= level; i++)
    		{
    			std::cout << "	";
    		}
    		
			std::cout << "#\n";
    	}
	};
};

Node* CreateExpTree (std::string str, bool spaces = 1)
	{
	std::string buffer;	
	
	std::stack<Node*> tree_buffer;
	
	for (int i = 0; i < str.size();)
	{	
		buffer = GetFirstOperation(str, i, spaces);
		
		Node* new_node = new Node(buffer);	
		
		
		if ( func_one_arg.end() != func_one_arg.find(buffer))
		{
			new_node->left = tree_buffer.top();
			
			tree_buffer.pop();
			tree_buffer.push(new_node);
		}
		else if ( (op_two_arg.end() != op_two_arg.find(buffer)) ||\ 
		        (func_two_arg.end() != func_two_arg.find(buffer)) )
		{ 	
			new_node->right = tree_buffer.top();
			
			tree_buffer.pop();
			
			new_node->left = tree_buffer.top();
			
			tree_buffer.pop();
			
			tree_buffer.push(new_node);
		}
		else
		{
			tree_buffer.push(new_node);	
		}
		
		i+= (buffer.size() + 1);	
	}
	return tree_buffer.top();
}

Node* Derivation (Node* root_node, std::string deriv_var)
{
	Node* deriv = new Node();
		
	if (root_node->left == nullptr && root_node->right == nullptr)
	{
		deriv->operation = (root_node->operation.compare(deriv_var) ? "0" : "1");
		return deriv;
	}
		
	if (!root_node->operation.compare("sin"))
	{
		deriv->operation = "*";
			
		Node* cos_node = new Node ("cos");
		cos_node->left = root_node->left->Copy();
			
		deriv->left = cos_node;
		deriv->right = Derivation(root_node->left, deriv_var);
			
		return deriv;
	}
	else if (!root_node->operation.compare("cos"))
	{
		deriv->operation = "*";
			
		Node* sin_node = new Node("sin", root_node->left->Copy());
			
		Node* m_one_node = new Node("-1");
			
		Node* mul_node = new Node("*", sin_node, m_one_node);

		deriv->left = mul_node;
		deriv->right = Derivation(root_node->left, deriv_var);
			
		return deriv;
			
	}
	else if (!root_node->operation.compare("+") || !root_node->operation.compare("-"))
	{
		deriv->operation = root_node->operation;
		deriv->left = Derivation(root_node->left, deriv_var);
		deriv->right = Derivation(root_node->right, deriv_var);
			
		return deriv;
	}
	else if (!root_node->operation.compare("*"))
	{
		deriv->operation = "+";
		
		Node* mul_left = new Node("*", root_node->left->Copy(), Derivation(root_node->right, deriv_var));
		Node* mul_right = new Node("*", Derivation(root_node->left, deriv_var), root_node->right->Copy());
		
		deriv->left = mul_left;
		deriv->right = mul_right;
		
		return deriv;
	}
	else if (!root_node->operation.compare("/"))
	{
		Node* mul_a = new Node("*", Derivation(root_node->left, deriv_var), root_node->right->Copy());
		Node* mul_b = new Node("*", root_node->left->Copy(), Derivation(root_node->right, deriv_var));	
		Node* mul_c = new Node("*", root_node->right->Copy(), root_node->right->Copy());
		
		Node* sub   = new Node("-", mul_a, mul_b);
		
		deriv->operation = "/";
		deriv->left = sub;
		deriv->right = mul_c;
		
		return deriv;
	}
	else if (!root_node->operation.compare("ln"))
	{
		deriv->operation = "*";
		deriv->right = Derivation(root_node->left, deriv_var);
		
		Node* one = new Node("1");
		
		Node* div = new Node("/", one, root_node->left->Copy());
		
		deriv->left = div;
		
		return deriv;
	}
	else if (!root_node->operation.compare("^"))
	{
		Node* new_root = CreateExpTree("e x ln y * ^ 1 *");
		
		new_root->left->right->left->left = root_node->left->Copy();
		new_root->left->right->right = root_node->right->Copy();
		
		new_root->right = Derivation(new_root->left->right, deriv_var);
		
		return new_root;
	}	
		
	return deriv;
}

double Calc(Node* root, std::unordered_map<std::string, double> values)
{	
	if (root->left == nullptr && root->right == nullptr)
	{
		if (var.end() != std::find(var.begin(), var.end(), root->operation)) 
		{
			return values[root->operation];
		}
		else if (constants.end() != constants.find(root->operation))
		{
			return constants[root->operation];
		}
		else 
		{
			return std::stod(root->operation);
		}
	}

	if (func_one_arg.end() != func_one_arg.find(root->operation))
	{
		return func_one_arg[root->operation](Calc(root->left, values));
	}
		
	if (op_two_arg.end() != op_two_arg.find(root->operation))
	{
		return op_two_arg[root->operation](Calc(root->left, values), Calc(root->right, values));
	}
	
	if (func_two_arg.end() != func_two_arg.find(root->operation))
	{
		return func_two_arg[root->operation](Calc(root->left, values), Calc(root->right, values));
	}
}

Node* Reduce(Node* root)
{
	if (root->left == nullptr && root->right == nullptr)
	{
		return root->Copy();
	}
	
	Node* ret = new Node();
	
	Node* left_r = Reduce(root->left);
	Node* right_r = Reduce(root->right);
	
	if (isdigit(left_r->operation[0]) || left_r->operation[0] == '.')
	{
		if (right_r == nullptr)
		{
			ret->operation = std::to_string(func_one_arg[root->operation](std::stod(left_r->operation)));
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}			
		else
		{
			if (func_two_arg.end() != func_two_arg.find(root->operation))
			{
				ret->operation = std::to_string(func_two_arg[root->operation](std::stod(left_r->operation), std::stod(right_r->operation)));
				ret->left = nullptr;
				ret->right = nullptr;
			
				return ret;
			}
			else if (op_two_arg.end() != op_two_arg.find(root->operation))
			{
				ret->operation = std::to_string(op_two_arg[root->operation](std::stod(left_r->operation), std::stod(right_r->operation)));
				ret->left = nullptr;
				ret->right = nullptr;
				
				return ret;
			}
		}
	}
	
	if (!root->operation.compare("+"))
	{
		if ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 0)
		{
			ret->operation = right_r->operation;
			ret->left = (right_r->left == nullptr ? nullptr : right_r->left->Copy());
			ret->right = (right_r->right == nullptr ? nullptr : right_r->right->Copy());
			
			return ret; 
		}
		
		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 0)
		{
			ret->operation = left_r->operation;
			ret->left = (left_r->left == nullptr ? nullptr : left_r->left->Copy());
			ret->right = (left_r->right == nullptr ? nullptr : left_r->right->Copy());
			
			return ret; 
		}	
	}
	
	if (!root->operation.compare("-"))
	{	
		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 0)
		{
			ret->operation = left_r->operation;
			ret->left = (left_r->left == nullptr ? nullptr : left_r->left->Copy());
			ret->right = (left_r->right == nullptr ? nullptr : left_r->right->Copy());
			
			return ret; 
		}
		
		if (left_r->IsEqual(right_r))
		{
			ret->operation = std::to_string(0);
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
	}
	
	if (!root->operation.compare("*"))
	{
		if ( ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 0) ||
		((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 0))
		{
			ret->operation = "0";
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
		
		if ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 1)
		{
			ret->operation = right_r->operation;
			ret->left = right_r->left->Copy();
			ret->right = right_r->right->Copy();
			
			return ret;
		}

		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 1)
		{
			ret->operation = left_r->operation;
			ret->left = left_r->left->Copy();
			ret->right = left_r->right->Copy();
			
			return ret;
		}
	}
	
	if (!root->operation.compare("/"))
	{
		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 1)
		{
			ret->operation = left_r->operation;
			ret->left = left_r->left->Copy();
			ret->right = left_r->right->Copy();
			
			return ret;
		}
		
		if ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 0)
		{
			ret->operation = "0";
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
	}
	
	if (!root->operation.compare("^"))
	{
		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 1)
		{
			ret->operation = root->operation;
			ret->left = left_r->left->Copy();
			ret->right = left_r->right->Copy();
			
			return ret;
		}
		
		if ((isdigit(right_r->operation[0]) || right_r->operation[0] == '.') && std::stod(right_r->operation) == 0)
		{
			ret->operation = "1";
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
		
		if ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 1)
		{
			ret->operation = "1";
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
		
		if ((isdigit(left_r->operation[0]) || left_r->operation[0] == '.') && std::stod(left_r->operation) == 0)
		{
			ret->operation = "0";
			ret->left = nullptr;
			ret->right = nullptr;
			
			return ret;
		}
	}
	
	ret->operation = root->operation;
	ret->left = left_r;
	ret->right = right_r;
	return ret;
}

std::string GetInfix(Node* root)
{
	std::string ret;
	
	if (func_one_arg.end() != func_one_arg.find(root->operation))
	{
		ret.append(root->operation + "(" + GetInfix(root->left) + ")");
		return ret;
	}
	else if (func_two_arg.end() != func_two_arg.find(root->operation))
	{
		ret.append(root->operation + "(" + GetInfix(root->left) + ", " + GetInfix(root->right) + ")");
		return ret;
	}
	else if (op_two_arg.end() != op_two_arg.find(root->operation))	
	{
		ret.append("(" + GetInfix(root->left) + root->operation + GetInfix(root->right) + ")");
		return ret;
	}
	else
	{
		return root->operation;
	}
}

int main()
{
	//Node* c = b.CreateExpTree("2");
	
	//std::cout << c->operation;
		
	//Node* tree = CreateExpTree(str);
	//tree->print();
	
	//Node* d_tree = Derivation(tree, "x");
	//d_tree->print();
	//std::cout << GetFirstOperation("4.53sin5*x(6)", 0, false);
	//std::cout << Calc(d_tree, 89);
	std::string str = "(x+1)^(x+2)*ln(sin(x^2))-(x+1)^(x+2)";
	std::string data = GetPrefix(InfixStrProc(str));
	Node* root = CreateExpTree(data);
	root->print();
	
	std::cout << GetInfix(root) << "\n";
	
	//Node* reduced = Reduce(root);
	//reduced->print();
	
	Node* deriv_root = Derivation(root, "x");
	deriv_root->print();
	
	std::cout << GetInfix(deriv_root);
		
	std::unordered_map<std::string, double>values({{"x", 1.2}, {"y", 1.09}, {"z", 5}});
	std::cout << Calc(deriv_root, values);
	
	//std::cout << InfixStrProc("(-x)");
}