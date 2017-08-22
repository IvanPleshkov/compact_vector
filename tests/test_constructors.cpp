#include "tests_runner.h"
#include "../compact_vector.h"

COMPACT_VECTOR_TEST(constructor_1)
{
	compact_vector<uint8_t> vector();
}


COMPACT_VECTOR_TEST(constructor_2)
{
	COMPACT_VECTOR_ASSERT(true);
}


COMPACT_VECTOR_TEST(constructor_3)
{
	throw new std::exception();
}
