/**
 **	File ......... HttpGetSocket.cpp
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
#else
#include <errno.h>
#endif
#include "Utility.h"
#include "Parse.h"
#include "StdLog.h"
#include "SocketHandler.h"
#include "HttpGetSocket.h"


HttpGetSocket::HttpGetSocket(SocketHandler& h,const std::string& url_in,const std::string& filename)
:HTTPSocket(h)
,m_fil(NULL)
,m_bComplete(false)
,m_content_length(0)
,m_content_ptr(0)
{
	url_this(url_in,m_host,m_port,m_url,m_to_file);
	if (filename.size())
	{
		m_to_file = filename;
	}
	SetLineProtocol();

	if (!Open(m_host,m_port))
	{
		if (!Connecting())
		{
			Handler().LogError(this, "HttpGetSocket", -1, "connect() failed miserably", LOG_LEVEL_FATAL);
			SetCloseAndDelete();
		}
	}
}


HttpGetSocket::HttpGetSocket(SocketHandler& h,const std::string& host,port_t port,const std::string& url,const std::string& to_file)
:HTTPSocket(h)
,m_host(host)
,m_port(port)
,m_url(url)
,m_to_file(to_file)
,m_fil(NULL)
,m_bComplete(false)
,m_content_length(0)
,m_content_ptr(0)
{
	SetLineProtocol();

	if (!Open(m_host,m_port))
	{
		if (!Connecting())
		{
			Handler().LogError(this, "HttpGetSocket", -1, "connect() failed miserably", LOG_LEVEL_FATAL);
			SetCloseAndDelete();
		}
	}
}


HttpGetSocket::~HttpGetSocket()
{
	if (m_fil)
		fclose(m_fil);
}


void HttpGetSocket::OnConnect()
{
	SetMethod( "GET" );
	SetUrl( m_url );
	AddResponseHeader( "Accept", "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1");
	AddResponseHeader( "Accept-Language", "en-us,en;q=0.5");
	AddResponseHeader( "Accept-Encoding", "gzip,deflate");
	AddResponseHeader( "Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
#ifdef _VERSION
	AddResponseHeader( "User-agent", "C++Sockets/" _VERSION "");
#else
	AddResponseHeader( "User-agent", "C++ Sockets library");
#endif
	if (m_port != 80)
		AddResponseHeader( "Host", m_host + " " + Utility::l2string(m_port) );
	else
		AddResponseHeader( "Host", m_host );
	SendRequest();
}


void HttpGetSocket::OnContent()
{
	if (m_fil)
	{
		fflush(m_fil);
		fclose(m_fil);
	}
	m_fil = NULL;
	SetCloseAndDelete(true);
	m_bComplete = true;
}


void HttpGetSocket::OnDelete()
{
	if (m_fil)
	{
		OnContent();
	}
}


void HttpGetSocket::OnFirst()
{
/*
	printf("IsRequest: %s\n",IsRequest() ? "YES" : "NO");
	if (IsRequest())
	{
		printf(" Method: %s\n",GetMethod().c_str());
		printf(" URL: %s\n",GetUrl().c_str());
		printf(" Http version: %s\n",GetHttpVersion().c_str());
	}

	printf("IsResponse: %s\n",IsResponse() ? "YES" : "NO");
	if (IsResponse())
	{
		printf(" Http version: %s\n",GetHttpVersion().c_str());
		printf(" Status: %s\n",GetStatus().c_str());
		printf(" Status text: %s\n",GetStatusText().c_str());
	}
*/
	if (!IsResponse())
	{
		SetCloseAndDelete();
	}
	if (GetStatus() != "200")
	{
		printf("Failed (status %s): %s\n",GetStatus().c_str(),GetStatusText().c_str());
		SetCloseAndDelete();
	}
}


void HttpGetSocket::OnHeader(const std::string& key,const std::string& value)
{
	if (!strcasecmp(key.c_str(),"content-type"))
	{
		m_content_type = value;
	}
	else
	if (!strcasecmp(key.c_str(),"content-length"))
	{
		m_content_length = atol(value.c_str());
	}
}


void HttpGetSocket::OnHeaderComplete()
{
	m_fil = fopen(m_to_file.c_str(),"wb");
	if (!m_fil)
	{
		Handler().LogError(this, "OnHeaderComplete", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
	}
}


void HttpGetSocket::OnData(const char *buf,size_t len)
{
	if (m_fil)
	{
		fwrite(buf,1,len,m_fil);
	}
	m_content_ptr += len;
	if (m_content_length > 0 && m_content_ptr == m_content_length)
	{
		OnContent();
	}
}


void HttpGetSocket::url_this(const std::string& url_in,std::string& host,port_t& port,std::string& url,std::string& file)
{
	Parse pa(url_in,"/");
	pa.getword(); // http
	host = pa.getword();
	if (strstr(host.c_str(),":"))
	{
		Parse pa(host,":");
		pa.getword(host);
		port = static_cast<port_t>(pa.getvalue());
	}
	else
	{
		port = 80;
	}
	url = "/" + pa.getrest();
	{
		Parse pa(url,"/");
		std::string tmp = pa.getword();
		while (tmp.size())
		{
			file = tmp;
			tmp = pa.getword();
		}
	}
} // url_this


