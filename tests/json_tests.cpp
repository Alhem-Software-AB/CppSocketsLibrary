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
    CPPUNIT_TEST(testCharAssignment);
    CPPUNIT_TEST(testShortAssignment);
    CPPUNIT_TEST(testLongAssignment);
    CPPUNIT_TEST(testDoubleAssignment);
    CPPUNIT_TEST(testConstCharAssignment);
    CPPUNIT_TEST(testStringAssignment);
    CPPUNIT_TEST(testBoolAssignment);
    CPPUNIT_TEST(testArrayAssignment);
    CPPUNIT_TEST(testObjectAssignment);
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

    void testCharAssignment() {
        Json src('b');
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, dest.Type());
        CPPUNIT_ASSERT_EQUAL('b', (char)dest);
    }

    void testShortAssignment() {
        short v = 321;
        Json src(v);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, dest.Type());
        CPPUNIT_ASSERT_EQUAL(v, (short)dest);
    }

    void testLongAssignment() {
        Json src(42L);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_INTEGER, dest.Type());
        CPPUNIT_ASSERT_EQUAL(42L, (long)dest);
    }

    void testDoubleAssignment() {
        double v = 2.718;
        Json src(v);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_REAL, dest.Type());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(v, (double)dest, 1e-9);
    }

    void testConstCharAssignment() {
        Json src("assign");
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_STRING, dest.Type());
        CPPUNIT_ASSERT_EQUAL(string("assign"), (string)dest);
    }

    void testStringAssignment() {
        string v = "strings";
        Json src(v);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_STRING, dest.Type());
        CPPUNIT_ASSERT_EQUAL(v, (string)dest);
    }

    void testBoolAssignment() {
        Json src(false);
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_BOOLEAN, dest.Type());
        CPPUNIT_ASSERT_EQUAL(false, (bool)dest);
    }

    void testArrayAssignment() {
        Json src(Json::TYPE_ARRAY);
        src.Add(Json(1L));
        src.Add(Json("two"));
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_ARRAY, dest.Type());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), dest.GetArray().size());
        Json::json_list_t::const_iterator it = dest.GetArray().begin();
        CPPUNIT_ASSERT_EQUAL(1L, (long)(*it));
        ++it;
        CPPUNIT_ASSERT_EQUAL(string("two"), (string)(*it));
        dest.Add(Json(3L));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), src.GetArray().size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), dest.GetArray().size());
    }

    void testObjectAssignment() {
        Json src(Json::TYPE_OBJECT);
        src["one"] = Json(1L);
        src["two"] = Json("second");
        Json dest;
        dest = src;
        CPPUNIT_ASSERT_EQUAL(Json::TYPE_OBJECT, dest.Type());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), dest.GetObject().size());
        CPPUNIT_ASSERT_EQUAL(1L, (long)dest["one"]);
        CPPUNIT_ASSERT_EQUAL(string("second"), (string)dest["two"]);
        dest["one"] = Json(99L);
        CPPUNIT_ASSERT_EQUAL(1L, (long)src["one"]);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(JsonConstructorAssignmentTest);

#ifndef COMBINED_TESTS
int main() {
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}
#endif

