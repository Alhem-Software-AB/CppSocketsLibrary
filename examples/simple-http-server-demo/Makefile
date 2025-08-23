SOCKETS =	../../dist
PLATFORM =	linux-x86-64

CFLAGS =	`$(SOCKETS)/bin/Sockets-config` \
		-I$(SOCKETS)/include \
		-Wall \
		-std=c++17 \
		-MD

LDFLAGS =	-rdynamic

LIBS =		-L$(SOCKETS)/lib -lSockets -lssl -lcrypto \
		-lpthread

include		$(SOCKETS)/Makefile.version
include		$(SOCKETS)/Makefile.Defines.$(PLATFORM)

PROGS =		simple_http_server

all:		$(PROGS)

SRC =		simple_http_server.cpp
OBJ =		$(patsubst %.cpp, %.o, $(SRC))

simple_http_server: \
		$(OBJ)
		$(link-target)

clean:
		rm -f $(OBJ) $(PROGS)

-include	*.d
