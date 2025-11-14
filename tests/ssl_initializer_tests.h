#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/SSLInitializer.h"

class SSLInitializerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SSLInitializerTest);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST_SUITE_END();

public:
	void testConstruct()
	{
		SSLInitializer init;
		CPPUNIT_ASSERT(init.bio_err != nullptr);
	}
};
