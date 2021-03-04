#pragma once
#include "tables.h"

class moonInterpreter
{
private:
	// output file
	ofstream myFile;
	// registers
	bool registers[15] = {false};
	// table
	symbolTable* myTable;
	// stack pointer
	string stack;
	// frame pointer
	string frame;
	// total memory use
	int counter;
	// total temporary memory
	int tempCounter = 0;
	// total if in use
	int ifCounter = 0;
	// total for in use
	int forCounter = 0;
	// check if r2 need to be reset
	bool useR2 = false;
	// get register
	string getReg();
	// free register
	void freeReg(string reg);
	// dimension of array
	int getDimension(string arraySize);
	// pass tree
	void traverse(node* root);
	// process assignStat node
	void assignStat(node* root);
	// process if node
	void ifStatement(node* root);
	// process for node
	void forStatement(node* root);
	// process read node
	void read(node* node);
	// process write node
	void write(node* root);
	// process arithExpr node
	string arithExpr(node* root);
	// process term node in arithexpr
	string term(node* root);
	// process factor node in term
	string factor(node* root);
	// process expr node in term
	string expr(node* root);
	// process statblock node
	void statblock(node* root);
	// get ref given root
	string getVar(node* root);
	// get ref offset of a var
	int getOffset(string varName, node* root, symbolTable* table = NULL);
	// check if node1 is part of node2
	bool partOf(node* n1, node* n2);
	// set a table resRef
	void setTableRes(symbolTable* table);
	// return array size string
	string getType(string name, node* root, symbolTable* table = NULL);
	// return type of all
	string getTypeAll(node* root);
	// return table to string
	symbolTable* getTable(string name, symbolTable* currentTable = NULL);
	// set for offset
	void setFor(int forOffset, node* root, symbolTable* currentTable = NULL);
public:
	// Create variables
	moonInterpreter(symbolTable* tableRoot, string fileName);
	// Make Statements
	void run(node* root);
};

