#include <fstream>
#include <iostream>
#include "../header/printer.hpp"
#include "../header/functions.hpp"
using namespace std;

ofstream *printer::lOut = 0;
ofstream *printer::dOut = 0;
int printer::latexEnd = 0;

void printer::printDot(node *n) {
	*dOut << (unsigned int) n << "[label=\"" << *n << "\"];" << endl;
	if (n->left) {
		*dOut << (unsigned int) n << " -> " << (unsigned int) n->left << endl;
		printDot(n->left);
	}
	if (n->right) {
		*dOut << (unsigned int) n << " -> " << (unsigned int) n->right << endl;
		printDot(n->right);
	}
}


void printer::printDot(tree *t) {
	printDot(t, t->accessName());
}

void printer::printDot(tree *t, string title) {
	if (dOut == 0)
		dOut = new ofstream("tree.dot");
	static int num = 0;
	*dOut << "digraph tree" << num++ << " {" << endl;
	*dOut << "node [fontname=\"Arial\"];" << endl;
	node* root = t->getRoot();
	if (!root)
		*dOut << "\n";
	else if (!root->right && !root->left)
		*dOut << *root << endl;
	else
		printDot(root);
	*dOut << "labelloc=\"t\"\nlabel=\"" << title << "\"\n}" << endl;
}

void printer::printDot(function *func) {
	for(int i = 0; i < func->getComponentCount(); i++) {
		tree *t = func->getComponent(i);
		printDot(t, func->accessName() + (t->accessName() != "" ? " - " + t->accessName() : ""));
	}
}

string printer::printLatex(node *n) {
	string s = "";
	if (n->is_Number()) {
		if (*n == "pi")
			s += "\\pi";
		else
			s += n->getValue() + " ";
	} else if (*n == "+" || *n == "-")
		s += printLatex(n->left) + (*n == "+" ? "+" : "-")
				+ printLatex(n->right);
	else if (*n == "*") {
		string s2 = printLatex(n->right);
		bool brackets = stringContains(s2, '-') || stringContains(s2, '+');
		if(brackets) s2 = "(" + s2;
		if (n->left->is_Number() && !isdigit(s2[0]))
			s += printLatex(n->left) + s2;
		else
		 s += printLatex(n->left) + " \\cdot " + s2;
		if(brackets) s += ")";
	} else if (*n == "/")
		s += " \\frac{" + printLatex(n->left) + "}{" + printLatex(n->right)
				+ "}";
	else if (*n == "^") { // exponents: a^b
		if (n->left->left)
			s += "(";
		s += printLatex(n->left);
		if (n->left->left)
			s += ")";
		s += "^{" + printLatex(n->right) + "} ";
	} else if (isFunction(n->getValue())) {
		string s2 = printLatex(n->left);
		s += "\\" + n->getValue() + "{" + (s2.size() > 2 && n->getValue() != "sqrt" ? "(" + s2 + ")" : s2) + "} ";
	} else
		s += n->getValue() + " ";
	return s;
}

void printer::printLatex(tree *t) {
	if (lOut == 0) {
		lOut = new ofstream("function.tex");
		*lOut << "\\documentclass[preview]{standalone}\n"
				<< "\\usepackage{amsmath}\n"
				<< "\\author{Differentiator - By Joep Geuskens}\n"
				<< "\\begin{document}" << endl;
		latexEnd = lOut->tellp();
		*lOut << "\\end{document}" << endl;
	}
	lOut->seekp(latexEnd);
	*lOut << t->accessName() << ":" << endl;
	*lOut << "\\begin{equation*}" << endl;
	node* root = t->getRoot();
	if (!root)
		return;
	else if (!root->right && !root->left)
		*dOut << *root << endl;
	else
		*lOut << printLatex(root);
	*lOut << "\n\\end{equation*}" << endl;
	latexEnd = lOut->tellp();
	*lOut << "\\end{document}" << endl;
}

void printer::printLatex(function *func) {
	if (lOut == 0) {
			lOut = new ofstream("function.tex");
			*lOut << "\\documentclass[preview]{standalone}\n"
					<< "\\usepackage{amsmath}\n"
					<< "\\author{Differentiator - By Joep Geuskens}\n"
					<< "\\begin{document}" << endl;
			latexEnd = lOut->tellp();
			*lOut << "\\end{document}" << endl;
		}
		lOut->seekp(latexEnd);
		*lOut << func->accessName() << ":" << endl;
		*lOut << "\\begin{equation*}" << endl;
		if(func->getComponentCount() > 1)
			*lOut << "\\begin{pmatrix}" << endl;
		for(int i = 0; i < func->getComponentCount(); i++) {
			node* root = func->getComponent(i)->getRoot();
			if (!root)
				return;
			else if (!root->right && !root->left)
				*dOut << *root << endl;
			else
				*lOut << printLatex(root);
			*lOut << "\\\\";
		}
		if(func->getComponentCount() > 1)
			*lOut << "\n\\end{pmatrix}";
		*lOut << "\n\\end{equation*}" << endl;
		latexEnd = lOut->tellp();
		*lOut << "\\end{document}" << endl;
}

string printer::printText(node *n) {
	string s = "";
	if (n->is_Number()) {
		s += n->getValue();
	} else if (*n == "+" || *n == "-")
		s += printText(n->left) + (*n == "+" ? "+" : "-")
				+ printText(n->right);
	else if (*n == "*") {
		string s2 = printText(n->right);
		bool brackets = stringContains(s2, '-') || stringContains(s2, '+');
		if(brackets) s2 = "(" + s2;
		if (n->left->is_Number() && !isdigit(s2[0]))
			s += printText(n->left) + s2;
		else
			s += printText(n->left) + "*" + s2;
		if(brackets) s += ")";
	} else if (*n == "/") {
		if(n->left->left) s += "(" + printText(n->left) + ")/";
		else s += printText(n->left) + "/";
		if(n->right->left) s+= "(" + printText(n->right) + ")";
		else s += printText(n->right);
	} else if (*n == "^") { // exponents: a^b
		if (n->left->left) s += "(" + printText(n->left) + ")^";
		else s += printText(n->left) + "^";
		if(n->right->left) s += "(" + printText(n->right) + ")";
		else s += printText(n->right);
	} else if (isFunction(n->getValue())) {
		string s2 = printText(n->left);
		s += n->getValue() + (s2.size() > 2 ? "(" + s2 + ")" : s2);
	} else
		s += n->getValue();
	return s;
}

void printer::printText(tree *t) {
	if(t->accessName() != "")
		cout << t->accessName() << ":";
	cout << printText(t->getRoot()) << endl;
}

void printer::printText(function *func) {
	cout << func->accessName() << ":" << endl;
	for(int i = 0; i < func->getComponentCount(); i++) {
		cout << "\t";
		printText(func->getComponent(i));
	}
}

void printer::close() {
	if(lOut)
		lOut->close();
	if(dOut)
		dOut->close();
}
