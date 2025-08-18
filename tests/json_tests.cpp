#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/Json.h"

using namespace std;

class JsonConstructorAssignmentTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(JsonConstructorAssignmentTest);
    CPPUNIT_TEST(testDefaultConstructor);
    CPPUNIT_TEST(testCharConstructor);
    CPPUNIT_TEST(testShortConstructor);
    CPPUNIT_TEST(testLongConstructor);
    CPPUNIT_TEST(testDoubleConstructor);
    CPPUNIT_TEST(testConstCharConstructor);
    CPPUNIT_TEST(testStringConstructor);
    CPPUNIT_TEST(testBoolConstructor);
    CPPUNIT_TEST(testArrayConstructor);
    CPPUNIT_TEST(testObjectConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testCopyAssignment);
    CPPUNIT_TEST_SUITE_END();

public:
    void testDefaultConstructor() {
        Json j;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_UNKNOWN, j.Type());
    }

    void testCharConstructor() {
        Json j('a');
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, j.Type());
        CPPUNIT_ASSERT_EQUAL('a', (char)j);
    }

    void testShortConstructor() {
        short v = 123;
        Json j(v);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, j.Type());
        CPPUNIT_ASSERT_EQUAL(v, (short)j);
    }

    void testLongConstructor() {
        long v = 123456L;
        Json j(v);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, j.Type());
        CPPUNIT_ASSERT_EQUAL(v, (long)j);
    }

    void testDoubleConstructor() {
        double v = 3.14;
        Json j(v);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_REAL, j.Type());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(v, (double)j, 1e-9);
    }

    void testConstCharConstructor() {
        Json j("hello");
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_STRING, j.Type());
        CPPUNIT_ASSERT_EQUAL(string("hello"), (string)j);
    }

    void testStringConstructor() {
        string v = "world";
        Json j(v);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_STRING, j.Type());
        CPPUNIT_ASSERT_EQUAL(v, (string)j);
    }

    void testBoolConstructor() {
        Json j(true);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_BOOLEAN, j.Type());
        CPPUNIT_ASSERT_EQUAL(true, (bool)j);
    }

    void testArrayConstructor() {
        Json j(Json::TYPE_ARRAY);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_ARRAY, j.Type());
    }

    void testObjectConstructor() {
        Json j(Json::TYPE_OBJECT);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_OBJECT, j.Type());
    }

    void testCopyConstructor() {
        Json src("copy");
        Json copy(src);
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_STRING, copy.Type());
        CPPUNIT_ASSERT_EQUAL(string("copy"), (string)copy);
    }

    void testCopyAssignment() {
        Json src(42L);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, dest.Type());
        CPPUNIT_ASSERT_EQUAL(42L, (long)dest);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(JsonConstructorAssignmentTest);

int main() {
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}

