#include "tables.h"

symbolTable* tables::getTable() { return allSymbolTables.at(allSymbolTables.size() - 1); }

tables::tables() {};

bool tables::run(node* root)
{
	symbolTable* tempTable = new symbolTable;
	tempTable->name = "Global";
	tempTable->root = root;
	for (int i = 0; i < (signed int)root->childrens.size(); i++){
		if (root->childrens[i]->identifier == "classDecl") {
			// class
			symbol* temp = new symbol;
			string className;
			for (int j = 1; j < (signed int)root->childrens[i]->childrens.size(); j++) {
				if (root->childrens[i]->childrens[j]->identifier == "{") {
					break;
				}
				if (root->childrens[i]->childrens[j]->childrens.size() == 0)
					className += root->childrens[i]->childrens[j]->identifier;
				else
					className += root->childrens[i]->childrens[j]->childrens[0]->identifier;
			}
			temp->name = className;
			temp->kind = "class";
			temp->treeLink = root->childrens[i];
			symbolTable* newTable = classTable(root->childrens[i]);
			allSymbolTables.push_back(newTable);
			tempTable->symbols.push_back(make_pair(temp, newTable));
		}
		else if (root->childrens[i]->identifier == "funcDef") {
			// function
			symbol* temp = new symbol;
			string funcName;
			temp->kind = "function";
			temp->treeLink = root->childrens[i];
			// funcdef -> funchead -> type -> terminal
			temp->type = root->childrens[i]->childrens[0]->childrens[0]->childrens[0]->identifier + " : ";
			int j = 1;
			for (; j < (signed int)root->childrens[i]->childrens[0]->childrens.size(); j++) {
				if (root->childrens[i]->childrens[0]->childrens[j]->identifier == "(") {
					j++;
					break;
				}
				if (root->childrens[i]->childrens[0]->childrens[j]->childrens.size() == 0)
					funcName += root->childrens[i]->childrens[0]->childrens[j]->identifier;
				else
					funcName += root->childrens[i]->childrens[0]->childrens[j]->childrens[0]->identifier;
			}
			temp->name = funcName;
			for (; j < (signed int)root->childrens[i]->childrens[0]->childrens.size(); j++) {
				if (root->childrens[i]->childrens[0]->childrens[j]->identifier == ")") {
					break;
				}
				// iterate over fparams childrens
				for (int k = 0; k < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens.size(); k++) {
					// iterate over fparams next generation
					if (root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->identifier != "fParamsTail") {
						for (int w = 0; w < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens.size(); w++) {
							temp->type += root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens[w]->identifier + " ";
						}
					}
					else {
						for (int w = 0; w < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens.size(); w++) {
							if (root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens[w]->childrens.size() == 0)
								temp->type += root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens[w]->identifier + " ";
							else
								for (int y = 0; y < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens[w]->childrens.size(); y++) {
									temp->type += root->childrens[i]->childrens[0]->childrens[j]->childrens[k]->childrens[w]->childrens[y]->identifier + " ";
								}
						}
					}
				}
				temp->type += " ";
			}

			symbolTable* newTable = funcTable(root->childrens[i], temp->name);
			// Check if function has params
			for (int j = 0; j < (signed int)root->childrens[i]->childrens[0]->childrens.size(); j++) {
				if (root->childrens[i]->childrens[0]->childrens[j]->identifier == "fParams") {
					// Add params
					symbol* tempNew = new symbol;
					tempNew->kind = "variable";
					tempNew->type = root->childrens[i]->childrens[0]->childrens[j]->childrens[0]->childrens[0]->identifier + " ";
					tempNew->name = root->childrens[i]->childrens[0]->childrens[j]->childrens[1]->identifier;
					tempNew->treeLink = root->childrens[0]->childrens[j];
					for (int t = 2; t < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens.size(); t++) {
						for (int y = 0; y < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens.size(); y++) {
							if (root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[y]->identifier == ",") {
								// Add second parameter
								symbol* tempNew2 = new symbol;
								tempNew2->kind = "variable";
								tempNew2->type = root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[y + 1]->childrens[0]->identifier + " ";
								tempNew2->name = root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[y + 2]->identifier;
								tempNew2->treeLink = root->childrens[i]->childrens[0]->childrens[j];
								for (int r = y + 3; r < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens.size(); r++) {
									for (int rr = 0; rr < (signed int)root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[r]->childrens.size(); rr++) {
										tempNew2->type += root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[r]->childrens[rr]->identifier + " ";
									}
								}
								newTable->symbols.push_back(make_pair(tempNew2, nullptr));
								break;
							}
							else {
								tempNew->type += root->childrens[i]->childrens[0]->childrens[j]->childrens[t]->childrens[y]->identifier + " ";
							}
						}
					}
					newTable->symbols.push_back(make_pair(tempNew, nullptr));
					break;
				}
			}
			allSymbolTables.push_back(newTable);
			tempTable->symbols.push_back(make_pair(temp, newTable));
		}
		else if (root->childrens[i]->identifier == "main"){
			i++;
			// main function
			symbol* temp = new symbol;
			temp->name = "main";
			temp->kind = "function";
			temp->treeLink = root->childrens[i];
			symbolTable* newTable = funcTable(root->childrens[i], temp->name);
			allSymbolTables.push_back(newTable);
			tempTable->symbols.push_back(make_pair(temp, newTable));
		}
	}
	allSymbolTables.push_back(tempTable);
	fixTables();
	if (testDuplicates())
		return true;
	return testTables(root);
}

