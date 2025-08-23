#define COMBINED_TESTS

#include "json_tests.cpp"
#include "socket_handler_ep_tests.cpp"
#include "http_server_test.cpp"

int main(int argc, char* argv[])
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::current_path(exe_path.parent_path());

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}

