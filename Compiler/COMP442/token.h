#pragma once
#include <iostream>

enum Type {
	// ND tokens
	not_token,	// This is not a valid token
	error_token,
	eol_token,

// Tokens in language
/*
e = equal
a = smaller
b = bigger
*/
	// Lexicals
	id_lexical,
	int_lexical,
	float_lexical,
	// Operators first row
	ee_operator,
	sb_operator,
	s_operator,
	b_operator,
	se_operator,
	be_operator,
	semicolon,
	comma,
	dot,
	colon,
	double_colon,
	// Operators second row
	addition,
	substraction,
	multiplication,
	division,
	assignment,
	and_logic,
	not_logic,
	or_logic,
	// Operators third row
	open_parenthesis,
	close_parenthesis,
	open_brace,
	close_brace,
	open_square,
	close_square,
	comment_multiline,
	comment,
	// Operators fourth line
	if_operator,
	then_operator,
	else_operator,
	for_operator,
	class_operator,
	integer_operator,
	float_operator,
	read_operator,
	write_operator,
	return_operator,
	main_operator
};

struct Token {
	// lexeme of token
	std::string lexeme;
	// token type
	Type type;
	// token line number
	int line;
	// token index
	int index;
};