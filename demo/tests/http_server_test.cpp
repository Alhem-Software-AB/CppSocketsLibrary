#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestFailure.h>
#include <fstream>
#include <iterator>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <map>

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
            chdir("..");
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
        system("make -C ../../src -j2 >/dev/null");
        system("make -C .. >/dev/null");
        pid_t pid;
        startServer(pid);
        system("curl -s http://127.0.0.1:8080/ > output.html");
        stopServer(pid);
        std::ifstream expected("../index.html", std::ios::binary);
        std::string exp((std::istreambuf_iterator<char>(expected)), {});
        std::ifstream actual("output.html", std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove("output.html");
        CPPUNIT_ASSERT_EQUAL(exp, act);
    }

    void testMissingIndexHtml()
    {
        std::rename("../index.html", "../index.html.bak");
        system("make -C ../../src -j2 >/dev/null");
        system("make -C .. >/dev/null");
        pid_t pid;
        startServer(pid);
        system("curl -s http://127.0.0.1:8080/ > output.html");
        stopServer(pid);
        std::ifstream actual("output.html", std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove("output.html");
        std::rename("../index.html.bak", "../index.html");
        CPPUNIT_ASSERT(act.find("index.html not found") != std::string::npos);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HttpServerTest);

int main(int argc, char* argv[])
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::current_path(exe_path.parent_path());

    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    controller.addListener(&result);
    CppUnit::BriefTestProgressListener progress;
    controller.addListener(&progress);

    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);

    auto xmlEscape = [](const std::string& data) {
        std::string out;
        out.reserve(data.size());
        for (char c : data) {
            switch (c) {
                case '&': out += "&amp;"; break;
                case '<': out += "&lt;"; break;
                case '>': out += "&gt;"; break;
                case '"': out += "&quot;"; break;
                case '\'': out += "&apos;"; break;
                default: out += c; break;
            }
        }
        return out;
    };

    std::map<std::string, const CppUnit::TestFailure*> failures;
    for (const auto& failure : result.failures()) {
        failures[failure->failedTestName()] = failure;
    }

    std::ofstream xml("cppunit-report.xml");
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<testsuite name=\"CppUnit\" tests=\"" << result.runTests()
        << "\" failures=\"" << result.testFailures()
        << "\" errors=\"" << result.testErrors() << "\">\n";

    for (const auto& test : result.tests()) {
        std::string full = test->getName();
        auto pos = full.find("::");
        std::string classname = pos == std::string::npos ? "" : full.substr(0, pos);
        std::string name = pos == std::string::npos ? full : full.substr(pos + 2);
        xml << "  <testcase classname=\"" << classname << "\" name=\"" << name << "\"";
        auto it = failures.find(full);
        if (it == failures.end()) {
            xml << "/>\n";
        } else {
            const auto* f = it->second;
            std::string msg = f->thrownException()->what();
            xml << ">";
            if (f->isError()) {
                xml << "<error message=\"" << xmlEscape(msg) << "\"/>";
            } else {
                xml << "<failure message=\"" << xmlEscape(msg) << "\"/>";
            }
            xml << "</testcase>\n";
        }
    }

    xml << "</testsuite>\n";

    return result.wasSuccessful() ? 0 : 1;
}
