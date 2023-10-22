CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
SRCS=$(wildcard ../src/*.c)
OBJS=$(subst ../src/main.o, , $(patsubst %.c, %.o, $(SRCS)))

.PHONY: gtest_build, clean, debug


# testing setup
TEST=run_test
GTEST_LIB=/gtest/googletest/include/gtest
TEST_FLAGS=-L$(GTEST_LIB) -lgtest

# building libraries
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Add new test target in form of
#
# test_name: example: test_name.cpp $(OBJS)
# 	$(CXX) $(TEST_FLAGS) $< -o $@
#	./$@
#
# then run with make test_name

example: test.cpp $(OBJS)
	$(CXX) $(TEST_FLAGS) $< -o $@
	./$@


literal_vector: literal_vector.cpp ../src/literal_vector.o
	$(CXX) $(TEST_FLAGS) $< -o $@
	./$@


generate_tokens: generate_tokens.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

gtest_build:
	git clone https://github.com/google/googletest.git gtest
	cd gtest; mkdir build; cd build; cmake ..; make

