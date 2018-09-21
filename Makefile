CC	= gcc
CXX	= g++
STD	= -std=c++11
OPTS	= -Wall
DBG	= -O0 -g
INCL	= -I.
WARN	= -Wno-nonportable-include-path
.SUFFIXES: .xo

CFLAGS	= $(OPTS) $(DBG) $(INCL) $(WARN)
CXXFLAGS = $(STD) $(OPTS) $(DBG) $(INCL) $(WARN)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

.c.xo:
	$(CC) -c $(CFLAGS) -DTEST -Wno-unused-function $< -o $*.xo

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $*.o

OBJS	= si5351a.o rational.o

all:	libsi5351a.a pi_gen

pi_gen:	pi_gen.o libsi5351a.a
	$(CC) pi_gen.o -L. -lsi5351a -o ./pi_gen

testbed: testbed.o si5351a.xo libsi5351a.a
	$(CXX) testbed.o si5351a.xo -L. -lsi5351a -o ./testbed

libsi5351a.a: $(OBJS)
	ar cr libsi5351a.a $(OBJS)

clean:
	rm -f *.o *.xo core .errs.t

clobber: clean
	rm -f *.a testbed
