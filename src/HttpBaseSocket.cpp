/**
 **	\file AjpBaseSocket.cpp
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
#include "HttpBaseSocket.h"
#include "IFile.h"
#include "Utility.h"
#include "HttpResponse.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x)
#endif


HttpBaseSocket::HttpBaseSocket(ISocketHandler& h)
:HTTPSocket(h)
,m_res_file(NULL)
,m_b_keepalive(false)
{
}


HttpBaseSocket::~HttpBaseSocket()
{
}


void HttpBaseSocket::OnFirst()
{
DEB(fprintf(stderr, "  %s %s %s\n", GetMethod().c_str(), GetUri().c_str(), GetHttpVersion().c_str());)
	m_req.SetHttpMethod( GetMethod() );
	m_req.SetUri( GetUri() );
	m_req.SetHttpVersion( GetHttpVersion() );

	m_req.SetAttribute("query_string", GetQueryString() );

	m_req.SetRemoteAddr( GetRemoteAddress() );
	m_req.SetRemoteHost( "" ); // %!
	m_req.SetServerName( GetSockAddress() );
	m_req.SetServerPort( GetSockPort() );
}


void HttpBaseSocket::OnHeader(const std::string& key,const std::string& value)
{
DEB(fprintf(stderr, "  (request)OnHeader %s: %s\n", key.c_str(), value.c_str());)
	if (Utility::ToLower(key) == "cookie")
		m_req.AddCookie(value);
	else
		m_req.SetHeader(key, value);
}


void HttpBaseSocket::OnHeaderComplete()
{
	m_body_size_left = atol( m_req.Header("content-length").c_str() );
	if (m_body_size_left > 0)
	{
		m_req.InitBody( m_body_size_left );
	}
	else
	{
		// execute
		Execute();
	}
}


void HttpBaseSocket::OnData(const char *buf,size_t sz)
{
	m_req.Write( buf, sz );
	m_body_size_left -= sz;
	if (!m_body_size_left)
	{
		m_req.CloseBody();

		// execute
		Execute();
	}
}


// --------------------------------------------------------------------------------------
void HttpBaseSocket::Execute()
{
	// parse form data / query_string and cookie header if available
	m_req.ParseBody();

	// prepare page
	OnExec( m_req );

DEB(printf(" *** http version: %s\n", m_req.HttpVersion().c_str());
printf(" ***   connection: %s\n", m_req.Header("connection").c_str());)
	if ( !(m_req.HttpVersion().size() > 4 && m_req.HttpVersion().substr(m_req.HttpVersion().size() - 4) == "/1.1") ||
			m_req.Header("connection") == "close")
	{
		m_b_keepalive = false;
DEB(printf(" *** keepalive: false\n");)
	}
	else
	{
		m_b_keepalive = true;
DEB(printf(" *** keepalive: true\n");)
	}
	m_req.Reset();
	Reset();
}


// --------------------------------------------------------------------------------------
void HttpBaseSocket::Respond(const HttpResponse& res)
{
//	res.SetHeader("connection", "close");

	SetHttpVersion( res.HttpVersion() );
	SetStatus( Utility::l2string(res.HttpStatusCode()) );
	SetStatusText( res.HttpStatusMsg() );

	if (!ResponseHeaderIsSet("content-length"))
	{
		AddResponseHeader( "content-length", Utility::l2string( res.GetFile().size() ) );
	}
	for (std::map<std::string, std::string>::const_iterator it = res.Headers().begin(); it != res.Headers().end(); ++it)
	{
		AddResponseHeader( it -> first, it -> second );
	}
	std::list<std::string> vec = res.CookieNames();
	for (std::list<std::string>::iterator it2 = vec.begin(); it2 != vec.end(); it2++)
	{
		AppendResponseHeader( "set-cookie", res.Cookie(*it2) );
	}
	SendResponse();

	m_res_file = &res.GetFile();

	OnTransferLimit();
}


// --------------------------------------------------------------------------------------
void HttpBaseSocket::OnTransferLimit()
{
	char msg[32768];
	size_t n = m_res_file -> fread(msg, 1, 32768);
	while (n > 0)
	{
		SendBuf( msg, n );
		if (GetOutputLength() > 1)
		{
			SetTransferLimit( 1 );
			break;
		}
		n = m_res_file -> fread(msg, 1, 32768);
	}
	if (!GetOutputLength())
	{
		if (!m_b_keepalive)
		{
			SetCloseAndDelete();
		}
	}
}


// --------------------------------------------------------------------------------------
void HttpBaseSocket::Reset()
{
	HTTPSocket::Reset();
	m_body_size_left = 0;
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif


