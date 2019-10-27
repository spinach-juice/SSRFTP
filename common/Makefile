CCOMP=g++
CFLAGS=-O3 -Wall
LIBFLAGS=-lboost_system -pthread

SOURCE=packet.cpp

OBJS=$(SOURCE:.cpp=.o)

.PHONY: clean

all: $(OBJS)

.cpp.o:
	$(CCOMP) $(CFLAGS) -c -o $@ $< $(LIBFLAGS)

clean:
	rm $(OBJS) $(EXECUTABLE)
