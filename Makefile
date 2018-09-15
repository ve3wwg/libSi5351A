CC	= gcc
CXX	= g++
STD	= -std=c++11
OPTS	= -Wall
DBG	= -O0 -g
INCL	= -I.

CFLAGS	= $(OPTS) $(DBG) $(INCL)
CXXFLAGS = $(STD) $(OPTS) $(DBG) $(INCL)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $*.o

OBJS	=

all:	libsi5351a.a

libsi5351a.a: si5351a.o
	ar cr libsi5351a.a si5351a.o

clean:
	rm -f *.o core .errs.t

clobber: clean
	rm -f *.a
