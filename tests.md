# Tests Overview

## Current test entry points

1. **C++ unit/integration test binary**: `tests/all_tests`
   - Build command: `make tests`
   - Run command: `./tests/all_tests`
   - Build recipe lives in `tests/Makefile`.
2. **Python unit tests**: `tests/test_detect_platform.py`
   - Run command: `python3 -m unittest tests/test_detect_platform.py`
   - Covers platform-detection behavior in `detect_platform.py`.

## Folder structure (tests-related)

```text
tests/
├── Makefile
├── all_tests.cpp
├── base64_tests.cpp
├── base64_tests.h
├── http_server_test.cpp
├── http_server_test.h
├── json_tests.cpp
├── json_tests.h
├── socket_handler_ep_tests.cpp
├── socket_handler_ep_tests.h
├── socket_handler_tests.cpp
├── socket_handler_tests.h
├── ssl_initializer_tests.cpp
├── ssl_initializer_tests.h
├── tcp_socket_tests.cpp
├── tcp_socket_tests.h
├── test_detect_platform.py
├── udp_socket_tests.cpp
├── udp_socket_tests.h
├── utility_stub.cpp
├── utility_tests.cpp
└── utility_tests.h
```

## `all_tests` composition

`tests/all_tests.cpp` registers all CppUnit suites via the global registry and runs them with verbose per-test output.

Source files compiled into `all_tests` are listed in `tests/Makefile` (`SRC4`):

- `tests/all_tests.cpp`
- `src/Json.cpp`
- `src/Exception.cpp`
- `tests/json_tests.cpp`
- `tests/socket_handler_ep_tests.cpp`
- `tests/http_server_test.cpp`
- `tests/base64_tests.cpp`
- `tests/utility_tests.cpp`
- `tests/tcp_socket_tests.cpp`
- `tests/udp_socket_tests.cpp`
- `tests/socket_handler_tests.cpp`
- `tests/ssl_initializer_tests.cpp`

## CppUnit suites and test cases

- `JsonConstructorAssignmentTest`
  - `testDefaultConstructor`
  - `testCharConstructor`
  - `testShortConstructor`
  - `testLongConstructor`
  - `testDoubleConstructor`
  - `testConstCharConstructor`
  - `testStringConstructor`
  - `testBoolConstructor`
  - `testArrayConstructor`
  - `testObjectConstructor`
  - `testCopyConstructor`
  - `testCharAssignment`
  - `testShortAssignment`
  - `testLongAssignment`
  - `testDoubleAssignment`
  - `testConstCharAssignment`
  - `testStringAssignment`
  - `testBoolAssignment`
  - `testArrayAssignment`
  - `testObjectAssignment`

- `SocketHandlerEpTest`
  - `testConstruct`
  - `testFactoryCreate`
  - `testSelectWithoutSockets`

- `HttpServerTest`
  - `testIndexHtml`
  - `testMissingIndexHtml`
  - `testMaxConnections`

- `Base64Test`
  - `testEncode`
  - `testDecode`
  - `testDecodeLength`

- `UtilityTest`
  - `testHex2Unsigned`
  - `testAtoi64`

- `TcpSocketTest`
  - `testProtocol`
  - `testOpenInvalidHost`

- `UdpSocketTest`
  - `testOpenInvalidHost`

- `SocketHandlerTest`
  - `testConstruct`
  - `testSelectWithoutSockets`
  - `testAddAndRemove`

- `SSLInitializerTest`
  - `testConstruct`

## Additional test-related scripts

- `examples/simple-http-server-demo/test.sh`
  - Ad-hoc shell test script for the demo server.
  - Not part of `make tests` or `tests/all_tests`.
