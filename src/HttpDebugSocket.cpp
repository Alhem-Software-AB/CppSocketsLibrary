/** \file HttpDebugSocket.cpp
 **	\date  2004-10-08
**/
/*
Copyright (C) 2004,2005  Anders Hedstr�m (grymse@alhem.net)

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
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
#include "Utility.h"

#include "HttpDebugSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


HttpDebugSocket::HttpDebugSocket(SocketHandler& h) : HTTPSocket(h)
,m_content_length(0)
,m_read_ptr(0)
{
}


HttpDebugSocket::~HttpDebugSocket()
{
}


void HttpDebugSocket::Init()
{
	if (GetParent() -> GetPort() == 443)
	{
		EnableSSL();
	}
}


void HttpDebugSocket::OnFirst()
{
	Send(
		"HTTP/1.1 200 OK\n"
		"Content-type: text/html\n"
		"Connection: close\n"
		"Server: HttpDebugSocket/1.0\n"
		"\n");
	Send(
		"<html><head><title>Echo Request</title></head>"
		"<body><h3>Request Header</h3><pre style='background: #e0e0e0'>");
	Send(GetMethod() + " " + GetUrl() + " " + GetHttpVersion() + "\n");
}


void HttpDebugSocket::OnHeader(const std::string& key,const std::string& value)
{
	if (!strcasecmp(key.c_str(),"content-length"))
		m_content_length = atoi(value.c_str());

	Send(key + ": " + value + "\n");
}


void HttpDebugSocket::OnHeaderComplete()
{
	if (GetMethod() == "GET")
	{
		Send("</pre><hr></body></html>");
		SetCloseAndDelete();
	}
	else
	{
		Send("</pre><h3>Request Body</h3><pre style='background: #e0e0e0'>");
	}
}


void HttpDebugSocket::OnData(const char *p,size_t l)
{
	SendBuf(p,l);
	m_read_ptr += (int)l;
	if (m_read_ptr >= m_content_length && m_content_length)
	{
		Send("</pre><hr></body></html>");
		SetCloseAndDelete();
	}
}


#ifdef SOCKETS_NAMESPACE
}
#endif

