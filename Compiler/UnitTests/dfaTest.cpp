#include "CppUnitTest.h"
#include "../COMP442/dfa.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(dfaTest)
	{
	public:
		
		TEST_METHOD(Start_state)
		{
			dfa dfa;
			state* test = dfa.new_startstate();
			Assert::IsTrue(test->isStart);
		}

		TEST_METHOD(Default_state) {
			dfa dfa;
			state* test = dfa.new_state();
			Assert::IsFalse(test->isStart);
			Assert::IsFalse(test->isFinal);
			Assert::AreEqual(static_cast<int>(Type::not_token), static_cast<int>(test->token_type));
		}

		TEST_METHOD(Special_state) {
			dfa dfa;
			state* test = dfa.new_state(true, true, Type::dot);
			Assert::IsTrue(test->isFinal);
			Assert::IsTrue(test->testForward);
			Assert::AreEqual(static_cast<int>(Type::dot), static_cast<int>(test->token_type));
		}

		TEST_METHOD(Table) {
			dfa dfa;
			state* start = dfa.new_startstate();
			state* end = dfa.new_state(true);
			dfa.addLink(start, "x", end);
			state* current = dfa.table(start->id, "x");
			Assert::AreEqual(end->id, current->id);
		}

		TEST_METHOD(Output) {
			dfa dfa;
			state* start = dfa.new_startstate();
			state* bridge = dfa.new_state();
			state* end = dfa.new_state(true);
			dfa.addLink(start, "x", start);
			dfa.addLink(start, "y", bridge);
			dfa.addLink(bridge, "y", bridge);
			dfa.addLink(bridge, "epsilon", end);
			Assert::IsFalse(NULL == dfa.getOutput("xy"));
			Assert::IsTrue(dfa.getOutput("yyyyyy")->isFinal);
		}
	};
}