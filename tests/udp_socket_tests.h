#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/UdpSocket.h"
#include "../src/SocketHandler.h"
#include "../src/StdoutLog.h"

class UdpSocketTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(UdpSocketTest);
	CPPUNIT_TEST(testOpenInvalidHost);
	CPPUNIT_TEST_SUITE_END();

public:
	void testOpenInvalidHost()
	{
		StdoutLog log;
		SocketHandler h(&log);
		UdpSocket s(h);
		CPPUNIT_ASSERT(!s.Open("invalid.host", 1));
	}
};
