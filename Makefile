CC	= gcc
CXX	= g++
STD	= -std=c++11
OPTS	= -Wall
DBG	= -Os -g
INCL	= -I.

CFLAGS	= $(OPTS) $(DBG) $(INCL) $(WARN)
CXXFLAGS = $(STD) $(OPTS) $(DBG) $(INCL) $(WARN)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $*.o

all:	pi_gen

pi_gen:	pi_gen.o si5351a.o
	$(CC) pi_gen.o si5351a.o -o ./pi_gen

clean:
	rm -f *.o *.xo core .errs.t

clobber: clean
	rm -f *.a pi_gen
