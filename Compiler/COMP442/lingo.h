#pragma once
#include "dfa.h"

class lingo
{
private:
	shared_ptr<dfa> set;
public:
	// all map from lexemes
	unordered_map<string, Type> tokenFuncMap = {
		{"if", Type::if_operator},
		{"then", Type::then_operator},
		{"else", Type::else_operator},
		{"for", Type::for_operator},
		{"class", Type::class_operator},
		{"integer", Type::integer_operator},
		{"float", Type::float_operator},
		{"read", Type::read_operator},
		{"write", Type::write_operator},
		{"return", Type::return_operator},
		{"main", Type::main_operator}
	};

	// default uses set
	lingo();
	~lingo();
	// return dfa for this sets
	shared_ptr<dfa> getLingo();
	// update token given lexeme
	void updateType(Token* token);

	// create digit [1-9]
	void digitLink(state* start, state* end);
	// create non zero [0-9]
	void zeroLink(state* start, state* end);
	// create letter
	void letterLink(state* start, state* end);
	// create num
	void alphanumLink(state* start, state* end);
};