# CppSocketsLibrary

The C++ class library is a versatile and user-friendly tool for network programming. It wraps the Berkeley Sockets C API and works on a variety of platforms including Unix and Windows. The library has been widely adopted, both in commercial and open-source applications, and offers several advanced features such as SSL support, IPv6 compatibility, and various socket types (tcp, udp, sctp, http). The source code is freely available under the GNU GPL license or an alternative license.

The library was designed to simplify the complexities of network programming and provide a single-threaded approach to managing multiple sockets. Instead of using the traditional C API, it provides a convenient Socket class that handles address translations and owns the file descriptor. The library also features callback methods such as OnRead(), OnWrite(), OnConnect(), and OnAccept(), to report events and handle logic related to the sockets. The sockets are monitored and managed using the Select() method in a SocketHandler class. The library has been tested on Linux, Windows 2000, and to some extent on Solaris and Mac OS X.

## Building and Creating a Distribution

The source code for the library lives in the `src/` directory. To build the
library and package a redistributable archive containing the essential headers
and binaries, run the following commands from the project root:

```bash
cd src
make PLATFORM=linux-x86-64
make dist PLATFORM=linux-x86-64
```

After the `dist` target completes, the top-level `dist/` folder will contain
`bin`, `lib`, and `include` directories with the compiled utilities, static
libraries, and header files required to use the library.

## Building the Tests

The test programs in the top-level `tests/` directory depend on the headers,
libraries, and build recipes produced by the commands above. Attempting to run
`make` in `tests/` before both building the library and creating the
distribution will fail because the required `dist/` files are missing. A
typical flow for building the tests is:

```bash
cd src
make PLATFORM=linux-x86-64
make dist PLATFORM=linux-x86-64
cd ..
make -C tests PLATFORM=linux-x86-64
```

Running `make dist` ensures that the `tests/` Makefile can include
`dist/Makefile.version` and `dist/Makefile.Defines.<platform>` and link against
the produced binaries.
