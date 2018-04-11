#include "../header/simplifier.hpp"
#include "../header/printer.hpp"
#include "../header/functions.hpp"
#include <iostream>
#include <cmath>
using namespace std;
simplifier::simplifier(tree *t) : t(t), current(t->getRoot()) {}

tree* simplifier::process() {
	current = t->getRoot();
	while(checkNode(current)) changed = false;
	return t;
}

bool simplifier::checkNode(node *n) {
	if(!n->left) return false;
	vector<node*> N(0);
	string ops;
	bool couldCombine = false;
	if(*n == "+" || *n == "-") {
		ops = "+-";
		couldCombine = true;
	} else if(*n=="*") {
		ops = "*";
		couldCombine = true;
	} else if(*n=="^") {
		if(n->right->is_Number()) {
			if (n->left->is_Number()) {
				double num = pow(n->left->getNumber(), n->right->getNumber());
				replaceNode(n, node(num));
				changed = true;
			} else if (n->right->getNumber() == 1) {
				replaceNode(n, node(*n->left));
				changed = true;
			} else if (n->right->getNumber() == 0) {
				replaceNode(n, node(1));
				changed = true;
			}
		}
	} else if(*n=="ln" && *n->left =="e") {
		replaceNode(n, node(1));
		changed = true;
	}

	while (couldCombine) {
		vector<node*> N(0);
		findConnections(N, n, ops.size(), ops.c_str());
		couldCombine = false;
		for(unsigned int i = 0; i < N.size(); i++) {
			for(unsigned int j = N.size()-1; j>=0; j--) {
				if(i == j) { couldCombine = false; break;}
				if (N.at(j) != N.at(i) && canCombine(N[i]->left, N[j]->left, N[j]->getValue().c_str()[0])) {
					combine(N.at(i), N[j]);
					couldCombine = true;
					changed = true;
					break;
				}
			}
			if(couldCombine) break;
		}
		/*for (vector<node*>::const_reverse_iterator rit = N.rbegin();
				rit != N.rend(); rit++) {
			if (*rit != N.at(0) && canCombine(N.at(0)->left, (*rit)->left, (*rit)->getValue().c_str()[0])) {
				combine(N.at(0), (*rit));
				couldCombine = true;
				changed = true;
				break;
			}
		}*/

		if(couldCombine) //If n isn't the correct sign (+ or -) the loop has to stop.
			couldCombine = stringContains(ops, n->getValue()[0]);
	}
	return changed;
}

void simplifier::replaceNode(node *n, node newNode) {
	node::remove(n->right);
	node::remove(n->left);
	*n = newNode;
}

void simplifier::findConnections(vector<node*>& N, node *n, int i, const char c[]) {
	bool right = false, left = false;
	//TODO: can't be recursive. Nodes need to be added layer by layer.
	for(int j = 0; j < i; j++) {
		if(*n->left == c[j]) left = true;
		if(*n->right == c[j]) right = true;
	}

	if(!left) {
		checkNode(n->left);
		node* n1 = new node(n->getValue());
		n1->left = n->left;
		n1->right = n;
		N.push_back(n1);
	}

	if(!right) {
		checkNode(n->right);
		node* n1 = new node(n->getValue());
		n1->left = n->right;
		n1->right = n;
		N.push_back(n1);
	}

	if(left) findConnections(N, n->left, i, c);
	if(right) findConnections(N, n->right, i, c);
}