symbolTable* tables::classTable(node* root) {
	symbolTable* tempTable = new symbolTable;
	tempTable->root = root;
	string className;
	int j = 1;
	for (; j < (signed int)root->childrens.size(); j++) {
		if (root->childrens[j]->identifier == "{") {
			j++;
			break;
		}
		if (root->childrens[j]->childrens.size() == 0)
			className += root->childrens[j]->identifier;
		else
			className += root->childrens[j]->childrens[0]->identifier;
	}
	tempTable->name = className;

	for (; j < (signed int)root->childrens.size(); j++) {
		if (root->childrens[j]->identifier == "}")
			break;
		if (root->childrens[j]->identifier == "varDecl") {
			symbol* temp = new symbol;
			temp->kind = "variable";
			temp->treeLink = root->childrens[j];
			temp->name = root->childrens[j]->childrens[1]->identifier;
			temp->type = root->childrens[j]->childrens[0]->childrens[0]->identifier + " ";
			for (int k = 2; k < (signed int)root->childrens[j]->childrens.size(); k++) {
				if (root->childrens[j]->childrens[k]->identifier == ";")
					break;
				if (root->childrens[j]->childrens[k]->childrens.size() == 0)
					temp->type += root->childrens[j]->childrens[k]->identifier + " ";
				else
					for (int y = 0; y < (signed int)root->childrens[j]->childrens[k]->childrens.size(); y++) {
						temp->type += root->childrens[j]->childrens[k]->childrens[y]->identifier + " ";
					}
			}
			tempTable->symbols.push_back(make_pair(temp, nullptr));
		}
		if (root->childrens[j]->identifier == "funcDecl") {
			symbol* temp = new symbol;
			temp->kind = "function";
			temp->type = root->childrens[j]->childrens[0]->childrens[0]->identifier + " : ";
			temp->treeLink = root->childrens[j];
			string funcName;
			int k = 1;
			for (; k < (signed int)root->childrens[j]->childrens.size(); k++) {
				if (root->childrens[j]->childrens[k]->identifier == "(") {
					k++;
					break;
				}
				if (root->childrens[j]->childrens[k]->childrens.size() == 0)
					funcName += root->childrens[j]->childrens[k]->identifier;
				else
					funcName += root->childrens[j]->childrens[k]->childrens[0]->identifier;
			}
			temp->name = funcName;
			for (; k < (signed int)root->childrens[j]->childrens.size(); k++) {
				if (root->childrens[j]->childrens[k]->identifier == ")") {
					break;
				}
				for (int w = 0; w < (signed int)root->childrens[j]->childrens[k]->childrens.size(); w++) {
					if (root->childrens[j]->childrens[k]->childrens[w]->childrens.size() == 0) {
						temp->type += root->childrens[j]->childrens[k]->childrens[w]->identifier + " ";
					}
					else {
						for (int i = 0; i < (signed int)root->childrens[j]->childrens[k]->childrens[w]->childrens.size(); i++) {
							if (root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->identifier == ",") {
								temp->type += root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->identifier + " ";
								temp->type += root->childrens[j]->childrens[k]->childrens[w]->childrens[i + 1]->childrens[0]->identifier + " ";
								temp->type += root->childrens[j]->childrens[k]->childrens[w]->childrens[i + 2]->identifier + " ";
								i += 2;
							}
							else {
								if (root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->childrens.size() == 0) {
									temp->type += root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->identifier + " ";
								}
								else {
									for (int t = 0; t < (signed int)root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->childrens.size(); t++) {
										temp->type += root->childrens[j]->childrens[k]->childrens[w]->childrens[i]->childrens[t]->identifier + " ";
									}
								}
							}
						}
					}
				}
			}

			tempTable->symbols.push_back(make_pair(temp, nullptr));
		}
	}

	return tempTable;
}

