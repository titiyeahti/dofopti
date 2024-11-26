CC=gcc
CXXFLAGS= -g -Wall -Wextra -O3 -fsanitize=address
LDLIBS= -lglpk -lsqlite3
TARGETS= dofopti.out
OBJECTS= glpapi.o reader.o

all: $(TARGETS)

constraint.out : constraint.c constraint.tab.c lex.yy.c
	gcc -o $@ $^

lex.yy.c : constraint.l bison
	flex constraint.l

bison : constraint.y
	bison -d $^ -Wcounterexample

test_db.out : test_db.cpp data_import.o 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -ljsoncpp -o $@

data_import.o : data_import.cpp data_import.h 
	g++ $^ $(CXXFLAGS) $(LDLIBS) -ljsoncpp -c $@


%.out: %.c $(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out *.yy.* *.tab.* $(TARGETS)
