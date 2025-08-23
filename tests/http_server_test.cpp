#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <fstream>
#include <iterator>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include <cstdio>

static std::filesystem::path repoRoot()
{
    static std::filesystem::path root;
    if (root.empty())
    {
        auto p = std::filesystem::current_path();
        while (!p.empty())
        {
            if (std::filesystem::exists(p / "examples" / "simple-http-server-demo") && std::filesystem::exists(p / "src"))
            {
                root = p;
                break;
            }
            p = p.parent_path();
        }
        if (root.empty())
            root = std::filesystem::current_path();
    }
    return root;
}

static std::filesystem::path demoDir()
{
    return repoRoot() / "examples" / "simple-http-server-demo";
}

class HttpServerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(HttpServerTest);
    CPPUNIT_TEST(testIndexHtml);
    CPPUNIT_TEST(testMissingIndexHtml);
    CPPUNIT_TEST(testMaxConnections);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override {}
    void tearDown() override {}

    void startServer(pid_t &pid)
    {
        struct rlimit rl;
        rl.rlim_cur = RLIM_INFINITY;
        rl.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &rl);

        pid = fork();
        if (pid == 0) {
            chdir(demoDir().c_str());
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

    void buildDemo()
    {
        std::string build_src = "make -C " + (repoRoot() / "src").string() + " -j2 >/dev/null";
        std::string build_demo = "make -C " + demoDir().string() + " >/dev/null";
        system(build_src.c_str());
        system(build_demo.c_str());
    }

    void testIndexHtml()
    {
        buildDemo();
        pid_t pid;
        startServer(pid);
        std::filesystem::path output = demoDir() / "output.html";
        std::string cmd = "curl -s http://127.0.0.1:8080/ > " + output.string();
        system(cmd.c_str());
        stopServer(pid);
        std::ifstream expected((demoDir() / "index.html").string(), std::ios::binary);
        std::string exp((std::istreambuf_iterator<char>(expected)), {});
        std::ifstream actual(output.string(), std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove(output.string().c_str());
        CPPUNIT_ASSERT_EQUAL(exp, act);
    }

    void testMissingIndexHtml()
    {
        std::filesystem::path index_html = demoDir() / "index.html";
        std::filesystem::path backup = demoDir() / "index.html.bak";
        std::rename(index_html.c_str(), backup.c_str());
        buildDemo();
        pid_t pid;
        startServer(pid);
        std::filesystem::path output = demoDir() / "output.html";
        std::string cmd = "curl -s http://127.0.0.1:8080/ > " + output.string();
        system(cmd.c_str());
        stopServer(pid);
        std::ifstream actual(output.string(), std::ios::binary);
        std::string act((std::istreambuf_iterator<char>(actual)), {});
        std::remove(output.string().c_str());
        std::rename(backup.c_str(), index_html.c_str());
        CPPUNIT_ASSERT(act.find("index.html not found") != std::string::npos);
    }

    void testMaxConnections()
    {
        buildDemo();
        pid_t pid;
        startServer(pid);
        const int MAX_CONN = 50;
        std::vector<std::thread> threads;
        std::vector<int> results(MAX_CONN);
        for (int i = 0; i < MAX_CONN; ++i)
        {
            threads.emplace_back([i, &results]() {
                results[i] = system("curl -s http://127.0.0.1:8080/ > /dev/null");
            });
        }
        for (auto &t : threads)
            t.join();
        stopServer(pid);
        for (int r : results)
            CPPUNIT_ASSERT_EQUAL(0, r);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HttpServerTest);

#ifndef COMBINED_TESTS
int main(int argc, char* argv[])
{
    std::filesystem::path exe_path = argv[0];
    std::filesystem::current_path(exe_path.parent_path());

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    return runner.run() ? 0 : 1;
}
#endif

