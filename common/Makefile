CCOMP=g++
CFLAGS=-O3 -Wall -std=c++11 -fPIC
LIBFLAGS=-lboost_system -pthread

SOURCE=packet.cpp util.cpp rest_server.cpp communicator.cpp
TEST_SRC=test_main.cpp packet_tests.cpp util_tests.cpp rest_server_tests.cpp communicator_tests.cpp

OBJS=$(SOURCE:.cpp=.o)
TEST_OBJS=$(TEST_SRC:.cpp=.o)

LIB_NAME=SSRFTP_common
LIBFILE=lib$(LIB_NAME).so

TEST_EXE=tests

.PHONY: clean run_tests

all: $(LIBFILE)

$(LIBFILE): $(OBJS)
	$(CCOMP) $(CFLAGS) -o $(LIBFILE) $(OBJS) -shared $(LIBFLAGS)

.cpp.o:
	$(CCOMP) $(CFLAGS) -c -o $@ $< $(LIBFLAGS)

run_tests: $(TEST_EXE)
	sudo ./$(TEST_EXE)

$(TEST_EXE): $(OBJS) $(TEST_OBJS)
	$(CCOMP) $(CFLAGS) -o $(TEST_EXE) $(OBJS) $(TEST_OBJS) $(LIBFLAGS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_EXE) $(LIBFILE)
