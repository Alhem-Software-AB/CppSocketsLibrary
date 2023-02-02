/*
 **	File ......... ListenSocket.h
 **	Published ....  2004-02-13
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004,2005  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

#ifdef _WIN32
#include <stdlib.h>
#else
#include <errno.h>
#endif

#include "Socket.h"
#include "SocketHandler.h"
#include "StdLog.h"
#include "TcpSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** Binds incoming port number to new Socket class X. 
	\ingroup basic */
template <class X>
class ListenSocket : public Socket
{
public:
	ListenSocket(SocketHandler& h,bool use_creator = true) : Socket(h), m_port(0), m_depth(0), m_creator(NULL)
	,m_bHasCreate(false) {
		if (use_creator)
		{
			m_creator = new X(h);
			Socket *tmp = m_creator -> Create();
			if (tmp && dynamic_cast<X *>(tmp))
			{
				m_bHasCreate = true;
			}
			if (tmp)
			{
				delete tmp;
			}
		}
	}
	~ListenSocket() {
		if (m_creator)
		{
			delete m_creator;
		}
	}

	/** bind() to port 0 - a random port */
	int Bind()
	{
		int depth = 3; // think of maybe increasing this value if needed
		ipaddr_t l = 0;
		struct sockaddr_in sa;
		SOCKET s;

		if ( (s = CreateSocket4(SOCK_STREAM)) == INVALID_SOCKET)
		{
			return -1;
		}
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0); // choose any port
		memcpy(&sa.sin_addr, &l, 4);
		if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			Handler().LogError(this, "bind", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		if (listen(s, depth) == -1)
		{
			Handler().LogError(this, "listen", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		// Find out what port was choosen
		int sockaddr_length = sizeof(sockaddr);
		getsockname(s, (struct sockaddr *)&sa, (socklen_t*)&sockaddr_length);
		m_port = ntohs(sa.sin_port);
		m_depth = depth;
		Attach(s);
		return 0;
	}

	/** bind to port with optional listen queue length (depth) */
	int Bind(port_t port, int depth = 3)
	{
		ipaddr_t l = 0;
		struct sockaddr_in sa;
		SOCKET s;

		if ( (s = CreateSocket4(SOCK_STREAM)) == INVALID_SOCKET)
		{
			return -1;
		}
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons( port );
		memcpy(&sa.sin_addr, &l, 4);
		if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			Handler().LogError(this, "bind", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		if (listen(s, depth) == -1)
		{
			Handler().LogError(this, "listen", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		m_port = port;
		m_depth = depth;
		Attach(s);
		return 0;
	}

	/** bind to port on a specified address */
	int Bind(const std::string& adapter, port_t port, int depth = 3)
	{
		ipaddr_t l = 0;
		ipaddr_t tmp;
		if (u2ip(adapter, tmp))
		{
			l = tmp;
		}
		struct sockaddr_in sa;
		SOCKET s;

		if ( (s = CreateSocket4(SOCK_STREAM)) == INVALID_SOCKET)
		{
			return -1;
		}
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons( port );
		memcpy(&sa.sin_addr, &l, 4);
		if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			Handler().LogError(this, "bind", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		if (listen(s, depth) == -1)
		{
			Handler().LogError(this, "listen", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			return -1;
		}
		m_port = port;
		m_depth = depth;
		Attach(s);
		return 0;
	}

	/** ipv6 bind to port with optional listen queue length (depth) */
#ifdef IPPROTO_IPV6
	int Bind6(port_t port, int depth = 3)
	{
		struct sockaddr_in6 sa;
		SOCKET s;

		if ( (s = CreateSocket6(SOCK_STREAM)) != INVALID_SOCKET)
		{
			memset(&sa, 0, sizeof(sa));
			sa.sin6_family = AF_INET6;
			sa.sin6_port = htons( port );
			sa.sin6_flowinfo = 0;
			sa.sin6_scope_id = 0;
			// sa.sin6_addr is all 0
			if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != -1)
			{
				if (listen(s, depth) != -1)
				{
					m_port = port;
					m_depth = depth;
					Attach(s);
					return 0;
				}
				else
				{
					Handler().LogError(this, "listen", Errno, StrError(Errno), LOG_LEVEL_FATAL);
				}
			}
			else
			{
				Handler().LogError(this, "bind", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			}
			closesocket(s);
		}
		return -1;
	}
#endif

	/** ipv6 bind to port on a specified address */
#ifdef IPPROTO_IPV6
	int Bind6(const std::string& address, port_t port, int depth = 3)
	{
		struct sockaddr_in6 sa;
		SOCKET s;

		if ( (s = CreateSocket6(SOCK_STREAM)) != INVALID_SOCKET)
		{
			struct in6_addr a;
			memset(&sa, 0, sizeof(sa));
			sa.sin6_family = AF_INET6;
			sa.sin6_port = htons( port );
			sa.sin6_flowinfo = 0;
			sa.sin6_scope_id = 0;
			// sa.sin6_addr is all 0
			if (u2ip(address, a))
			{
				sa.sin6_addr = a;
			}
			if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != -1)
			{
				if (listen(s, depth) != -1)
				{
					m_port = port;
					m_depth = depth;
					Attach(s);
					return 0;
				}
				else
				{
					Handler().LogError(this, "listen", Errno, StrError(Errno), LOG_LEVEL_FATAL);
				}
			}
			else
			{
				Handler().LogError(this, "bind", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			}
			closesocket(s);
		}
		return -1;
	}
#endif

	port_t GetPort()
	{
		return m_port;
	}

	int GetDepth()
	{
		return m_depth;
	}

	void OnRead()
	{
		socklen_t len;
		struct sockaddr *saptr;
		socklen_t *lenptr = &len;
		SOCKET a_s;

#ifdef IPPROTO_IPV6
		if (IsIpv6())
		{
			struct sockaddr_in6 sa;

			saptr = (struct sockaddr *)&sa;
			*lenptr = sizeof(struct sockaddr_in6);
			a_s = accept(GetSocket(), saptr, lenptr);
			if (a_s == INVALID_SOCKET)
			{
				Handler().LogError(this, "accept", Errno, StrError(Errno), LOG_LEVEL_ERROR);
				return;
			}
			Socket *tmp;
			if (m_bHasCreate)
				tmp = m_creator -> Create();
			else
				tmp = new X(Handler());
			TcpSocket *tcp = dynamic_cast<TcpSocket *>(tmp);
			tmp -> SetIpv6();
			tmp -> SetParent(this);
			tmp -> Attach(a_s);
			tmp -> SetNonblocking(true);
			tmp -> SetRemoteAddress( (struct sockaddr *)saptr, len);
			if (tcp)
				tcp -> SetConnected(true);
			tmp -> Init();
			Handler().Add(tmp);
			tmp -> SetDeleteByHandler(true);
			if (Handler().OkToAccept())
			{
				if (tmp -> IsSSL()) // SSL Enabled socket
					tmp -> OnSSLAccept();
				else
					tmp -> OnAccept();
			}
			else
			{
				Handler().LogError(this, "accept", -1, "Not OK to accept", LOG_LEVEL_FATAL);
				tmp -> SetCloseAndDelete();
			}
			return;
		}
#endif
		struct sockaddr_in sa;

		saptr = (struct sockaddr *)&sa;
		*lenptr = sizeof(struct sockaddr_in);
		a_s = accept(GetSocket(), saptr, lenptr);
		if (a_s == INVALID_SOCKET)
		{
			Handler().LogError(this, "accept", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return;
		}
		Socket *tmp;
		if (m_bHasCreate)
			tmp = m_creator -> Create();
		else
			tmp = new X(Handler());
		TcpSocket *tcp = dynamic_cast<TcpSocket *>(tmp);
		tmp -> SetParent(this);
		tmp -> Attach(a_s);
		tmp -> SetNonblocking(true);
		tmp -> SetRemoteAddress( (struct sockaddr *)saptr, len);
		if (tcp)
			tcp -> SetConnected(true);
		tmp -> Init();
		Handler().Add(tmp);
		tmp -> SetDeleteByHandler(true);
		if (Handler().OkToAccept())
		{
			if (tmp -> IsSSL()) // SSL Enabled socket
				tmp -> OnSSLAccept();
			else
				tmp -> OnAccept();
		}
		else
		{
			Handler().LogError(this, "accept", -1, "Not OK to accept", LOG_LEVEL_FATAL);
			tmp -> SetCloseAndDelete();
		}
	}

//	X *GetCreator() { return m_creator; }

	/** This method is not supposed to be used, because accept() is
	    handled automatically in the OnRead() method. */
        virtual SOCKET Accept(SOCKET socket, struct sockaddr *saptr, socklen_t *lenptr)
        {
                return accept(socket, saptr, lenptr);
        }


protected:
	ListenSocket(const ListenSocket& ) {}
private:
	ListenSocket& operator=(const ListenSocket& ) { return *this; }
	port_t m_port;
	int m_depth;
	X *m_creator;
	bool m_bHasCreate;
};



#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _LISTENSOCKET_H
