/**
 **	File ......... Socket.cpp
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
#endif
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <fcntl.h>
#include <time.h>
#include "socket_include.h"
#include "Parse.h"
#include "SocketHandler.h"
#include "SocketThread.h"

#include "Socket.h"

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x) 
#endif


Socket::Socket(SocketHandler& h)
:m_handler(h)
,m_socket(-1)
,m_bDel(false)
,m_bClose(false)
,m_bConnecting(false)
,m_tCreate(time(NULL))
,m_line_protocol(false)
,m_ssl_connecting(false)
//,m_tActive(time(NULL))
//,m_timeout(0)
,m_detach(false)
,m_detached(false)
,m_pThread(NULL)
{
}


Socket::~Socket()
{
	if (m_socket != -1)
	{
		Close();
	}
}


void Socket::Init()
{
}


void Socket::OnRead()
{
}


void Socket::OnWrite()
{
}


void Socket::OnException()
{
	SetCloseAndDelete();
}


void Socket::OnDelete()
{
}


void Socket::OnConnect()
{
}


bool Socket::CheckConnect()
{
	int err;
	socklen_t errlen = sizeof(err);
	bool r = true;
#ifdef _WIN32
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char *)&err, &errlen);
#else
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &errlen);
#endif
	if (err)
	{
DEB(		printf("OnConnect() failed: %s\n",strerror(err));)
		SetCloseAndDelete( true );
		r = false;
	}
	SetConnecting(false);
	return r;
}


void Socket::OnAccept()
{
}


/*
void Socket::OnCallback(int )
{
}
*/


int Socket::Close()
{
	int n;
	if (shutdown(m_socket, SHUT_RDWR) == -1)
	{
		// failed...
	}
	if ((n = closesocket(m_socket)) == -1)
	{
		// failed...
	}
	m_socket = -1;
	return n;
}


SOCKET Socket::CreateSocket(int type)
{
	int optval;
	SOCKET s;

	s = socket(AF_INET, type, 0);
	if (s == -1)
	{
		perror("socket() failed");
		return -1;
	}

	if (type == SOCK_STREAM)
	{
		optval = 1;
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) == -1)
		{
			perror("setsockopt() failed");
			closesocket(s);
			return -1;
		}

		optval = 1;
		if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) == -1)
		{
			perror("setsockopt() failed");
			closesocket(s);
			return -1;
		}
	}

	return s;
}


bool Socket::isip(const std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (!isdigit(str[i]) && str[i] != '.')
			return false;
	return true;
}


bool Socket::u2ip(const std::string& str, ipaddr_t& l)
{
	if (isip(str))
	{
		Parse pa((char *)str.c_str(),".");
		union {
			struct {
				unsigned char b1;
				unsigned char b2;
				unsigned char b3;
				unsigned char b4;
			} a;
			ipaddr_t l;
		} u;
		u.a.b1 = pa.getvalue();
		u.a.b2 = pa.getvalue();
		u.a.b3 = pa.getvalue();
		u.a.b4 = pa.getvalue();
		l = u.l;
DEB(		printf("u2ip: %08lX\n",l);)
		return true;
	}
	else
	{
		struct hostent *he = gethostbyname( str.c_str() );
		if (!he)
			return false;
		memcpy(&l,he -> h_addr,4);
		return true;
	}
	return false;
}


void Socket::l2ip(ipaddr_t ip,std::string& str)
{
DEB(	printf("l2ip: %08lX\n",ip);)
	union {
		struct {
			unsigned char b1;
			unsigned char b2;
			unsigned char b3;
			unsigned char b4;
		} a;
		ipaddr_t l;
	} u;
	u.l = ip;
	char tmp[100];
	sprintf(tmp,"%u.%u.%u.%u",u.a.b1,u.a.b2,u.a.b3,u.a.b4);
	str = tmp;
DEB(	printf("    : %s\n",tmp);)
}


void Socket::Attach(SOCKET s) 
{
	m_socket = s;
}


SOCKET Socket::GetSocket() 
{
	return m_socket;
}


void Socket::SetDeleteByHandler(bool x) 
{
	m_bDel = x;
}


bool Socket::DeleteByHandler() 
{
	return m_bDel;
}


void Socket::SetCloseAndDelete(bool x) 
{
	m_bClose = x;
}


bool Socket::CloseAndDelete() 
{
	return m_bClose;
}


void Socket::SetConnecting(bool x) 
{
	m_bConnecting = x;
	m_tConnect = time(NULL);
}


bool Socket::Connecting() 
{
	return m_bConnecting;
}


void Socket::SetRemoteAddress(struct sockaddr* sa,socklen_t l) 
{
	memcpy(&m_sa,sa,l);
}


SocketHandler& Socket::Handler() 
{
	return m_handler;
}


ipaddr_t Socket::GetRemoteIP()
{
	ipaddr_t l = 0;
	struct sockaddr_in* saptr = (struct sockaddr_in*)&m_sa;
	memcpy(&l,&saptr -> sin_addr,4);
	return l;
}


port_t Socket::GetRemotePort()
{
	struct sockaddr_in* saptr = (struct sockaddr_in*)&m_sa;
	return ntohs(saptr -> sin_port);
}


std::string Socket::GetRemoteAddress()
{
	std::string str;
	l2ip(GetRemoteIP(),str);
	return str;
}


std::string Socket::GetRemoteHostname()
{
	std::string str;
	long l = GetRemoteIP();
#ifdef _WIN32
	struct hostent *he = gethostbyaddr( (char *)&l,sizeof(long),AF_INET);
#else
	struct hostent *he = gethostbyaddr(&l,sizeof(long),AF_INET);
#endif
	if (!he)
		return GetRemoteAddress();
	str = he -> h_name;
	return str;
}


bool Socket::SetNonblocking(bool bNb)
{
#ifdef _WIN32
	unsigned long l = bNb ? 1 : 0;
	int n = ioctlsocket(m_socket, FIONBIO, &l);
	if (n != 0)
	{
		int errcode;
		errcode = WSAGetLastError();
		fprintf(stderr,"ioctlsocket(FIONBIO) failed, errcode %d\n",errcode);
		return false;
	}
	return true;
#else
	if (bNb)
	{
		if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
		{
			perror("fcntl()");
			return false;
		}
	}
	else
	{
		if (fcntl(m_socket, F_SETFL, 0) == -1)
		{
			perror("fcntl()");
			return false;
		}
	}
	return true;
#endif
}


bool Socket::SetNonblocking(bool bNb,SOCKET s)
{
#ifdef _WIN32
	unsigned long l = bNb ? 1 : 0;
	int n = ioctlsocket(s, FIONBIO, &l);
	if (n != 0)
	{
		int errcode;
		errcode = WSAGetLastError();
		fprintf(stderr,"ioctlsocket(FIONBIO) failed, errcode %d\n",errcode);
		return false;
	}
	return true;
#else
	if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl()");
		return false;
	}
	return true;
#endif
}


void Socket::Set(bool bRead,bool bWrite,bool bException)
{
	m_handler.Set(m_socket,bRead,bWrite,bException);
}


time_t Socket::GetConnectTime()
{
	return time(NULL) - m_tConnect;
}


bool Socket::Ready()
{
	if (m_socket != -1 && !Connecting() && !CloseAndDelete())
		return true;
	return false;
}


bool Socket::Detach()
{
	if (!DeleteByHandler())
		return false;
	if (m_pThread)
		return false;
	if (m_detached)
		return false;
	m_detach = true;
	return true;
}


void Socket::DetachSocket()
{
	m_pThread = new SocketThread(*this);
	m_pThread -> SetRelease(true);
}


