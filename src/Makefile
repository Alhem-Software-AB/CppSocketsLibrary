VERSION =	1.7.2

INCLUDE =	-I/usr/devel/include -I.
PREFIX =	/usr/local
# .h files installed to $(PREFIX)/include/Sockets
# static lib .a files installed to $(PREFIX)/lib

CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD -D_VERSION='"$(VERSION)"'
# uncomment depending on your operating system
#CFLAGS +=	-DLINUX
#CFLAGS +=	-DMACOSX 
# solaris > 8
#CFLAGS +=	-DSOLARIS
# solaris 8
#CFLAGS +=	-DSOLARIS8
# windows and/or cygwin
#CFLAGS +=	-D_WIN32
# cygwin
#CFLAGS +=	-D__CYGWIN__

# uncomment if openssl is installed
CFLAGS += 	-DHAVE_OPENSSL


CPPFLAGS =	$(CFLAGS) 


PROGS =		

all:		libSockets.a $(PROGS) libSocketsEx.a Sockets-config

Sockets-config:	Sockets-config.o
		g++ -o $@ $^

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
		StdoutLog.o \
		HttpPutSocket.o \
		HttpPostSocket.o \
		HttpDebugSocket.o \
		Utility.o \
		PoolSocket.o 

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
		@mkdir -p $(PREFIX)/lib
		cp libSockets.a $(PREFIX)/lib
		cp libSocketsEx.a $(PREFIX)/lib
		@mkdir -p $(PREFIX)/include/Sockets
		cp -a *.h $(PREFIX)/include/Sockets
		install Sockets-config $(PREFIX)/bin

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
