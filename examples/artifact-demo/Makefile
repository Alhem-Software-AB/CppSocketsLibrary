SOCKETS =	dist
PLATFORM =	linux-x86-64

CFLAGS =	`$(SOCKETS)/bin/Sockets-config` \
		-I$(SOCKETS)/include \
		-Wall \
		-std=c++17 \
		-MD

LDFLAGS =	-rdynamic

LIBS =		-L$(SOCKETS)/lib -lSockets -lssl -lcrypto \
		-lpthread

include 	$(SOCKETS)/Makefile.version
include 	$(SOCKETS)/Makefile.Defines.$(PLATFORM)

all:		chat_server

chat_server:	chat_server.o
		$(link-target)

clean:
		rm -f chat_server.o chat_server

-include	*.d
