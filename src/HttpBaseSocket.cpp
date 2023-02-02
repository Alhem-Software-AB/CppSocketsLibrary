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

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


HttpBaseSocket::HttpBaseSocket(ISocketHandler& h)
:HTTPSocket(h)
,m_res(m_req)
{
}


HttpBaseSocket::~HttpBaseSocket()
{
}


void HttpBaseSocket::OnFirst()
{
	m_req.SetHttpMethod( GetMethod() );
	m_req.SetUri( GetUri() );
	m_req.SetHttpVersion( GetHttpVersion() );

	m_req.SetAttribute("query_string", GetQueryString() );
}


void HttpBaseSocket::OnHeader(const std::string& key,const std::string& value)
{
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
		OnExec(m_req, m_res);
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
		OnExec(m_req, m_res);
	}
}


void HttpBaseSocket::Respond()
{
	m_res.SetHeader("connection", "close");
	SetHttpVersion( m_req.HttpVersion() );
	SetStatus( Utility::l2string(m_res.HttpStatusCode()) );
	SetStatusText( m_res.HttpStatusMsg() );

	AddResponseHeader( "content-length", Utility::l2string( m_res.GetFile().size() ) );
	for (std::map<std::string, std::string>::const_iterator it = m_res.Headers().begin(); it != m_res.Headers().end(); ++it)
	{
		AddResponseHeader( it -> first, it -> second );
	}
	SendResponse();

	{
		char msg[32768];
		size_t n = m_res.GetFile().fread(msg, 1, 32768);
		while (n > 0)
		{
			SendBuf( msg, n );
			if (GetOutputLength() > 1)
			{
				SetTransferLimit( 1 );
				break;
			}

			//
			n = m_res.GetFile().fread(msg, 1, 32768);
		}
		if (!GetOutputLength())
		{
			if (m_req.HttpVersion() == "HTTP/1.0" ||
					m_req.Header("connection") == "close")
			{
				SetCloseAndDelete();
			}
		}
	}
}


void HttpBaseSocket::OnTransferLimit()
{
	{
		char msg[32768];
		size_t n = m_res.GetFile().fread(msg, 1, 32768);
		while (n > 0)
		{
			SendBuf( msg, n );
			if (GetOutputLength() > 1)
			{
				SetTransferLimit( 1 );
				break;
			}

			//
			n = m_res.GetFile().fread(msg, 1, 32768);
		}
		if (!GetOutputLength())
		{
			if (m_req.HttpVersion() == "HTTP/1.0" ||
					m_req.Header("connection") == "close")
			{
				SetCloseAndDelete();
			}
		}
	}
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

