SOCKETS=../src

CXX=g++
CXXFLAGS=`$(SOCKETS)/Sockets-config` -I$(SOCKETS) -Wall -std=c++17
LDFLAGS=
LIBS=$(SOCKETS)/libSockets.a -lssl -lcrypto -lpthread

all: simple_http_server

simple_http_server: simple_http_server.o ; $(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

clean: ; rm -f simple_http_server.o simple_http_server

