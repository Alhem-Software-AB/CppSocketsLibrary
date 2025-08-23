SOCKETS =       ../dist

CXX =           g++

CXXFLAGS =      -I$(SOCKETS)/include \
                -std=c++17 \
                `pkg-config --cflags cppunit` \
                -MD

LIBS_CPPUNIT =  `pkg-config --libs cppunit`

LIBS_SOCKETS =  $(LIBS_CPPUNIT) -L$(SOCKETS)/lib -lSockets -lssl -lcrypto -lxml2 -lpthread

all:            json_tests socket_handler_ep_tests http_server_test all_tests

json_tests:     json_tests.cpp ../src/Json.cpp ../src/Exception.cpp utility_stub.cpp
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_CPPUNIT)

socket_handler_ep_tests: \
                socket_handler_ep_tests.cpp
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_SOCKETS)

http_server_test: \
                http_server_test.cpp
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_SOCKETS)

all_tests:              all_tests.cpp ../src/Json.cpp ../src/Exception.cpp
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_SOCKETS)

clean:
		rm -f json_tests socket_handler_ep_tests http_server_test all_tests

-include        *.d

