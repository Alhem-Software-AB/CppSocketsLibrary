SOCKETS =       ../dist
PLATFORM =	linux-x86-32
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
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

resolve:	resolve.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

sockets_test:	sockets_test.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

base64:		base64.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

semtest:	semtest.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

echoserver:	echoserver.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

stressclient:	stressclient.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

http:		http.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

httpd:		httpd.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

retry:		retry.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

resume:		resume.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

http_post:	http_post.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

x:		x.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

copy:		copy.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

crlf:		crlf.o ../src/libSockets.a 
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

https:		https.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

sloppy_http:	sloppy_http.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

chunked:	chunked.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

threadstress:	threadstress.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

listener:	listener.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

scanr:		scanr.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

httpd_test:	httpd_test.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

json:		json.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

detach:		detach.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

bind_address:	bind_address.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

httpget:	httpget.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

udp:		udp.o ../src/libSockets.a
		$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

clean:
		rm -f *.o *~ slask *.d $(PROGS)

-include	*.d

setup:
		@echo No longer necessary
