CCOMP=g++
CFLAGS=-O3 -Wall
LIBFLAGS=-lboost_system -pthread

EXECUTABLE=SSRFTP_client

SOURCE=client.cpp

OBJS=$(SOURCE:.cpp=.o)

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CCOMP) $(CFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBFLAGS)

.cpp.o:
	$(CCOMP) $(CFLAGS) -c -o $@ $< $(LIBFLAGS)

clean:
	rm $(OBJS) $(EXECUTABLE)
