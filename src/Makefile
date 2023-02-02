VERSION =	2.0
DIFF_VERSION =	1.9.9

INCLUDE =	-I.
PREFIX =	/usr/local
# .h files installed to $(PREFIX)/include/Sockets
# static lib .a files installed to $(PREFIX)/lib

CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD -D_VERSION='"$(VERSION)"'
#CFLAGS +=	-D_DEBUG

CXX =		g++

# The flag _THREADSAFE_SOCKETS does NOT mean that all operations in 
# SocketHandler/Socket derivatives becomes thread safe unfortunately.
# It merely enables a small fix to sync writing to the socket
# output buffer / internal reading from that output buffer.
# 2006-01-26: obsolete
#CFLAGS +=	-D_THREADSAFE_SOCKETS

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

# if you want the library to use openssl, make sure the next line is uncommented
CFLAGS += 	-DHAVE_OPENSSL

# uncomment these three lines if the library should be in its own namespace
#CFLAGS +=	-DSOCKETS_NAMESPACE=sockets
#CFLAGS +=	-DSOCKETS_NAMESPACE_STR='"sockets"'

CPPFLAGS =	$(CFLAGS) 

HTDOCS =	/usr/local/apache/www.alhem.net/htdocs

PROGS =		Sockets-config

all:		libSockets.a $(PROGS)

libs:		all libSocketsEx.a

Sockets-config:	Sockets-config.o
		$(CXX) -o $@ $^

OBJS =		CircularBuffer.o \
		Socket.o \
		SocketHandler.o \
		TcpSocket.o \
		UdpSocket.o \
		Parse.o \
		Base64.o \
		HttpGetSocket.o \
		HTTPSocket.o \
		Thread.o SocketThread.o \
		StdoutLog.o \
		HttpPutSocket.o \
		HttpPostSocket.o \
		HttpDebugSocket.o \
		Utility.o \
		PoolSocket.o \
		ResolvServer.o \
		ResolvSocket.o \
		HttpdSocket.o \
		HttpdCookies.o \
		HttpdForm.o \
		File.o \
		MemFile.o \
		socket_include.o \
		Mutex.o \
		Lock.o \
		EventTime.o Event.o IEventOwner.o EventHandler.o

libSockets.a:	$(OBJS)
		ar cru $@ $(OBJS)
		ranlib $@

# SocketsEx
EX =		Uid.o \
		NullCrypt.o \
		CTcpSocket.o \
		MinderSocket.o MinionSocket.o MinderHandler.o

libSocketsEx.a:	$(EX)
		ar cru $@ $(EX)
		ranlib $@

diff:
		diff -C 3 $(HTDOCS)/Sockets/Sockets-$(DIFF_VERSION) . | \
			/usr/local/bin/d2html > \
			$(HTDOCS)/Sockets/latest_diff.html

install:	all
		@mkdir -p $(PREFIX)/lib
		cp libSockets.a $(PREFIX)/lib
		cp libSocketsEx.a $(PREFIX)/lib
		@mkdir -p $(PREFIX)/include/Sockets
		cp -a *.h $(PREFIX)/include/Sockets
		@rm -f $(PREFIX)/include/Sockets/SSLSocket.*
		@rm -f $(PREFIX)/include/Sockets/HttpsGetSocket.*
		@rm -f $(PREFIX)/include/Sockets/HttpsSocket.*
		@mkdir -p $(PREFIX)/bin
		install Sockets-config $(PREFIX)/bin

clean:
		rm -f *.o *~ slask *.d $(PROGS) *.a */*~

-include	*.d

# no binary files, zip will translate lf to cr lf
FILES =		*.h *.cpp Makefile Project/*.ds* \
		README.macosx gpl.txt mkdot.sh \
		Project.net/Sockets/*.vcproj Project.net/Sockets/*.sln \
		Project.net/Test/*.vcproj \
		DevCpp/*.dev 

tar:		clean
		rm -f MinderSocket_T.h
		rm -f uuid.h
		rm -f Stdin.*
		rm -f ListenSocketBase.*
		tar czf Sockets-$(VERSION).tar.gz $(FILES)
		zip -l Sockets-$(VERSION).zip $(FILES)
		/usr/local/bin/tarfix.sh Sockets-$(VERSION)
		cp Sockets-$(VERSION).tar.gz $(HTDOCS)/Sockets
		cp Sockets-$(VERSION).zip $(HTDOCS)/Sockets

docs:		clean
		doxygen dox2.cfg
		rm -f MinderSocket_T.h
		rm -f uuid.h
		rm -f Stdin.*
		rm -f sockets_test.cpp
		rm -f ListenSocketBase.*
		./mkdot.sh
		doxygen doxygen.cfg
