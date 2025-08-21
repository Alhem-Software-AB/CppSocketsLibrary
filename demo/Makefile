SOCKETS=../dist

CXX=g++
CXXFLAGS=`$(SOCKETS)/bin/Sockets-config` -I$(SOCKETS)/include -Wall -std=c++17
LDFLAGS=
LIBS=-L$(SOCKETS)/lib -lSockets -lssl -lcrypto -lpthread

all: simple_http_server

simple_http_server: simple_http_server.o ; $(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

clean: ; rm -f simple_http_server.o simple_http_server

