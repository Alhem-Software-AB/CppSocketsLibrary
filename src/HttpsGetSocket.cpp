/**
 **	File ......... HttpsGetSocket.cpp
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
//#include <assert.h>
//#include <stdio.h>
#ifdef _WIN32
#define strcasecmp stricmp
#endif

#include "socket_include.h"
#include "SocketHandler.h"
#include "Parse.h"
#include "HttpsGetSocket.h"

#define DEB(x) x


HttpsGetSocket::HttpsGetSocket(SocketHandler& h,const std::string& host,port_t port,const std::string& url,const std::string& to_file)
:HttpsSocket(h)
,m_host(host)
,m_port(port)
,m_url(url)
,m_to_file(to_file)
,m_fil(NULL)
,m_bComplete(false)
,m_content_length(0)
,m_content_ptr(0)
{
	if (!Open(m_host,m_port))
	{
		if (!Connecting())
		{
			fprintf(stderr,"connect() failed\n");
DEB(			printf("connect() failed\n");)
			SetCloseAndDelete();
		}
	}
	SetLineProtocol();
}


HttpsGetSocket::~HttpsGetSocket()
{
	if (m_fil)
		fclose(m_fil);
}


void HttpsGetSocket::OnSSLInitDone()
{
	std::string str =
		"GET " + m_url + " HTTP/1.0\n"
		"Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1\n"
		"Accept-Language: en-us,en;q=0.5\n"
		"Accept-Encoding: gzip,deflate\n"
		"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\n"
		"Host: " + m_host + ":" + Utility::l2string(m_port) + "\n"
		"\n";
DEB(		printf("%s\n",str.c_str());)
	Send(str);
}


/*
#define BUFSIZE 5000
void HttpsGetSocket::ReadLine()
{
DEB(	printf("HttpsGetSocket ReadLine ibuf GetLength = %d\n",ibuf.GetLength());)
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
//printf("%c\n",tmp[i]);
			if (m_bHeader)
			{
				if (tmp[i] == 13 || tmp[i] == 10)
				{
					tmp[i] = 0;
					m_line += (tmp + x);
					OnLine( m_line );
//					i++;
					if (i + 1 < n && (tmp[i + 1] == 13 || tmp[i + 1] == 10))
						i++;
					x = i + 1;
					m_line = "";
				}
			}
			else
			{
				if (m_fil && (m_content_ptr < m_content_length || !m_content_length) )
				{
					fwrite(&tmp[i],1,1,m_fil);
					m_content_ptr++;
				}
				if (m_content_length && m_content_ptr == m_content_length)
				{
					OnContent();
				}
			}
		}
		m_line += (tmp + x);
	}
}


void HttpsGetSocket::OnLine(const std::string& line)
{
	Parse pa(line,":");
	std::string key = pa.getword();

DEB(	printf("OnLine: %s\n",line.c_str());)
DEB(	printf(" line size: %d\n",line.size());)
	if (key.size() > 3 && key.substr(0,4) == "HTTP")
	{
		int result = pa.getvalue();
		if (result != 200)
		{
DEB(			printf("result != 200 (%d)\n",result);)
			SetCloseAndDelete();
		}
	}
	else
	if (!strcasecmp(key.c_str(),"content-type"))
	{
		m_content_type = pa.getrest();
	}
	else
	if (!strcasecmp(key.c_str(),"content-length"))
	{
		m_content_length = pa.getvalue();
	}
	else
	if (key == "")
	{
		m_fil = fopen(m_to_file.c_str(),"wb");
		if (!m_fil)
		{
DEB(			printf("couldn't open '%s' for writing\n",m_to_file.c_str());)
			SetCloseAndDelete();
		}
		if (m_content_length || m_content_type.size() )
		{
			m_bHeader = false;
		}
	}
}
*/


void HttpsGetSocket::OnContent()
{
DEB(	printf("Content read\n");)
	if (m_fil)
	{
		fflush(m_fil);
		fclose(m_fil);
	}
	m_fil = NULL;
	SetCloseAndDelete(true);
	m_bComplete = true;
}


void HttpsGetSocket::OnDelete()
{
DEB(	printf("OnDelete()\n");)
	if (m_fil)
	{
		OnContent();
	}
}


void HttpsGetSocket::OnFirst()
{
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


void HttpsGetSocket::OnHeader(const std::string& key,const std::string& value)
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


void HttpsGetSocket::OnHeaderComplete()
{
	m_fil = fopen(m_to_file.c_str(),"wb");
	if (!m_fil)
	{
DEB(		fprintf(stderr,"couldn't open '%s' for writing\n",m_to_file.c_str());)
		SetCloseAndDelete();
	}
}


void HttpsGetSocket::OnData(const char *buf,size_t len)
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


void HttpsGetSocket::InitAsClient()
{
	InitializeContext(SSLv23_method());
}


