CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

all: parent child

parent: parent.c
	$(CC) $(CFLAGS) parent.c -o parent

child: child.c
	$(CC) $(CFLAGS) child.c -o child

clean:
	rm -f parent child
