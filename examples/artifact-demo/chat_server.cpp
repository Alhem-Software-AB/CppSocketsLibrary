#include "TcpSocket.h"
#include "SocketHandler.h"
#include "SocketHandlerEp.h"
#include "ListenSocket.h"
#include "StdoutLog.h"
#include <set>
#include <memory>
#include <cstring>

#ifdef SOCKETS_NAMESPACE
using namespace SOCKETS_NAMESPACE;
#endif

class ChatSocket : public TcpSocket
{
public:
    ChatSocket(ISocketHandler& h) : TcpSocket(h)
    {
        SetLineProtocol();
        sockets.insert(this);
    }
    ~ChatSocket() override
    {
        sockets.erase(this);
    }

    void OnLine(const std::string& line) override
    {
        std::string msg = line + "\r\n";
        for (ChatSocket* sock : sockets)
        {
            if (sock != this)
            {
                sock->Send(msg);
            }
        }
    }

private:
    static std::set<ChatSocket*> sockets;
};

std::set<ChatSocket*> ChatSocket::sockets;

int main(int argc, char* argv[])
{
    bool use_epoll = false;
    for (int i = 1; i < argc; ++i)
    {
        if (!std::strcmp(argv[i], "-epoll"))
        {
            use_epoll = true;
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

    ListenSocket<ChatSocket> server(*handler);
    if (server.Bind(7000))
    {
        fprintf(stderr, "Failed to bind port 7000\n");
        return 1;
    }
    handler->Add(&server);
    while (handler->GetCount())
    {
        handler->Select(1, 0);
    }
    return 0;
}
