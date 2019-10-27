CCOMP=g++
CFLAGS=-O3 -Wall
LIBFLAGS=-lboost_system -pthread

COMMON_DIR=../common

INCFLAGS=-I$(COMMON_DIR)

EXECUTABLE=SSRFTP_server

SOURCE=server.cpp

OBJS=$(SOURCE:.cpp=.o)

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBFLAGS)

.cpp.o:
	$(CCOMP) $(CFLAGS) $(INCFLAGS) -c -o $@ $< $(LIBFLAGS)

clean:
	rm $(OBJS) $(EXECUTABLE)
