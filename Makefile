############################################################################
# Makefile for Burrows-Wheeler transform library and sample program
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O2 -o

# libraries
LIBS = -L. -Loptlist -lbwt -loptlist

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

sample$(EXE):	sample.o libbwt.a  optlist/liboptlist.a
		$(LD) $< $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c bwxform.h optlist/optlist.h
		$(CC) $(CFLAGS) $<

libbwt.a:	bwxform.o
		ar crv libbwt.a bwxform.o
		ranlib libbwt.a

bwxform.o:	bwxform.c bwxform.h
		$(CC) $(CFLAGS) $<

optlist/liboptlist.a:
		cd optlist && $(MAKE) liboptlist.a

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
		cd optlist && $(MAKE) clean
