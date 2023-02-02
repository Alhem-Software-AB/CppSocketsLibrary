#include <StdoutLog.h>
#include <SocketHandler.h>
#include <TcpSocket.h>
#include <ListenSocket.h>
#include <Utility.h>
#include <Parse.h>
#include <HttpGetSocket.h>
#include <PoolSocket.h>
#include <Socket.h>
#include <HttpDebugSocket.h>


class MyHandler : public SocketHandler
{
public:
	MyHandler(StdLog *p) : SocketHandler(p),m_quit(false) {}
	~MyHandler() {}

	void List(TcpSocket *p) {
		for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
		{
			Socket *p0 = (*it).second;
			if (dynamic_cast<PoolSocket *>(p0))
			{
				p -> Send("PoolSocket\n");
			}
			else
			if (dynamic_cast<HttpGetSocket *>(p0))
			{
				p -> Send("HttpGetSocket\n");
			}
			else
			if (dynamic_cast<TcpSocket *>(p0))
			{
				p -> Send("TcpSocket\n");
			}
			else
			{
				p -> Send("Some kind of Socket\n");
			}
			bool r;
			bool w;
			bool e;
			Get(p -> GetSocket(), r, w, e);
			char slask[1000];
			sprintf(slask,"  Read: %s  Write: %s  Exception: %s\n",
				r ? "SET" : "not set",
				w ? "SET" : "not set",
				e ? "SET" : "not set");
			p -> Send( slask );
		}
	}
	void SetQuit() { m_quit = true; }
	bool Quit() { return m_quit; }

private:
	bool m_quit;
};


class MySocket : public TcpSocket
{
public:
	MySocket(SocketHandler& h) : TcpSocket(h) {
	}
	void OnAccept() {
		int port = GetParent() -> GetPort();
		Send("I'm the server at port " + 
			Utility::l2string(port) + "\n");
		SetCloseAndDelete();
	}
};


class OrderSocket : public TcpSocket
{
public:
	OrderSocket(SocketHandler& h) : TcpSocket(h) {
		SetLineProtocol();
	}
	void OnAccept() {
		Send("Cmd (get,quit,list,stop)>");
	}
	void OnLine(const std::string& line) {
		Parse pa(line);
		std::string cmd = pa.getword();
		std::string arg = pa.getrest();
		if (cmd == "get")
		{
			HttpGetSocket *p = new HttpGetSocket(Handler(), arg, "tmpfile.html");
			p -> SetHttpVersion("HTTP/1.1");
			p -> AddResponseHeader("Connection", "keep-alive");
			Handler().Add( p );
			p -> SetDeleteByHandler();
			Send("Reading url '" + arg + "'\n");
		}
		else
		if (cmd == "quit")
		{
			Send("Goodbye!\n");
			SetCloseAndDelete();
		}
		else
		if (cmd == "list")
		{
			static_cast<MyHandler&>(Handler()).List( this );
		}
		else
		if (cmd == "stop")
		{
			static_cast<MyHandler&>(Handler()).SetQuit();
		}
		else
		{
			Send("Huh?\n");
		}
		Send("Cmd>");
	}
	void OnDelete() {
		printf("OrderSocket::OnDelete()\n");
	}
};


class TestSocket : public TcpSocket
{
public:
	TestSocket(SocketHandler& h) : TcpSocket(h) {
		SetLineProtocol();
	}
	void OnConnect() {
		printf("TestSocket connected, sending QUIT\n");
		Send( "quit\n" );
	}
	void OnLine(const std::string& line) {
		printf("TestSocket: %s\n", line.c_str());
	}
	void OnDelete() {
		printf("TestSocket::OnDelete()\n");
	}
};


int main()
{
	StdoutLog log;
	MyHandler h(&log);

	// first server
	ListenSocket<MySocket> l1(h);
	l1.Bind(1024);
	h.Add(&l1);

	// second server
	ListenSocket<MySocket> l2(h);
	l2.Bind(1025);
	h.Add(&l2);

	// line server
	ListenSocket<OrderSocket> l3(h);
	l3.Bind(1026);
	h.Add(&l3);

	// http debug
	ListenSocket<HttpDebugSocket> l4(h);
	l4.Bind(8080);
	h.Add(&l4);

	TestSocket ts(h);
	ts.Open("localhost", 1026);
	h.Add(&ts);

	h.Select(1, 0);
	while (!h.Quit())
	{
		h.Select(1,0);
	}

	return 0;
}


