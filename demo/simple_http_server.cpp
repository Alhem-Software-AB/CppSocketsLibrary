#include "HttpdSocket.h"
#include "SocketHandler.h"
#include "SocketHandlerEp.h"
#include "ListenSocket.h"
#include "StdoutLog.h"
#include "Utility.h"
#include <fstream>
#include <cstring>
#include <memory>

#ifdef SOCKETS_NAMESPACE
using namespace SOCKETS_NAMESPACE;
#endif

// Simple HTTP server socket that always serves "index.html"
class IndexSocket : public HttpdSocket
{
public:
    IndexSocket(ISocketHandler& h) : HttpdSocket(h) {}

    void Exec() override
    {
        std::ifstream file("index.html", std::ios::binary);
        if (!file.is_open())
        {
            const std::string msg = "index.html not found\n";
            SetStatus("404");
            SetStatusText("Not Found");
            AddResponseHeader("Content-type", "text/plain");
            AddResponseHeader("Content-length", Utility::l2string((long)msg.size()));
            AddResponseHeader("Connection", "close");
            SendResponse();
            Send(msg);
            SetCloseAndDelete();
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

        SetStatus("200");
        SetStatusText("OK");
        AddResponseHeader("Content-type", "text/html");
        AddResponseHeader("Content-length", Utility::l2string((long)content.size()));
        AddResponseHeader("Connection", "close");
        SendResponse();
        if (!content.empty())
        {
            SendBuf(const_cast<char*>(content.data()), content.size());
        }
        SetCloseAndDelete();
    }

private:
};

int main(int argc, char* argv[])
{
    bool use_epoll = false;
    for (int i = 1; i < argc; ++i)
    {
        if (!std::strcmp(argv[i], "-epoll"))
        {
            use_epoll = true;
        }
        else if (!std::strcmp(argv[i], "-h"))
        {
            printf("Usage: %s [-epoll]\n", argv[0]);
            return 0;
        }
    }

    StdoutLog log;
    std::unique_ptr<ISocketHandler> handler;
    if (use_epoll)
    {
        handler = std::make_unique<SocketHandlerEp>(&log);
    }
    else
    {
        handler = std::make_unique<SocketHandler>(&log);
    }

    ListenSocket<IndexSocket> server(*handler);
    if (server.Bind(8080))
    {
        fprintf(stderr, "Failed to bind port 8080\n");
        return 1;
    }
    handler->Add(&server);
    while (handler->GetCount())
    {
        handler->Select(1, 0);
    }
    return 0;
}

