#include <string>
#include "reader.h"
#include "grammar.h"
#include "tables.h"
#include "moonInterpreter.h"

int main()
{
	cout << "Input file name: ";
	string testFile;
	cin >> testFile;
	lingo ling;
	reader read(&ling);
	vector<Token> tokens;

	cout << "reading file..." << endl;
	read.setFile(testFile);
	cout << "to tokens..." << endl;
	while (!read.eot()) {
		tokens.push_back(read.nextToken());
	}

	cout << "fixing tokens..." << endl;
	for (int i = 0; i < (signed)tokens.size(); i++) {
		if (tokens.at(i).type == Type::error_token || tokens.at(i).type == Type::not_token) {
			tokens.erase(tokens.begin() + i);
			i--;
		}
	}

	ofstream tokenFile;
	tokenFile.open("Output/tokens.txt");
	for (int i = 0; i < (signed)tokens.size(); i++) {
		tokenFile << "lexeme: " << tokens[i].lexeme;
		tokenFile << right << setw(16) << setfill(' ') << "type: " << tokens[i].type;
		tokenFile << right << setw(16) << setfill(' ') << "line: " << tokens[i].line;
		tokenFile << right << setw(16) << setfill(' ') << "index: " << tokens[i].index;
		tokenFile << endl;
	}
	tokenFile.close();

	cout << "making grammar..." << endl;
	grammar gram("grammar.txt");
	gram.printLines();
	cout << "testing grammar..." << endl;
	if (!gram.makeTree(tokens, &ling))
		return 0;
	gram.printTree();

	tables allTables;
	node* root = gram.getTree()[0];
	bool testTables = allTables.run(root);
	allTables.printTables();
	if (testTables)
		return 0;

	cout << "Output file name: ";
	string fileName;
	cin >> fileName;

	moonInterpreter mI(allTables.getTable(), fileName);
	mI.run(root->childrens.at(root->childrens.size() - 2));
}
