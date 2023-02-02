VERSION =	1.6

INCLUDE =	-I/usr/devel/include -I.
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD -D_VERSION='"$(VERSION)"' 
# manual autoconf ....

# uncomment if your operating system is linux, mac os x, or solaris
#CFLAGS +=	-DLINUX
#CFLAGS +=	-DMACOSX 
#CFLAGS +=	-DSOLARIS

# uncomment if openssl is installed
CFLAGS += 	-DHAVE_OPENSSL


CPPFLAGS =	$(CFLAGS) 


PROGS =		

all:		libSockets.a $(PROGS) libSocketsEx.a


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
		HttpsSocket.o \
		Thread.o SocketThread.o \
		StdoutLog.o 

libSockets.a:	$(OBJS)
		ar cru $@ $(OBJS)
		ranlib $@

# SocketsEx
EX =		Uid.o \
		MinderSocket.o MinionSocket.o MinderHandler.o

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
		rm -f Mutex.*
		rm -f MinderSocket_T.h
		rm -f uuid.h
		rm -f Stdin.*
		./mkdot.sh
		tar czf Sockets-$(VERSION).tar.gz *.h *.cpp Makefile Project/*.dsp \
			README.macosx gpl.txt
		cp Changelog /usr/local/apache/www.libsockets.net/htdocs
		doxygen doxygen.cfg
		/usr/local/bin/tarfix.sh Sockets-$(VERSION)
		cp Sockets-$(VERSION).tar.gz /usr/local/apache/www.libsockets.net/htdocs

docs:		clean
		rm -f Mutex.*
		rm -f MinderSocket_T.h
		rm -f uuid.h
		rm -f Stdin.*
		./mkdot.sh
		doxygen doxygen.cfg
