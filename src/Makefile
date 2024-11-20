CC=gcc
CXXFLAGS= -g -Wall -Wextra -O3 
LDLIBS= -lglpk -ljsoncpp -lsqlite3
TARGETS= testglpapi.out
OBJECTS= glpapi.o

all: $(TARGETS)

test_db.out : test_db.cpp data_import.o 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -o $@

data_import.o : data_import.cpp data_import.h 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -c $@

%.out: %.c $(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
