############################################################################
# Makefile for Burrows-Wheeler transform library and sample program
#
#   $Id: Makefile,v 1.3 2007/09/17 13:21:19 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.3  2007/09/17 13:21:19  michael
#   Changes required for LGPL v3.
#
#   Revision 1.2  2007/07/16 02:08:37  michael
#   Use -pedantic option when compiling.
#
#   Revision 1.1.1.1  2004/08/23 04:34:18  michael
#   Burrows-Wheeler Transform
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -pedantic -ansi -c
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
	DEL = rm
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
