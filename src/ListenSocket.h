/**
 **	File ......... ListenSocket.h
 **	Published ....  2004-02-13
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004  Anders Hedstrom

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

#include "Socket.h"


template <class X>
class ListenSocket : public Socket
{
public:
	ListenSocket(SocketHandler& h) : Socket(h)
	,m_port(0)
	,m_depth(0) {
	}
	~ListenSocket() {
	}

	int Bind(port_t port,int depth = 3) {
		ipaddr_t l = 0;
		struct sockaddr_in sa;
		SOCKET s;

		s = CreateSocket(SOCK_STREAM);
		if (s == -1)
		{
			perror("CreateSocket() failed");
			return -1;
		}

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET; // hp -> h_addrtype;
		sa.sin_port = (int)htons( port );
		memcpy(&sa.sin_addr,&l,4);

		if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			perror("bind() failed");
			closesocket(s);
			return -1;
		}

		if (listen(s, depth) == -1)
		{
			perror("listen() failed");
			closesocket(s);
			return -1;
		}

		m_port = port;
		m_depth = depth;

		Attach(s);
		return 0;
	}
	int Bind(const std::string& adapter,port_t port,int depth = 3) {
		ipaddr_t l = 0;
		ipaddr_t tmp;
		if (u2ip(adapter,tmp))
		{
			l = tmp;
		}
		struct sockaddr_in sa;
		SOCKET s;

		s = CreateSocket(SOCK_STREAM);
		if (s == -1)
		{
			perror("CreateSocket() failed");
			return -1;
		}

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET; // hp -> h_addrtype;
		sa.sin_port = (int)htons( port );
		memcpy(&sa.sin_addr,&l,4);

		if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			perror("bind() failed");
			closesocket(s);
			return -1;
		}

		if (listen(s, depth) == -1)
		{
			perror("listen() failed");
			closesocket(s);
			return -1;
		}

		m_port = port;
		m_depth = depth;

		Attach(s);
		return 0;
	}

	port_t GetPort() {
		return m_port;
	}

	void OnRead() {
		struct sockaddr_in sa;
		socklen_t len;
		struct sockaddr *saptr;
		socklen_t *lenptr;
		SOCKET a_s;

		saptr = (struct sockaddr *)&sa;
		lenptr = &len;
		*lenptr = sizeof(struct sockaddr_in);
		a_s = accept(GetSocket(), saptr, lenptr);
		if (a_s == -1)
		{
			perror("accept() failed");
		}
		else
		{
			X *tmp = new X(Handler());
			tmp -> Init();
			tmp -> Attach(a_s);
			tmp -> SetRemoteAddress( (struct sockaddr *)saptr,len);
			Handler().Add(tmp);
			tmp -> SetDeleteByHandler(true);
			if (Handler().OkToAccept())
			{
				tmp -> OnAccept();
			}
			else
			{
				tmp -> SetCloseAndDelete();
			}
		}
	}

private:
	port_t m_port;
	int m_depth;
};




#endif // _LISTENSOCKET_H


