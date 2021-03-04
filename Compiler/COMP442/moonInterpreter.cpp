#include "moonInterpreter.h"

moonInterpreter::moonInterpreter(symbolTable* tableRoot, string fileName)
{
	// Make file to write moon code
	myFile.open("Output\\" + fileName);
	cout << "To moon code..." << endl;

	// set pointer
	myFile << "\t\tentry % program entry" << endl;
	myFile << "\t\talign % following instruction align" << endl;
	stack = getReg();
	myFile << "\t\taddi " << stack << ", r0 , topaddr % init stack pointer" << endl;
	frame = getReg();
	myFile << "\t\taddi " << frame << ", r0 , topaddr % init frame pointer" << endl;

	// counter
	counter = 0;
	myTable = tableRoot;

	// create all variable ref to moon
	for (int i = 0; i < (signed)tableRoot->symbols.size(); i++) {
		if (tableRoot->symbols[i].first->name == "main") {
			for (int j = 0; j < (signed)tableRoot->symbols[i].second->symbols.size(); j++) {
				if (tableRoot->symbols[i].second->symbols[j].first->kind == "variable") {
					// reserve space for int
					if (tableRoot->symbols[i].second->symbols[j].first->type.find("integer") != string::npos) {
						// No array
						if (tableRoot->symbols[i].second->symbols[j].first->type == "integer ") {
							tableRoot->symbols[i].second->symbols[j].first->resRef = counter;
							counter += 4;
						}
						// With array
						else {
							string type = tableRoot->symbols[i].second->symbols[j].first->type;
							type = type.substr(type.find("["));
							tableRoot->symbols[i].second->symbols[j].first->resRef = counter;
							counter += getDimension(type) * 4;
						}
					}
					// reserve space for float
					else if (tableRoot->symbols[i].second->symbols[j].first->type.find("float") != string::npos) {
						// not implemented
					}
					// reserve space for object
					else {
						symbolTable* classTable = NULL;
						for (int k = 0; k < (signed)tableRoot->symbols.size(); k++) {
							if (tableRoot->symbols[i].second->symbols[j].first->type.find(tableRoot->symbols[k].first->name.substr(0, tableRoot->symbols[k].first->name.find(":"))) != string::npos) {
								classTable = tableRoot->symbols[k].second;
							}
						}
						if (classTable->resRef == 0) {
							setTableRes(classTable);
						}
						// no array
						if (tableRoot->symbols[i].second->symbols[j].first->type.find("[") == string::npos) {
							tableRoot->symbols[i].second->symbols[j].first->resRef = counter;
							counter += classTable->resRef;
						}
						// array
						else {
							string type = tableRoot->symbols[i].second->symbols[j].first->type;
							type = type.substr(type.find("["));
							tableRoot->symbols[i].second->symbols[j].first->resRef = counter;
							counter += classTable->resRef * getDimension(type);
						}
					}
				}
			}
		}
	}

	// add space for variables
	myFile << "\t\tsubi " << stack << ", " << stack << ", " << counter << " % set the stack pointer to the top" << endl;
}

void moonInterpreter::run(node* root) {
	// loop all children
	traverse(root);

	// close file
	myFile << "\t\thlt % close program" << endl;
	myFile.close();
}

void moonInterpreter::traverse(node* root) {
	for (int i = 0; i < (signed)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "statement") {
			// process statement
			if (root->childrens[i]->childrens[0]->identifier == "assignStat")
				assignStat(root->childrens[i]->childrens[0]);
			if (root->childrens[i]->childrens[0]->identifier == "if")
				ifStatement(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "for")
				forStatement(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "read")
				read(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "write")
				write(root->childrens[i]);
		}
	}
}

