#include <iostream>
#include "../header/node.hpp"
#include "../header/functions.hpp"
using namespace std;

node::node(char v) : value(0), isNumber(v=='e') {
	s.push_back(v);
}
node::node(const node& n) {
	value = n.value;
	s = n.s;
	isNumber = n.isNumber;
	if(n.left)
		left = new node(*n.left);
	if(n.right)
		right = new node(*n.right);
}
node::node(const string str) : s(str), value(0), isNumber(str=="pi" || str=="e") {}
node::node(const double v) : value(v), isNumber(true) {}
node::node(const int v) : value(double(v)), isNumber(true) {}

ostream& operator<<(ostream& s, node& n) {
	string v = n.getValue();
	s << v;
	return s;
}

bool node::operator==(node &n) {
	if(n.isNumber == isNumber) {
		return (n.getNumber() == getNumber());
	} else {
		return n.s == s;
	}
}

bool node::operator==(const char* c) {
	return (s == c);
}

bool node::operator==(char c) {
	return (s[0]==c);
}

bool node::operator==(double &d) {
	return (d == value);
}

bool node::is_Number() {
	return isNumber;
}

bool node::equals(node* n) {
	if(isNumber) return (n->is_Number() ? value==n->getNumber() : false);
	if(n->getValue() == getValue()) {
		if(left && right) return (left->equals(n->left) && right->equals(n->right)) || (left->equals(n->right) && right->equals(n->left));
		else if(left) return (!n->right ? left->equals(n->left) : false);
		else return true;
	}
	return false;
}

double& node::getNumber() {
	return value;
}

void node::remove(node *n) {
	if(!n) return;
	if(n->right)
		delete n->right;
	if(n->left)
		delete n->left;
	delete n;
}

string& node::accessText() {
	return s;
}

string node::getValue() {
	if(isNumber)
		return (s=="pi" ? "pi" : (s=="e" ? "e" : convertToString(value)));
	return s;
}
