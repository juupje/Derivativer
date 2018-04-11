#pragma once
#include<string>
using namespace std;

class node {
	string s;
	double value;
	bool isNumber = false;
public:
	node(const string);
	node(const char v);
	node(const double v);
	node(const int v);
	node(const node& n);
	static void remove(node *n);
	bool operator==(const char *c);
	bool operator==(char c);
	bool operator==(double &d);
	bool operator==(node &n);
	bool is_Number();
	bool equals(node*);
	double& getNumber();
	string getValue();
	string& accessText();
	node* right = 0;
	node* left = 0;
};

ostream& operator<<(ostream& s, node& n);
