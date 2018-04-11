#pragma once
#include "../header/tree.hpp"

class differentiator {
	tree* t;
	node* current, *prev;
	char variable = 'x';
	bool isConstant(node *n);
public:
	differentiator(tree *t);
	~differentiator();
	tree* process(char);
	node* getDerivative(node* n);
};
