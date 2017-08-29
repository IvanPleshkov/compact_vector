cmake_minimum_required(VERSION 2.8)
project(compact_vector_test)

file(GLOB compact_vector_test_SRC
    "*.h"
    "*.cpp"
	"tests/*.cpp"
	"tests/*.h"
)

add_executable(compact_vector_test ${compact_vector_test_SRC})
