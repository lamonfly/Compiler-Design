#include "dfa.h"

dfa::dfa()
{
	state_count = 0;
	start_state = NULL;
}

dfa::~dfa()
{
	// remove states
	for (state* i: states) {
		delete i;
	}
	states.clear();
}

state* dfa::new_startstate() {
	start_state = new_state();
	start_state->isStart = true;
	return start_state;
}

state* dfa::new_state(bool isFinal, bool testForward, Type type) {
	// add state to the states
	state* nState = new state{ ++state_count, false, isFinal, testForward, type};
	states.push_back(nState);
	// create link map for this state
	shared_ptr<unordered_map<string, state*>> links;
	links = shared_ptr<unordered_map<string, state*>>(new unordered_map<string, state*>);
	// add link to our links table
	link_table.emplace(nState->id, links);
	return nState;
}

void dfa::addLink(state* from, string link, state* to) {
	// check if state is there
	if (link_table.find(from->id) != link_table.end() && link_table.find(to->id) != link_table.end()) {
		// make link
		shared_ptr<unordered_map<string, state*>> links;
		links = link_table.at(from->id);
		links->emplace(link, to);
	}
}

state* dfa::table(int from, string link) {
	shared_ptr<unordered_map<string, state*>> links;
	links = link_table.at(from);
	unordered_map<string, state*>::iterator found = links->find(link);
	// no transition for this state
	if (found == links->end()) {
		return NULL;
	}

	return found->second;
}

state* dfa::getOutput(string input) {
	int index = 0;
	string links;
	string finalInput = input + " ";
	state* current = start_state;

	while (index <= finalInput.length()) {
		links = finalInput.substr(index, 1);
		state* nState = table(current->id, links);

		if (nState != NULL) {
			// next state
			current = nState;
		}
		else {
			// check for epsilon
			state* eState = table(current->id, "epsilon");
			if (eState == NULL) {
				return NULL;
			}
			// change to epsilon
			current = eState;
		}

		if (current != NULL) {
			index++;
			if (current->isFinal && index >= finalInput.length()) {
				return current;
			}
		}
	}

	return current;
}

state* dfa::getStart() {
	return start_state;
}