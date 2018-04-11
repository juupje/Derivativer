#include "../header/tree.hpp"
#include "../header/differentiator.hpp"
#include "../header/functions.hpp"
#include "../header/printer.hpp"
#include <iostream>
using namespace std;

differentiator::differentiator(tree *t) : t(t), current(0), prev(0) {
}

differentiator::~differentiator() {
}

tree* differentiator::process(char _variable) {
	cout << "Deriving " << t->getFunction() << " with respect to " << _variable << endl;
	variable = _variable;
	tree *tr;
	tr = new tree(getDerivative(t->getRoot()));
	tr->accessName() = "Derivative";
	cout << "Derivative: ";
	printer::printText(tr);
	return tr;
}

node* differentiator::getDerivative(node *n) {
	cout << "Getting derivative of " << *n << endl;
	if(isConstant(n)) return new node(0);
	if(*n == variable) return new node(1);
	node *d;
	if(*n == "+") {
		//derivative is f'+g'
		d = new node('+');
		d->right = getDerivative(n->right);
		d->left = getDerivative(n->left);
	} else if(*n == "-") {
		//derivative is f'-g'
		d = new node('-');
		d->right = getDerivative(n->right);
		d->left = getDerivative(n->left);
	} else if(*n == "*") {
		//derivative is f'g+fg'
		if(n->left->is_Number()) { //f'=0
			d = new node('*');
			d->left = new node(*n->left);
			d->right = getDerivative(n->right);
		} else {
			d = new node('+');
			d->left = new node('*');
			d->right = new node('*');
			d->left->left = new node(*n->left);
			d->left->right = getDerivative(n->right);
			d->right->left = getDerivative(n->left);
			d->right->right = new node(*n->right);
		}
	} else if(*n == "/") { //fractions: n/d
		if(n->left->is_Number() && n->right->is_Number()) //n/d is a number => derivative = 0
			d = new node(0);
		else if(n->right->is_Number()) { //d is a number: derivative = n'/d
			d = new node('/');
			d->left = getDerivative(d->left);
			d->right = new node(*d->right);
		} else { //derivative is (nd'-dn')/n^2
			d = new node('/');
			d->right = new node('^');
			d->right->left = new node(*n->right);
			d->right->right = new node(2);
			d->left = new node('-');
			d->left->left = new node('*');
			d->left->right = new node('*');
			d->left->left->left = new node(*n->right); //denominator
			d->left->left->right = getDerivative(n->left); //derivative of numerator
			d->left->right->left = getDerivative(n->right); //derivative of denominator
			d->left->right->right = new node(*n->left); //numerator
		}
	} else if(*n == "^") { // exponents: a^b
		if(isConstant(n->left) && isConstant(n->right)) return new node(0); // a^b is a number => derivative = 0
		if(n->right->is_Number()) { //b is a constant => derivative is b*a^(b-1)*a'
			d = new node('*');
			d->right = new node('^');
			d->right->left = new node(*n->left);
			if(!(*n->left == "e" || *n->left == "pi")) {
				d->left = new node(double(n->right->getNumber()));
				d->right->right = new node(double(n->right->getNumber() - 1));
			} else {
				d->left = new node(n->right->getValue());
				d->right->right = new node('-');
				d->right->right->left = new node(n->right->getValue());
				d->right->right->right = new node(1);
			}
		} else if(n->left->is_Number()){ //a is a constant => derivative is a^b*ln(a)*b'
			d = new node('*');
			d->left = new node(*n);
			if(*n->left == "e") //a is e
				//derivative is a^b*b'
				d->right = getDerivative(n->right);
			 else {
				 //derivative is a^b*ln(a)*b'
				d->right = new node('*');
				d->right->left = new node("ln");
				d->right->left->left = new node(*n->left);
				d->right->right = getDerivative(n->right);
			 }
		}
	} else if(isFunction(n->getValue())) {
		if(*n == "sin") {
			//derivative is cos(f)*f'
			d = new node('*');
			d->left = new node("cos");
			d->left->left = new node(*n->left);
			d->left->accessText() = "cos";
			d->right = getDerivative(n->left);
		} else if(*n == "cos") {
			//derivative is -1*sin(f)*f'
			d = new node('*');
			d->left = new node('*');
			d->left->left = new node(-1);
			d->left->right = new node("sin");
			d->left->right->left = new node(*n->left);
			d->right = getDerivative(n->left);
		} else if(*n == "tan") {
			//derivative is f'/cos(f)^2
			d = new node('/');
			d->left = getDerivative(n->left);
			d->right = new node('^');
			d->right->left = new node("cos");
			d->right->left->left = new node(*n->left);
			d->right->right = new node(2);
		} else if(*n == "ln") {
			//derivative = f'/f
			d = new node('/');
			d->left = getDerivative(n->left);
			d->right = new node(*n->left);
		} else if(*n == "log") {
			//derivative is f'/(f*ln(10))
			d = new node('/');
			d->right = new node('*');
			d->right->left = new node(*n->left);
			d->right->right = new node("ln");
			d->right->right->left = new node(10);
			d->left = getDerivative(n->left);
		} else if(*n == "exp") {
			//derivative of exp(f) is f'*exp(f);
			d = new node('*');
			d->left= getDerivative(n->left);
			d->right = new node(*n);
		} else if(*n == "acos") {
			//derivative of acos(f) is -f'/sqrt(1-f^2)
			d = new node('/');
			d->left = new node('*');
			d->left->left= new  node(-1);
			d->left->right = getDerivative(n->left);
			d->right = new node("sqrt");
			d->right->left = new node('-');
			d->right->left->left = new node(1);
			d->right->left->right = new node('^');
			d->right->left->right->left = new node(*n->left);
			d->right->left->right->right = new node(2);
		} else if(*n == "asin") {
			//derivative of asin(f) is f'/sqrt(1-f^2);
			d = new node('/');
			d->left = getDerivative(n->left);
			d->right = new node("sqrt");
			d->right->left = new node('-');
			d->right->left->left = new node(1);
			d->right->left->right = new node('^');
			d->right->left->right->left = new node(*n->left);
			d->right->left->right->right = new node(2);
		} else if(*n == "atan") {
			//Derivative of atan(f) is f'/(1+x^2);
			d = new node('/');
			d->left = getDerivative(n->left);
			d->right = new node('+');
			d->right->right = new node(1);
			d->right->left = new node('^');
			d->right->left->left = new node(*n->left);
			d->right->left->right = new node(2);
		} else if(*n == "sinh") {
			//derivative of sinh(f) is cosh(f)*f'
			d = new node("*");
			d->left = new node("cosh");
			d->left->left = new node(*n->left);
			d->right = getDerivative(n->left);
		} else if(*n == "cosh") {
			//Derivative of cosh(f) is sinh(f)*f'
			d = new node("*");
			d->left = new node("sinh");
			d->left->left = new node(*n->left);
			d->right = getDerivative(n->left);
		} else if(*n == "tanh") {
			//Derivative of tanh(f) is (1-tanh(f)^2)*f'
			d = new node('*');
			d->left = new node('-');
			d->left->left = new node(1);
			d->left->right = new node('^');
			d->left->right->left = new node(*n);
			d->left->right->right = new node(2);
			d->right = getDerivative(n->left);
		} else if(*n == "abs") {
			//Derivative of |x| = sgn(x) = x/|x|*x' (undefined for x=0)
			d = new node('*');
			d->left = new node('/');
			d->left->left = new node(*n->left);
			d->left->right = new node(*n);
			d->right = getDerivative(n->left);
		} else if(*n == "sqrt") {
			//Derivative of sqrt(f)=f/(2*sqrt(f))
			d = new node('/');
			d->left = getDerivative(n->left);
			d->right = new node('*');
			d->right->left = new node(2);
			d->right->right = new node(*n);
		}
	} else {
		cout << "unknown symbol " << n->getValue() << " treating it as a constant." << endl;
		d = new node(0);
	}
	return d;
}

bool differentiator::isConstant(node *n) {
	if (!n->left && !n->right && !(*n == variable)) return true;
	if(!n->right) { //only left exists, which means that n must be a function

	}
	return false;
}
