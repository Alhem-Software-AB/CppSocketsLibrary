/**
 **	File ......... HTTPSocket.cpp
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
//#include <stdio.h>

#include "Parse.h"
#include "HTTPSocket.h"




HTTPSocket::HTTPSocket(SocketHandler& h)
:TcpSocket(h)
,m_first(true)
,m_header(true)
,m_request(false)
,m_response(false)
{
	SetLineProtocol();
}


HTTPSocket::~HTTPSocket()
{
}


#define BUFSIZE 10000

void HTTPSocket::OnRead()
{
	TcpSocket::OnRead();
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


void HTTPSocket::ReadLine()
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
			m_http_version = pa.getword();
			m_request = true;
		}
		m_first = false;
		OnFirst();
		return;
	}
	if (!line.size())
	{
		m_header = false;
		OnHeaderComplete();
		return;
	}
	Parse pa(line,":");
	std::string key = pa.getword();
	std::string value = pa.getrest();
	OnHeader(key,value);
}


