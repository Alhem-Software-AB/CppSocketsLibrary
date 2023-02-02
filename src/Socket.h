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
#include <vector>
#include <list>

#include "socket_include.h"
#include <time.h>


class SocketHandler;
class SocketThread;


//	typedef std::list<std::string> string_v;

class Socket
{
	friend class SocketHandler;
public:
	Socket(SocketHandler&);
	virtual ~Socket();

	virtual Socket *Create() { return NULL; }
	virtual void Init();

	void Attach(SOCKET s);
	SOCKET GetSocket();
	virtual int Close();
	SOCKET CreateSocket4(int type,const std::string& protocol = "");
	SOCKET CreateSocket6(int type,const std::string& protocol = "");
	void Set(bool bRead,bool bWrite,bool bException = true);
	bool Ready();

	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnException();
	virtual void OnDelete();
	virtual void OnConnect();
	virtual void OnAccept();
	virtual void OnLine(const std::string& );
	virtual void OnSSLInitDone();
	virtual void OnConnectFailed();
	virtual void OnOptions(int family,int type,int protocol,SOCKET);
	virtual void OnSocks4Connect();
	virtual void OnSocks4ConnectFailed();
	virtual bool OnSocks4Read();

	virtual bool CheckConnect();
	virtual void ReadLine();
	virtual bool SSLCheckConnect();

	void SetSSLConnecting(bool = true);
	bool SSLConnecting();
	void SetLineProtocol(bool = true);
	bool LineProtocol();
	void SetDeleteByHandler(bool = true);
	bool DeleteByHandler();
	void SetCloseAndDelete(bool = true);
	bool CloseAndDelete();
	void SetConnecting(bool = true);
	bool Connecting();
	time_t GetConnectTime();

	/** ipv4 and ipv6 */
	bool isip(const std::string&);
	/** ipv4 */
	bool u2ip(const std::string&, ipaddr_t&);
	/** ipv6 */
#ifdef IPPROTO_IPV6
	bool u2ip(const std::string&, struct in6_addr&);
#endif
	/** ipv4 */
	void l2ip(const ipaddr_t,std::string& );
	/** ipv6 */
#ifdef IPPROTO_IPV6
	void l2ip(const struct in6_addr&,std::string& ,bool mixed = false);
#endif

	/** ipv4 and ipv6 */
	void SetRemoteAddress(struct sockaddr* sa,socklen_t);
	void GetRemoteSocketAddress(struct sockaddr& sa,socklen_t& sa_len);
	/** ipv4 */
	ipaddr_t GetRemoteIP4();
	/** ipv6 */
#ifdef IPPROTO_IPV6
	struct in6_addr GetRemoteIP6();
#endif
	/** ipv4 and ipv6 */
	port_t GetRemotePort();
	/** ipv4 and ipv6 */
	std::string GetRemoteAddress();
	/** ipv4 and ipv6(not implemented) */
	std::string GetRemoteHostname();

	SocketHandler& Handler() const;
	bool SetNonblocking(bool);
	bool SetNonblocking(bool, SOCKET);

	time_t Uptime() { return time(NULL) - m_tCreate; }

/*
	void SetTimeout(time_t x) { m_timeout = x; }
	time_t Timeout() { return m_timeout; }
	void Touch() { m_tActive = time(NULL); }
	time_t Inactive() { return time(NULL) - m_tActive; }
*/
	virtual void OnDetached() {} // Threading
	void SetDetach(bool x = true) { m_detach = x; }
	bool IsDetach() { return m_detach; }
	void SetDetached(bool x = true) { m_detached = x; }
	bool IsDetached() { return m_detached; }
	bool Detach();

	void SetIpv6(bool x = true) { m_ipv6 = x; }
	bool IsIpv6() { return m_ipv6; }

	Socket *GetParent();
	void SetParent(Socket *);
	virtual port_t GetPort();

	// pooling
	void SetIsClient() { m_bClient = true; }
	void SetSocketType(int x) { m_socket_type = x; }
	int GetSocketType() { return m_socket_type; }
	void SetSocketProtocol(const std::string& x) { m_socket_protocol = x; }
	const std::string& GetSocketProtocol() { return m_socket_protocol; }
	void SetClientRemoteAddr(ipaddr_t a) { m_client_remote_addr = a; }
	ipaddr_t& GetClientRemoteAddr() { return m_client_remote_addr; }
	void SetClientRemotePort(port_t p) { m_client_remote_port = p; }
	port_t GetClientRemotePort() { return m_client_remote_port; }
	void SetRetain() { if (m_bClient) m_bRetain = true; }
	bool Retain() { return m_bRetain; }
	void SetLost() { m_bLost = true; }
	bool Lost() { return m_bLost; }
	void SetCallOnConnect(bool x = true) { m_call_on_connect = x; }
	bool CallOnConnect() { return m_call_on_connect; }

	// copy connection parameters from sock
	void CopyConnection(Socket *sock);

	void SetReuse(bool x) { m_opt_reuse = x; }
	void SetKeepalive(bool x) { m_opt_keepalive = x; }

	// dns
	int Resolve(const std::string& host,port_t port);
	virtual void Resolved(int id,ipaddr_t,port_t) {}

	/** socket still in socks4 negotiation mode */
	bool Socks4() { return m_bSocks4; }
	void SetSocks4(bool x = true) { m_bSocks4 = x; }

	void SetSocks4Host(ipaddr_t a) { m_socks4_host = a; }
	void SetSocks4Host(const std::string& );
	void SetSocks4Port(port_t p) { m_socks4_port = p; }
	void SetSocks4Userid(const std::string& x) { m_socks4_userid = x; }
	ipaddr_t GetSocks4Host() { return m_socks4_host; }
	port_t GetSocks4Port() { return m_socks4_port; }
	const std::string& GetSocks4Userid() { return m_socks4_userid; }

protected:
	Socket(const Socket& ); // do not allow use of copy constructor
	void DetachSocket(); // protected, friend class SocketHandler;

private:
#ifdef _WIN32
static	WSAInitializer m_winsock_init;
#endif
	Socket& operator=(const Socket& ) { return *this; }
	//
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
	bool m_detach;
	bool m_detached;
	SocketThread *m_pThread;
	bool m_ipv6;
	struct sockaddr m_sa; // remote, from accept
	socklen_t m_sa_len;
	Socket *m_parent;
	// pooling, ipv4
	int m_socket_type;
	std::string m_socket_protocol;
	bool m_bClient; // only client connections are pooled
	ipaddr_t m_client_remote_addr;
	port_t m_client_remote_port;
	bool m_bRetain; // keep connection on close
	bool m_bLost; // connection lost
	bool m_call_on_connect;
	bool m_opt_reuse;
	bool m_opt_keepalive;
	bool m_bSocks4; // socks4 negotiation mode (TcpSocket)
	ipaddr_t m_socks4_host;
	port_t m_socks4_port;
	std::string m_socks4_userid;
};


#endif // _SOCKETBASE_H