symbolTable* tables::funcTable(node* root, string name) {
	symbolTable* tempTable = new symbolTable;
	tempTable->name = name;
	if (root->identifier != "funcBody")
		root = root->childrens[1];

	tempTable->root = root;

	for (int i = 1; i < (signed int)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "}")
			break;
		if (root->childrens[i]->identifier == "varDecl") {
			symbol* temp = new symbol;
			temp->kind = "variable";
			temp->name = root->childrens[i]->childrens[1]->identifier;
			temp->type = root->childrens[i]->childrens[0]->childrens[0]->identifier + " ";
			temp->treeLink = root->childrens[i];
			for (int k = 2; k < (signed int)root->childrens[i]->childrens.size(); k++) {
				if (root->childrens[i]->childrens[k]->identifier == ";")
					break;
				if (root->childrens[i]->childrens[k]->childrens.size() == 0)
					temp->type += root->childrens[i]->childrens[k]->identifier + " ";
				else
					for (int y = 0; y < (signed int)root->childrens[i]->childrens[k]->childrens.size(); y++) {
						temp->type += root->childrens[i]->childrens[k]->childrens[y]->identifier + " ";
					}
			}
			tempTable->symbols.push_back(make_pair(temp, nullptr));
		}
		if (root->childrens[i]->identifier == "statement") {
			if (root->childrens[i]->childrens[0]->identifier == "for") {
				symbol* temp = new symbol;
				stringstream ss;
				ss << root->childrens[i];
				temp->name = "for->" + ss.str();
				temp->kind = "loop";
				temp->treeLink = root->childrens[i];

				symbolTable* tempTable2 = new symbolTable;
				tempTable2->name = temp->name;
				tempTable2->root = root->childrens[i];
				symbol* temp2 = new symbol;
				temp2->name = root->childrens[i]->childrens[3]->identifier;
				temp2->kind = "variable";
				temp2->type = root->childrens[i]->childrens[2]->childrens[0]->identifier + " ";
				temp2->treeLink = root->childrens[i];
				tempTable2->symbols.push_back(make_pair(temp2, nullptr));

				tempTable->symbols.push_back(make_pair(temp, tempTable2));
				allSymbolTables.push_back(tempTable2);
			}
		}
	}

	return tempTable;
}

