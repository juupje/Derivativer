#pragma once
#include<vector>
#include "../header/tree.hpp"
#include "../header/node.hpp"

class simplifier {
	tree *t;
	node* current;
	bool changed = false;
	bool checkNode(node*);
	void findConnections(vector<node*>&, node*, int, const char[]);
	bool canCombine(node*, node*, const char);
	void combine(node*, node*);
	void replaceNode(node* n, node newNode);
public:
	simplifier(tree*);
	tree* process();
};
