/**
 **	File ......... HttpDebugSocket.cpp
 **	Published ....  2004-10-08
**/
/*
Copyright (C) 2004  Anders Hedstr�m (grymse@alhem.net)

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
#include "Utility.h"

#include "HttpDebugSocket.h"


HttpDebugSocket::HttpDebugSocket(SocketHandler& h) : HTTPSocket(h)
,m_content_length(0)
,m_read_ptr(0)
{
}


HttpDebugSocket::~HttpDebugSocket()
{
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
	m_read_ptr += l;
	if (m_read_ptr >= m_content_length && m_content_length)
	{
		Send("</pre><hr></body></html>");
		SetCloseAndDelete();
	}
}