void tables::printTables() {

	ofstream myFile("Output/SymbolTables.txt");

	if (myFile.is_open()) {
		for (int i = 0; i < (signed int)allSymbolTables.size(); i++) {
			myFile << "+-+-+-+" << endl;
			myFile << left << setw(30) << setfill(' ') << allSymbolTables[i]->name << endl;
			myFile << endl;
			for (int j = 0; j < (signed int)allSymbolTables[i]->symbols.size(); j++) {
				myFile << left << setw(25) << setfill(' ') << allSymbolTables[i]->symbols[j].first->name;
				myFile << left << setw(25) << setfill(' ') << allSymbolTables[i]->symbols[j].first->kind;
				myFile << left << setw(25) << setfill(' ') << allSymbolTables[i]->symbols[j].first->type << endl;
			}
			myFile << endl;
		}
		myFile.close();
	}
}

void tables::fixTables() {
	// Get global table
	symbolTable* globalTable = allSymbolTables.back();
	// Iterate over global table symbols
	for (int i = 0; i < (signed int)globalTable->symbols.size(); i++) {
		// if inherits
		if (globalTable->symbols.at(i).first->name.find("::") != string::npos) {
			// table name and function name
			string tableName = globalTable->symbols.at(i).first->name.substr(0, globalTable->symbols.at(i).first->name.find("::"));
			string funcName = globalTable->symbols.at(i).first->name.substr(globalTable->symbols.at(i).first->name.find("::") + 2);
			// iterate over glbal table symbols
			for (int k = 0; k < (signed int)globalTable->symbols.size(); k++) {
				if (globalTable->symbols[k].first->name.substr(0, tableName.size()) == tableName) {
					for (int w = 0; w < (signed int)globalTable->symbols[k].second->symbols.size(); w++) {
						if (globalTable->symbols[k].second->symbols[w].first->name == funcName) {
							globalTable->symbols[k].second->symbols[w].second = globalTable->symbols.at(i).second;
							break;
						}
					}
					break;
				}
			}
			globalTable->symbols.erase(globalTable->symbols.begin() + i);
			i--;
		}
	}

	// add values inherited to tables
	for (int i = 0; i < (signed)allSymbolTables.size(); i++) {
		if (allSymbolTables[i]->name.find(":") != string::npos) {
			string toMatch = allSymbolTables[i]->name.substr(allSymbolTables[i]->name.find(":") + 1);
			for (int w = 0; w < i; w++) {
				if (!allSymbolTables[w]->name.compare(toMatch)) {
					for (int k = 0; k < (signed)allSymbolTables[w]->symbols.size(); k++) {
						// given that it does not exist
						bool exist = false;
						for (int u = 0; u < (signed)allSymbolTables[i]->symbols.size(); u++) {
							if (allSymbolTables[i]->symbols[u].first->name == allSymbolTables[w]->symbols[k].first->name) {
								exist = true;
								break;
							}
						}
						if (!exist) {
							allSymbolTables[i]->symbols.push_back(allSymbolTables[w]->symbols.at(k));
						}
					}
				}
			}
		}
	}
}