void moonInterpreter::assignStat(node* root) {
	// get lhs and rhs of equation
	node* lhs = root->childrens[0];
	node* rhs = root->childrens[2];

	// handle rhs
	string rhsValue;
	rhsValue = expr(rhs);

	string rhsReg;
	// not a register
	if (rhsValue.at(0) != 'r') {
		rhsReg = getReg();
		myFile << left << setw(8) << setfill(' ') << "\t\taddi " << rhsReg << ",  r0, " << rhsValue << endl;
	}
	// register
	else {
		rhsReg = rhsValue;
	}

	// handle lhs
	string lhsValue;
	lhsValue = getVar(lhs);
	// sw rhs in lhs
	myFile << left << setw(8) << setfill(' ') << "\t\tsw" << lhsValue << "(r2), " << rhsReg << endl;
	if (useR2) {
		myFile << "\t\taddi " << frame << ", r0 , topaddr % reinit frame pointer" << endl;
		useR2 = false;
	}

	freeReg(rhsReg);
}

void moonInterpreter::ifStatement(node* root) {
	// may need to number if and else (for if inside another if)
	string tempVar = expr(root->childrens[2]);
	ifCounter++;
	int currentIf = ifCounter;

	myFile << left << setw(8) << setfill(' ') << "\t\tbz " << tempVar << ", else_" << currentIf << "% if statement" << endl;

	// root->childrens[5] (then) -> statblock
	statblock(root->childrens[5]);

	myFile << left << setw(8) << setfill(' ') << "\t\tj endif_" << currentIf << " % jump out of the else block" << endl;
	myFile << "\t\telse_" << currentIf;

	// root->childrens[7] (else) -> statblock
	statblock(root->childrens[7]);

	myFile << "\t\tendif_" << currentIf << "  nop % end of the if statement" << endl;

	freeReg(tempVar);
}

void moonInterpreter::forStatement(node* root) {
	int forOffset;
	string result;
	// set table offset to correct one
	tempCounter += 4;
	forOffset = -tempCounter;
	setFor(forOffset, root);
	forOffset -= counter;

	// 2 to 5 new variable
	myFile << "\t\tsubi " << stack << ", " << stack << ", 4" << endl;
	result = expr(root->childrens[5]);
	if (result.at(0) == 'r') {
		myFile << "\t\tsw " << forOffset << "(r2), " << result << endl;
		freeReg(result);
	}
	else
	{
		string tempReg = getReg();
		myFile << "\t\taddi " << tempReg << ", r0, " << result << endl;
		myFile << "\t\tsw " << forOffset << "(r2), " << tempReg << endl;
		freeReg(tempReg);
	}

	forCounter++;
	int currentFor = forCounter;
	myFile << "\t\t% start of the loop" << endl;
	myFile << left << setw(8) << setfill(' ') << "\t\tgoFor_" << forCounter;
	// 7 test
	myFile << "\t\tbz " << expr(root->childrens[7]) << ", endFor_" << forCounter << endl;
	// 11 statblock
	statblock(root->childrens[11]);

	// 9 assign stat
	assignStat(root->childrens[9]);

	myFile << "\t\tj goFor_" << forCounter << " % jump to start of loop" << endl;
	myFile << "\t\tendFor_" << forCounter << " nop % end of the loop" << endl;
	myFile << "\t\taddi " << stack << ", " << stack << ", 4" << endl;
	tempCounter -= 4;
}

void moonInterpreter::read(node* root) {
	// getc (not done)
	node* value = root->childrens[2];
	string tempReg = getReg();
	myFile << left << setw(8) << setfill(' ') << "\t\tgetc " << tempReg << endl;
	myFile << left << setw(8) << setfill(' ') << "\t\tsw " << getVar(value) << "(r2), " << tempReg << endl;
	if (useR2) {
		myFile << "\t\taddi " << frame << ", r0 , topaddr % reinit frame pointer" << endl;
		useR2 = false;
	}
	freeReg(tempReg);
}

