############################################################################
# Makefile for Burrows-Wheeler transform library and sample program
#
#   $Id: Makefile,v 1.1.1.1 2004/08/23 04:34:18 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.1.1.1  2004/08/23 04:34:18  michael
#   Burrows-Wheeler Transform
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -ansi -c
LDFLAGS = -O2 -o

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

sample$(EXE):	sample.o bwxform.o getopt.o
		$(LD) $^ $(LDFLAGS) $@

sample.o:	sample.c bwxform.h getopt.h
		$(CC) $(CFLAGS) $<

bwxform.o:	bwxform.c bwxform.h
		$(CC) $(CFLAGS) $<

getopt.o:	getopt.c getopt.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) sample$(EXE)
