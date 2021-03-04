#include "lingo.h"

lingo::lingo()
{
	// empty dfa
	set = shared_ptr<dfa> (new dfa);

	// create dfa for given language
	state* start = set->new_startstate();

	// error
	state* errorState = set->new_state(true, false, Type::error_token);
	set->addLink(start, "epsilon", errorState);
	set->addLink(errorState, "epsilon", errorState);

	// id state link
	state* idBridge = set->new_state();
	state* idState = set->new_state(true, true, Type::id_lexical);
	letterLink(start, idBridge);
	alphanumLink(idBridge, idBridge);
	set->addLink(idBridge, "epsilon", idState);

	// integer
	state* altIntBridge = set->new_state();
	state* intBridge = set->new_state();
	set->addLink(start, "0", altIntBridge);
	digitLink(start, intBridge);
	zeroLink(intBridge, intBridge);
	state* intState = set->new_state(true, true, Type::int_lexical);
	set->addLink(altIntBridge, "epsilon", intState);
	set->addLink(intBridge, "epsilon", intState);

	// float
	state* floatState = set->new_state(true, true, Type::float_lexical);
	state* bridgeF1 = set->new_state();
	state* bridgeF2 = set->new_state();
	state* bridgeF3 = set->new_state();
	set->addLink(intBridge, ".", bridgeF1);
	digitLink(bridgeF1, bridgeF2);
	digitLink(bridgeF2, bridgeF2);
	set->addLink(bridgeF1, "0", bridgeF3);
	set->addLink(bridgeF2, "0", bridgeF3);
	set->addLink(bridgeF3, "0", bridgeF3);
	digitLink(bridgeF3, bridgeF2);
	set->addLink(bridgeF2, "epsilon", floatState);
	// exp part
	state* expBridge1 = set->new_state();
	set->addLink(bridgeF2, "e", expBridge1);
	state* expBridge2 = set->new_state();
	state* expBridge3 = set->new_state();
	state* expBridge4 = set->new_state();
	set->addLink(expBridge1, "-", expBridge2);
	digitLink(expBridge2, expBridge3);
	digitLink(expBridge1, expBridge3);
	zeroLink(expBridge3, expBridge3);
	set->addLink(expBridge1, "0", expBridge4);
	set->addLink(expBridge2, "0", expBridge4);
	set->addLink(expBridge4, "epsilon", floatState);
	set->addLink(expBridge3, "epsilon", floatState);

	// dot
	state* dotBridge = set->new_state();
	set->addLink(start, ".", dotBridge);
	state* dotState = set->new_state(true, false, Type::dot);
	set->addLink(dotBridge, "epsilon", dotState);

	// comma
	state* commaBridge = set->new_state();
	set->addLink(start, ",", commaBridge);
	state* commaState = set->new_state(true, false, Type::comma);
	set->addLink(commaBridge, "epsilon", commaState);

	// ;
	state* semcBridge = set->new_state();
	set->addLink(start, ";", semcBridge);
	state* semcState = set->new_state(true, false, Type::semicolon);
	set->addLink(semcBridge, "epsilon", semcState);

	// :
	state* cBridge = set->new_state();
	set->addLink(start, ":", cBridge);
	state* cState = set->new_state(true, true, Type::colon);
	set->addLink(cBridge, "epsilon", cState);
	// ::
	state* ccBridge = set->new_state();
	state* ccState = set->new_state(true, false, Type::double_colon);
	set->addLink(cBridge, ":", ccBridge);
	set->addLink(ccBridge, "epsilon", ccState);

	// <
	state* sBridge = set->new_state();
	set->addLink(start, "<", sBridge);
	state* sState = set->new_state(true, true, Type::s_operator);
	set->addLink(sBridge, "epsilon", sState);
	// <=
	state* seBridge = set->new_state();
	state* seState = set->new_state(true, false, Type::se_operator);
	set->addLink(sBridge, "=", seBridge);
	set->addLink(seBridge, "epsilon", seState);
	// <>
	state* sbBridge = set->new_state();
	state* sbState = set->new_state(true, false, Type::sb_operator);
	set->addLink(sBridge, ">", sbBridge);
	set->addLink(sbBridge, "epsilon", sbState);

	// >
	state* bBridge = set->new_state();
	set->addLink(start, ">", bBridge);
	state* bState = set->new_state(true, true, Type::b_operator);
	set->addLink(bBridge, "epsilon", bState);
	// >=
	state* beBridge = set->new_state();
	state* beState = set->new_state(true, false, Type::be_operator);
	set->addLink(bBridge, "=", beBridge);
	set->addLink(beBridge, "epsilon", beState);

	// =
	state* eBridge = set->new_state();
	set->addLink(start, "=", eBridge);
	state* eState = set->new_state(true, true, Type::assignment);
	set->addLink(eBridge, "epsilon", eState);
	// ==
	state* eeBridge = set->new_state();
	state* eeState = set->new_state(true, false, Type::ee_operator);
	set->addLink(eBridge, "=", eeBridge);
	set->addLink(eeBridge, "epsilon", eeState);

	// +
	state* addBridge = set->new_state();
	set->addLink(start, "+", addBridge);
	state* addState = set->new_state(true, false, Type::addition);
	set->addLink(addBridge, "epsilon", addState);
	// -
	state* minusBridge = set->new_state();
	set->addLink(start, "-", minusBridge);
	state* minusState = set->new_state(true, false, Type::substraction);
	set->addLink(minusBridge, "epsilon", minusState);
	// *
	state* multBridge = set->new_state();
	set->addLink(start, "*", multBridge);
	state* multState = set->new_state(true, false, Type::multiplication);
	set->addLink(multBridge, "epsilon", multState);

	// /
	state* divBridge = set->new_state();
	set->addLink(start, "/", divBridge);
	state* divState = set->new_state(true, true, Type::division);
	set->addLink(divBridge, "epsilon", divState);

	// /**/
	state* mcommBridge = set->new_state(false, false, Type::comment_multiline);
	set->addLink(divBridge, "*", mcommBridge);
	set->addLink(mcommBridge, "epsilon", mcommBridge);
	state* mcommBridge0 = set->new_state(false, false, Type::comment_multiline);
	set->addLink(mcommBridge, "*", mcommBridge0);
	state* mcommBridge1 = set->new_state(false, false, Type::comment_multiline);
	set->addLink(mcommBridge0, "/", mcommBridge1);
	set->addLink(mcommBridge0, "*", mcommBridge0);
	set->addLink(mcommBridge0, "epsilon", mcommBridge);
	state* mcommState = set->new_state(true, false, Type::comment_multiline);
	set->addLink(mcommBridge1, "epsilon", mcommState);

	// //
	state* commBridge = set->new_state(false, false, Type::comment);
	set->addLink(divBridge, "/", commBridge);
	state* commState = set->new_state(true, false, Type::comment);
	set->addLink(commBridge, "epsilon", commState);
	set->addLink(commState, "epsilon", commState);

	// ()
	state* roBridge = set->new_state();
	set->addLink(start, "(", roBridge);
	state* roState = set->new_state(true, false, Type::open_parenthesis);
	set->addLink(roBridge, "epsilon", roState);
	state* rcBridge = set->new_state();
	set->addLink(start, ")", rcBridge);
	state* rcState = set->new_state(true, false, Type::close_parenthesis);
	set->addLink(rcBridge, "epsilon", rcState);
	// {}
	state* boBridge = set->new_state();
	set->addLink(start, "{", boBridge);
	state* boState = set->new_state(true, false, Type::open_brace);
	set->addLink(boBridge, "epsilon", boState);
	state* bcBridge = set->new_state();
	set->addLink(start, "}", bcBridge);
	state* bcState = set->new_state(true, false, Type::close_brace);
	set->addLink(bcBridge, "epsilon", bcState);
	// []
	state* soBridge = set->new_state();
	set->addLink(start, "[", soBridge);
	state* soState = set->new_state(true, false, Type::open_square);
	set->addLink(soBridge, "epsilon", soState);
	state* scBridge = set->new_state();
	set->addLink(start, "]", scBridge);
	state* scState = set->new_state(true, false, Type::close_square);
	set->addLink(scBridge, "epsilon", scState);

	// &&
	state* andBridge = set->new_state();
	set->addLink(start, "&", andBridge);
	state* andBridge0 = set->new_state();
	set->addLink(andBridge, "&", andBridge0);
	state* andState = set->new_state(true, false, Type::and_logic);
	set->addLink(andBridge0, "epsilon", andState);
	// !
	state* notBridge = set->new_state();
	set->addLink(start, "!", notBridge);
	state* notState = set->new_state(true, false, Type::not_logic);
	set->addLink(notBridge, "epsilon", notState);
	// ||
	state* orBridge = set->new_state();
	set->addLink(start, "|", orBridge);
	state* orBridge0 = set->new_state();
	set->addLink(orBridge, "|", orBridge0);
	state* orState = set->new_state(true, false, Type::or_logic);
	set->addLink(orBridge0, "epsilon", orState);
}


