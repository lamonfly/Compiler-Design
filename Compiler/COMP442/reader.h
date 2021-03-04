#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "lingo.h"

class reader
{
private:
	// lingo
	lingo* lang;
	// links and etc for given lingo
	shared_ptr<dfa> tokenizer;
	// file stream
	ifstream file;
	// index
	int line;
	int index;
	string lineData;
	// logic
	bool firstToken;
	bool eotBool;
	bool eol = true;

public:
	reader();
	// read with specific lingo
	reader(lingo* ling);
	~reader();
	bool setFile(string path);
	// give final token
	Token nextToken();
	// get partial token
	Token getToken();
	// to next line
	void nextLine();
	// end of tokens
	bool eot();
	// get eol token
	Token getEolToken();
};

