/*
 **	File ......... HTTPSocket.h
 **	Published ....  2004-04-06
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004,2005  Anders Hedstrom

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
#ifndef _HTTPSOCKET_H
#define _HTTPSOCKET_H

#include <map>
#include "TcpSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** \defgroup http HTTP Sockets */
/** HTTP request/response base class. 
	\ingroup http */
class HTTPSocket : public TcpSocket
{
	typedef std::map<std::string,std::string> string_m;
public:
	HTTPSocket(SocketHandler& );
	~HTTPSocket();

	void OnRead();
	void ReadLine();
	void OnLine(const std::string& line);

	/** Callback executes when first line has been received.
		GetMethod, GetUrl/GetUri, and GetHttpVersion are valid when this callback is executed. */
	virtual void OnFirst() = 0;
	/** For each header line this callback is executed.
		\param key Http header name
		\param value Http header value */
	virtual void OnHeader(const std::string& key,const std::string& value) = 0;
	/** Callback fires when all http headers have been received. */
	virtual void OnHeaderComplete() = 0;
	/** Chunk of http body data recevied. */
	virtual void OnData(const char *,size_t) = 0;

	const std::string& GetMethod();
	void SetMethod(const std::string& x);
	const std::string& GetUrl();
	void SetUrl(const std::string& x);
	const std::string& GetUri();
	void SetUri(const std::string& x);
	const std::string& GetQueryString();
	const std::string& GetHttpVersion();
	const std::string& GetStatus();
	const std::string& GetStatusText();
	bool IsRequest();
	bool IsResponse();

	void SetHttpVersion(const std::string& x);
	void SetStatus(const std::string& x);
	void SetStatusText(const std::string& x);
	void AddResponseHeader(const std::string& x,const std::string& y);
	void AddResponseHeader(const std::string& x,char *format, ...);
	void SendResponse();
	void SendRequest();

	/** Implement this to return your own User-agent string. */
	virtual std::string MyUseragent();

protected:
	HTTPSocket(const HTTPSocket& s) : TcpSocket(s) {}
	/** Reset state of socket to sucessfully implement keep-alive. */
	virtual void Reset();

private:
	HTTPSocket& operator=(const HTTPSocket& ) { return *this; }
	bool m_first;
	bool m_header;
	std::string m_line;
	std::string m_method;
	std::string m_url;
	std::string m_uri;
	std::string m_query_string;
	std::string m_http_version;
	std::string m_status;
	std::string m_status_text;
	bool m_request;
	bool m_response;
	string_m m_response_header;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _HTTPSOCKET_H
