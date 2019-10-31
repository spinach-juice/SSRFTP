CCOMP=g++
CFLAGS=-O3 -Wall -std=c++11
LIBFLAGS=-lboost_system -pthread

SOURCE=packet.cpp util.cpp
TEST_SRC=test_main.cpp packet_tests.cpp util_tests.cpp

OBJS=$(SOURCE:.cpp=.o)
TEST_OBJS=$(TEST_SRC:.cpp=.o)

TEST_EXE=tests

.PHONY: clean run_tests

all: $(OBJS)

.cpp.o:
	$(CCOMP) $(CFLAGS) -c -o $@ $< $(LIBFLAGS)

run_tests: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): $(OBJS) $(TEST_OBJS)
	$(CCOMP) $(CFLAGS) -o $(TEST_EXE) $(LIBFLAGS) $(OBJS) $(TEST_OBJS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_EXE)
