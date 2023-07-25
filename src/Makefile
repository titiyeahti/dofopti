CC=g++
CXXFLAGS= -g -Wall -Wextra 
LDLIBS= -lglpk -ljsoncpp -lsqlite3
TARGETS= testjson test_libs test_db
OBJECTS= data_import.o

all: $(TARGETS)

test%: test%.cpp $(OBJECTS)
	g++ $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
