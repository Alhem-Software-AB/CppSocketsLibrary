/**
 **	File ......... HttpPutSocket.cpp
 **	Published ....  2004-10-30
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
#else
#include <errno.h>
#endif
#include "SocketHandler.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "Utility.h"
#include "Parse.h"

#include "HttpPutSocket.h"




HttpPutSocket::HttpPutSocket(SocketHandler& h,const std::string& url)
:HTTPSocket(h)
{
	std::string host;
	{
		Parse pa(url,"/");
		pa.getword(); // 'http:'
		host = pa.getword();
		SetUrl( "/" + pa.getrest() );
	}
	{
		Parse pa(host,":");
		m_host = pa.getword();
		m_port = (port_t)pa.getvalue();
		if (!m_port)
		{
			m_port = 80;
		}
	}
}


HttpPutSocket::~HttpPutSocket()
{
}


void HttpPutSocket::SetFile(const std::string& file)
{
	struct stat st;
	if (!stat(file.c_str(), &st))
	{
		m_filename = file;
		m_content_length = st.st_size;
	}
	else
	{
		Handler().LogError(this, "SetFile", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
	}
}


void HttpPutSocket::SetContentType(const std::string& type)
{
	m_content_type = type;
}



void HttpPutSocket::Open()
{
	// why do I have to specify TcpSocket:: to get to the Open() method??
	TcpSocket::Open(m_host, m_port);
}


void HttpPutSocket::OnConnect()
{
	SetMethod( "PUT" );
	SetHttpVersion( "HTTP/1.1" );
	AddResponseHeader( "Host", m_host );
	AddResponseHeader( "Content-type", m_content_type );
	AddResponseHeader( "Content-length", Utility::l2string(m_content_length) );
	SendRequest();

	FILE *fil = fopen(m_filename.c_str(), "rb");
	if (fil)
	{
		int n;
		char buf[2000];
		while ((n = fread(buf, 1, 2000, fil)) > 0)
		{
			SendBuf(buf, n);
		}
		fclose(fil);
	}
}



void HttpPutSocket::OnFirst()
{
	int status = atoi(GetStatus().c_str());
	printf("Response status %d: %s\n", status, GetStatusText().c_str());
}


void HttpPutSocket::OnHeader(const std::string& ,const std::string& )
{
}


void HttpPutSocket::OnHeaderComplete()
{
	SetCloseAndDelete();
}


void HttpPutSocket::OnData(const char *,size_t)
{
}


