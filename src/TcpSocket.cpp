/**
 **	File ......... TcpSocket.cpp
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
#define strcasecmp stricmp
#endif
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

#include "socket_include.h"
#include "SocketHandler.h"
#include "TcpSocket.h"

using std::string;

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x)  
#endif


TcpSocket::TcpSocket(SocketHandler& h) : Socket(h)
,ibuf(10240)
,obuf(32768)
,m_line("")
{
}


TcpSocket::~TcpSocket()
{
}


bool TcpSocket::Open(ipaddr_t ip,port_t port)
{
	SOCKET s = CreateSocket(SOCK_STREAM);
	if (s == -1)
	{
		perror("CreateSocket() failed");
		return false;
	}
	ipaddr_t l = ip;
	{
		struct sockaddr_in sa;
		socklen_t sa_len = sizeof(sa);

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET; // hp -> h_addrtype;
		sa.sin_port = htons( port );
		memcpy(&sa.sin_addr,&l,4);

		if (!SetNonblocking(true, s))
		{
			closesocket(s);
			return false;
		}
		int n = connect(s, (struct sockaddr *)&sa, sa_len);
		if (n == -1)
		{
#ifdef _WIN32
			int errcode;
			errcode = WSAGetLastError();
			if (errcode != WSAEWOULDBLOCK)
#else
			if (errno != EINPROGRESS)
#endif
			{
				perror("connect() failed 4");
				closesocket(s);
				return false;
			}
			else
			{
				SetConnecting( true ); // this flag will control fd_set's
			}
		}
		SetRemoteAddress( (struct sockaddr *)&sa,sa_len);
		Attach(s);
		return !Connecting();
	}
	return false;
}


bool TcpSocket::Open(const string &host,port_t port)
{
	SOCKET s = CreateSocket(SOCK_STREAM);
	if (s == -1)
	{
		perror("CreateSocket() failed");
		return false;
	}
	ipaddr_t l;
	if (u2ip(host,l))
	{
		struct sockaddr_in sa;
		socklen_t sa_len = sizeof(sa);

{
	std::string ipstr;
	l2ip(l,ipstr);
DEB(	printf("Connecting to: %s:%d\n",ipstr.c_str(),port);)
}

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET; // hp -> h_addrtype;
		sa.sin_port = htons( port );
		memcpy(&sa.sin_addr,&l,4);

		if (!SetNonblocking(true, s))
		{
			closesocket(s);
			return false;
		}
		int n = connect(s, (struct sockaddr *)&sa, sa_len);
		if (n == -1)
		{
#ifdef _WIN32
			int errcode;
			errcode = WSAGetLastError();
			if (errcode != WSAEWOULDBLOCK)
#else
			if (errno != EINPROGRESS)
#endif
			{
				perror("connect() failed 5");
				closesocket(s);
				return false;
			}
			else
			{
				SetConnecting(true);
			}
		}
		SetRemoteAddress((struct sockaddr *)&sa,sa_len);
		Attach(s);
		return !Connecting();
	}
	return false;
}


void TcpSocket::OnRead()
{
	int n = ibuf.Space();
	char buf[1024];
	if (!n)
		return; // bad
	n = readsocket(GetSocket(),buf,(n < 1024) ? n : 1024);
	if (n == -1)
	{
		SetCloseAndDelete(true); // %!
DEB(		perror("read() error");)
	}
	else
	if (!n)
	{
		SetCloseAndDelete(true);
DEB(		printf("read() returns 0\n");)
	}
	else
	{
DEB(		printf("read %d bytes\n",n);)
		if (!ibuf.Write(buf,n))
		{
			// overflow
		}
	}
DEB(printf("TcpSocket::OnRead() ok\n");)
}


void TcpSocket::OnWrite()
{
/*
	assert(GetSocket() != INVALID_SOCKET);
	if (obuf.GetL() <= 0)
	{
printf("OnWrite abort because: nothing to write\n");
		Set(true, false);
		return;
	}
	assert(obuf.GetL() > 0);
	if (!Handler().Valid(this))
	{
printf("OnWrite abort because: not valid\n");
		return;
	}
	if (!Ready())
	{
printf("OnWrite abort because: not ready\n");
		return;
	}
*/
	int n = writesocket(GetSocket(),obuf.GetStart(),obuf.GetL());
