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


UdpSocket::UdpSocket(SocketHandler& h) : Socket(h)
{
}


UdpSocket::~UdpSocket()
{
}


SOCKET UdpSocket::Bind4(port_t &port,int range)
{
	SOCKET s = CreateSocket4(SOCK_DGRAM, "udp");
	if (s == -1)
	{
		return -1;
	}

	struct sockaddr_in sa;
	socklen_t sa_len = sizeof(sa);
	ipaddr_t l = 0;

	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET; // hp -> h_addrtype;
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
		closesocket(s);
		return -1;
	}
	Attach(s);
	return s;
}


SOCKET UdpSocket::Bind6(port_t &port,int range)
{
	SOCKET s = CreateSocket6(SOCK_DGRAM, "udp");
	if (s == -1)
	{
		return -1;
	}

	struct sockaddr_in6 sa;
	socklen_t sa_len = sizeof(sa);

	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6; // hp -> h_addrtype;
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
		closesocket(s);
		return -1;
	}
	Attach(s);
	return s;
}


void UdpSocket::SendTo(const std::string &str)
{
	// sendto()...

}


