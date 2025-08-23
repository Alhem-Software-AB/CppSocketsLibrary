#include <Sockets/SocketHandler.h>
#include <Sockets/UdpSocket.h>
#include <Sockets/Exception.h>
#include <Sockets/Ipv4Address.h>

#include <iostream>

class cSocket : public UdpSocket
{
public:
	cSocket(ISocketHandler& h) : UdpSocket(h) {}

	virtual void OnRawData(const char *buf,size_t len,struct sockaddr *sa,socklen_t sa_len) {
		std::string data(buf, len);
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;
		Ipv4Address addr(*ipv4);
 		std::cout << "Client received: '" << data << "' from: " << addr.Convert(true) << std::endl;
	}

	void SendPing() {
		std::string data("Ping?");
		SendTo("localhost", 12345, data);
	}
};

class sSocket : public UdpSocket
{
public:
	sSocket(ISocketHandler& h) : UdpSocket(h) {}

	virtual void OnRawData(const char *buf,size_t len,struct sockaddr *sa,socklen_t sa_len) {
		std::string data(buf, len);
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;
		Ipv4Address addr(*ipv4);
		std::cout << "Server received: '" << data << "' from: " << addr.Convert(true) << std::endl;
		data = "Pong!";
		SendTo(addr, data);
	}
};

int main(int , char *[])
{
	try
	{
		SocketHandler h;

		// create and bind server socket
		sSocket sock(h);
		port_t port = 12345;
		sock.Bind(port);
		h.Add(&sock);

		// create and bind client socket
		cSocket client(h);
		port = 12346;
		client.Bind(port);
		h.Add(&client);

		time_t t = time(NULL);
		while (true)
		{
			h.Select(0, 100000);
			if (time(NULL) != t)
			{
				t = time(NULL);
				client.SendPing();
			}
		}
	}
	catch (const Exception& e)
	{
		std::cerr << e.ToString() << std::endl;
	}
}

