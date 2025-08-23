SOCKETS =       ../dist
PLATFORM =	linux-x86-64
INCLUDE =	-I$(SOCKETS)/include
LIBS =		-L$(SOCKETS)/lib -lSockets
#-L$(SOCKETS) -lSockets 

CFLAGS =	`$(SOCKETS)/bin/Sockets-config`

CFLAGS +=	-I.
CFLAGS +=	-MD

include		$(SOCKETS)/Makefile.version
include		$(SOCKETS)/Makefile.Defines.$(PLATFORM)

CPPFLAGS =	$(CFLAGS)

#LIBS +=	-lsctp

LDFLAGS +=	-rdynamic

PROGS =		events resolve sockets_test base64 semtest \
		echoserver stressclient http httpd retry resume \
		http_post x copy crlf https sloppy_http chunked \
		threadstress listener httpd_test scanr json \
		detach bind_address httpget udp

all:		$(PROGS)

events:		events.o ../src/libSockets.a 
		$(link-target)

resolve:	resolve.o ../src/libSockets.a 
		$(link-target)

sockets_test:	sockets_test.o ../src/libSockets.a 
		$(link-target)

base64:		base64.o ../src/libSockets.a 
		$(link-target)

semtest:	semtest.o ../src/libSockets.a 
		$(link-target)

echoserver:	echoserver.o ../src/libSockets.a 
		$(link-target)

stressclient:	stressclient.o ../src/libSockets.a 
		$(link-target)

http:		http.o ../src/libSockets.a 
		$(link-target)

httpd:		httpd.o ../src/libSockets.a 
		$(link-target)

retry:		retry.o ../src/libSockets.a 
		$(link-target)

resume:		resume.o ../src/libSockets.a 
		$(link-target)

http_post:	http_post.o ../src/libSockets.a 
		$(link-target)

x:		x.o ../src/libSockets.a 
		$(link-target)

copy:		copy.o ../src/libSockets.a 
		$(link-target)

crlf:		crlf.o ../src/libSockets.a 
		$(link-target)

https:		https.o ../src/libSockets.a
		$(link-target)

sloppy_http:	sloppy_http.o ../src/libSockets.a
		$(link-target)

chunked:	chunked.o ../src/libSockets.a
		$(link-target)

threadstress:	threadstress.o ../src/libSockets.a
		$(link-target)

listener:	listener.o ../src/libSockets.a
		$(link-target)

scanr:		scanr.o ../src/libSockets.a
		$(link-target)

httpd_test:	httpd_test.o ../src/libSockets.a
		$(link-target)

json:		json.o ../src/libSockets.a
		$(link-target)

detach:		detach.o ../src/libSockets.a
		$(link-target)

bind_address:	bind_address.o ../src/libSockets.a
		$(link-target)

httpget:	httpget.o ../src/libSockets.a
		$(link-target)

udp:		udp.o ../src/libSockets.a
		$(link-target)

clean:
		rm -f *.o *~ slask *.d $(PROGS)

-include	*.d

setup:
		@echo No longer necessary
