#include <ctype.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include "../header/tree.hpp"
#include "../header/functions.hpp"
using namespace std;

void tree::nextChar() {
	if (s[++pos] != 0) {
		c = s[pos];
	} else
		c = -1;
}

bool tree::consume(const char chToCheck) {
	while (c == ' ')
		nextChar();
	if (c == chToCheck) {
		nextChar(); // Consume the character by moving to the next one
		return true;
	}
	return false;
}


node* tree::getFactor() {
	node* n;
	int position = pos;
	if(c == '(') {
		tree t2(s, pos);
		pos = t2.getPosition();
		c = s[pos];
		return t2.getRoot();
	} else if(c==')')
		return 0;
	else if (isdigit(c) || c == '.' || c=='-') {
		do {
			nextChar();
		} while(isdigit(c) || c == '.');
		string s1 = s.substr(position, pos - position);
		if(s1 == "-") {
			if(p == 0) root = p = new node('*');
			else *p = node('*');
			p->left = new node(-1.0);
			p->right = getFactor();
			return p->right;
		} else
			n = new node(stod(s1));
	} else if (c == 'x') {
		n = new node('x');
		nextChar();
	} else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			nextChar();
		string str = s.substr(position, pos-position);
		if(str == "pi" || str == "e")
			n = new node(str);
		else if(isFunction(str)) {
			n = new node(str);
			n->left = getFactor();
		} else if(str.size() == 1) { //Treat as variable
			n = new node(str);
		} else {
			cout << "Unknown function: " << str << endl;
			exit(-1);
		}
	}
	return n;
}

node* tree::getTerm() {
	node *n;
	if(consume(')'))
		return 0;
	else if(consume('+')) {
		n = new node('+');
		n->left = root;
		n->right = getFactor();
		root = n;
		return n->right;
	} else if(consume('-')) {
		n = new node('-');
		n->left = root;
		n->right = getFactor();
		root = n;
		return n->right;
	} else if(consume('*')) {
		n = new node(*p);
		*p = node('*');
		p->left = n;
		p->right = getFactor();
		return p->right;
	} else if(consume('/')) {
		n = new node(*p);
		*p = node('/');
		p->left = n;
		p->right = getFactor();
		return p->right;
	} else if(consume('^')) {
		n = new node(*p);
		*p = node('^');
		p->left = n;
		p->right = getFactor();
		return p;
	} else if(!isdigit(c)) {
		n = new node(*p);
		*p = node('*');
		p->left = n;
		p->right = getFactor();
		return p->right;
	} else if(consume('(')) {
		tree t2(s, pos);
		pos = t2.getPosition();
		c = s[pos];
		return t2.getRoot();
	} else {
		cout << "Unknown character: " << c << endl;
		exit(-1);
	}
	return 0;
}

int tree::getPosition() { return pos;}

tree::tree(char* v[]) : subtree(false), currentID(id()), root(0), pos(-1) {
	s = string(*v);
	name = s;
	init();
}

tree::tree(string str, int &p) : subtree(true), currentID(id()), root(0), pos(p), name(str) {
	s = str;
	init();
}

tree::tree(string str) : subtree(false), currentID(id()), root(0), pos(-1), name(str) {
	s = str;
	init();
}

tree::tree(node* n) : subtree(false), currentID(id()), root(n), pos(-1), s(""), name("") {}

tree::~tree() {
	if(!subtree)
		remove(root);
}

void tree::remove(node *n) {
	node::remove(n);
}

void tree::init() {
	nextChar();
	root = p = getFactor();
	while(c>0 && p != 0 && p!=nullptr) {
		p = getTerm();
	}
}

node* tree::getRoot() {
	return root;
}

string& tree::accessName() {return name;}

string tree::getFunction() { return s;}
