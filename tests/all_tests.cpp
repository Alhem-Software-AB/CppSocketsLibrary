#define COMBINED_TESTS

#include "json_tests.h"
#include "socket_handler_ep_tests.h"
#include "http_server_test.h"
#include "base64_tests.h"
#include "utility_tests.h"

int main(int , char* argv[])
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::current_path(exe_path.parent_path());

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}

