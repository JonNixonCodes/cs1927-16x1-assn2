CC = gcc
CFLAGS = -Wall -Werror -g

.PHONY:	clean clobber

all: assn2

%o:%c
	$(CC) $(CFLAGS) -c $<

assn2: main.o Graph.o Agent.o Stack.o Lists.o
	$(CC) $(CFLAGS) -o assn2 main.o Graph.o Agent.o Stack.o Lists.o

clean:
	rm -f *.o core

clobber: clean
	rm -f assn2
