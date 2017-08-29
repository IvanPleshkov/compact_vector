#include "tests_runner.h"
#include "../compact_vector.h"

std::string test_string = "hello_world_hello_world_hello_world_hello_world";

COMPACT_VECTOR_TEST(constructor_1)
{
	compact_vector<uint8_t> vector(10);
}

COMPACT_VECTOR_TEST(constructor_2)
{
	compact_vector<uint8_t> vector(100);
}

COMPACT_VECTOR_TEST(constructor_3)
{
	uint8_t default_value = 100;
	size_t size = 10;
	compact_vector<uint8_t> vector(size, default_value);

	COMPACT_VECTOR_ASSERT(vector.size() == size);
	for (int i = 0; i < vector.size(); i++)
		COMPACT_VECTOR_ASSERT(vector.at(i) == default_value);
}

COMPACT_VECTOR_TEST(constructor_4)
{
	uint8_t default_value = 100;
	size_t size = 100;
	compact_vector<uint8_t> vector(size, default_value);

	auto t = vector.size();
	COMPACT_VECTOR_ASSERT(vector.size() == size);
	for (int i = 0; i < vector.size(); i++)
		COMPACT_VECTOR_ASSERT(vector.at(i) == default_value);
}

COMPACT_VECTOR_TEST(constructor_5)
{
	compact_vector<std::string, 10> vector(10);
}

COMPACT_VECTOR_TEST(constructor_6)
{
	compact_vector<std::string, 10> vector(100);
}

COMPACT_VECTOR_TEST(constructor_7)
{
	size_t size = 10;
	compact_vector<std::string, 10> vector(size, test_string);

	COMPACT_VECTOR_ASSERT(vector.size() == size);
	for (int i = 0; i < vector.size(); i++)
		COMPACT_VECTOR_ASSERT(vector.at(i) == test_string);
}

COMPACT_VECTOR_TEST(constructor_8)
{
	size_t size = 100;
	compact_vector<std::string, 10> vector(size, test_string);

	auto t = vector.size();
	COMPACT_VECTOR_ASSERT(vector.size() == size);
	for (int i = 0; i < vector.size(); i++)
		COMPACT_VECTOR_ASSERT(vector.at(i) == test_string);
}

COMPACT_VECTOR_TEST(constructor_end)
{
	throw new std::exception();
}