bool tables::testTables(node* root) {
	bool found = false;
	for (int i = 0; i < (signed int)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "assignStat") {
			// manage assignment operations
			node* lhs = root->childrens[i]->childrens[0];
			node* rhs = root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 1];
			string lhsType = getType(getVar(lhs), lhs);
			string rhsType = getVarAll(rhs);
			if (lhsType != "" && rhsType != "") {
				if (lhsType.find(rhsType) == string::npos) {
					// review value
					lhsType = reviewType(getVar(lhs));
					if (lhsType.find(rhsType) == string::npos) {
						if ((lhsType.find("float") != string::npos && rhsType.find("integer") != string::npos)
							|| (lhsType.find("integer") != string::npos && rhsType.find("float") != string::npos)) {
							// this is still ok because we can end up right
						}
						else {
							cout << "Type mismatch at line " << getPos(root->childrens[i]) << endl;
							cout << lhsType << " != " << rhsType << endl;
							found = true;
						}
					}
				}
			}
		}
		if (root->childrens[i]->identifier == "variable") {
			if (getType(getVar(root->childrens[i]), root->childrens[i]) == "") {
				cout << "Undefined variable at " << getPos(root->childrens[i]) << endl;
				found = true;
			}
		}
		// Array size
		if (root->childrens[i]->identifier == "variable" || root->childrens[i]->identifier == "idnest") {
			int arraySize = 0;
			string arrayType;
			for (int w = 0; w < (signed int)root->childrens[i]->childrens.size(); w++) {
				if (root->childrens[i]->childrens[w]->identifier == "indice") {
					arraySize++;
					if (arrayType.empty()) {
						arrayType = getType(root->childrens[i]->childrens[w - 1]->identifier, root);
						if (arrayType == "")
							arrayType = getType(root->childrens[i]->childrens[w - 1]->identifier);
					}
				}
			}
			if (arraySize != 0) {
				for (int w = 0; w < (signed int)arrayType.size(); w++) {
					if (arrayType.at(w) == '[')
						arraySize--;
				}
				if (arraySize != 0) {
					if (arrayType != "") {
						cout << "Array dimension error at " << getPos(root) << endl;
						found = true;
					}
				}
			}
		}
		// deals with class dot
		if (root->childrens[i]->identifier == "idnest") {
			symbolTable* topTable = nullptr;
			string rootTable = getType(root->childrens[i]->childrens[0]->identifier, root) + " ";
			if (rootTable == " ") {
				rootTable = reviewType(root->childrens[i]->childrens[0]->identifier) + " ";
			}
			rootTable = rootTable.substr(0, rootTable.find(" "));
			for (int w = 0; w < (signed int)allSymbolTables.size(); w++){
				if (allSymbolTables[w]->name.substr(0, rootTable.size()).compare(rootTable) == 0) {
					topTable = allSymbolTables[w];
					break;
				}
			}
			bool dotCheck = false;
			if (topTable) {
				for (int w = 0; w < (signed int)topTable->symbols.size(); w++) {
					string rhs;
					if (root->childrens[i + 1]->childrens.size() == 0) {
						rhs = root->childrens[i + 1]->identifier;
					}
					else {
						rhs = root->childrens[i + 1]->childrens[0]->identifier;
					}

					if (topTable->symbols[w].first->name == rhs) {
						dotCheck = true;
						break;
					}
				}
				if (!dotCheck) {
					cout << "Undefined member in class at " << getPos(root) << endl;
					found = true;
				}
			}
		}
		// return match
		if (root->childrens[i]->identifier == "return") {
			string returnType = getVarAll(root->childrens[i+2]);
			for (int w = 0; w < (signed int)allSymbolTables.size(); w++) {
				for (int y = 0; y < (signed int)allSymbolTables[w]->symbols.size(); y++) {
					if (allSymbolTables[w]->symbols[y].first->kind == "function") {
						if (partOf(root->childrens[i], allSymbolTables[w]->symbols[y].second->root)) {
							if (allSymbolTables[w]->symbols[y].first->type.substr(0, returnType.size()) != returnType) {
								cout << "Wrong return type at " << getPos(root) << ", should be " << allSymbolTables[w]->symbols[y].first->type.substr(0, allSymbolTables[w]->symbols[y].first->type.find(" ")) << endl;
								found = true;
							}
							break;
						}
					}
				}
			}
		}
		if (root->childrens[i]->identifier == "functionCall") {
			string funcName;
			for (int w = 0; w < (signed int)root->childrens[i]->childrens.size(); w++) {
				if (root->childrens[i]->childrens[w]->identifier != "idnest") {
					funcName = root->childrens[i]->childrens[w]->identifier;
					break;
				}
			}
			string arg1;
			string arg2;
			for (int w = 0; w < (signed int)root->childrens[i]->childrens[root->childrens[i]->childrens.size()-2]->childrens.size(); w++) {
				if (root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]->childrens[w]->identifier == "aParamsTail") {
					arg2 = getType(getVar(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]->childrens[w]->childrens[1]), root->childrens[i]);
				}
				arg1 = getVar(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]->childrens[w]);
				if (arg1.find(",") != string::npos)
					arg1 = arg1.substr(arg1.find(",") + 1);
				arg1 = getType(arg1, root->childrens[i]);
			}
			string funcType = getType(funcName);
			funcType = funcType.substr(funcType.find(":") + 2);
			if (funcType.find(",") != string::npos) {
				if (arg2 == "") {
					cout << "Missing argument for function call at " << getPos(root->childrens[i]) << endl;
					found = true;
				}
				else {
					if (funcType.substr(0, funcType.find(",")).find(arg1) != string::npos) {
						cout << "Wrong argument for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
						found = true;
					}
					if (funcType.substr(funcType.find(",")).find(arg2) != string::npos) {
						cout << "Wrong argument for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
						found = true;
					}
				}
			}
			else if (funcType.size() > 4){
				if (funcType.find(",") != string::npos) {
					cout << "Argument needed for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
					found = true;
				}
				else if (arg2 != "") {
					cout << "Argument not needed for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
					found = true;
				}
				else {
					if (arg1 != "") {
						if (funcType.find(arg1) != string::npos) {
							cout << "Wrong argument for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
							found = true;
						}
					}
					else {
						cout << "Missing argument for function call at " << getPos(root->childrens[i]) << endl;
						found = true;
					}
				}
			}
			else {
				if (arg1 != "" && arg2 != "") {
					cout << "Argument not needed for function call at " << getPos(root->childrens[i]->childrens[root->childrens[i]->childrens.size() - 2]) << endl;
					found = true;
				}
			}
		}
		if (root->childrens[i]->childrens.size() > 0) {
			if (testTables(root->childrens[i]))
				found = true;
		}
	}
	return found;
}

