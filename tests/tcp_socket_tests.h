#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/TcpSocket.h"
#include "../src/SocketHandler.h"
#include "../src/StdoutLog.h"
#include <netinet/in.h>

class TcpSocketTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TcpSocketTest);
	CPPUNIT_TEST(testProtocol);
	CPPUNIT_TEST(testOpenInvalidHost);
	CPPUNIT_TEST_SUITE_END();

public:
	void testProtocol()
	{
		StdoutLog log;
		SocketHandler h(&log);
		TcpSocket s(h);
		CPPUNIT_ASSERT_EQUAL(static_cast<int>(IPPROTO_TCP), s.Protocol());
	}

	void testOpenInvalidHost()
	{
		StdoutLog log;
		SocketHandler h(&log);
		TcpSocket s(h);
		CPPUNIT_ASSERT(!s.Open("invalid.host", 1));
	}
};
