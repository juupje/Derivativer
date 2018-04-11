#include<iostream>
#include<cstdlib>
#include<vector>
#include<string>
#include<windows.h>
#include<stdio.h>
#include "../header/tree.hpp"
#include "../header/differentiator.hpp"
#include "../header/printer.hpp"
#include "../header/simplifier.hpp"
#include "../header/function.hpp"
#include "../header/functions.hpp"
using namespace std;

bool executeCommand(const char* prog, const char* args, bool wait) {
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = prog;
	ShExecInfo.lpParameters = args;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	bool success = ShellExecuteEx(&ShExecInfo);
	if(wait)
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	if(!success) {
		DWORD dw = GetLastError();
		char szMsg[250];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dw, 0, szMsg, sizeof(szMsg), NULL);
		cout << szMsg << endl;
	}
	return success;
}

void help(char* name) {
	cout << "Usage: " << name << " formula(s) [arguments]\n"
		<< "To enter multiple components, separate them by semi-colons." << endl;
	cout << "\nArguments:\n"
			<< "\tp export the current function to the dot file.\n"
			<< "\tl export the current function to the latex file.\n"
			<< "\tP same as p and converts the dot file to pdf.\n"
			<< "\tL same as l and converts the latex file to pdf.\n"
			<< "\tT prints the current function in normal text format.\n"
			<< "\t-d[variable] derives the current function with respect to the given variable, or x if none is given.\n"
			<< "\t-s simplifies the current tree.\n"
			<< "\t--var= sets the components to which the formulas apply. This is mainly used for calculating divergence and curl. Has to be the first argument.\n"
			<< "\t--div calculate the divergence of the current formula.\n"
			<< "\t--grad calculate the gradient of the current formula (only if formula has one component).\n"
			<< "\t--curl calculate the curl of the current formula (only if the formula has three components).\n"
			<< "\t--help -h ? display help." << endl;
}

int main(int c, char* v[]) {
	if(c<2) { cout<<"Incorrect number of arguments (expected at least 1, got " << (c-1) << ")."<<endl; return -1;}
	function func = function(v[1]);
	func.accessName() = "Original function";
	if(stringContains(string(v[1]), ';')) {
		if(c <= 3) {
			cout << "Unexpected amount of arguments for multicomponent function.";
			return -1;
		}
		string vararg = string(v[2]);
		if(!vararg.compare(0, 6, "--var=")) {
			string var = vararg.substr(6, vararg.size());
			func.setVars(&var[0]);
		} else {
			cout << "Please add the var argument when dealing with vector-valued functions. See help for more information." << endl;
			return -1;
		}
	}
	if(c == 2) {
		if(string(v[1]) == "--help" || string(v[1]) == "-h" || string(v[1]) == "?") {
			help(v[0]);
			return 0;
		}
		printer::printDot(&func);
		func.setVars((char*) "x");
		func.derive('x');
		printer::printDot(&func);
		cout << "Creating Latex file..." << endl;
		printer::printLatex(&func);
		printer::printText(&func);
		cout << "Finished!" << endl;
		return 0;
	}

	func.createTrees();
	cout << "Detected " << func.getComponentCount() << " components." << endl;
	string path = string(v[0]);
	path = path.substr(0, path.find_last_of("\\"));
	for(int i = 2; i < c; i++) {
		string arg = string(v[i]);
		if(arg.size() >= 2) {
			if(!arg.compare(0, 2, "-d")) {
				string var = arg.substr(2, arg.size());
				func.derive(var == "" ? 'x' : var[0]);
			}
		}
		if(arg == "p")
			printer::printDot(&func);
		else if(arg == "l")
			printer::printLatex(&func);
		else if(arg == "T")
			printer::printText(&func);
		else if(arg == "P") {
			printer::printDot(&func);
			bool success = executeCommand("dot.exe", "-Tps2 tree.dot -o tree.ps", true);
			if(!success)	cout << "1: Something went wrong: couldn't convert dot to pdf." << endl;
			if(success) success = executeCommand("ps2pdf", "tree.ps", true);
			if(!success)	cout << "2: Something went wrong: couldn't convert dot to pdf." << endl;
			if(success) success = executeCommand("tree.pdf", '\0', false);
			if(!success)	cout << "3: Something went wrong: couldn't open pdf file." << endl;
			else cout << "Successfully created dot file and converted it to pdf format." << endl;
			remove("tree.ps");
		} else if(arg == "L") {
			printer::printLatex(&func);
			bool success = executeCommand("pdflatex", "function.tex", true);
			if(!success)	cout << "1: Something went wrong: couldn't convert latex file to pdf." << endl;
			if(success) success = executeCommand("function.pdf", '\0', false);
			if(!success)	cout << "2: Something went wrong: couldn't open pdf file." << endl;
			else cout << "Successfully created latex file and converted it to pdf format." << endl;
			remove("function.aux");
			remove("function.log");
		} else if(arg == "-s") {
			//tr = (simplifier(tr)).process();
			func.simplify();
			func.accessName() += " simplified";
		} else if(arg == "--div") {
			func.divergence();
		} else if(arg =="--curl") {
			func.curl();
		}
	}
	printer::close();
	return 0;
}
