/**
 **	File ......... UdpSocket.cpp
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
#ifdef _WIN32
#pragma warning(disable:4786)
#include <stdlib.h>
#else
#include <errno.h>
#endif
#include <stdio.h>
#include <map>

#include "StdLog.h"
#include "SocketHandler.h"
#include "UdpSocket.h"


#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x) 
#endif


UdpSocket::UdpSocket(SocketHandler& h,size_t ibufsz) : Socket(h)
,m_connected(false)
,m_ibuf(new char[ibufsz])
,m_ibufsz(ibufsz)
{
}


UdpSocket::~UdpSocket()
{
	delete m_ibuf;
}


SOCKET UdpSocket::Bind4(port_t &port,int range)
{
	SOCKET s = GetSocket();
	if (s == INVALID_SOCKET)
	{
		s = CreateSocket4(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return s;
		}
		Attach(s);
	}
	struct sockaddr_in sa;
	socklen_t sa_len = sizeof(sa);
	ipaddr_t l = 0;

	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons( port );
	memmove(&sa.sin_addr,&l,4);

	int n = bind(s, (struct sockaddr *)&sa, sa_len);
	int tries = range;
	while (n == -1 && tries--)
	{
		sa.sin_port = htons( ++port );
		n = bind(s, (struct sockaddr *)&sa, sa_len);
	}
	if (n == -1)
	{
		Handler().LogError(this, "bind", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		Close();
		return INVALID_SOCKET;
	}
	return s;
}


#ifdef IPPROTO_IPV6
SOCKET UdpSocket::Bind6(port_t &port,int range)
{
	SOCKET s = GetSocket();
	if (s == INVALID_SOCKET)
	{
		s = CreateSocket6(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return s;
		}
		Attach(s);
	}
	struct sockaddr_in6 sa;
	socklen_t sa_len = sizeof(sa);

	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_port = htons( port );
	sa.sin6_flowinfo = 0;
	sa.sin6_scope_id = 0;
	// sa.sin6_addr is all 0's

	int n = bind(s, (struct sockaddr *)&sa, sa_len);
	int tries = range;
	while (n == -1 && tries--)
	{
		sa.sin6_port = htons( ++port );
		n = bind(s, (struct sockaddr *)&sa, sa_len);
	}
	if (n == -1)
	{
		Handler().LogError(this, "bind", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		Close();
		return INVALID_SOCKET;
	}
	return s;
}
#endif


/** if you wish to use Send, first Open a connection */
bool UdpSocket::Open4(ipaddr_t l,port_t port)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket4(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return false;
		}
		Attach(s);
	}
	struct sockaddr_in sa;
	socklen_t sa_len = sizeof(sa);

	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons( port );
	memmove(&sa.sin_addr,&l,4);

	if (connect(GetSocket(), (struct sockaddr *)&sa, sa_len) == -1)
	{
		Handler().LogError(this, "connect", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return false;
	}
	m_connected = true;
	return true;
}


bool UdpSocket::Open4(const std::string& host,port_t port)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket4(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return false;
		}
		Attach(s);
	}
	ipaddr_t a;
	if (u2ip(host, a))
	{
		return Open4(a, port);
	}
	return false;
}


#ifdef IPPROTO_IPV6
bool UdpSocket::Open6(struct in6_addr& a,port_t port)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket6(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return false;
		}
		Attach(s);
	}
	struct sockaddr_in6 sa;
	socklen_t sa_len = sizeof(sa);

	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_port = htons( port );
	sa.sin6_flowinfo = 0;
	sa.sin6_scope_id = 0;
	sa.sin6_addr = a;

	if (connect(GetSocket(), (struct sockaddr *)&sa, sa_len) == -1)
	{
		Handler().LogError(this, "connect", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return false;
	}
	m_connected = true;
	return true;
}


bool UdpSocket::Open6(const std::string& host,port_t port)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket6(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return false;
		}
		Attach(s);
	}
	struct in6_addr a;
	if (u2ip(host, a))
	{
		return Open6(a, port);
	}
	return false;
}
#endif


