#pragma once
#include "node.hpp"
#include "tree.hpp"
#include "function.hpp"

class printer {
	static string printLatex(node *n);
	static string printText(node *n);
	static void printDot(node *n);
	static ofstream *dOut;
	static ofstream *lOut;
	static int latexEnd;
public:
	static void initialize();
	static void printLatex(tree *t);
	static void printLatex(function *func);
	static void printText(tree *t);
	static void printText(function *func);
	static void printDot(tree *t);
	static void printDot(tree *t, string name);
	static void printDot(function *func);
	static void setDotFile(char v[]);
	static void close();
};
