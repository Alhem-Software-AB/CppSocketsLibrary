VERSION =	1.2.1

INCLUDE =	-I/usr/devel/include -I.
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD -D_VERSION='"$(VERSION)"'
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
		CTcpSocket.o \
		HTTPSocket.o \
		HttpsSocket.o 
# SocketsEx
EX =		Uid.o \
		MinderSocket.o MinionSocket.o MinderHandler.o

PROGS =		

all:		libSockets.a $(PROGS) libSocketsEx.a

libSockets.a:	$(OBJS)
		ar cru $@ $(OBJS)
		ranlib $@

libSocketsEx.a:	$(EX)
		ar cru $@ $(EX)
		ranlib $@

install:	all
		cp libSockets.a /usr/local/lib
		cp libSocketsEx.a /usr/local/lib
		mkdir -p /usr/local/include/Sockets
		cp -a *.h /usr/local/include/Sockets

clean:
		rm -f *.o *~ slask *.d $(PROGS) *.a

-include	*.d

tar:		clean
		rm -f Thread.*
		rm -f Mutex.*
		rm -f SocketThread.*
		rm -f MinderSocket_T.h
		./mkdot.sh
		tar czf /usr/local/apache/www.alhem.net/htdocs/Sockets/Sockets-$(VERSION).tar.gz *.h *.cpp Makefile Project/*.dsp
		cp Changelog /usr/local/apache/www.alhem.net/htdocs/Sockets
		doxygen doxygen.cfg
