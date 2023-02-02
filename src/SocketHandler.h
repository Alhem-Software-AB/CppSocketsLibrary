/**
 **	File ......... SocketHandler.h
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
#ifndef _SOCKETHANDLER_H
#define _SOCKETHANDLER_H

#include <vector>
#include <map>
#include <string>

#include "socket_include.h"


class Socket;




class SocketHandler
{
  typedef std::map<int,Socket *> socket_m;

public:
	SocketHandler();
	virtual ~SocketHandler();

	void Add(Socket *);
	void Set(SOCKET s,bool bRead,bool bWrite,bool bException = true);
	int Select(long sec,long usec);
	void StatLoop(long s,long us);
	bool Valid(Socket *);
	virtual bool OkToAccept() { return true; }

	const std::string& GetLocalHostname();
	ipaddr_t GetLocalIP();
	const std::string& GetLocalAddress();

/*
	template <class X>
	void DoCallback(int code) {
		for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
		{
			Socket *p = (*it).second;
			X *p2 = dynamic_cast<X *>(p);
			if (p2)
			{
				p2 -> OnCallback( code );
			}
		}
	}
	template <class X>
	bool Exists() {
		for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
		{
			Socket *p = (*it).second;
			X *p2 = dynamic_cast<X *>(p);
			if (p2)
			{
				return true;
			}
		}
		return false;
	}
	template <class X>
	int Count() {
		int q = 0;
		for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
		{
			Socket *p = (*it).second;
			X *p2 = dynamic_cast<X *>(p);
			if (p2)
			{
				q++;
			}
		}
		return q;
	}
*/

	size_t GetCount() { return m_sockets.size(); }
	void SetSlave(bool x = true) { m_slave = x; }

protected:
	socket_m m_sockets;
	socket_m m_add;

private:
	int m_maxsock;
	std::string m_host;
	ipaddr_t m_ip;
	std::string m_addr;
	fd_set m_rfds;
	fd_set m_wfds;
	fd_set m_efds;
	int m_preverror;
	bool m_slave;
};




#endif // _SOCKETHANDLER_H
