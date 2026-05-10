#include <cppunit/extensions/HelperMacros.h>
#include <limits>
#include "../src/HttpClientSocket.h"
#include "../src/SocketHandler.h"
#include "../src/StdoutLog.h"

class HttpClientSocketVulnTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(HttpClientSocketVulnTest);
	CPPUNIT_TEST(testNegativeContentLengthWrapsToHugeSizeT);
	CPPUNIT_TEST_SUITE_END();

public:
	void testNegativeContentLengthWrapsToHugeSizeT()
	{
		StdoutLog log;
		SocketHandler h(&log);
		HttpClientSocket s(h, "http://127.0.0.1/");

		s.OnHeader("content-length", "-1");

		CPPUNIT_ASSERT_EQUAL(std::numeric_limits<size_t>::max(), s.GetContentLength());
	}
};
