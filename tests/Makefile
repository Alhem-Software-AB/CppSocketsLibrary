CXX=g++
CXXFLAGS=-I../src -std=c++11 `pkg-config --cflags cppunit`
LIBS_CPPUNIT=`pkg-config --libs cppunit`
LIBS_SOCKETS=$(LIBS_CPPUNIT) -L../src -lSockets -lssl -lcrypto -lxml2 -lpthread

all: json_tests socket_handler_ep_tests

json_tests: json_tests.cpp ../src/Json.cpp ../src/Exception.cpp utility_stub.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_CPPUNIT)

socket_handler_ep_tests: socket_handler_ep_tests.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_SOCKETS)

clean:
	rm -f json_tests socket_handler_ep_tests
