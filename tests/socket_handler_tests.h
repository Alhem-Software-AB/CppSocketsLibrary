#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/SocketHandler.h"
#include "../src/StdoutLog.h"
#include "../src/TcpSocket.h"

class SocketHandlerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SocketHandlerTest);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST(testSelectWithoutSockets);
	CPPUNIT_TEST(testAddAndRemove);
	CPPUNIT_TEST_SUITE_END();

public:
	void testConstruct()
	{
		StdoutLog log;
		SocketHandler h(&log);
		CPPUNIT_ASSERT_EQUAL(0, (int)h.GetCount());
	}

	void testSelectWithoutSockets()
	{
		StdoutLog log;
		SocketHandler h(&log);
		CPPUNIT_ASSERT_EQUAL(0, h.Select(0, 0));
	}

	void testAddAndRemove()
	{
		StdoutLog log;
		SocketHandler h(&log);
		TcpSocket s(h);
		h.Add(&s);
		CPPUNIT_ASSERT_EQUAL(1, (int)h.GetCount());
		h.ISocketHandler_Del(&s);
	}
};
