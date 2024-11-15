CC=gcc
CXXFLAGS= -g -Wall -Wextra -O3 
LDLIBS= -lglpk -ljsoncpp -lsqlite3
TARGETS= testglpapi.out
OBJECTS= glpapi.o

all: $(TARGETS)

%.out: %.c $(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf *.o *.out $(TARGETS)
