CXX=g++
CXXFLAGS=`../src/Sockets-config` -I../src -Wall
LDFLAGS=
LIBS=../src/libSockets.a -lssl -lcrypto -lpthread

all: simple_http_server

simple_http_server: simple_http_server.o
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

clean:
	rm -f simple_http_server.o simple_http_server

