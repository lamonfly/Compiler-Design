#include "reader.h"

reader::reader()
{
}

reader::reader(lingo* ling) {
	lang = ling;
	tokenizer = ling->getLingo();
	firstToken = true;
	eotBool = false;
	line = 0;
}

reader::~reader()
{
}

bool reader::setFile(string path) {
	file.open(path);
	if (file.is_open()) {
		return true;
	}
	return false;
}

Token reader::nextToken() {
	nextLine();
	if (eol) {
		index++;
		eol = false;
		return getEolToken();
	}
	Token temp = getToken();
	if (temp.type == Type::comment) {
		eol = true;
	}
	return temp;
}

Token reader::getToken() {
	// use line to get next token given index
	Token temp;
	int secondIndex = 1;
	string substring;
	state* tempState = NULL;

	do {
		if (lineData.size() < index + secondIndex) {
			firstToken = true;
			if (tempState != NULL && tempState->token_type == Type::comment_multiline) {
				string tempLine = lineData;
				nextLine();
				index++;
				lineData = tempLine + lineData;
			}
			else {
				nextLine();
				temp = getEolToken();
				break;
			}
		}
		else {
			substring = lineData.substr(index, secondIndex);
			tempState = tokenizer->getOutput(substring);

			if (tempState != NULL && tempState->token_type == Type::comment) {
				firstToken = true;
				temp.lexeme = lineData;
				temp.type = tempState->token_type;
				break;
			}

			if (tempState == NULL) {
				int thirdIndex = 1;
				if ((index + secondIndex + thirdIndex > lineData.length())) {
					secondIndex--;
					substring = lineData.substr(index, secondIndex);
					tempState = tokenizer->getOutput(substring);
					temp.lexeme = substring;
					temp.type = tempState->token_type;
					break;
				}

				while ((index + secondIndex + thirdIndex) <= lineData.length()) {
					string substr2 = lineData.substr(index, secondIndex + thirdIndex);
					tempState = tokenizer->getOutput(substr2);
					if (tempState != NULL) {
						secondIndex += thirdIndex - 1;
						break;
					}
					else if ((index + secondIndex + thirdIndex) == lineData.length()) {
						secondIndex--;
						substr2 = lineData.substr(index, secondIndex);
						if (substr2.size() > 0) {
							tempState = tokenizer->getOutput(substr2);
							temp.lexeme = substr2;
							if (tempState == NULL) {
								temp.type = Type::error_token;
							}
							else {
								temp.type = tempState->token_type;
							}
							goto endloop;
						}
						else {
							secondIndex++;
						}
					}
					thirdIndex++;
				}
				secondIndex += 1;
			}
			else if (tempState->testForward) {
				secondIndex++;
				if (lineData.size() < index + secondIndex) {
					if (tempState != NULL) {
						temp.lexeme = substring;
						temp.type = tempState->token_type;
						break;
					}
				}
			}
			else if (tempState->token_type == Type::error_token) {
				temp.lexeme = substring;
				temp.type = tempState->token_type;
				break;
			}
			else if (tempState->isFinal){
				//token found
				temp.lexeme = substring;
				temp.type = tempState->token_type;
				break;
			}
			else {
				secondIndex++;
			}
		}

	} while (true);
	endloop:;

	lang->updateType(&temp);
	temp.line = line;
	temp.index = index;
	index += secondIndex;
	return temp;
}

void reader::nextLine() {
	if (firstToken) {
		if (!getline(file, lineData)) {
			eotBool = true;
		}

		firstToken = false;
		line++;
		index = -1;
	}
}

bool reader::eot() {
	return eotBool;
}

Token reader::getEolToken() {
	Token eolToken;
	eolToken.lexeme = "endl";
	eolToken.type = Type::eol_token;
	eolToken.index = -1;
	eolToken.line = line;
	return eolToken;
}