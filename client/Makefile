CCOMP=g++
CFLAGS=-O3 -Wall -std=c++11
LIBFLAGS=-lboost_system -pthread

COMMON_DIR=../common

INCFLAGS=-I$(COMMON_DIR)

EXECUTABLE=SSRFTP_client

SOURCE=client.cpp

OBJS=$(SOURCE:.cpp=.o) ../common/packet.o ../common/util.o

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBFLAGS)

.cpp.o:
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -c -o $@ $< $(LIBFLAGS)

clean:
	rm $(OBJS) $(EXECUTABLE)
