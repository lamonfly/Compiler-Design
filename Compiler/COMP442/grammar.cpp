#include "grammar.h"

grammar::grammar(string path)
{
	file.open(path);
	string stringLine = "";
	line* prevLine = NULL;

	if (file.is_open()) {
		while (getline(file, stringLine)) {
			line* tempLine = new line;
			istringstream iss(stringLine);
			vector<string> stringTokens(istream_iterator<string>{iss}, istream_iterator<string>());

			if (stringTokens.size() >= 2 && stringTokens[1] == "->") {
				tempLine->identifier = stringTokens[0];
				stringTokens.erase(stringTokens.begin() ,stringTokens.begin() + 2);
			}
			else if (stringTokens.size() >= 2 && stringTokens[0] == "|"){
				tempLine = prevLine;
			}

			for (std::vector<string>::size_type i = 0; i != stringTokens.size(); i++) {
				tempLine->terms.push_back(stringTokens[i]);
			}

			prevLine = tempLine;
			if (find(tempLine->identifier) == NULL) {
				lines.push_back(tempLine);
			}
		}
		file.close();
	}

	startLine = lines.at(0);
}

grammar::~grammar()
{
}

void grammar::printLines() {
	ofstream myfile;
	myfile.open("Output/grammarVerification.txt");
	myfile << "Start: " << &startLine->identifier << endl;

	for (vector<line*>::size_type i = 0; i != lines.size(); i++) {
		myfile << lines.at(i)->identifier << " (" << lines.at(i) << ")" <<" -> ";
		for (int j = 0; j != lines.at(i)->terms.size(); j++) {
			if (lines.at(i)->terms.at(j) == "|"){
				myfile << "OR ";
			}
			else {
				if (find(lines.at(i)->terms.at(j))) {
					myfile << find(lines.at(i)->terms.at(j)) << " ";
				}
				else {
					myfile << "TERMINAL ";
				}
			}
		}
		myfile << endl;
	}

	myfile.close();
}

line* grammar::find(string identifier) {
	for (vector<line*>::size_type i = 0; i != lines.size(); i++) {
		if (lines[i]->identifier == identifier) {
			return lines[i];
		}
	}
	return NULL;
}

bool grammar::makeTree(vector<Token> newTokens, lingo* lang) {
	tokens = newTokens;
	ling = lang;
	tokenIndex = 0;

	do {
		if (getLeaf(startLine->identifier)) {
			cout << "Grammar tree valid!" << endl;
			break;
		}
		else {
			if (errorIndex == tokens.size() - 1) {
				cout << "Grammar tree invalid." << endl;
				cout << "Unable to correct." << endl;
				return false;
			}
			else {
				if (tokens.size() > errorIndex) {
					cout << "Error token " << tokens.at(errorIndex).lexeme << " at line: " << tokens.at(errorIndex).line << " and index: " << tokens.at(errorIndex).index << endl;
					tokens.erase(tokens.begin() + errorIndex);
				}
				else {
					return false;
				}
			}
		}
	} while (true);
	fixTree();
	return true;
}

void grammar::printTree() {
	ofstream myfile;
	myfile.open("Output/treeAST.txt");

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		if (tree.at(i)->childrens.size() > 0) {
			myfile << tree.at(i)->identifier << " -> ";
			for (vector<node*>::size_type j = 0; j != tree.at(i)->childrens.size(); j++) {
				myfile << tree.at(i)->childrens.at(j)->identifier << " ";
			}
			myfile << endl;
		}
	}

	myfile.close();
}

vector<node*> grammar::getTree() {
	return tree;
}

