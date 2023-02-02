/**
 **	File ......... HttpPostSocket.h
 **	Published ....  2004-10-30
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
#ifndef _HTTPPOSTSOCKET_H
#define _HTTPPOSTSOCKET_H

#include "HTTPSocket.h"


class SocketHandler;

class HttpPostSocket : public HTTPSocket
{
public:
	// client constructor, url = 'http://host:port/resource'
	HttpPostSocket(SocketHandler&,const std::string& url);
	~HttpPostSocket();

	// these must be specified before connecting / adding to handler
	void AddField(const std::string& name,const std::string& value);
	void AddMultilineField(const std::string& name,std::list<std::string>& values);
	void AddFile(const std::string& name,const std::string& filename,const std::string& type);

	// use this to post with content-type multipart/form-data
	// when adding a file to the post, this is the default and only content-type
	void SetMultipart();

	// connect to host:port derived from url in constructor
	void Open();

	// http put client implemented in OnConnect
	void OnConnect();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);


private:
	HttpPostSocket(const HttpPostSocket& s) : HTTPSocket(s) {} // copy constructor
	HttpPostSocket& operator=(const HttpPostSocket& ) { return *this; } // assignment operator
	void DoMultipartPost();
	//
	std::string m_host;
	port_t m_port;
	std::map<std::string,std::list<std::string> > m_fields;
	std::map<std::string,std::string> m_files;
	std::string m_boundary;
	std::map<std::string,long> m_content_length;
	std::map<std::string,std::string> m_content_type;
	bool m_bMultipart;
};




#endif // _HTTPPOSTSOCKET_H
