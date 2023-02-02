/**
 **	File ......... Socket.h
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
#ifndef _SOCKETBASE_H
#define _SOCKETBASE_H

#include <string>
#include <time.h>
#include <vector>
#include "socket_include.h"


class SocketHandler;

	typedef std::vector<std::string> string_v;

class Socket
{
public:
	Socket(SocketHandler&);
	virtual ~Socket();

	virtual void Init();

	void Attach(SOCKET s);
	SOCKET GetSocket();
	virtual int Close();
	SOCKET CreateSocket(int type);
	void Set(bool bRead,bool bWrite,bool bException = true);
	bool Ready();

	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnException();
	virtual void OnDelete();
	virtual void OnConnect();
	virtual void OnAccept();
//	virtual void OnCallback(int );
	virtual void OnLine(const std::string& ) {}
	virtual void OnSSLInitDone() {}

	virtual bool CheckConnect();
	virtual void ReadLine() {}
	virtual bool SSLCheckConnect() { return false; }

	void SetSSLConnecting(bool x = true) { m_ssl_connecting = x; }
	bool SSLConnecting() { return m_ssl_connecting; }
	void SetLineProtocol(bool x = true) { m_line_protocol = x; }
	bool LineProtocol() { return m_line_protocol; }
	void SetDeleteByHandler(bool x = true);
	bool DeleteByHandler();
	void SetCloseAndDelete(bool x = true);
	bool CloseAndDelete();
	void SetConnecting(bool x = true);
	bool Connecting();
	time_t GetConnectTime();

	bool isip(const std::string&);
	bool u2ip(const std::string&, ipaddr_t&);
	void l2ip(ipaddr_t,std::string& );

	void SetRemoteAddress(struct sockaddr* sa,socklen_t);
	ipaddr_t GetRemoteIP();
	port_t GetRemotePort();
	std::string GetRemoteAddress();
	std::string GetRemoteHostname();

	SocketHandler& Handler();
	bool SetNonblocking(bool);
	bool SetNonblocking(bool, SOCKET);

	time_t Uptime() { return time(NULL) - m_tCreate; }

/*
	void SetTimeout(time_t x) { m_timeout = x; }
	time_t Timeout() { return m_timeout; }
	void Touch() { m_tActive = time(NULL); }
	time_t Inactive() { return time(NULL) - m_tActive; }
*/

protected:
	struct sockaddr m_sa; // remote, from accept

private:
	SocketHandler& m_handler;
	SOCKET m_socket;
	bool m_bDel;
	bool m_bClose;
	bool m_bConnecting;
	time_t m_tConnect;
	time_t m_tCreate;
	bool m_line_protocol;
	bool m_ssl_connecting;
//	time_t m_tActive;
//	time_t m_timeout;
};




#endif // _SOCKETBASE_H