void moonInterpreter::write(node* root) {
	// putc
	node* value = root->childrens[2];
	string consoleReg = expr(value);
	if (consoleReg.at(0) == 'r') {
		myFile << left << setw(8) << setfill(' ') << "\t\tputc " << consoleReg << endl;
		freeReg(consoleReg);
	}
	else {
		string tempReg = getReg();
		myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", r0, " << consoleReg << endl;
		myFile << left << setw(8) << setfill(' ') << "\t\tputc " << tempReg << endl;
		freeReg(tempReg);
	}
}
string moonInterpreter::arithExpr(node* root) {
	// no arithmatic
	if (root->childrens.size() == 1) {
		return term(root->childrens[0]);
	}
	else {
		string tempReg = getReg();
		for (int i = 0; i < (signed)root->childrens.size(); i++) {
			// addition and difference (addOp)
			if (root->childrens[i]->identifier == "term") {
				string tempVal = term(root->childrens[i]);
				// add first value to register
				if (i == 0) {
					if (tempVal.at(0) == 'r') {
						freeReg(tempReg);
						tempReg = tempVal;
					}
					else {
						myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", r0, " << tempVal << endl;
					}
				}
				// add other value to register
				else {
					if (root->childrens[i - 1]->childrens[0]->identifier == "+") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tadd " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == "-") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tsub " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tsubi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else {
						// ||
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tor " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tori " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
				}
			}
		}
		return tempReg;
	}
}

string moonInterpreter::term(node* root) {
	// no arithmatic
	if (root->childrens.size() == 1) {
		return factor(root->childrens[0]);
	}
	else {
		string tempReg = getReg();
		for (int i = 0; i < (signed)root->childrens.size(); i++) {
			// mult or division (multOp)
			if (root->childrens[i]->identifier == "factor") {
				string tempVal = factor(root->childrens[i]);
				// add first value to register
				if (i == 0) {
					if (tempVal.at(0) == 'r') {
						freeReg(tempReg);
						tempReg = tempVal;
					}
					else {
						myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", r0, " << tempVal << endl;
					}
				}
				// add second value to register
				else {
					if (root->childrens[i - 1]->childrens[0]->identifier == "*") {
						if (tempVal.at(0) != 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tmuli " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tmull " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == "/") {
						// no float
						if (tempVal.at(0) != 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tdivi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tdiv " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
					}
					else {
						// && operator
						if (tempVal.at(0) != 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tand " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tandi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
					}
				}
			}
		}
		return tempReg;
	}
}

string moonInterpreter::factor(node* root) {
	if (root->childrens.size() == 1) {
		if (root->childrens[0]->childrens.size() == 0) {
			return root->childrens[0]->identifier;
		}
		// variable
		else {
			string tempVarReg = getReg();
			string tempVarRef = getVar(root->childrens[0]);
			myFile << left << setw(8) << setfill(' ') << "\t\tlw " << tempVarReg << ", " << tempVarRef << "(r2)" << endl;
			if (useR2) {
				myFile << "\t\taddi " << frame << ", r0 , topaddr % reinit frame pointer" << endl;
				useR2 = false;
			}
			return tempVarReg;
		}
	}
	else {
		if (root->childrens[0]->identifier == "(") {
			return arithExpr(root->childrens[1]);
		}
		else if (root->childrens[0]->identifier == "sign") {
			// invert or remain number
			string result = factor(root->childrens[1]);
			if (root->childrens[0]->childrens[0]->identifier == "-") {
				if (result.at(0) == 'r') {
					myFile << left << setw(8) << setfill(' ') << "\t\tsub " << result << ", r0, " << result << endl;
				}
				else {
					string tempReg = getReg();
					myFile << left << setw(8) << setfill(' ') << "\t\tsubi " << tempReg << ", r0, " << result << endl;
					result = tempReg;
				}
			}
			return result;
		}
		else if (root->childrens[0]->identifier == "!") {
			string result = factor(root->childrens[1]);
			if (result.at(0) == 'r') {
				myFile << left << setw(8) << setfill(' ') << "\t\tnot " << result << result << endl;
			}
			else {
				string tempReg = getReg();
				myFile << left << setw(8) << setfill(' ') << "\t\tnot " << tempReg << result << endl;
				result = tempReg;
			}
			return result;
		}
		else {
			cout << "factor not handled" << endl;
			return "NULL";
		}
	}
}

string moonInterpreter::expr(node* root) {
	// no arithmatic
	if (root->childrens.size() == 1) {
		return arithExpr(root->childrens[0]);
	}
	else {
		string tempReg = getReg();
		for (int i = 0; i < (signed)root->childrens.size(); i++) {
			// compare (relOp)
			if (root->childrens[i]->identifier == "arithExpr") {
				string tempVal = arithExpr(root->childrens[i]);
				// add first value to register
				if (i == 0) {
					if (tempVal.at(0) == 'r') {
						freeReg(tempReg);
						tempReg = tempVal;
					}
					else {
						myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", r0, " << tempVal << endl;
					}
				}
				// add other value
				else {
					if (root->childrens[i - 1]->childrens[0]->identifier == "==") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tceq " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tceqi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == "<>") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tcne " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tcnei " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == "<") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tclt " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tclti " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == ">") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tcgt " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tcgti " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == "<=") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tcle " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tclei " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else if (root->childrens[i - 1]->childrens[0]->identifier == ">=") {
						if (tempVal.at(0) == 'r') {
							myFile << left << setw(8) << setfill(' ') << "\t\tcge " << tempReg << ", " << tempReg << ", " << tempVal << endl;
							freeReg(tempVal);
						}
						else {
							myFile << left << setw(8) << setfill(' ') << "\t\tcgei " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						}
					}
					else {
						cout << "relOp not handled in Expr" << endl;
					}
				}
			}
		}
		return tempReg;
	}
}

void moonInterpreter::statblock(node* root) {
	for (int i = 0; i < (signed)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "statement") {
			// process statement
			if (root->childrens[i]->childrens[0]->identifier == "assignStat")
				assignStat(root->childrens[i]->childrens[0]);
			if (root->childrens[i]->childrens[0]->identifier == "if")
				ifStatement(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "for")
				forStatement(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "read")
				read(root->childrens[i]);
			if (root->childrens[i]->childrens[0]->identifier == "write")
				write(root->childrens[i]);
		}
	}
}

int moonInterpreter::getDimension(string arraySize) {
	int total = 1;
	string tempSize = arraySize;
	do {
		total *= stoi(tempSize.substr(2, tempSize.find("]") - 2));
		tempSize = tempSize.substr(tempSize.find("]") + 2);
		if (tempSize.find("[") == string::npos)
			break;
	} while (true);
	return total;
}

string moonInterpreter::getReg() {
	for (int i = 0; i < 15; i++) {
		if (registers[i] == false) {
			registers[i] = true;
			return "r" + to_string(i + 1);
		}
	}
	return "rALLUSED";
}

void moonInterpreter::freeReg(string reg) {
	registers[stoi(reg.substr(1)) - 1] = false;
}

string moonInterpreter::getVar(node* root) {
	bool isFunction = false;
	// get varName;
	string varName;
	for (int i = 0; i < (signed)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "indice")
			useR2 = true;
		if (root->childrens[i]->childrens.size() == 0 && root->childrens[i]->identifier != "." 
			&& root->childrens[i]->identifier != "(" && root->childrens[i]->identifier != ")")
			varName = root->childrens[i]->identifier;
		if (root->childrens[i]->identifier == "(")
			isFunction = true;
	}

	if (isFunction) {
		// execute function
		// return register with returned value or return object
		cout << "This function was called but not implemented: " << varName << endl;
	}

	else if (useR2) {
		string tempReg = getReg();
		myFile << left << setw(8) << setfill(' ') << "\t\tadd "<< tempReg <<", r0, r0 % Hold array position" << endl;
		// get var array size
		string tempType = getType(varName, root);
		if (tempType == "") {
			tempType = getTypeAll(root);
			if (tempType == "")
				tempType = getType(varName, NULL);
		}
		string arraySize = tempType.substr(tempType.find("[") + 1);
		if (arraySize.find("[") != string::npos) {
			arraySize = arraySize.substr(arraySize.find("[") + 1);
		}
		else {
			arraySize = "";
		}

		for (int i = 0; i < (signed)root->childrens.size(); i++) {
			if (root->childrens[i]->identifier == "indice") {
				string tempVal = arithExpr(root->childrens[i]->childrens[1]);
				if (arraySize != "") {
					int multiplier = stoi(arraySize.substr(1, arraySize.find("]")));
					if (arraySize.find("[") != string::npos) {
						arraySize = arraySize.substr(arraySize.find("[") + 1);
					}
					else {
						arraySize = "";
					}

					if (tempVal.at(0) == 'r') {
						myFile << left << setw(8) << setfill(' ') << "\t\tmuli "<< tempVal << ", " << tempVal << ", " << multiplier << endl;
						myFile << left << setw(8) << setfill(' ') << "\t\tadd " << tempReg << ", " << tempReg << ", "  << tempVal << endl;
						freeReg(tempVal);
					}
					else {
						myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", " << tempReg << ", " << stoi(tempVal) * multiplier << endl;
					}
				}
				else {
					if (tempVal.at(0) == 'r') {
						myFile << left << setw(8) << setfill(' ') << "\t\tadd " << tempReg << ", " << tempReg << ", " << tempVal << endl;
						freeReg(tempVal);
					}
					else {
						myFile << left << setw(8) << setfill(' ') << "\t\taddi " << tempReg << ", " << tempReg << ", " << tempVal << endl;
					}
				}
			}
		}

		// multiply tempReg by size of variable
		if (tempType.find("integer") != string::npos) {
			myFile << left << setw(8) << setfill(' ') << "\t\tmuli " << tempReg << ", " << tempReg << ", 4" << endl;
		}
		else if (tempType.find("float") != string::npos) {

		}
		else{
			symbolTable* classTable = NULL;
			for (int k = 0; k < (signed)myTable->symbols.size(); k++) {
				if (tempType.find(myTable->symbols[k].first->name) != string::npos) {
					classTable = myTable->symbols[k].second;
				}
			}
			myFile << left << setw(8) << setfill(' ') << "\t\tmuli " << tempReg << ", " << tempReg << ", " << classTable->resRef << endl;
		}

		// add to r2
		myFile << left << setw(8) << setfill(' ') << "\t\tadd " << frame << ", " << frame << ", " << tempReg << endl;

		freeReg(tempReg);
	}

	return to_string(getOffset(varName, root) - counter);
}

int moonInterpreter::getOffset(string varName, node* root, symbolTable* table) {
	// set source
	symbolTable* tempTable;
	if (table == NULL)
		tempTable = myTable;
	else
		tempTable = table;

	if (tempTable->resRef == NULL) {
		setTableRes(tempTable);
	}

	// traverse table 
	for (int i = 0; i < (signed)tempTable->symbols.size(); i++) {
		if (tempTable->symbols[i].first->name == varName) {
			return tempTable->symbols[i].first->resRef;
		}
		// add function to check that it is in scope
		else if (tempTable->symbols[i].second != NULL && partOf(root, tempTable->symbols[i].second->root)) {
			int tempOut = getOffset(varName, root, tempTable->symbols[i].second);
			if (tempOut != NULL)
				return tempOut;
		}
	}

	if (table == NULL) {
		string baseOffset = "0";
		for (int i = 0; i < (signed)root->childrens.size(); i++) {
			if (root->childrens[i]->identifier == "idnest") {
				if (!useR2)
					baseOffset = getVar(root->childrens[i]);
				else
				{
					useR2 = false;
					baseOffset = getVar(root->childrens[i]);
					useR2 = true;
				}
				string tableOffset = getType(root->childrens[i]->childrens[0]->identifier, root->childrens[i]);

				symbolTable* tempTable = getTable(tableOffset);

				for (int j = 0; j < (signed)tempTable->symbols.size(); j++) {
					if (tempTable->symbols[j].first->name == varName) {
						baseOffset = to_string(atoi(baseOffset.c_str()) + tempTable->symbols[j].first->resRef + counter);
					}
				}
			}
		}
		return atoi(baseOffset.c_str());
	}

	return NULL;
}

bool moonInterpreter::partOf(node* n1, node* n2) {
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

void moonInterpreter::setTableRes(symbolTable* table) {
	int counter = 0;
	for (int i = 0; i < (signed)table->symbols.size(); i++) {
		if (table->symbols[i].first->kind == "variable") {
			if (table->symbols[i].first->type.find("integer") != string::npos) {
				// No array
				if (table->symbols[i].first->type == "integer ") {
					table->symbols[i].first->resRef = counter;
					counter += 4;
				}
				// With array
				else {
					string type = table->symbols[i].first->type;
					type = type.substr(type.find("["));
					table->symbols[i].first->resRef = counter;
					counter += getDimension(type) * 4;
				}
			}
			else {
				if (table->symbols[i].first->type.find("[") == string::npos) {
					table->symbols[i].first->resRef = counter;
					symbolTable* tempTable = getTable(table->symbols[i].first->type);
					if (tempTable->resRef == NULL)
						setTableRes(tempTable);
					counter += tempTable->resRef;
				}
				else {
					string type = table->symbols[i].first->type;
					table->symbols[i].first->resRef = counter;
					symbolTable* tempTable = getTable(type);
					if (tempTable->resRef == NULL)
						setTableRes(tempTable);
					type = type.substr(type.find("["));
					counter += getDimension(type) * tempTable->resRef;
				}
			}
		}
	}
	table->resRef = counter;
}

string moonInterpreter::getType(string name, node* root, symbolTable* table) {
	// set source
	symbolTable* tempTable;
	if (table == NULL)
		tempTable = myTable;
	else
		tempTable = table;

	// traverse table 
	for (int i = 0; i < (signed)tempTable->symbols.size(); i++) {
		if (tempTable->symbols[i].first->name == name) {
			return tempTable->symbols[i].first->type;
		}
		// add function to check that it is in scope
		else if (tempTable->symbols[i].second != NULL && (root == NULL || partOf(root, tempTable->symbols[i].second->root))) {
			string tempOut = getType(name, root, tempTable->symbols[i].second);
			if (tempOut != "")
				return tempOut;
		}
	}
	return "";
}

string moonInterpreter::getTypeAll(node* root) {
	string varName;
	string tableName;
	// loop to get base table
	for (int i = 0; i < (signed)root->childrens.size(); i++) {
		if (root->childrens[i]->identifier == "idnest")
			tableName = getType(root->childrens[i]->childrens[0]->identifier, root->childrens[i]);
		if (root->childrens[i]->childrens.size() == 0 && root->childrens[i]->identifier != "."
			&& root->childrens[i]->identifier != "(" && root->childrens[i]->identifier != ")")
			varName = root->childrens[i]->identifier;
	}

	// get table from name we got
	symbolTable* tempTable = getTable(tableName);

	for (int i = 0; i < (signed)tempTable->symbols.size(); i++) {
		if (!tempTable->symbols[i].first->name.compare(varName)) {
			return tempTable->symbols[i].first->type;
		}
	}
	return "";
}

symbolTable* moonInterpreter::getTable(string name, symbolTable* currentTable) {
	symbolTable* tempTable;
	if (currentTable == NULL) {
		tempTable = myTable;
	}
	else
	{
		tempTable = currentTable;
	}

	for (int i = 0; i < (signed)tempTable->symbols.size(); i++) {
		if (tempTable->symbols[i].second != NULL) {
			if (tempTable->symbols[i].first->name.compare(name))
				return myTable->symbols[i].second;

			symbolTable* tempTable2 = getTable(name, tempTable->symbols[i].second);
			if (tempTable2 != NULL)
				return tempTable2;
		}
	}

	return NULL;
}

void moonInterpreter::setFor(int forOffset, node* root, symbolTable* currentTable) {
	symbolTable* tempTable;
	if (currentTable == NULL) {
		tempTable = myTable;
	}
	else
	{
		tempTable = currentTable;
	}

	string rootId;
	stringstream ss;
	ss << root;
	rootId = ss.str();
	for (unsigned i = 0; i < tempTable->symbols.size(); i++) {
		if (tempTable->symbols[i].first->name.find(rootId) != string::npos) {
			tempTable->symbols[i].first->resRef = forOffset;
			tempTable->symbols[i].second->resRef = forOffset;
			tempTable->symbols[i].second->symbols[0].first->resRef = forOffset;
			break;
		}
		if (tempTable->symbols[i].second != NULL && partOf(root, tempTable->symbols[i].second->root)) {
			setFor(forOffset, root, tempTable->symbols[i].second);
		}
	}
	ss.clear();
}