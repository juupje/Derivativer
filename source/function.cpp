#include "../header/function.hpp"
#include "../header/differentiator.hpp"
#include "../header/simplifier.hpp"
#include <string>
#include <iostream>

using namespace std;

function::function(char *f) : vars((char*) "x") {
	string s = string(f);
	fstrings = vector<string>(0);
	int begin = 0;
	for(unsigned int i = 0; i < s.size(); i++) {
		if(s[i] == ';') {
			fstrings.push_back(s.substr(begin, i-begin));
			begin = i+1;
		}
	}
	fstrings.push_back(s.substr(begin, s.size()));
	ftrees = vector<tree*>(fstrings.size());
	components = fstrings.size();
	variables = components;
	name = "Original function";
}

void function::setVars(char *v) {
	vars = v;
	variables = 0;
	for(int i = 0; v[i] != 0; i++)
		variables++;
}

void function::createTrees() {
	for(unsigned int i = 0; i < ftrees.size(); i++) {
		ftrees[i] = new tree(fstrings[i]);
		ftrees[i]->accessName() = string(1, vars[i]);
	}
	if(getComponentCount() == 1)
		ftrees[0]->accessName() = "";
	processed = true;
}

void function::derive(char c) {
	if(!processed) createTrees();
	for(unsigned int i = 0; i < ftrees.size(); i++) {
		ftrees[i] = (differentiator(ftrees[i])).process(c);
		ftrees[i]->accessName() = string(1, vars[i]);
	}
	if(getComponentCount() == 1)
			ftrees[0]->accessName() = "";
	name = "Derivative";
}

void function::gradient() {
	if(!processed) createTrees();
	if(components != 1) {
		cout << "The Gradient is only defined to a scalar function. Exiting..." << endl;
		exit(0);
	}

	for(int i = 0; i < variables; i++) {
		if(i == 0)
			ftrees[i] = (differentiator(ftrees[0])).process(vars[i]);
		else
			ftrees.push_back((differentiator(ftrees[0])).process(vars[i]));
		ftrees[i]->accessName() = string(1, vars[i]);
	}
	name = "Gradient";
}

void function::divergence() {
	//div F(x,y,z)=sum(i, 0,n, dF_xi/dx_i);
	vector<node*> n = vector<node*>(ftrees.size());
	for(unsigned int i = 0; i < ftrees.size(); i++) {
		n[i] = differentiator(ftrees[i]).process(vars[i])->getRoot();
	}
	node* n1 = n[0];
	node* n2 = n1;
	for(unsigned int i = 0; i < ftrees.size()-1;i++) {
		n2 = new node('+');
		n2->left = n1;
		n2->right = n[i+1];
		n1 = n2;
	}
	ftrees.clear();
	components = 1;
	ftrees.push_back(new tree(n2));
	ftrees[0]->accessName() = "";
	name = "Divergence";
}

void function::curl() {
	//curl F(x,y,z)=sum(ijk, 0, 3, dF_xi/dx_j*e_k*eps_ijk);
	if(components != 3) {
		cout << "The Curl of a vectorfield is only defined for a 3-dimensional field. Exiting...";
		exit(0);
	}
	vector<tree*> trees = vector<tree*>(ftrees.size());
	for(int i = 0; i < 3; i++) {
		node *n = new node('-');
		n->left = differentiator(ftrees[(i+2)%3]).process(vars[(i+4)%3])->getRoot();
		n->right = differentiator(ftrees[(i+4)%3]).process(vars[(i+2)%3])->getRoot();
		trees[i] = new tree(n);
		trees[i]->accessName() = string(1, vars[i]);
	}
	ftrees = trees;
	name = "Curl";
}

int function::getComponentCount() {
	return components;
}

tree* function::getComponent(int i) {
	return ftrees[i];
}

void function::simplify() {
	for(unsigned int i = 0; i < ftrees.size(); i++)
		ftrees[i] = (simplifier(ftrees[i])).process();
}

string& function::accessName() {
	return name;
}

