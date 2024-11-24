CC=gcc
CXXFLAGS= -g -Wall -Wextra -O3 -fsanitize=address
LDLIBS= -lglpk -lsqlite3
TARGETS= testglpapi.out test3.out test_faux.out
OBJECTS= glpapi.o

all: $(TARGETS)

test_db.out : test_db.cpp data_import.o 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -ljsoncpp -o $@

data_import.o : data_import.cpp data_import.h 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -ljsoncpp -c $@

%.out: %.c $(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
