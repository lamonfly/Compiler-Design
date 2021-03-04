#pragma once
#include "grammar.h"
#include <iomanip>

// Table entry
struct symbol{
	string name;
	string kind;
	string type;
	node* treeLink;
	int resRef;
};

struct symbolTable{
	string name;
	// Stores table entry
	vector<pair<symbol*, symbolTable*>> symbols;
	// Where table is in tree
	node* root;
	int resRef = 0;
};

class tables
{
private:
	// Store all symbol tables
	vector<symbolTable*> allSymbolTables;
	// Create class table
	symbolTable* classTable(node* root);
	// Create function table
	symbolTable* funcTable(node* root, string name);
	// reformat tables
	void fixTables();
	// test for code errors
	bool testTables(node* root);
	// test for duplicate entries
	bool testDuplicates();
	// get position of error
	string getPos(node* root);
	// get variable name
	string getVar(node* root);
	// get all variable (return nothing if they are not the same)
	string getVarAll(node* root);
	// get type from table
	string getType(string value, node* root = NULL);
	// return if subtree is part of subtree
	bool partOf(node* n1, node* n2);
	// return term type
	string getTerm(node* root);
	// review type
	string reviewType(string value);
public:
	// initialise
	tables();
	//
	bool run(node* root);
	// print content
	void printTables();
	// get tables
	symbolTable* getTable();
};

