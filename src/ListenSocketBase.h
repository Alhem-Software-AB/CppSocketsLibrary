/**
 **	File ......... ListenSocketBase.h
 **	Published ....  2005-03-06
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
#ifndef _LISTENSOCKETBASE_H
#define _LISTENSOCKETBASE_H

#ifdef _WIN32
#include <stdlib.h>
#else
#include <errno.h>
#endif

#include "Socket.h"


class ListenSocketBase : public Socket
{
public:
	ListenSocketBase(SocketHandler& h,Socket& );

	/** bind() to port 0 - a random port */
	int Bind();

	/** bind to port with optional listen queue length (depth) */
	int Bind(port_t port, int depth = 3);

	/** bind to port on a specified address */
	int Bind(const std::string& adapter, port_t port, int depth = 3);

	/** ipv6 bind to port with optional listen queue length (depth) */
#ifdef IPPROTO_IPV6
	int Bind6(port_t port, int depth = 3);
#endif

	/** ipv6 bind to port on a specified address */
#ifdef IPPROTO_IPV6
	int Bind6(const std::string& address, port_t port, int depth = 3);
#endif

	port_t GetPort();

	int GetDepth();

	void OnRead();
	Socket& GetBase() const { return m_base; }

protected:
	ListenSocketBase(const ListenSocketBase& s) : Socket(s),m_base(s.GetBase()) {}
private:
	ListenSocketBase& operator=(const ListenSocketBase& ) { return *this; }
	Socket& m_base;
	port_t m_port;
	int m_depth;
};


#endif // _LISTENSOCKETBASE_H
