#include "../header/functions.hpp"
using namespace std;

const string functions[] = {"sin", "cos", "tan", "log", "ln", "exp", "acos", "asin", "atan", "sinh", "cosh", "tanh", "abs", "sqrt"};
const int n = 14;

bool isFunction(string str) {
	for(int i = 0; i < n; i++) if(functions[i]==str) return true;
	return false;
}

string convertToString(const double d) {
	string str = to_string(d);
	str.erase(str.find_last_not_of('0') + 1, string::npos);
	if(str.back() == '.') str.pop_back();
	return str;
}

bool stringContains(string str, char c) {
	for(int i = 0; i < str.size(); i++)
		if(str[i]==c) return true;
	return false;
}
