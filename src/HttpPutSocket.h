/**
 **	File ......... HttpPutSocket.h
 **	Published ....  2004-10-30
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
#ifndef _HTTPPUTSOCKET_H
#define _HTTPPUTSOCKET_H

#include "HTTPSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class SocketHandler;

class HttpPutSocket : public HTTPSocket
{
public:
	// client constructor, url = 'http://host:port/resource'
	HttpPutSocket(SocketHandler&,const std::string& url);
	~HttpPutSocket();

	// these must be specified before connecting / adding to handler
	void SetFile(const std::string& );
	void SetContentType(const std::string& );

	// connect to host:port derived from url in constructor
	void Open();

	// http put client implemented in OnConnect
	void OnConnect();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);

private:
	HttpPutSocket(const HttpPutSocket& s) : HTTPSocket(s) {} // copy constructor
	HttpPutSocket& operator=(const HttpPutSocket& ) { return *this; } // assignment operator
	//
	std::string m_filename;
	std::string m_content_type;
	long m_content_length;
	std::string m_host;
	port_t m_port;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _HTTPPUTSOCKET_H
