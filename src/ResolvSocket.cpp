/** \file ResolvSocket.cpp
 **	\date  2005-03-24
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004-2006  Anders Hedstrom

This library is made available under the terms of the GNU GPL.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

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
/*
#if defined(_WIN32) || defined(__CYGWIN__)
#pragma warning(disable:4786)
#include <winsock.h>
#else
#include <netdb.h>
#endif
*/
#include "ResolvSocket.h"
#include "Utility.h"
#include "Parse.h"
#include "ISocketHandler.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x)
#endif


ResolvSocket::ResolvSocket(ISocketHandler& h,Socket *parent)
:TcpSocket(h)
,m_bServer(false)
,m_parent(parent)
{
	SetLineProtocol();
}


ResolvSocket::~ResolvSocket()
{
}


void ResolvSocket::OnLine(const std::string& line)
{
	Parse pa(line, ":");
	if (m_bServer)
	{
		m_query = pa.getword();
		m_data = pa.getrest();
DEB(		printf("ResolvSocket server; query=%s, data=%s\n", m_query.c_str(), m_data.c_str());)
		if (!Detach()) // detach failed?
		{
			SetCloseAndDelete();
		}
		return;
	}
	std::string key = pa.getword();
	std::string value = pa.getrest();

	if (key == "Failed" && m_parent)
	{
DEB(		printf("************ Resolve failed\n");)
		m_parent -> OnResolveFailed(m_resolv_id);
		m_parent = NULL;
	}
	else
	if (key == "Name" && !m_resolv_host.size() && m_parent)
	{
		m_parent -> OnResolved(m_resolv_id, value);
		m_parent = NULL;
	}
	else
	if (key == "A" && m_parent)
	{
		ipaddr_t l;
		Utility::u2ip(value, l); // ip2ipaddr_t
		m_parent -> OnResolved(m_resolv_id, l, m_resolv_port);
		m_parent = NULL; // always use first ip in case there are several
	}
}


