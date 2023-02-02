// HttpdSocket.cpp
/*
Copyright (C) 2001-2004  Anders Hedstrom (grymse@alhem.net)

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
#define strcasecmp stricmp
#define setenv(x,y,z) SetEnvironmentVariable(x,y)
#endif
#include "Utility.h"
#include "Base64.h"
#include "HttpdCookies.h"
#include "HttpdForm.h"
#include "MemFile.h"
#include "HttpdSocket.h"

#define DEB(x)
/*
#define DEB(x) { \
	FILE *fil = fopen("httpdlog","at"); \
	if (!fil) \
		fil = fopen("httpdlog","wt"); \
	if (fil) { x; fclose(fil); } \
}
*/



// statics
int HttpdSocket::m_request_count = 0;
std::string HttpdSocket::m_start = "";


HttpdSocket::HttpdSocket(SocketHandler& h)
:HTTPSocket(h)
,m_content_length(0)
,m_file(NULL)
,m_received(0)
,m_request_id(++m_request_count)
,m_cookies(NULL)
,m_form(NULL)
{
	m_http_date = datetime2httpdate(GetDate());
	if (!m_start.size())
		m_start = m_http_date;
}


HttpdSocket::~HttpdSocket()
{
	if (m_file)
	{
		delete m_file;
	}
	if (m_cookies)
		delete m_cookies;
	if (m_form)
		delete m_form;
}


void HttpdSocket::OnFirst()
{
//	printf("Request: %s %s %s\n",GetMethod().c_str(),GetUrl().c_str(),GetHttpVersion().c_str());
}


void HttpdSocket::OnHeader(const std::string& key,const std::string& val)
{
	if (!strcasecmp(key.c_str(),"content-length"))
	{
		m_content_length = atoi(val.c_str());
		m_content_length_str = val;
	}
	else
	if (!strcasecmp(key.c_str(),"cookie"))
	{
		m_http_cookie = val;
	}
	else
	if (!strcasecmp(key.c_str(),"content-type"))
	{
		m_content_type = val;
	}
	else
	if (!strcasecmp(key.c_str(),"if-modified-since"))
	{
		m_if_modified_since = val;
	}
}


void HttpdSocket::OnHeaderComplete()
{
	m_cookies = new HttpdCookies(m_http_cookie);

	if (GetMethod() == "GET")
	{
		setenv("QUERY_STRING", GetQueryString().c_str(), 1);
	}
	setenv("REQUEST_METHOD", GetMethod().c_str(), 1);
	setenv("HTTP_COOKIE", m_http_cookie.c_str(), 1);
	setenv("CONTENT_TYPE", m_content_type.c_str(), 1);
	setenv("CONTENT_LENGTH", m_content_length_str.c_str(), 1);
#ifdef WIN32
	{
		char slask[1000];
		if (GetMethod() == "GET")
		{
			sprintf(slask,"QUERY_STRING=%s", GetQueryString().c_str());
			_putenv(slask);
		}
		sprintf(slask,"REQUEST_METHOD=%s", GetMethod().c_str());
		_putenv(slask);
		sprintf(slask,"HTTP_COOKIE=%s", m_http_cookie.c_str());
		_putenv(slask);
		sprintf(slask,"CONTENT_TYPE=%s", m_content_type.c_str());
		_putenv(slask);
		sprintf(slask,"CONTENT_LENGTH=%s", m_content_length_str.c_str());
		_putenv(slask);
	}
#endif

	if (GetMethod() == "POST")
	{
		m_file = new MemFile;
	}
	else
	if (GetMethod() == "GET")
	{
		m_form = new HttpdForm(GetQueryString(), GetQueryString().size() );
		if (GetUri() == "/image")
		{
			std::string str64 = 
				"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";
			Send64(str64, "image/png");
		}
		else
		{
			Exec();
		}
	}
	else
	{
		AddResponseHeader("Date", GetHttpDate());
		AddResponseHeader("Connection", "close");
		SetStatus("405");
		SetStatusText("Method not allowed");
		SendResponse();
	}
}


void HttpdSocket::OnData(const char *p,size_t l)
{
	if (m_file)
	{
		m_file -> fwrite(p,1,l);
	}
	m_received += l;
	if (m_received >= m_content_length && m_content_length)
	{
		// all done
		if (m_file && !m_form)
		{
			m_form = new HttpdForm(m_file);
			if (GetUri() == "/image")
			{
				std::string str64 = 
					"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";
				Send64(str64, "image/png");
			}
			else
			{
				Exec();
			}
		}
	}
}


void HttpdSocket::Send64(const std::string& str64, const std::string& type)
{
	Base64 bb;
	char slask[100];

	if (!strcasecmp(m_start.c_str(), m_if_modified_since.c_str()))
	{
		SetStatus("304");
		SetStatusText("Not Modified");
		SendResponse();
	}
	else
	{
		size_t len = bb.decode_length(str64);
		unsigned char *buf = new unsigned char[len];

		SetStatus("200");
		SetStatusText("OK");

		sprintf(slask,"%d",len);
		AddResponseHeader("Content-length", slask );
		AddResponseHeader("Content-type", type.c_str() );
		AddResponseHeader("Last-modified", m_start);
		SendResponse();

		bb.decode(str64, buf, len);
		SendBuf( (char *)buf, len);
		delete[] buf;
	}
}


std::string HttpdSocket::datetime2httpdate(const std::string& dt)
{
	char slask[8];
	struct tm tp;
	time_t t;
	char *days[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
	char *months[] = { "Jan","Feb","Mar","Apr","May","Jun",
	                   "Jul","Aug","Sep","Oct","Nov","Dec" };
	int i;
	char s[40];

/* 1997-12-16 09:50:40 */

	if (dt.size() == 19) // && (int)strlen(dt) == 19)
	{
		slask[4] = 0;
		tp.tm_year = atoi(strncpy(slask,dt.c_str(),4)) - 1900;
		slask[2] = 0;
		i = atoi(strncpy(slask,dt.c_str() + 5,2)) - 1;
		tp.tm_mon = i >= 0 ? i : 0;
		tp.tm_mday = atoi(strncpy(slask,dt.c_str() + 8,2));
		tp.tm_hour = atoi(strncpy(slask,dt.c_str() + 11,2));
		tp.tm_min = atoi(strncpy(slask,dt.c_str() + 14,2));
		tp.tm_sec = atoi(strncpy(slask,dt.c_str() + 17,2));
		tp.tm_wday = 0;
		tp.tm_yday = 0;
		tp.tm_isdst = 0;
		t = mktime(&tp);
		if (t == -1)
		{
			Handler().LogError(this, "datetime2httpdate", 0, "mktime() failed");
		}

		sprintf(s,"%s, %02d %s %d %02d:%02d:%02d GMT",
		 days[tp.tm_wday],
		 tp.tm_mday,
		 months[tp.tm_mon],
		 tp.tm_year + 1900,
		 tp.tm_hour,tp.tm_min,tp.tm_sec);
	} 
	else
	{
		*s = 0;
	}
	return s;
}


std::string HttpdSocket::GetDate()
{
	time_t t = time(NULL);
	struct tm* tp = localtime(&t);
	char slask[40];
	if (tp)
	{
		sprintf(slask,"%d-%02d-%02d %02d:%02d:%02d",
			tp -> tm_year + 1900,
			tp -> tm_mon + 1,
			tp -> tm_mday,
			tp -> tm_hour,tp -> tm_min,tp -> tm_sec);
	}
	else
	{
		*slask = 0;
	}
	return slask;
}