DEB(	printf("OnWrite: %d bytes sent\n",n);)
	if (n == -1)
	{
#ifdef _WIN32
		int x = WSAGetLastError();
DEB(	printf("write() error, errcode = %d\n",x);)
#endif
		SetCloseAndDelete(true); // %!
DEB(		perror("write() error");)
	}
	else
	if (!n)
	{
//		SetCloseAndDelete(true);
DEB(		printf("write() returns 0\n");)
	}
	else
	{
DEB(		printf(" %d bytes written\n",n);)
		obuf.Remove(n);
	}
	// check m_mes
	while (obuf.Space() && m_mes.size())
	{
		MES *p = m_mes[0];
		if (obuf.Space() > p -> left())
		{
			obuf.Write(p -> curbuf(),p -> left());
			delete p;
//printf("\n m_mes erase()\n");
			m_mes.erase(m_mes.begin());
		}
		else
		{
			size_t sz = obuf.Space();
			obuf.Write(p -> curbuf(),sz);
			p -> ptr += sz;
		}
	}
	if (obuf.GetLength())
		Set(true, true);
	else
		Set(true, false);
}


void TcpSocket::Send(const string &str)
{
	SendBuf(str.c_str(),str.size());
}


void TcpSocket::SendBuf(const char *buf,size_t len)
{
	int n = obuf.GetLength();
	if (!Ready())
	{
//		fprintf(stderr,"Attempt to write to a non-ready socket\n");
		return;
	}
DEB(	printf("trying to send %d bytes;  buf before = %d bytes\n",len,n);)
	if (m_mes.size() || len > obuf.Space())
	{
		MES *p = new MES(buf,len);
//printf("\n m_mes push_back()\n");
		m_mes.push_back(p);
	}
	if (m_mes.size())
	{
		while (obuf.Space() && m_mes.size())
		{
			MES *p = m_mes[0];
			if (obuf.Space() > p -> left())
			{
				obuf.Write(p -> curbuf(),p -> left());
				delete p;
//printf("\n m_mes erase()\n");
				m_mes.erase(m_mes.begin());
			}
			else
			{
				size_t sz = obuf.Space();
				obuf.Write(p -> curbuf(),sz);
				p -> ptr += sz;
			}
		}
	}
	else
	{
		if (!obuf.Write(buf,len))
		{
DEB(			printf(" Send overflow\n");)
			// overflow
		}
	}
	if (!n)
	{
		OnWrite();
	}
}


void TcpSocket::OnLine(const std::string& )
{
}


#define BUFSIZE 16384

void TcpSocket::ReadLine()
{
	if (ibuf.GetLength())
	{
		size_t x = 0;
		size_t n = ibuf.GetLength();
		char tmp[BUFSIZE];

		n = (n >= BUFSIZE) ? BUFSIZE - 1 : n;
		ibuf.Read(tmp,n);
		tmp[n] = 0;

		for (size_t i = 0; i < n; i++)
		{
			while (tmp[i] == 13 || tmp[i] == 10)
			{
				char c = tmp[i];
				tmp[i] = 0;
				if (tmp[x])
				{
					m_line += (tmp + x);
				}
				OnLine( m_line );
				i++;
				if (i < n && (tmp[i] == 13 || tmp[i] == 10) && tmp[i] != c)
				{
					i++;
				}
				x = i;
				m_line = "";
			}
		}
		if (tmp[x])
		{
			m_line += (tmp + x);
		}
	}
}


