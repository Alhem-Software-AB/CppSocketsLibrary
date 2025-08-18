CXX=g++
CXXFLAGS=-I../src -std=c++11 `pkg-config --cflags cppunit`
LIBS=`pkg-config --libs cppunit`

all: json_tests

json_tests: json_tests.cpp ../src/Json.cpp ../src/Exception.cpp utility_stub.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f json_tests
