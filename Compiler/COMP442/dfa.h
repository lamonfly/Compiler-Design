#pragma once
#include <unordered_map>
#include <string>
#include "token.h"

using namespace std;

struct state {
	// identifier of the state
	int id;
	// start or final state
	bool isStart;
	bool isFinal;
	// can backtrack
	bool testForward;
	// lexeme it accepts
	Type token_type;
};

class dfa
{
private:
	// how many states there are
	int state_count;
	// structure that stores the table
	unordered_map<int, shared_ptr<unordered_map<string, state*>>> link_table;
	vector<state*> states;
	state* start_state;

public:
	dfa();
	~dfa();
	// makes start state for dfa
	state* new_startstate();
	// makes state for dfa
	state* new_state(bool isFinal = false, bool testForward = false, Type type = not_token);
	// add link between states using string
	void addLink(state* from, string link, state* to);
	// get the link state from from
	state* table(int from, string link);
	// returns the ouput state
	state* getOutput(string input);
	// return start state
	state* getStart();
};