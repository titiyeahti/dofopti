CC=g++
CXXFLAGS= -g -Wall -Wextra -O3 
LDLIBS= -lglpk -ljsoncpp -lsqlite3
TARGETS= test_db.out
OBJECTS= data_import.o problem.o

all: $(TARGETS)

%.out: %.cpp $(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