void ResolvSocket::OnDetached()
{
DEB(	printf("ResolvSocket::OnDetached(); query=%s, data=%s\n", m_query.c_str(), m_data.c_str());)
#ifndef _WIN32
	if (m_query == "getaddrinfo")
	{
		struct addrinfo hints;
		struct addrinfo *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_flags |= AI_CANONNAME;
		int n = getaddrinfo(m_data.c_str(), NULL, &hints, &res);
		if (!n)
		{
			while (res)
			{
				Send("Flags: " + Utility::l2string(res -> ai_flags) + "\n");
				Send("Family: " + Utility::l2string(res -> ai_family) + "\n");
				Send("Socktype: " + Utility::l2string(res -> ai_socktype) + "\n");
				Send("Protocol: " + Utility::l2string(res -> ai_protocol) + "\n");
				Send("Addrlen: " + Utility::l2string(res -> ai_addrlen) + "\n");
				std::string tmp;
				Base64 bb;
				bb.encode( (unsigned char *)res -> ai_addr, res -> ai_addrlen, tmp, false);
				Send("Address: " + tmp + "\n");
				// base64-encoded sockaddr
				Send("Canonname: ");
				Send( res -> ai_canonname );
				Send("\n");
				Send("\n");
				//
				res = res -> ai_next;
			}
			freeaddrinfo(res);
		}
		else
		{
			std::string error = "Error: ";
			error += gai_strerror(n);
			Send( error + "\n" );
			Send("\n");
		}
	}
	else
#endif // _WIN32
	if (m_query == "gethostbyname")
	{
#ifdef LINUX
		struct hostent he;
		struct hostent *result;
		int myerrno;
		char buf[2000];
		// solaris safe?
		int n = gethostbyname_r(m_data.c_str(), &he, buf, sizeof(buf), &result, &myerrno);
		if (!n)
		{
			Send("Name: " + (std::string)he.h_name);
			size_t i = 0;
			while (he.h_aliases[i])
			{
				Send("Alias: " + (std::string)he.h_aliases[i] + "\n");
				i++;
			}
			Send("AddrType: " + Utility::l2string(he.h_addrtype) + "\n");
			Send("Length: " + Utility::l2string(he.h_length) + "\n");
			i = 0;
			while (he.h_addr_list[i])
			{
				// let's assume 4 byte IPv4 addresses
				char ip[40];
				*ip = 0;
				for (int j = 0; j < 4; j++)
				{
					if (*ip)
						strcat(ip,".");
					sprintf(ip + strlen(ip),"%u",(unsigned char)he.h_addr_list[i][j]);
				}
				Send("A: " + (std::string)ip + "\n");
				i++;
			}
		}
		else
		{
			Send("Failed\n");
		}
		Send("\n");
#else
		struct hostent *h = gethostbyname(m_data.c_str());
		if (h)
		{
			Send("Name: " + (std::string)h -> h_name + "\n");
			size_t i = 0;
			while (h -> h_aliases[i])
			{
				Send("Alias: " + (std::string)h -> h_aliases[i] + "\n");
				i++;
			}
			Send("AddrType: " + Utility::l2string(h -> h_addrtype) + "\n");
			Send("Length: " + Utility::l2string(h -> h_length) + "\n");
			i = 0;
			while (h -> h_addr_list[i])
			{
				// let's assume 4 byte IPv4 addresses
				char ip[40];
				*ip = 0;
				for (int j = 0; j < 4; j++)
				{
					if (*ip)
						strcat(ip,".");
					sprintf(ip + strlen(ip),"%u",(unsigned char)h -> h_addr_list[i][j]);
				}
				Send("A: " + (std::string)ip + "\n");
				i++;
			}
		}
		else
		{
			Send("Failed\n");
		}
		Send( "\n" );
#endif
	}
	else
	if (m_query == "gethostbyaddr")
	{
		// input: ipv4 ip address
		ipaddr_t a;
		Utility::u2ip(m_data.c_str(), a);
		struct hostent *h = gethostbyaddr( (const char *)&a, sizeof(a), AF_INET);
		if (h)
		{
			Send("Name: " + (std::string)h -> h_name + "\n");
			size_t i = 0;
			while (h -> h_aliases[i])
			{
				Send("Alias: " + (std::string)h -> h_aliases[i] + "\n");
				i++;
			}
			Send("AddrType: " + Utility::l2string(h -> h_addrtype) + "\n");
			Send("Length: " + Utility::l2string(h -> h_length) + "\n");
			i = 0;
			while (h -> h_addr_list[i])
			{
				// let's assume 4 byte IPv4 addresses
				char ip[40];
				*ip = 0;
				for (int j = 0; j < 4; j++)
				{
					if (*ip)
						strcat(ip,".");
					sprintf(ip + strlen(ip),"%u",(unsigned char)h -> h_addr_list[i][j]);
				}
				Send("A: " + (std::string)ip + "\n");
				i++;
			}
		}
		else
		{
			Send("Failed\n");
		}
		Send("\n");
	}
	else
	{
		std::string msg = "Unknown query type: " + m_query;
		Handler().LogError(this, "OnDetached", 0, msg);
		Send("Unknown\n\n");
	}
	SetCloseAndDelete();
}


void ResolvSocket::OnConnect()
{
	if (m_resolv_host.size())
	{
		std::string msg = "gethostbyname " + m_resolv_host + "\n";
		Send( msg );
		return;
	}
	std::string tmp;
	Utility::l2ip(m_resolv_address, tmp);
	std::string msg = "gethostbyaddr " + tmp + "\n";
	Send( msg );
}


void ResolvSocket::OnDelete()
{
	if (m_parent)
	{
		m_parent -> OnResolveFailed(m_resolv_id);
		m_parent = NULL;
	}
}


#ifdef SOCKETS_NAMESPACE
}
#endif

