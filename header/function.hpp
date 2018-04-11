#pragma once
#include "../header/tree.hpp"
#include <vector>

class function {
	vector<string> fstrings;
	char* vars;
	int components;
	int variables;
	string name;
	vector<tree*> ftrees;
	bool processed = false;
public:
	function(char* f);
	void setVars(char* vars);
	void createTrees();
	void derive(char var);
	void divergence();
	void gradient();
	void curl();
	void simplify();
	int getComponentCount();
	tree* getComponent(int i);
	string& accessName();
};
