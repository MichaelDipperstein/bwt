############################################################################
# Makefile for Burrows-Wheeler transform library and sample program
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O2 -o

# libraries
LIBS = -L. -lbwt -loptlist

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm -f
endif

all:		sample$(EXE)

sample$(EXE):	sample.o libbwt.a liboptlist.a
		$(LD) $< $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c bwxform.h optlist.h
		$(CC) $(CFLAGS) $<

libbwt.a:	bwxform.o
		ar crv libbwt.a bwxform.o
		ranlib libbwt.a

bwxform.o:	bwxform.c bwxform.h
		$(CC) $(CFLAGS) $<

liboptlist.a:	optlist.o
		ar crv liboptlist.a optlist.o
		ranlib liboptlist.a

optlist.o:	optlist.c optlist.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
