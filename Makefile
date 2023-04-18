CC=gcc
CFLAGS= -g -pedantic -std=gnu17 -Wall -Werror -Wextra -lm -lcrypto

.PHONY: all
all: nyufile

nyufile: nyufile.o argparse.o

nyufile.o: nyufile.c argparse.h

argparse.o: argparse.c argparse.h

.PHONY: clean
clean:
	rm -f *.o nyufile

test:
	zip nyufile.zip Makefile *.c *.h;
	mv nyufile.zip nyufile-autograder
