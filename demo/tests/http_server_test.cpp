#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <fstream>
#include <iterator>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <string>

class HttpServerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(HttpServerTest);
    CPPUNIT_TEST(testIndexHtml);
    CPPUNIT_TEST(testMissingIndexHtml);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override {}
    void tearDown() override {}

    void startServer(pid_t &pid)
    {
        pid = fork();
        if (pid == 0) {
            execl("./simple_http_server", "./simple_http_server", nullptr);
            std::exit(1);
        }
        sleep(1);
    }

    void stopServer(pid_t pid)
    {
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
    }

    void testIndexHtml()
    {
        system("make -C ../src -j2 >/dev/null");
        system("make >/dev/null");
        pid_t pid;
        startServer(pid);
        system("curl -s http://127.0.0.1:8080/ > output.html");
        stopServer(pid);
        std::ifstream expected("index.html", std::ios::binary);
        std::string exp((std::istreambuf_iterator<char>(expected)), {});
        std::ifstream actual("output.html", std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove("output.html");
        CPPUNIT_ASSERT_EQUAL(exp, act);
    }

    void testMissingIndexHtml()
    {
        std::rename("index.html", "index.html.bak");
        system("make -C ../src -j2 >/dev/null");
        system("make >/dev/null");
        pid_t pid;
        startServer(pid);
        system("curl -s http://127.0.0.1:8080/ > output.html");
        stopServer(pid);
        std::ifstream actual("output.html", std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove("output.html");
        std::rename("index.html.bak", "index.html");
        CPPUNIT_ASSERT(act.find("index.html not found") != std::string::npos);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HttpServerTest);

int main()
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}
