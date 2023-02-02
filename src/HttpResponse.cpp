/**
 **	\file HttpResponse.cpp
 **	\date  2007-10-05
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2007  Anders Hedstrom

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
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <stdarg.h>
#include <stdio.h>

#include "HttpResponse.h"
#include "HttpRequest.h"
#include "MemFile.h"
#include "File.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// --------------------------------------------------------------------------------------
HttpResponse::HttpResponse(HttpRequest& req) : HttpTransaction()
, m_req(req)
, m_http_status_code(0)
, m_file( new MemFile )
{
}


// --------------------------------------------------------------------------------------
HttpResponse::~HttpResponse()
{
	delete m_file;
}


// --------------------------------------------------------------------------------------
const HttpRequest& HttpResponse::Request() const
{
	return m_req;
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetHttpStatusCode(int value)
{
	m_http_status_code = value;
}


int HttpResponse::HttpStatusCode() const
{
	return m_http_status_code;
}



// --------------------------------------------------------------------------------------
void HttpResponse::SetHttpStatusMsg(const std::string& value)
{
	m_http_status_msg = value;
}


const std::string& HttpResponse::HttpStatusMsg() const
{
	return m_http_status_msg;
}


// --------------------------------------------------------------------------------------
void HttpResponse::Write( const std::string& str )
{
	Write( str.c_str(), str.size() );
}


// --------------------------------------------------------------------------------------
void HttpResponse::Write( const char *buf, size_t sz )
{
	m_file -> fwrite( buf, 1, sz );
}


// --------------------------------------------------------------------------------------
void HttpResponse::Writef( const char *format, ... )
{
	va_list ap;
	va_start(ap, format);
	char tmp[10000];
	vsprintf(tmp, format, ap);
	va_end(ap);
	m_file -> fwrite( tmp, 1, strlen(tmp) );
}


// --------------------------------------------------------------------------------------
void HttpResponse::SetFile( const std::string& path )
{
	delete m_file;
	m_file = new File();
	m_file -> fopen( path, "rb" );
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
