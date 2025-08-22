INCLUDE =	-I..

CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD
CPPFLAGS =	$(CFLAGS)

PROGS =		util

all:		$(PROGS)

util:		util.o ../Parse.o
		g++ -o $@ $^ $(LIBS) $(LDFLAGS)

clean:
		rm -f *.o *~ slask *.d $(PROGS)

-include	*.d
