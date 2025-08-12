CXX=g++
CXXFLAGS=`../../src/Sockets-config` -I../../src `pkg-config --cflags cppunit` -Wall
LIBS=../../src/libSockets.a `pkg-config --libs cppunit` -lssl -lcrypto -lpthread

all: http_server_test

http_server_test: http_server_test.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f http_server_test.o http_server_test

setup:
	@./setup-build.sh