bool tables::testDuplicates() {
	bool found = false;
	for (int i = 0; i < (signed int)allSymbolTables.size(); i++) {
		for (int k = 0; k < (signed int)allSymbolTables[i]->symbols.size(); k++) {
			for (int y = 0; y < (signed int)allSymbolTables[i]->symbols.size(); y++) {
				if (k < y && allSymbolTables[i]->symbols.at(k).first->name == allSymbolTables[i]->symbols.at(y).first->name) {
					cout << "Duplicate symbol at " << getPos(allSymbolTables[i]->symbols.at(k).first->treeLink);
					cout << " and " << getPos(allSymbolTables[i]->symbols.at(y).first->treeLink) << endl;
					found = true;
				}
			}
		}
	}
	return found;
}

string tables::getPos(node* root) {
	node* leaf = root;
	do {
		leaf = leaf->childrens[0];
	} while (leaf->childrens.size() > 0);
	string output = "line " + to_string(leaf->terminalToken.line);
	return output + ", index " + to_string(leaf->terminalToken.index);
}

string tables::getVar(node* root) {
	string var;
	for (int i = 0; i < (signed int)root->childrens.size(); i++) {
		if (root->childrens[i]->childrens.size() > 0) {
			var.append(getVar(root->childrens[i]));
		}
		else
		{
			var.append(root->childrens[i]->identifier);
		}
	}
	return var;
}

string tables::getType(string value, node* root) {
	string check = value;
	if (check.find("[")) {
		check = check.substr(0, check.find("["));
	}
	if (check.find(".")) {
		check = check.substr(0, check.find("."));
	}

	string type;
	for (int i = 0; i < (signed int)allSymbolTables.size(); i++) {
		if (root == NULL || partOf(root, allSymbolTables[i]->root))
		for (int k = 0; k < (signed int)allSymbolTables[i]->symbols.size(); k++) {
			if (check == allSymbolTables[i]->symbols[k].first->name) {
				type = allSymbolTables[i]->symbols[k].first->type;
				goto foundType;
			}
		}
	}
	foundType:


	return type;
}

bool tables::partOf(node* n1, node* n2) {
	node* tempInside = n2;
	bool check = false;

	for (int i = 0; i < (signed int)n2->childrens.size(); i++) {
		if (n2->childrens[i] == n1) {
			check = true;
			break;
		}
		if (n2->childrens[i]->childrens.size() > 0) {
			if (partOf(n1, n2->childrens[i])) {
				check = true;
				break;
			}
		}
	}

	return check;
}

