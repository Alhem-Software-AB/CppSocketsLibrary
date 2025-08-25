#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/Base64.h"
#include <string>

class Base64Test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(Base64Test);
    CPPUNIT_TEST(testEncode);
    CPPUNIT_TEST(testDecode);
    CPPUNIT_TEST(testDecodeLength);
    CPPUNIT_TEST_SUITE_END();

public:
    void testEncode() {
        Base64 b;
        std::string out;
        b.encode("hello world", out, false);
        CPPUNIT_ASSERT_EQUAL(std::string("aGVsbG8gd29ybGQ="), out);
    }

    void testDecode() {
        Base64 b;
        std::string out;
        b.decode("aGVsbG8gd29ybGQ=", out);
        CPPUNIT_ASSERT_EQUAL(std::string("hello world"), out);
    }

    void testDecodeLength() {
        Base64 b;
        size_t len = b.decode_length("aGVsbG8gd29ybGQ=");
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), len);
    }
};

