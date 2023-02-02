/*
 **	File ......... SocketHandler.h
 **	Published ....  2004-02-13
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
#ifndef _SOCKETHANDLER_H
#define _SOCKETHANDLER_H

#include <map>
#include <list>

#include "socket_include.h"
#include "StdLog.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class Socket;
class PoolSocket;
class ResolvServer;

/** Socket container class, event generator. 
	\ingroup basic */
class SocketHandler
{
	/** Map type for holding file descriptors/socket object pointers. */
	typedef std::map<SOCKET,Socket *> socket_m;

public:
	/** SocketHandler constructor.
		\param log Optional log class pointer */
	SocketHandler(StdLog *log = NULL);
	virtual ~SocketHandler();

	/** Register StdLog object for error callback. 
		\param log Pointer to log class */
	void RegStdLog(StdLog *log);
	/** Log error to log class for print out / storage. */
	void LogError(Socket *,const std::string&,int,const std::string&,loglevel_t = LOG_LEVEL_WARNING);

	/** Add socket instance to socket map. */
	void Add(Socket *);
	/** Set read/write/exception file descriptor sets (fd_set). */
	void Set(SOCKET s,bool bRead,bool bWrite,bool bException = true);
	/** Wait for events, generate callbacks. */
	int Select(long sec,long usec);
	/** Check that a socket really is handled by this socket handler. */
	bool Valid(Socket *);
	/** Override and return false to deny all incoming connections. */
	virtual bool OkToAccept();
	/** Get status of read/write/exception file descriptor set for a socket. */
	void Get(SOCKET s,bool& r,bool& w,bool& e);

	/** ResolveLocal (hostname) - call once before calling any GetLocal method. */
	void ResolveLocal();
	/** Returns local hostname, ResolveLocal must be called once before using.
		\sa ResolveLocal */
	const std::string& GetLocalHostname();
	/** Returns local ip, ResolveLocal must be called once before using.
		\sa ResolveLocal */
	ipaddr_t GetLocalIP();
	/** Returns local ip number as string.
		\sa ResolveLocal */
	const std::string& GetLocalAddress();
#ifdef IPPROTO_IPV6
	/** Returns local ipv6 ip.
		\sa ResolveLocal */
	const struct in6_addr& GetLocalIP6();
	/** Returns local ipv6 address.
		\sa ResolveLocal */
	const std::string& GetLocalAddress6();
#endif

	/** Return number of sockets handled by this handler.  */
	size_t GetCount();
	/** Indicates that the handler runs under SocketThread. */
	void SetSlave(bool x = true);
	/** Find available open connection (used by connection pool). */
	PoolSocket *FindConnection(int type,const std::string& protocol,ipaddr_t,port_t);
#ifdef IPPROTO_IPV6
	/** Find available open connection (used by connection pool). */
	PoolSocket *FindConnection(int type,const std::string& protocol,in6_addr,port_t);
#endif

	/** Set socks4 server ip that all new tcp sockets should use. */
	void SetSocks4Host(ipaddr_t);
	/** Set socks4 server hostname that all new tcp sockets should use. */
	void SetSocks4Host(const std::string& );
	/** Set socks4 server port number that all new tcp sockets should use. */
	void SetSocks4Port(port_t);
	/** Set optional socks4 userid. */
	void SetSocks4Userid(const std::string& );
	/** If connection to socks4 server fails, immediately try direct connection to final host. */
	void SetSocks4TryDirect(bool x = true);
	/** Get socks4 server ip. 
		\return socks4 server ip */
	ipaddr_t GetSocks4Host();
	/** Get socks4 port number.
		\return socks4 port number */
	port_t GetSocks4Port();
	/** Get socks4 userid (optional).
		\return socks4 userid */
	const std::string& GetSocks4Userid();
	/** Check status of socks4 try direct flag.
		\return true if direct connection should be tried if connection to socks4 server fails */
	bool Socks4TryDirect();

	/** Enable asynchronous DNS. 
		\param port Listen port of asynchronous dns server */
	void EnableResolver(port_t port = 16667);
	/** Check resolver status.
		\return true if resolver is enabled */
	bool ResolverEnabled();
	/** Queue a dns request. */
	int Resolve(Socket *,const std::string& host,port_t);
	/** Get listen port of asynchronous dns server. */
	port_t GetResolverPort();
	/** Resolver thread ready for queries. */
	bool ResolverReady();

#ifdef IPPROTO_IPV6
	/** ipv6 address compare. */
	int in6_addr_compare(in6_addr,in6_addr);
#endif
	/** Enable connection pool (by default disabled). */
	void EnablePool(bool x = true);
	/** Check pool status. 
		\return true if connection pool is enabled */
	bool PoolEnabled();

protected:
	socket_m m_sockets; ///< Active sockets list
	socket_m m_add; ///< Sockets to be added to sockets list
	std::list<Socket *> m_delete; ///< Sockets to be deleted (failed when Add)

private:
	SocketHandler(const SocketHandler& ) {}
	SocketHandler& operator=(const SocketHandler& ) { return *this; }
	StdLog *m_stdlog; ///< Registered log class, or NULL
	SOCKET m_maxsock; ///< Highest file descriptor + 1 in active sockets list
	std::string m_host; ///< local hostname
	ipaddr_t m_ip; ///< local ip address
	std::string m_addr; ///< local ip address in string format
	fd_set m_rfds; ///< file descriptor set monitored for read events
	fd_set m_wfds; ///< file descriptor set monitored for write events
	fd_set m_efds; ///< file descriptor set monitored for exceptions
#ifdef _WIN32
	int m_preverror; ///< debug select() error
#endif
	bool m_slave; ///< Indicates that this is a SocketHandler run in SocketThread
#ifdef IPPROTO_IPV6
	struct in6_addr m_local_ip6; ///< local ipv6 address
#endif
	std::string m_local_addr6; ///< local ipv6 address in string format
	bool m_local_resolved; ///< ResolveLocal has been called if true
	ipaddr_t m_socks4_host; ///< Socks4 server host ip
	port_t m_socks4_port; ///< Socks4 server port number
	std::string m_socks4_userid; ///< Socks4 userid
	bool m_bTryDirect; ///< Try direct connection if socks4 server fails
	int m_resolv_id; ///< Resolver id counter
	ResolvServer *m_resolver; ///< Resolver thread pointer
	port_t m_resolver_port; ///< Resolver listen port
	bool m_b_enable_pool; ///< Connection pool enabled if true
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETHANDLER_H
