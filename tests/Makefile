SOCKETS =	../dist
PLATFORM =	linux-x86-64

CFLAGS =	-I$(SOCKETS)/include \
		-std=c++17 \
		`pkg-config --cflags cppunit` \
		-MD

LIBS_CPPUNIT =	`pkg-config --libs cppunit`

LIBS =		$(LIBS_CPPUNIT) -L$(SOCKETS)/lib -lSockets -lssl -lcrypto \
		-lxml2 -lpthread

include		$(SOCKETS)/Makefile.version
include		$(SOCKETS)/Makefile.Defines.$(PLATFORM)

PROGS =		all_tests

all:		$(PROGS)

SRC4 =		all_tests.cpp \
		../src/Json.cpp \
		../src/Exception.cpp \
		json_tests.cpp \
		socket_handler_ep_tests.cpp \
		http_server_test.cpp \

OBJ4 =		$(patsubst %.cpp, %.o, $(SRC4))

all_tests:	$(OBJ4)
		$(link-target)

clean:
		rm -f json_tests socket_handler_ep_tests http_server_test all_tests \
			*.d *~ slask fel fil $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)

-include	*.d

