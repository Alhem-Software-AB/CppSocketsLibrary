/**
 **	File ......... SocketHandler.cpp
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
#include <stdio.h>
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include "socket_include.h"

#include "Socket.h"
#include "TcpSocket.h"
#include "SocketHandler.h"
#include "UdpSocket.h"

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x) 
#endif


SocketHandler::SocketHandler()
:m_maxsock(0)
,m_host("")
,m_ip(0)
,m_preverror(-1)
{
	struct hostent *he;
	char h[256];

	FD_ZERO(&m_rfds);
	FD_ZERO(&m_wfds);
	FD_ZERO(&m_efds);

	// get local hostname and translate into ip-address
	*h = 0;
	gethostname(h,255);
	he = gethostbyname(h);
	if (he)
	{
		char ipad[100];
		union {
			ipaddr_t ip;
			struct {
				unsigned char b1;
				unsigned char b2;
				unsigned char b3;
				unsigned char b4;
			} a;
		} u;
		memmove(&u.ip,he -> h_addr,4);
		sprintf(ipad,"%u.%u.%u.%u",u.a.b1,u.a.b2,u.a.b3,u.a.b4);
		m_ip = u.ip;
		m_addr = ipad;
	}
	m_host = h;
}


SocketHandler::~SocketHandler()
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		Socket *p = (*it).second;
		p -> Close();
//		p -> OnDelete();
		if (p -> DeleteByHandler())
		{
			delete p;
		}
	}
}


void SocketHandler::Add(Socket *p)
{
	m_add[p -> GetSocket()] = p;
	if (p -> Connecting())
		Set(p -> GetSocket(),false,true);
	else
		Set(p -> GetSocket(),true,false);
	m_maxsock = (p -> GetSocket() > m_maxsock) ? p -> GetSocket() : m_maxsock;
}


void SocketHandler::Set(SOCKET s,bool bRead,bool bWrite,bool bException)
{
	if (s >= 0)
	{
		if (bRead)
		{
			if (!FD_ISSET(s, &m_rfds))
				FD_SET(s, &m_rfds);
		}
		else
			FD_CLR(s, &m_rfds);
		if (bWrite)
		{
			if (!FD_ISSET(s, &m_wfds))
				FD_SET(s, &m_wfds);
		}
		else
			FD_CLR(s, &m_wfds);
		if (bException)
		{
			if (!FD_ISSET(s, &m_efds))
				FD_SET(s, &m_efds);
		}
		else
			FD_CLR(s, &m_efds);
	}
}


int SocketHandler::Select(long sec,long usec)
{
	struct timeval tv;
	fd_set rfds = m_rfds;
	fd_set wfds = m_wfds;
	fd_set efds = m_efds;
	int n;
//DEB(	printf("tick\n");)

	while (m_add.size())
	{
		socket_m::iterator it = m_add.begin();
		SOCKET s = (*it).first;
		Socket *p = (*it).second;
		m_sockets[s] = p;
		m_add.erase(it);
	}
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	n = select(m_maxsock + 1,&rfds,&wfds,&efds,&tv);
	if (n == -1)
	{
#ifdef _WIN32
DEB(
		int errcode = WSAGetLastError();
		if (errcode != m_preverror)
		{
			printf("  select() errcode = %d\n",errcode);
			m_preverror = errcode;
			for (int i = 0; i <= m_maxsock; i++)
			{
				if (FD_ISSET(i, &m_rfds))
					printf("%4d: Read\n",i);
				if (FD_ISSET(i, &m_wfds))
					printf("%4d: Write\n",i);
				if (FD_ISSET(i, &m_efds))
					printf("%4d: Exception\n",i);
			}
		}
) // DEB
#else
		perror("select() failed");
#endif
	}
	else
//	if (n > 0)
	{
		for (socket_m::iterator it2 = m_sockets.begin(); it2 != m_sockets.end(); it2++)
		{
			SOCKET i = (*it2).first;
			Socket *p = (*it2).second;
			if (p && !p -> IsDetached() )
			{
				if (p -> SSLConnecting())
				{
					if (p -> SSLCheckConnect())
					{
						p -> OnSSLInitDone();
					}
				}
				else
				if (n > 0)
				{
					if (FD_ISSET(i, &rfds))
					{
						p -> OnRead();
						if (p -> LineProtocol())
						{
							p -> ReadLine();
						}
//						p -> Touch();
					}
					if (FD_ISSET(i, &wfds))
					{
						if (p -> Connecting())
						{
							if (p -> CheckConnect())
							{
DEB(								printf("calling OnConnect\n");)
								p -> OnConnect();
							}
//							p -> Touch();
						}
						else
						{
							p -> OnWrite();
//							p -> Touch();
						}
					}
					if (FD_ISSET(i, &efds))
					{
						p -> OnException();
					}
				}
			}
		}
	}
	for (socket_m::iterator it3 = m_sockets.begin(); it3 != m_sockets.end(); it3++)
	{
		Socket *p = (*it3).second;
		if (p && !p -> IsDetached())
		{
/*
			if (p && p -> Timeout() && p -> Inactive() > p -> Timeout())
			{
				p -> SetCloseAndDelete();
			}
*/
			if (p && p -> Connecting() && p -> GetConnectTime() > 5)
			{
//				fprintf(stderr,"Connect timeout - removing socket\n");
				p -> SetCloseAndDelete(true);
			}
			if (p && p -> CloseAndDelete() )
			{
				Set(p -> GetSocket(),false,false,false);
				p -> Close();
				p -> OnDelete();
				if (p -> DeleteByHandler())
				{
					delete p;
				}
				m_sockets.erase(it3);
				break;
			}
			if (p -> IsDetach())
			{
			}
		}
	}
	return n;
}


const std::string& SocketHandler::GetLocalHostname()
{
	return m_host;
}


ipaddr_t SocketHandler::GetLocalIP()
{
	return m_ip;
}


const std::string& SocketHandler::GetLocalAddress()
{
	return m_addr;
}


void SocketHandler::StatLoop(long sec,long usec)
{
	time_t t = time(NULL);
	int count = 0;
	for (;;)
	{
		if (Select(sec, usec) > 0)
			count++;
		time_t x = time(NULL);
		if (t != x)
		{
			if (count)
				printf("ticks: %d\n", count);
			t = x;
			count = 0;
		}
	}
}


bool SocketHandler::Valid(Socket *p0)
{
	for (socket_m::iterator it3 = m_sockets.begin(); it3 != m_sockets.end(); it3++)
	{
		Socket *p = (*it3).second;
		if (p0 == p)
			return true;
	}
	return false;
}