bool grammar::fixTree() {
	vector<node*> toDelete;

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		if (tree.at(i)->childrens.size() <= 0) {
			if (find(tree.at(i)->identifier) != NULL || tree.at(i)->identifier.at(0) == '\'') {
				toDelete.push_back(tree.at(i));
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		for (vector<node*>::size_type j = 0; j != tree.at(i)->childrens.size(); j++) {
			for (vector<node*>::size_type k = 0; k != toDelete.size() ; k++) {
				if (tree.at(i)->childrens.at(j) == toDelete.at(k)) {
					tree.at(i)->childrens.erase(tree.at(i)->childrens.begin() + j);
					j--;
					break;
				}
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		if (tree.at(i)->childrens.size() <= 0) {
			if (find(tree.at(i)->identifier) != NULL || tree.at(i)->identifier.at(0) == '\'') {
				tree.erase(tree.begin() + i);
				i--;
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		for (vector<node*>::size_type j = 0; j != tree.at(i)->childrens.size(); j++) {
			if (tree.at(i)->childrens.at(j)->childrens.size() <= 0) {
				if (find(tree.at(i)->childrens.at(j)->identifier) != NULL || tree.at(i)->childrens.at(j)->identifier.at(0) == '\'') {
					toDelete.push_back(tree.at(i)->childrens.at(j));
					tree.at(i)->childrens.erase(tree.at(i)->childrens.begin() + j);
					j--;
				}
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		for (vector<node*>::size_type j = 0; j != tree.at(i)->childrens.size(); j++) {
			if (tree.at(i)->childrens.at(j)->identifier.at(0) == '\'') {
				toDelete.push_back(tree.at(i)->childrens.at(j));
				tree.at(i)->childrens.at(j) = tree.at(i)->childrens.at(j)->childrens.at(0);
			}
			else if (tree.at(i)->childrens.at(j)->identifier.find("Sub") != string::npos || tree.at(i)->childrens.at(j)->identifier.find("varCall1") != string::npos) {
				toDelete.push_back(tree.at(i)->childrens.at(j));
				tree.at(i)->childrens.insert(tree.at(i)->childrens.begin() + j, toDelete.back()->childrens.begin(), toDelete.back()->childrens.end());
				tree.at(i)->childrens.erase(tree.at(i)->childrens.begin() + j + toDelete.back()->childrens.size());
				j--;
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		if (tree.at(i)->identifier == "varCall0") {
			if (tree.at(i)->childrens.at(tree.at(i)->childrens.size() - 1)->identifier == "functionCall") {
				toDelete.push_back(tree.at(i)->childrens.at(tree.at(i)->childrens.size() - 1));
				tree.at(i)->childrens.insert(tree.at(i)->childrens.end() - 1, toDelete.back()->childrens.begin(), toDelete.back()->childrens.end());
				tree.at(i)->childrens.erase(tree.at(i)->childrens.begin() + tree.at(i)->childrens.size() - 1);
				tree.at(i)->identifier = "functionCall";
			}
			else {
				tree.at(i)->identifier = "variable";
			}
		}
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		for (vector<node*>::size_type j = 0; j != toDelete.size(); j++) {
			if (toDelete.at(j) == tree.at(i)) {
				tree.erase(tree.begin() + i);
				i--;
				break;
			}
		}
	}

	int size = toDelete.size();

	for (vector<node*>::size_type k = 0; k != toDelete.size(); k++) {
		delete toDelete.at(k);
	}

	return size == 0;
}

void grammar::clear(node* toClear) {
	for (vector<node*>::size_type i = 0; i != toClear->childrens.size(); i++) {
		clear(toClear->childrens.at(i));
		i--;
	}

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		for (vector<node*>::size_type j = 0; j != tree.at(i)->childrens.size(); j++) {
			if (toClear == tree.at(i)->childrens.at(j)) {
				tree.at(i)->childrens.erase(tree.at(i)->childrens.begin() + j);
				j--;
			}
		}
	}

	vector<node*> toDelete;

	for (vector<node*>::size_type i = 0; i != tree.size(); i++) {
		if (toClear == tree.at(i)) {
			toDelete.push_back(tree.at(i));
			tree.erase(tree.begin() + i);
			i--;
		}
	}

	for (vector<node*>::size_type i = 0; i != toDelete.size(); i++) {
		delete toDelete.at(i);
	}
}

bool grammar::getLeaf(string identifier, node* parentNode) {
	line* parent = find(identifier);
	bool compile = true;
	if (parentNode == NULL) {
		node* tempNode = new node;
		tempNode->identifier = identifier;
		tree.push_back(tempNode);
		parentNode = tempNode;
	}

	// terminal non looping
	if (parent == NULL) {
		do {
			if (tokens.size() > tokenIndex && (tokens.at(tokenIndex).type == Type::eol_token || tokens.at(tokenIndex).type == Type::comment || tokens.at(tokenIndex).type == Type::comment_multiline)) {
				tokenIndex++;
				if (errorIndex < tokenIndex)
					errorIndex = tokenIndex;
			}
			else {
				break;
			}
		} while (true);

		Token tempToken;
		string substring = identifier.substr(1, identifier.size() - 2);
		tempToken.type = ling->getLingo()->getOutput(substring)->token_type;
		tempToken.lexeme = substring;
		ling->updateType(&tempToken);
		if (tokens.size() > tokenIndex && tokens.at(tokenIndex).type == tempToken.type) {
			node* tempChildNode = new node;
			tempChildNode->identifier = tokens.at(tokenIndex).lexeme;
			tempChildNode->terminalToken = tokens.at(tokenIndex);
			parentNode->childrens.push_back(tempChildNode);
			tree.push_back(tempChildNode);
			tokenIndex++;
			if (errorIndex < tokenIndex)
				errorIndex = tokenIndex;
			return true;
		}
		else {
			return false;
		}
	}
	
	for (vector<string>::size_type i = 0; i != parent->terms.size(); i++) {

		// Stop line if no longer valid
		if (!compile) {
			for (; i != parent->terms.size(); i++) {
				if (parent->terms.at(i) == "|") {
					break;
				}
				else if (i == parent->terms.size() - 1) {
					return compile;
				}
			}
		}

		// OR
		if (parent->terms.at(i) == "|") {
			if (!compile) {
				compile = true;
			}
			else {
				return true;
			}
		}

		// Loop zero or more time
		else if (parent->terms.at(i) == "{") {
			int tempIndex = i;
			int tempTokenIndex = tokenIndex;
			vector<node*> nodeInBrackets;
			do {
				i++;
				if (parent->terms.at(i) == "}") {
					i = tempIndex;
				}
				else {
					node* tempChildNode = new node;
					tempChildNode->identifier = parent->terms.at(i);
					parentNode->childrens.push_back(tempChildNode);
					tree.push_back(tempChildNode);
					if (!getLeaf(parent->terms.at(i), tempChildNode)) {
						clear(tempChildNode);
						tokenIndex = tempTokenIndex;
						do {
							i++;
							if (parent->terms.at(i) == "}") {
								goto endBracket;
							}
						} while (true);
					}
					else {
						tempTokenIndex = tokenIndex;
					}
				}
			} while (true);
			endBracket:;
		}

		// Loop zero or one time
		else if (parent->terms.at(i) == "[") {
			int tempTokenIndex = tokenIndex;
			vector<node*> nodeInBrackets;
			do {
				i++;
				if (parent->terms.at(i) == "]") {
					goto endSquare;
				}
				// Inside loop zero or more times
				else if (parent->terms.at(i) == "{") {
					int tempIndex = i;
					int tempTokenIndex = tokenIndex;
					do {
						i++;
						if (parent->terms.at(i) == "}") {
							i = tempIndex;
						}
						else {
							node* tempChildNode = new node;
							tempChildNode->identifier = parent->terms.at(i);
							parentNode->childrens.push_back(tempChildNode);
							tree.push_back(tempChildNode);
							if (!getLeaf(parent->terms.at(i), tempChildNode)) {
								clear(tempChildNode);
								tokenIndex = tempTokenIndex;
								do {
									i++;
									if (parent->terms.at(i) == "}") {
										goto endBracket2;
									}
								} while (true);
							}
							else {
								tempTokenIndex = tokenIndex;
							}
						}
					} while (true);
				endBracket2:;
				}
				else {
					node* tempChildNode = new node;
					tempChildNode->identifier = parent->terms.at(i);
					parentNode->childrens.push_back(tempChildNode);
					tree.push_back(tempChildNode);
					nodeInBrackets.push_back(tempChildNode);
					if (!getLeaf(parent->terms.at(i), tempChildNode)) {
						for (int k = 0; k < nodeInBrackets.size(); k++) {
							clear(nodeInBrackets.at(k));
						}
						tokenIndex = tempTokenIndex;
						do {
							i++;
							if (parent->terms.at(i) == "]")
								goto endSquare;
						} while (true);
					}
				}
			} while (true);
			endSquare:;
		}
		else if (parent->terms.at(i) == "EPSILON") {
			return true;
		}

		// is a non terminal
		else if (parent->terms.at(i).at(0) != '\'') {
			node* tempChildNode = new node;
			tempChildNode->identifier = parent->terms.at(i);
			parentNode->childrens.push_back(tempChildNode);
			tree.push_back(tempChildNode);
			compile = getLeaf(parent->terms.at(i), tempChildNode);
		}

		// is terminal
		else {
			do {
				if (tokens.size() > tokenIndex && (tokens.at(tokenIndex).type == Type::eol_token || tokens.at(tokenIndex).type == Type::comment || tokens.at(tokenIndex).type == Type::comment_multiline)) {
					tokenIndex++;
					if (errorIndex < tokenIndex)
						errorIndex = tokenIndex;
				}
				else {
					break;
				}
			} while (true);

			Token tempToken; 
			string substring = parent->terms.at(i).substr(1, parent->terms.at(i).size() - 2);
			tempToken.type = ling->getLingo()->getOutput(substring)->token_type;
			tempToken.lexeme = substring;
			ling->updateType(&tempToken);
			if (tokens.size() > tokenIndex && tokens.at(tokenIndex).type == tempToken.type) {
				node* tempChildNode = new node;
				tempChildNode->identifier = tokens.at(tokenIndex).lexeme;
				tempChildNode->terminalToken = tokens.at(tokenIndex);
				parentNode->childrens.push_back(tempChildNode);
				tree.push_back(tempChildNode);
				tokenIndex++;
				if (errorIndex < tokenIndex)
					errorIndex = tokenIndex;
			}
			else {
				compile = false;
			}
		}
	}

	return compile;
}

