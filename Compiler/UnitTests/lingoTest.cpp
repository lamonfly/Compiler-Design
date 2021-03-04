#include "CppUnitTest.h"
#include "../COMP442/lingo.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(lingoTest)
	{
	public:
		lingo ling;
		shared_ptr<dfa> tokenizer = ling.getLingo();

		TEST_METHOD(id) {
			Assert::IsTrue(Type::id_lexical == tokenizer->getOutput("main")->token_type);
			Assert::IsTrue(Type::id_lexical == tokenizer->getOutput("a12ad")->token_type);
			Assert::IsTrue(Type::id_lexical == tokenizer->getOutput("Fax")->token_type);
			Assert::IsTrue(Type::id_lexical == tokenizer->getOutput("PoPa")->token_type);
		}

		TEST_METHOD(function) {
			state* test = tokenizer->getOutput("write");
			Token testToken;
			testToken.lexeme = "write";
			testToken.type = test->token_type;
			ling.updateType(&testToken);
			Assert::IsTrue(Type::write_operator == testToken.type);
		}

		TEST_METHOD(integer_type) {
			Assert::IsTrue(Type::int_lexical == tokenizer->getOutput("12")->token_type);
			Assert::IsTrue(Type::int_lexical == tokenizer->getOutput("2000")->token_type);
			Assert::IsTrue(Type::int_lexical == tokenizer->getOutput("0")->token_type);
		}

		TEST_METHOD(float_type) {
			Assert::IsTrue(Type::float_lexical == tokenizer->getOutput("1.2")->token_type);
			Assert::IsTrue(Type::float_lexical == tokenizer->getOutput("12.2")->token_type);
			Assert::IsTrue(Type::float_lexical == tokenizer->getOutput("1.223")->token_type);
		}

		TEST_METHOD(sign) {
			Assert::IsTrue(Type::dot == tokenizer->getOutput(".")->token_type);
			Assert::IsTrue(Type::comma == tokenizer->getOutput(",")->token_type);
			Assert::IsTrue(Type::semicolon == tokenizer->getOutput(";")->token_type);
			Assert::IsTrue(Type::colon == tokenizer->getOutput(":")->token_type);
			Assert::IsTrue(Type::double_colon == tokenizer->getOutput("::")->token_type);
			Assert::IsTrue(Type::s_operator == tokenizer->getOutput("<")->token_type);
			Assert::IsTrue(Type::se_operator == tokenizer->getOutput("<=")->token_type);
			Assert::IsTrue(Type::sb_operator == tokenizer->getOutput("<>")->token_type);
			Assert::IsTrue(Type::b_operator == tokenizer->getOutput(">")->token_type);
			Assert::IsTrue(Type::be_operator == tokenizer->getOutput(">=")->token_type);
			Assert::IsTrue(Type::assignment == tokenizer->getOutput("=")->token_type);
			Assert::IsTrue(Type::ee_operator == tokenizer->getOutput("==")->token_type);
			Assert::IsTrue(Type::addition == tokenizer->getOutput("+")->token_type);
			Assert::IsTrue(Type::substraction == tokenizer->getOutput("-")->token_type);
			Assert::IsTrue(Type::multiplication == tokenizer->getOutput("*")->token_type);
			Assert::IsTrue(Type::division == tokenizer->getOutput("/")->token_type);
		}

		TEST_METHOD(comment) {
			Assert::IsTrue(Type::comment_multiline == tokenizer->getOutput("/* some test * */")->token_type);
			Assert::IsTrue(Type::comment == tokenizer->getOutput("// some test")->token_type);
		}

		TEST_METHOD(logic) {
			Assert::IsTrue(Type::and_logic == tokenizer->getOutput("&&")->token_type);
			Assert::IsTrue(Type::not_logic == tokenizer->getOutput("!")->token_type);
			Assert::IsTrue(Type::or_logic == tokenizer->getOutput("||")->token_type);
		}

		TEST_METHOD(bracket) {
			Assert::IsTrue(Type::open_parenthesis == tokenizer->getOutput("(")->token_type);
			Assert::IsTrue(Type::close_parenthesis == tokenizer->getOutput(")")->token_type);
			Assert::IsTrue(Type::open_brace == tokenizer->getOutput("{")->token_type);
			Assert::IsTrue(Type::close_brace == tokenizer->getOutput("}")->token_type);
			Assert::IsTrue(Type::open_square == tokenizer->getOutput("[")->token_type);
			Assert::IsTrue(Type::close_square == tokenizer->getOutput("]")->token_type);
		}

		TEST_METHOD(demo) {
			Assert::IsFalse(Type::open_parenthesis == tokenizer->getOutput("(")->token_type);
		}
	};
}