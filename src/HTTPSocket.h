/**
 **	File ......... HTTPSocket.h
 **	Published ....  2004-04-06
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
#ifndef _HTTPSOCKET_H
#define _HTTPSOCKET_H

#include <map>
#include <string>
#include "TcpSocket.h"


class HTTPSocket : public TcpSocket
{
	typedef std::map<std::string,std::string> string_m;
public:
	HTTPSocket(SocketHandler& );
	~HTTPSocket();

	void OnRead();
	void ReadLine();
	void OnLine(const std::string& );

	virtual void OnFirst() = 0;
	virtual void OnHeader(const std::string& ,const std::string& ) = 0;
	virtual void OnHeaderComplete() = 0;
	virtual void OnData(const char *,size_t) = 0;

	const std::string& GetMethod() { return m_method; }
	const std::string& GetUrl() { return m_url; }
	const std::string& GetHttpVersion() { return m_http_version; }
	const std::string& GetStatus() { return m_status; }
	const std::string& GetStatusText() { return m_status_text; }
	bool IsRequest() { return m_request; }
	bool IsResponse() { return m_response; }

	void SetMethod(const std::string& x) { m_method = x; }
	void SetStatus(const std::string& x) { m_status = x; }
	void SetStatusText(const std::string& x) { m_status_text = x; }
	void AddResponseHeader(const std::string& x,const std::string& y) { m_response_header[x] = y; }
	void SendResponse();

private:
	bool m_first;
	bool m_header;
	std::string m_line;
	std::string m_method;
	std::string m_url;
	std::string m_http_version;
	std::string m_status;
	std::string m_status_text;
	bool m_request;
	bool m_response;
	string_m m_response_header;
};




#endif // _HTTPSOCKET_H
