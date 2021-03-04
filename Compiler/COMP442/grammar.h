#pragma once
#include "lingo.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

struct line{
	string identifier;
	vector<string> terms;
};

struct node {
	string identifier;
	vector<node*> childrens;
	Token terminalToken;
};

class grammar
{
private:
	// file stream
	ifstream file;
	// First identifier
	line* startLine;
	// grammar info
	vector<line*> lines;
	// tokens to be used for tree
	vector<Token> tokens;
	// index for tokens
	int tokenIndex = 0;
	// error index for tokens
	int errorIndex = 0;
	// language
	lingo* ling;
	// complex tree
	vector<node*> tree;
public:
	grammar(string path);
	~grammar();
	// for debug
	void printLines();
	// find exisiting line
	line* find(string identifier);
	// create tree structure
	bool makeTree(vector<Token> newTokens, lingo* lang);
	// print tree information
	void printTree();
	// return tree graph
	vector<node*> getTree();
	// make the tree somewhat readable
	bool fixTree();
	// clear node
	void clear(node* toClear);
	// get tree leaf
	bool getLeaf(string identifier, node* parentNode = NULL);
};

