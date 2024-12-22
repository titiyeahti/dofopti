CC=gcc
CFLAGS= -g -Wall -Wextra -O3 -Iconfigfiles 
#-fsanitize=address
LDLIBS= -lglpk -lsqlite3 -ljson-c
TARGETS= dofopti.out treetest.out hunt.out
OBJECTS= glpapi.o reader.o linprobjson.o constree.o

all: $(TARGETS)

%.out: %.c $(OBJECTS)
	$(CC) $^ $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out *.yy.* *.tab.* $(TARGETS)