string tables::getVarAll(node* root) {
	string tempType;
	string type;
	root = root->childrens[0];
	for (int i = 0; i < (signed int)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier != "addOp") {
			tempType = getTerm(root->childrens[i]);
			if (type.empty())
				type = tempType;
			else if (tempType != type)
				return "";
		}
	}
	return tempType;
}

string tables::getTerm(node* root) {
	string currentType;
	for (int i = 0; i < (signed int)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "factor") {
			for (int k = 0; k < (signed int)root->childrens[i]->childrens.size(); k++) {
				if (root->childrens[i]->childrens[k]->childrens.size() > 0) {
					if (root->childrens[i]->childrens[k]->identifier == "variable") {
						string tempType = getType(getVar(root->childrens[i]->childrens[k]), root->childrens[i]->childrens[k]);
						tempType = tempType.substr(0, tempType.find(" "));
						if (currentType == "")
							currentType = tempType;
						else if (tempType != currentType) {
							if (tempType == "float" && currentType == "integer")
								currentType = tempType;
							else if (!(tempType == "integer" && currentType == "float"))
								return "";
						}
					}
					else if (root->childrens[i]->childrens[k]->identifier == "arithExpr"){
						for (int r = 0; r < (signed int)root->childrens[i]->childrens[k]->childrens.size(); r += 2) {
							string tempType = getTerm(root->childrens[i]->childrens[k]->childrens[r]);
							if (currentType == "")
								currentType = tempType;
							else if (tempType != currentType) {
								if (tempType == "float" && currentType == "integer")
									currentType = tempType;
								else if (!(tempType == "integer" && currentType == "float"))
									return "";
							}
						}
					}
					else if (root->childrens[i]->childrens[k]->identifier == "functionCall") {
						for (int e = 0; e < (signed int)root->childrens[i]->childrens[k]->childrens.size(); e++) {
							if (root->childrens[i]->childrens[k]->childrens[e]->childrens.size() == 0) {
								string tempType;
								if (e == 0) {
									tempType = getType(root->childrens[i]->childrens[k]->childrens[e]->identifier, root->childrens[i]);
								}
								else {
									tempType = getType(root->childrens[i]->childrens[k]->childrens[e]->identifier);
								}
								tempType = tempType.substr(0, tempType.find(" :"));
								if (currentType == "")
									currentType = tempType;
								else if (tempType != currentType) {
									if (tempType == "float" && currentType == "integer")
										currentType = tempType;
									else if (!(tempType == "integer" && currentType == "float"))
										return "";
								}
								break;
							}
						}
					}
					else {
						cout << "Debug no taken into accout:"<< root->childrens[i]->childrens[k]->identifier << endl;
					}
				}
				else {
					if (root->childrens[i]->childrens[k]->identifier != "(" && root->childrens[i]->childrens[k]->identifier != ")") {
						if (root->childrens[i]->childrens[k]->terminalToken.type == 4) {
							if (currentType == "")
								currentType = "integer";
							else if (currentType != "float" && currentType != "integer")
								return "";
						}
						else {
							if (currentType == "" || currentType == "integer")
								currentType = "float";
							else if (currentType != "float")
								return "";
						}
					}
				}
			}
		}
		else if (root->childrens[i]->identifier == "multOp") {
			if (root->childrens[i]->childrens[0]->identifier == "/") {
				if (currentType == "" || currentType == "integer")
					currentType = "float";
				else if (currentType != "float")
					return "";
			}
		}
	}
	return currentType;
}

string tables::reviewType(string value) {
	string tempVar = value;
	do {
		if (tempVar.find(".") != string::npos)
			tempVar = tempVar.substr(tempVar.find(".") + 1);
		else
			break;
	} while (true);

	return getType(tempVar);
}

// Add loops and others their own table