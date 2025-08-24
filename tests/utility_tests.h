#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/Utility.h"
#include <string>

class UtilityTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(UtilityTest);
    CPPUNIT_TEST(testHex2Unsigned);
    CPPUNIT_TEST(testAtoi64);
    CPPUNIT_TEST_SUITE_END();

public:
    void testHex2Unsigned() {
        CPPUNIT_ASSERT_EQUAL(0x1Au, Utility::hex2unsigned("1A"));
        CPPUNIT_ASSERT_EQUAL(0xFFu, Utility::hex2unsigned("ff"));
    }

    void testAtoi64() {
        CPPUNIT_ASSERT_EQUAL(static_cast<uint64_t>(0), Utility::atoi64("0"));
        CPPUNIT_ASSERT_EQUAL(static_cast<uint64_t>(123456789012345ULL), Utility::atoi64("123456789012345"));
    }
};

