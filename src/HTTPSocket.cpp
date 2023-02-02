/**
 **	File ......... HTTPSocket.cpp
 **	Published ....  2004-04-06
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
#ifdef _WIN32
#pragma warning(disable:4786)
#define strcasecmp stricmp
#endif
#include <stdio.h>
#include <stdarg.h>
#include "Parse.h"
#include "HTTPSocket.h"




HTTPSocket::HTTPSocket(SocketHandler& h)
:TcpSocket(h)
,m_first(true)
,m_header(true)
,m_http_version("HTTP/1.0")
,m_request(false)
,m_response(false)
{
	SetLineProtocol();
}


HTTPSocket::~HTTPSocket()
{
}


void HTTPSocket::OnRead()
{
	TcpSocket::OnRead();
	if (!m_header)
	{
		if (ibuf.GetLength())
		{
			size_t n = ibuf.GetLength();
			char tmp[TCP_BUFSIZE_READ];

			n = (n >= TCP_BUFSIZE_READ) ? TCP_BUFSIZE_READ : n;
			ibuf.Read(tmp,n);

			OnData(tmp,n);
		}
	}
}


void HTTPSocket::ReadLine()
{
	if (ibuf.GetLength())
	{
		size_t n = ibuf.GetLength();
		char tmp[TCP_BUFSIZE_READ];

		n = (n >= TCP_BUFSIZE_READ) ? TCP_BUFSIZE_READ : n;
		ibuf.Read(tmp,n);

		for (size_t i = 0; i < n; i++)
		{
			if (!m_header)
			{
				OnData(tmp + i,n - i);
				break;
			}
			switch (tmp[i])
			{
			case 13: // ignore
				break;
			case 10: // end of line
				OnLine(m_line);
				m_line = "";
				break;
			default:
				m_line += tmp[i];
			}
		}
	}
}


void HTTPSocket::OnLine(const std::string& line)
{
	if (m_first)
	{
		Parse pa(line);
		std::string str = pa.getword();
		if (str.substr(0,4) == "HTTP") // response
		{
			m_http_version = str;
			m_status = pa.getword();
			m_status_text = pa.getrest();
			m_response = true;
		}
		else // request
		{
			m_method = str;
			m_url = pa.getword();
			size_t spl = m_url.find("?");
			if (spl != std::string::npos)
			{
				m_uri = m_url.substr(0,spl);
				m_query_string = m_url.substr(spl + 1);
			}
			else
			{
				m_uri = m_url;
			}
			m_http_version = pa.getword();
			m_request = true;
		}
		m_first = false;
		OnFirst();
		return;
	}
	if (!line.size())
	{
		SetLineProtocol(false);
		m_header = false;
		OnHeaderComplete();
		return;
	}
	Parse pa(line,":");
	std::string key = pa.getword();
	std::string value = pa.getrest();
	OnHeader(key,value);
	if (!strcasecmp(key.c_str(), "connection") &&
	    !strcasecmp(value.c_str(), "keep-alive"))
	{
		SetRetain();
	}
}


void HTTPSocket::SendResponse()
{
	std::string msg;
	msg = m_http_version + " " + m_status + " " + m_status_text + "\r\n";
	for (string_m::iterator it = m_response_header.begin(); it != m_response_header.end(); it++)
	{
		std::string key = (*it).first;
		std::string val = (*it).second;
		msg += key + ": " + val + "\r\n";
	}
	msg += "\r\n";
	Send( msg );
}


void HTTPSocket::AddResponseHeader(const std::string& header, char *format, ...)
{
	char slask[5000];
	va_list ap;

	va_start(ap, format);
#ifdef _WIN32
	vsprintf(slask, format, ap);
#else
	vsnprintf(slask, 5000, format, ap);
#endif
	va_end(ap);

	m_response_header[header] = slask;
}


void HTTPSocket::SendRequest()
{
	std::string msg;
	msg = m_method + " " + m_url + " " + m_http_version + "\r\n";
	for (string_m::iterator it = m_response_header.begin(); it != m_response_header.end(); it++)
	{
		std::string key = (*it).first;
		std::string val = (*it).second;
		msg += key + ": " + val + "\r\n";
	}
	msg += "\r\n";
	Send( msg );
}


std::string HTTPSocket::MyUseragent() 
{
	std::string version = "C++Sockets/";
#ifdef _VERSION
	version += _VERSION;
#endif
	return version;
}


