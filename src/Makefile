CC=g++
CXXFLAGS= -g -Wall -Wextra -O3 
LDLIBS= -lglpk -ljsoncpp -lsqlite3
TARGETS= testjson.out test_libs.out test_db.out
OBJECTS= data_import.o

all: $(TARGETS)

%.out: %.cpp $(OBJECTS)
	g++ $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
