/**
 **	File ......... HttpGetSocket.h
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
#ifndef _HTTPGETSOCKET_H
#define _HTTPGETSOCKET_H

#include "HTTPSocket.h"


class HttpGetSocket : public HTTPSocket
{
public:
	HttpGetSocket(SocketHandler&,const std::string& ,const std::string& = "");
	HttpGetSocket(SocketHandler&,const std::string&,port_t,const std::string&,const std::string&);
	~HttpGetSocket();

	void OnConnect();
	void OnContent();
	void OnDelete();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);

	bool Complete() { return m_bComplete; }

	size_t GetContentLength() { return m_content_length; }
	size_t GetPos() { return m_content_ptr; }

	void url_this(const std::string& url_in,std::string& host,port_t& port,std::string& url,std::string& file);

protected:
	HttpGetSocket(const HttpGetSocket& s) : HTTPSocket(s) {}
private:
	HttpGetSocket& operator=(const HttpGetSocket& ) { return *this; }
	std::string m_host;
	port_t m_port;
	std::string m_url;
	std::string m_to_file;
	//
	FILE *m_fil;
	bool m_bComplete;
	//
	size_t m_content_length;
	std::string m_content_type;
	size_t m_content_ptr;
};




#endif // _HTTPGETSOCKET_H
