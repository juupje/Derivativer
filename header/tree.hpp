#pragma once
#include "node.hpp"
#include <fstream>
class tree {
public:
	tree(char *v[]);
	tree(string s, int &pos);
	tree(node* root);
	tree(string s);
	~tree();
	int getPosition();
	node* getRoot();
	string& accessName();
	string getFunction();
private:
	bool subtree = false;
	static int id() {
		static int ID=0;
		return ID++;
	}
	int currentID;
	node *root = 0, *p = 0;
	int pos;
	string s, name;
	char c;
	node* getTerm();
	node* getFactor();
	bool consume(const char chToCheck);
	void nextChar();
	void init();
	void remove(node *n);
};
