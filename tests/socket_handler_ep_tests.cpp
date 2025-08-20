#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <memory>
#include "../src/SocketHandlerEp.h"
#include "../src/StdoutLog.h"

class SocketHandlerEpTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SocketHandlerEpTest);
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST(testFactoryCreate);
    CPPUNIT_TEST(testSelectWithoutSockets);
    CPPUNIT_TEST_SUITE_END();

public:
    void testConstruct() {
        StdoutLog log;
        SocketHandlerEp h(&log);
        CPPUNIT_ASSERT_EQUAL(0, (int)h.GetCount());
    }

    void testFactoryCreate() {
        StdoutLog log;
        SocketHandlerEp base(&log);
        std::unique_ptr<ISocketHandler> h(base.Create(&log));
        CPPUNIT_ASSERT(dynamic_cast<SocketHandlerEp*>(h.get()) != nullptr);
    }

    void testSelectWithoutSockets() {
        StdoutLog log;
        SocketHandlerEp h(&log);
        CPPUNIT_ASSERT_EQUAL(0, h.Select(0, 0));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SocketHandlerEpTest);

#ifndef COMBINED_TESTS
int main() {
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}
#endif

