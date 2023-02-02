/**
 **	File ......... HttpDebugSocket.h
 **	Published ....  2004-09-27
**/
/*
Copyright (C) 2004  Anders Hedström (grymse@alhem.net)

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
#ifndef _HttpDebugSocket_H
#define _HttpDebugSocket_H

#include "HTTPSocket.h"

class SocketHandler;

class HttpDebugSocket : public HTTPSocket
{
public:
	HttpDebugSocket(SocketHandler&);
	~HttpDebugSocket();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);

private:
	int m_content_length;
	int m_read_ptr;
};


#endif // _HttpDebugSocket_H
