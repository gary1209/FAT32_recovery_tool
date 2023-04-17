CC=gcc
CFLAGS= -g -pedantic -std=gnu17 -Wall -Werror -Wextra -lm -lcrypto

.PHONY: all
all: nyufile

nyufile: nyufile.c

.PHONY: clean
clean:
	rm -f *.o nyufile

test:
	zip nyufile.zip Makefile *.c;
	mv nyufile.zip nyufile-autograder
