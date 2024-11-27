CC=gcc
CFLAGS= -g -Wall -Wextra -O3 -Iconfigfiles
LDLIBS= -lglpk -lsqlite3
TARGETS= dofopti.out
OBJECTS= glpapi.o reader.o

all: $(TARGETS)

%.out: %.c $(OBJECTS)
	$(CC) $^ $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out *.yy.* *.tab.* $(TARGETS)