/** send to specified address */
void UdpSocket::SendToBuf4(const std::string& h,port_t p,const char *data,size_t len,int flags)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket4(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return;
		}
		Attach(s);
	}
	ipaddr_t a;
	if (u2ip(h,a))
	{
		struct sockaddr_in sa;
		socklen_t sa_len = sizeof(sa);

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons( p );
		memmove(&sa.sin_addr,&a,4);

		if (sendto(GetSocket(),data,len,flags,(struct sockaddr *)&sa,sa_len) == -1)
		{
			Handler().LogError(this,"sendto",errno,strerror(errno),LOG_LEVEL_ERROR);
		}
	}
}


#ifdef IPPROTO_IPV6
void UdpSocket::SendToBuf6(const std::string& h,port_t p,const char *data,size_t len,int flags)
{
	if (GetSocket() == INVALID_SOCKET)
	{
		SOCKET s = CreateSocket6(SOCK_DGRAM, "udp");
		if (s == INVALID_SOCKET)
		{
			return;
		}
		Attach(s);
	}
	struct in6_addr a;
	if (u2ip(h,a))
	{
		struct sockaddr_in6 sa;
		socklen_t sa_len = sizeof(sa);

		memset(&sa,0,sizeof(sa));
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons( p );
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		sa.sin6_addr = a;

		if (sendto(GetSocket(),data,len,flags,(struct sockaddr *)&sa,sa_len) == -1)
		{
			Handler().LogError(this,"sendto",errno,strerror(errno),LOG_LEVEL_ERROR);
		}
	}
}
#endif


void UdpSocket::SendTo4(const std::string& a,port_t p,const std::string& str,int flags)
{
	SendToBuf4(a,p,str.c_str(),str.size(),flags);
}


#ifdef IPPROTO_IPV6
void UdpSocket::SendTo6(const std::string& a,port_t p,const std::string& str,int flags)
{
	SendToBuf6(a,p,str.c_str(),str.size(),flags);
}
#endif


/** send to connected address */
void UdpSocket::SendBuf(const char *data,size_t len,int flags)
{
	if (!m_connected)
	{
		Handler().LogError(this,"SendBuf",0,"not connected",LOG_LEVEL_ERROR);
		return;
	}
	if (send(GetSocket(),data,len,flags) == -1)
	{
		Handler().LogError(this,"send",errno,strerror(errno),LOG_LEVEL_ERROR);
	}
}


void UdpSocket::Send(const std::string& str,int flags)
{
	SendBuf(str.c_str(),str.size(),flags);
}


void UdpSocket::OnRead()
{
#ifdef IPPROTO_IPV6
	if (IsIpv6())
	{
		struct sockaddr_in6 sa;
		socklen_t sa_len = sizeof(sa);
		int n = recvfrom(GetSocket(), m_ibuf, m_ibufsz, 0, (struct sockaddr *)&sa, &sa_len);
		if (n == -1)
		{
			Handler().LogError(this, "recvfrom", errno, strerror(errno), LOG_LEVEL_ERROR);
			return;
		}
		if (sa_len != sizeof(sa))
		{
			Handler().LogError(this, "recvfrom", 0, "unexpected address struct size", LOG_LEVEL_WARNING);
		}
		this -> OnRawData(m_ibuf, n, (struct sockaddr *)&sa, sa_len);
		return;
	}
#endif
	struct sockaddr_in sa;
	socklen_t sa_len = sizeof(sa);
	int n = recvfrom(GetSocket(), m_ibuf, m_ibufsz, 0, (struct sockaddr *)&sa, &sa_len);
	if (n == -1)
	{
		Handler().LogError(this, "recvfrom", errno, strerror(errno), LOG_LEVEL_ERROR);
		return;
	}
	if (sa_len != sizeof(sa))
	{
		Handler().LogError(this, "recvfrom", 0, "unexpected address struct size", LOG_LEVEL_WARNING);
	}
	this -> OnRawData(m_ibuf, n, (struct sockaddr *)&sa, sa_len);
}


