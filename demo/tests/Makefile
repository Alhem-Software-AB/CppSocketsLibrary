SOCKETS =	../../dist

CXX=g++
CXXFLAGS=`$(SOCKETS)/bin/Sockets-config` -I$(SOCKETS)/include `pkg-config --cflags cppunit` -Wall
LIBS=$(SOCKETS)/lib/libSockets.a `pkg-config --libs cppunit` -lssl -lcrypto -lpthread

all: http_server_test

http_server_test: http_server_test.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f http_server_test.o http_server_test

setup:
	@echo No longer necessary