lingo::~lingo()
{
}

shared_ptr<dfa> lingo::getLingo() {
	return set;
}

void lingo::updateType(Token* token) {
	if (token->type == Type::id_lexical) {
		// check that id is in map
		unordered_map<string, Type>::const_iterator inTokenMap;
		inTokenMap = tokenFuncMap.find(token->lexeme);
		// change from id to correct one
		if (inTokenMap != tokenFuncMap.end()) {
			token->type = inTokenMap->second;
		}
	}
}

void lingo::digitLink(state* start, state* end) {
	set->addLink(start, "1", end);
	set->addLink(start, "2", end);
	set->addLink(start, "3", end);
	set->addLink(start, "4", end);
	set->addLink(start, "5", end);
	set->addLink(start, "6", end);
	set->addLink(start, "7", end);
	set->addLink(start, "8", end);
	set->addLink(start, "9", end);
}

void lingo::zeroLink(state* start, state* end) {
	set->addLink(start, "0", end);
	set->addLink(start, "1", end);
	set->addLink(start, "2", end);
	set->addLink(start, "3", end);
	set->addLink(start, "4", end);
	set->addLink(start, "5", end);
	set->addLink(start, "6", end);
	set->addLink(start, "7", end);
	set->addLink(start, "8", end);
	set->addLink(start, "9", end);
}

void lingo::letterLink(state* start, state* end) {
	string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < letters.size(); i++) {
		set->addLink(start, letters.substr(i, 1), end);
	}
}

void lingo::alphanumLink(state* start, state* end) {
	set->addLink(start, "_", end);
	letterLink(start, end);
	zeroLink(start, end);
}