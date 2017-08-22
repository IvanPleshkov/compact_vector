// Микрофреймворк для юнит-тестирования

#pragma once

#include <vector>
#include <functional>
#include <iostream>

struct Test
{
	std::string name;

	std::function<void()> test;

	Test(const char* name, std::function<void()> test);
};

struct TestsRunner
{
	static void AddTest(Test& test)
	{
		GetAllTests().push_back(test);
	}

	static std::vector<Test>& GetAllTests()
	{
		static std::vector<Test> tests;
		return tests;
	}

	static void RunAllTests()
	{
		std::vector<Test>& tests = GetAllTests();
		for (auto& test : tests)
		{
			bool success = true;
			try
			{
				test.test();
			}
			catch (...)
			{
				success = false;
			}

			PrintResult(test.name, success);
		}
	}

	static void PrintResult(std::string name, bool success)
	{
		if (success)
			printf("[OK] ");
		else
			printf("[FAIL] ");
		printf("%s\n", name.c_str());
	}
};

inline Test::Test(const char* name, std::function<void()> test) :
	name(name),
	test(test)
{
	TestsRunner::AddTest(*this);
}

#define COMPACT_VECTOR_TEST(name)		\
void name();							\
										\
namespace								\
{										\
Test test_##name(#name, name);			\
}										\
										\
void name()


#define COMPACT_VECTOR_ASSERT(expr)		\
if (!(expr)) throw std::exception();
