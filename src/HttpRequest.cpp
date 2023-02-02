/**
 **	\file HttpRequest.cpp
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
#include "HttpRequest.h"
#include "Utility.h"
#include "MemFile.h"
#include "HttpdForm.h"
#include "HttpdCookies.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// --------------------------------------------------------------------------------------
HttpRequest::HttpRequest() : HttpTransaction()
, m_server_port(0)
, m_is_ssl(false)
, m_body_file(NULL)
, m_form(NULL)
, m_cookies(NULL)
{
}


// --------------------------------------------------------------------------------------
HttpRequest::~HttpRequest()
{
	if (m_body_file)
	{
		delete m_body_file;
	}
	if (m_form)
	{
		delete m_form;
	}
	if (m_cookies)
	{
		delete m_cookies;
	}
}


// --------------------------------------------------------------------------------------
HttpRequest::HttpRequest(const HttpRequest&)
{
}


// --------------------------------------------------------------------------------------
HttpRequest& HttpRequest::operator=(const HttpRequest&)
{
	return *this;
}


// --------------------------------------------------------------------------------------
void HttpRequest::SetHttpMethod(const std::string& value)
{
	m_method = value;
}


const std::string& HttpRequest::HttpMethod() const
{
	return m_method;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetHttpVersion(const std::string& value)
{
	m_protocol = value;
}


const std::string& HttpRequest::HttpVersion() const
{
	return m_protocol;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetUri(const std::string& value)
{
	m_req_uri = Utility::rfc1738_decode(value);
}


const std::string& HttpRequest::Uri() const
{
	return m_req_uri;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetRemoteAddr(const std::string& value)
{
	m_remote_addr = value;
}


const std::string& HttpRequest::RemoteAddr() const
{
	return m_remote_addr;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetRemoteHost(const std::string& value)
{
	m_remote_host = value;
}


const std::string& HttpRequest::RemoteHost() const
{
	return m_remote_host;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetServerName(const std::string& value)
{
	m_server_name = value;
}


const std::string& HttpRequest::ServerName() const
{
	return m_server_name;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetServerPort(int value)
{
	m_server_port = value;
}


int HttpRequest::ServerPort() const
{
	return m_server_port;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetIsSsl(bool value)
{
	m_is_ssl = value;
}


bool HttpRequest::IsSsl() const
{
	return m_is_ssl;
}



// --------------------------------------------------------------------------------------
void HttpRequest::SetAttribute(const std::string& key, const std::string& value)
{
	m_attribute[Utility::ToLower(key)] = value;
}


void HttpRequest::SetAttribute(const std::string& key, long value)
{
	m_attribute[Utility::ToLower(key)] = Utility::l2string(value);
}


const std::string& HttpRequest::Attribute(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it;
	if ( (it = m_attribute.find(Utility::ToLower(key))) != m_attribute.end())
		return it -> second;
	return m_null;
}


// --------------------------------------------------------------------------------------
const std::map<std::string, std::string>& HttpRequest::Attributes() const
{
	return m_attribute;
}


// --------------------------------------------------------------------------------------
void HttpRequest::InitBody( size_t sz )
{
	if (!m_body_file)
		m_body_file = new MemFile;
	else
		fprintf(stderr, "Body data file already opened\n");
}


// --------------------------------------------------------------------------------------
void HttpRequest::Write( const char *buf, size_t sz )
{
	if (m_body_file)
		m_body_file -> fwrite(buf, 1, sz);
	else
		fprintf(stderr, "Write: Body data file not open\n");
}


// --------------------------------------------------------------------------------------
void HttpRequest::CloseBody()
{
	if (m_body_file)
		m_body_file -> fclose();
	else
		fprintf(stderr, "CloseBody: File not open\n");
}


// --------------------------------------------------------------------------------------
void HttpRequest::ParseBody()
{
	std::map<std::string, std::string>::const_iterator it;
	if ( (it = m_attribute.find("query_string")) != m_attribute.end())
	{
		std::string qs = it -> second;
		m_form = new HttpdForm( qs, qs.size() );
	}
	else
	if (m_body_file)
	{
		m_form = new HttpdForm( m_body_file, ContentType(), ContentLength() );
	}
	else
	{
		// dummy
		m_form = new HttpdForm( "", 0 );
	}
	m_cookies = new HttpdCookies( Cookie() );
}


// --------------------------------------------------------------------------------------
const HttpdForm& HttpRequest::Form() const
{
	return *m_form;
}


// --------------------------------------------------------------------------------------
const HttpdCookies& HttpRequest::Cookies() const
{
	return *m_cookies;
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