void simplifier::combine(node* aHead, node* bHead) {
	const char op = bHead->getValue().c_str()[0];
	node* a = aHead->left;
	node* aParent = aHead->right;
	node* b = bHead->left;
	node* bParent = bHead->right;
	node* bSibling = (bParent->left == b ? bParent->right : bParent->left);
	/*cout << "combining " << *a << " width " << *b << " op: " << op << endl;
	if(a->left)
		cout << " a left: " << *a->left;
	if(a->right)
		cout << " a right: " << *a->right;
	cout << endl;
	if(b->left)
		cout << " b left: " << *b->left;
	if(b->right)
		cout << " b right: " << *b->right;
	cout << endl;*/
	if (op == '+' || op == '-') {
		if (a->is_Number() && b->is_Number()) {

			if(*aParent == '-') a->getNumber() *= -1;
			a->getNumber() += (op == '-' ? -1 : 1) * (b->getNumber());
		} else if(a->is_Number() && a->getNumber() == 0) {
			*a = node(*b);
		} else if(b->is_Number() && b->getNumber() == 0) {
			//nothing needs to happen, as b will be removed
		} else if (((*a == '*' && *b == '*') || (*a == '/' && *b == '/'))) {
			//One of a's children and one of b's children needs to be a number (necessary criteria to pass canCombine());
			double num = (a->left->is_Number() ? a->left->getNumber() : a->right->getNumber()) * (*aParent == '-' ? -1 : 1);
			num += (op=='-' ? -1 : 1) * (b->left->is_Number() ? b->left->getNumber() : b->right->getNumber());
			if(a->left->is_Number()) a->left->getNumber() = num;
			else if(a->right->is_Number()) a->right->getNumber() = num;
		} else if(*a=='*') {
			//a+b has the form cx+x, which can be combined to (c+1)x
			double num = ((a->left->is_Number() ? a->left->getNumber() : a->right->getNumber()) + 1) * (*aParent == '-' ? -1 : 1);
			*a->left = node(num);
			a->right = new node(*b);
		} else if(*a=='b') {
			//a+b has the form x+cx, which can be combined to (c+1)x
			double num = ((b->left->is_Number() ? b->left->getNumber() : b->right->getNumber()) + 1) * (*aParent == '-' ? -1 : 1);
			*a = node('*');
			*a->left = node(num);
			a->right = new node(*(b->left->is_Number() ? b->right : b->left));
		} else { //a equals b
			a->right = new node(*a);
			*a = node('*');
			a->left = new node(2);
		}
	} else if (op == '*' || op == '/') {
		if (a->is_Number() && b->is_Number()) //a and b can be multiplied
			 a->getNumber() *= b->getNumber();
		else if(a->is_Number()) { //a is either 1 or 0
			if(a->getNumber() == 1) //x*1=x
				*a = node(*b);
			//If a==0 b will be removed and bParent will be set equal to 0
		} else if(b->is_Number()) { //b is either 1 or 0
			if(b->getNumber() == 0) {
				node::remove(a->left);
				node::remove(a->right);
				*a = node(0);
			}
			//if b == 1, bParent will be replaced by bSibling and a will become b.
		} else if (*a == '^' && *b == '^') { //the exponents can be added
			if(a->right->is_Number()) a->right->getNumber() += b->right->getNumber();
			else { //a->left equals b->left, a_l^a_r * b_l^b_r = a_l^(a_r+b_r)
				node* n = new node(op == '*' ? '+' : '-');
				n->left=a->right;
				n->right=b->right;
				a->right = n;
			}
		} else { //a equals b, so a*b=a^2
			a->left = new node(*a);
			*a = node('^');
			a->right = new node(2);
		}

	}
	if (a == bSibling) a = bParent;
	node::remove(b);
	*bParent = node(*bSibling);
	node::remove(bSibling);
	if(!a->right || !a->left) return;
	if(a->right->is_Number() && !(a->left->is_Number())) { //Switch a's children so that left is the number
		double num = a->right->getNumber();
		a->right = a->left;
 		a->left = new node(num);
	}
}

bool simplifier::canCombine(node* a, node* b, const char op) {
	if(op == '+' || op == '-') {
		if(a->is_Number() && b->is_Number()) return true;
		if(a->is_Number() && a->getNumber() == 0) return true;
		if(b->is_Number() && b->getNumber() == 0) return true;
		if(((*a == '*' && *b == '*' ) || (*a=='/' && *b == '/'))) { //it is not possible for both children to be numbers (they'd have been combined already)
			if(a->right->is_Number()) return (b->right->is_Number() ? a->left->equals(b->left) : (b->left->is_Number() ? a->left->equals(b->right) : false));
			else if(a->left->is_Number()) return (b->right->is_Number() ? a->right->equals(b->left) : (b->left->is_Number() ? a->right->equals(b->right) : false));
		} else if(*a == '*') return (a->left->is_Number() ? a->right->equals(b) : a->left->equals(b));
		else if(*b == '*') return (b->left->is_Number() ? b->right->equals(a) : b->left->equals(a));
		return a->equals(b);
	} else if(op == '*' || op == '/') {
		if(a->is_Number() && b->is_Number()) return true;
		else if(a->is_Number() && (a->getNumber() == 0 || a->getNumber() == 1)) return true;
		else if(b->is_Number() && (b->getNumber() == 0 || b->getNumber() == 1)) return true;
		else if(*a== '^' && *b == '^') return a->left->equals(b->left);
		else return a->equals(b);
	} else if(op == '^') return a->is_Number() && b->is_Number();
	return false;
}
