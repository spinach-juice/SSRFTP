CCOMP=g++

CFLAGS=-O3 -Wall -std=c++11

COMMONLIB=SSRFTP_common
COMMON_DIR=../common

LIBFLAGS=-lboost_system -l$(COMMONLIB) -g -pthread -L$(COMMON_DIR) -Wl,-rpath=$(COMMON_DIR)

INCFLAGS=-I$(COMMON_DIR)

EXECUTABLE=SSRFTP_server

SOURCE=server.cpp ShardChecker.cpp

OBJS=$(SOURCE:.cpp=.o)

CMNDEP=$(COMMON_DIR)/lib$(COMMONLIB).so

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS) $(CMNDEP)
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBFLAGS)

.cpp.o:
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -c -o $@ $< $(LIBFLAGS)

$(CMNDEP):
	cd $(COMMON_DIR) && $(MAKE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
