#define COMBINED_TESTS

#include "json_tests.h"
#include "socket_handler_ep_tests.h"
#include "http_server_test.h"
#include "base64_tests.h"
#include "utility_tests.h"
#include <cppunit/TestListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <iostream>

class VerboseTestProgressListener : public CppUnit::TestListener {
public:
    void startTest(CppUnit::Test* test) override {
        std::cout << "Running: " << test->getName() << std::endl;
    }
};

int main(int , char* argv[])
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::current_path(exe_path.parent_path());

    CppUnit::TextUi::TestRunner runner;
    VerboseTestProgressListener progress;
    runner.eventManager().addListener(&progress);
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run("", false, true, false) ? 0 : 1;
}

