CC = gcc
CFLAGS = -Wall -Werror -g

.PHONY:	clean clobber

all: assn2

%o:%c
	$(CC) $(CFLAGS) -c $<

assn2: main.o Graph.o Agent.o
	$(CC) -o assn2 main.o Graph.o Agent.o

clean:
	rm -f *.o core

clobber: clean
	rm -f assn2
