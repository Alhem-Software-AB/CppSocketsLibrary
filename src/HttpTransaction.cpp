/**
 **	\file HttpTransaction.cpp
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
#include "HttpTransaction.h"
#include "Utility.h"
#include "Parse.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// --------------------------------------------------------------------------------------
HttpTransaction::HttpTransaction()
{
}


// --------------------------------------------------------------------------------------
HttpTransaction::~HttpTransaction()
{
}


// --------------------------------------------------------------------------------------
void HttpTransaction::SetHeader(const std::string& key, const std::string& value)
{
	m_header[Utility::ToLower(key)] = value;
}


void HttpTransaction::SetHeader(const std::string& key, long value)
{
	m_header[Utility::ToLower(key)] = Utility::l2string(value);
}


const std::string& HttpTransaction::Header(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it;
	if ((it = m_header.find(Utility::ToLower(key))) != m_header.end())
		return it -> second;
	return m_null;
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetAccept(const std::string& value)
{
	SetHeader("accept", value);
}


const std::string& HttpTransaction::Accept() const
{
	return Header("accept");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetAcceptCharset(const std::string& value)
{
	SetHeader("accept-charset", value);
}


const std::string& HttpTransaction::AcceptCharset() const
{
	return Header("accept-charset");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetAcceptEncoding(const std::string& value)
{
	SetHeader("accept-encoding", value);
}


const std::string& HttpTransaction::AcceptEncoding() const
{
	return Header("accept-encoding");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetAcceptLanguage(const std::string& value)
{
	SetHeader("accept-language", value);
}


const std::string& HttpTransaction::AcceptLanguage() const
{
	return Header("accept-language");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetConnection(const std::string& value)
{
	SetHeader("connection", value);
}


const std::string& HttpTransaction::Connection() const
{
	return Header("connection");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetContentType(const std::string& value)
{
	SetHeader("content-type", value);
}


const std::string& HttpTransaction::ContentType() const
{
	return Header("content-type");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetContentLength(long value)
{
	SetHeader("content-length", value );
}


long HttpTransaction::ContentLength() const
{
	return atol(Header("content-length").c_str());
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetHost(const std::string& value)
{
	SetHeader("host", value);
}


const std::string& HttpTransaction::Host() const
{
	return Header("host");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetPragma(const std::string& value)
{
	SetHeader("pragma", value);
}


const std::string& HttpTransaction::Pragma() const
{
	return Header("pragma");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetReferer(const std::string& value)
{
	SetHeader("referer", value);
}


const std::string& HttpTransaction::Referer() const
{
	return Header("referer");
}



// --------------------------------------------------------------------------------------
void HttpTransaction::SetUserAgent(const std::string& value)
{
	SetHeader("user-agent", value);
}


const std::string& HttpTransaction::UserAgent() const
{
	return Header("user-agent");
}


// --------------------------------------------------------------------------------------
const std::map<std::string, std::string>& HttpTransaction::Headers() const
{
	return m_header;
}


// --------------------------------------------------------------------------------------
void HttpTransaction::Reset()
{
	while (!m_header.empty())
	{
		m_header.erase(m_header.begin());
	}
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif


