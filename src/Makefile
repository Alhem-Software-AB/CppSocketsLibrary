VERSION =	1.0

INCLUDE =	-I/usr/devel/include -I.
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD
CPPFLAGS =	$(CFLAGS)
#LIBS =		-L/usr/devel/lib -Lopenssl -L/usr/local/ssl/lib \
#		-lssl -lcrypto 

OBJS =		CircularBuffer.o \
		Socket.o \
		SocketHandler.o \
		TcpSocket.o \
		UdpSocket.o \
		Parse.o \
		Base64.o \
		NullCrypt.o \
		HttpGetSocket.o \
		SSLSocket.o \
		HttpsGetSocket.o \
		CTcpSocket.o 

PROGS =		

all:		libSockets.a $(PROGS)

libSockets.a:	$(OBJS)
		ar cru $@ $(OBJS)
		ranlib $@

install:	all
		cp libSockets.a /usr/local/lib
		mkdir -p /usr/local/include/Sockets
		cp -a *.h /usr/local/include/Sockets

clean:
		rm -f *.o *~ slask *.d $(PROGS) *.a

-include	*.d

tar:		clean
		rm -f Thread.*
		rm -f Mutex.*
		rm -f SocketThread.*
		./mkdot.sh
		tar czf /usr/local/apache/www.alhem.net/htdocs/Sockets/Sockets-$(VERSION).tar.gz *.h *.cpp Makefile
