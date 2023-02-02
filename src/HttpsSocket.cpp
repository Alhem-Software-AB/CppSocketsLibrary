/**
 **	File ......... HttpsSocket.cpp
 **	Published ....  2004-04-06
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
#ifdef HAVE_OPENSSL
#include "Parse.h"
#include "HttpsSocket.h"




HttpsSocket::HttpsSocket(SocketHandler& h)
:SSLSocket(h)
,m_first(true)
,m_header(true)
,m_request(false)
,m_response(false)
{
	SetLineProtocol();
}


HttpsSocket::~HttpsSocket()
{
}


#define BUFSIZE TCP_BUFSIZE_READ

void HttpsSocket::OnRead()
{
	SSLSocket::OnRead();
	if (!m_header)
	{
		if (ibuf.GetLength())
		{
			size_t n = ibuf.GetLength();
			char tmp[BUFSIZE];

			n = (n >= BUFSIZE) ? BUFSIZE - 1 : n;
			ibuf.Read(tmp,n);
			tmp[n] = 0;

			OnData(tmp,n);
		}
	}
}


void HttpsSocket::ReadLine()
{
	if (ibuf.GetLength())
	{
		size_t n = ibuf.GetLength();
		char tmp[BUFSIZE];

		n = (n >= BUFSIZE) ? BUFSIZE - 1 : n;
		ibuf.Read(tmp,n);
		tmp[n] = 0;

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


void HttpsSocket::OnLine(const std::string& line)
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
			m_http_version = pa.getword();
			m_request = true;
		}
		m_first = false;
		OnFirst();
		return;
	}
	if (!line.size())
	{
		SetLineProtocol( false );
		m_header = false;
		OnHeaderComplete();
		return;
	}
	Parse pa(line,":");
	std::string key = pa.getword();
	std::string value = pa.getrest();
	OnHeader(key,value);
}


void HttpsSocket::SendResponse()
{
	std::string msg;
	msg = m_http_version + " " + m_status + " " + m_status_text + "\n";
	for (string_m::iterator it = m_response_header.begin(); it != m_response_header.end(); it++)
	{
		std::string key = (*it).first;
		std::string val = (*it).second;
		msg += key + ": " + val + "\n";
	}
	msg += "\n";
	Send( msg );
}


#endif // HAVE_OPENSSL
