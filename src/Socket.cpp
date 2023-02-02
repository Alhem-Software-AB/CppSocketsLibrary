/** \file Socket.cpp
 **	\date  2004-02-13
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004-2006  Anders Hedstrom

This library is made available under the terms of the GNU GPL.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

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
#ifdef _WIN32
#pragma warning(disable:4786)
#include <stdlib.h>
#else
#include <errno.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include "ISocketHandler.h"
#include "SocketThread.h"
#include "Utility.h"

#include "Socket.h"
#include "TcpSocket.h"
#include "SocketAddress.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// statics
#ifdef _WIN32
WSAInitializer Socket::m_winsock_init;
#endif


Socket::Socket(ISocketHandler& h)
:m_handler(h)
,m_socket( INVALID_SOCKET )
,m_bDel(false)
,m_bClose(false)
,m_bConnecting(false)
,m_tCreate(time(NULL))
,m_line_protocol(false)
,m_ssl_connecting(false)
,m_detach(false)
,m_detached(false)
,m_pThread(NULL)
,m_ipv6(false)
,m_parent(NULL)
,m_socket_type(0)
,m_bClient(false)
,m_bRetain(false)
,m_bLost(false)
,m_call_on_connect(false)
,m_opt_reuse(true)
,m_opt_keepalive(true)
,m_bSocks4(false)
,m_socks4_host(h.GetSocks4Host())
,m_socks4_port(h.GetSocks4Port())
,m_socks4_userid(h.GetSocks4Userid())
,m_connect_timeout(5)
,m_b_enable_ssl(false)
,m_b_ssl(false)
,m_b_ssl_server(false)
,m_b_disable_read(false)
,m_b_retry_connect(false)
,m_connected(false)
,m_flush_before_close(true)
,m_connection_retry(0)
,m_retries(0)
,m_b_erased_by_handler(false)
,m_slave_handler(NULL)
,m_tClose(0)
,m_shutdown(0)
,m_client_remote_address(NULL)
,m_remote_address(NULL)
{
}


Socket::~Socket()
{
	Handler().Remove(this);
	if (m_socket != INVALID_SOCKET && !m_bRetain)
	{
		Close();
	}
	if (m_client_remote_address)
	{
		delete m_client_remote_address;
	}
	if (m_remote_address)
	{
		delete m_remote_address;
	}
/*
	// SocketThread will delete itself
	if (m_pThread)
	{
		delete m_pThread;
	}
*/
}


void Socket::Init()
{
}


void Socket::OnRead()
{
}


void Socket::OnWrite()
{
}


void Socket::OnException()
{
#ifdef _WIN32
	if (Connecting())
	{
		if (Socks4())
			OnSocks4ConnectFailed();
		else
		if (GetConnectionRetry() == -1 ||
			(GetConnectionRetry() &&
			 GetConnectionRetries() < GetConnectionRetry() ))
		{
			// even though the connection failed at once, only retry after
			// the connection timeout
			// should we even try to connect again, when CheckConnect returns
			// false it's because of a connection error - not a timeout...
		}
		else
		{
			SetConnecting(false); // tnx snibbe
			SetCloseAndDelete();
			OnConnectFailed();
		}
		return;
	}
#endif
	// errno valid here?
	int err;
	socklen_t errlen = sizeof(err);
#ifdef _WIN32
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char *)&err, &errlen);
#else
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &errlen);
#endif
	Handler().LogError(this, "exception on select", err, StrError(err), LOG_LEVEL_FATAL);
	SetCloseAndDelete();
}


void Socket::OnDelete()
{
}


void Socket::OnConnect()
{
}


bool Socket::CheckConnect()
{
	int err;
	socklen_t errlen = sizeof(err);
	bool r = true;
#ifdef _WIN32
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char *)&err, &errlen);
#else
	getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &errlen);
#endif
	if (err)
	{
		Handler().LogError(this, "connect failed", err, StrError(err), LOG_LEVEL_FATAL);
		r = false;
	}
	// don't reset connecting flag on error here, we want the OnConnectFailed timeout later on
	/// \todo add to read fd_set here
	if (r) // ok
	{
		Set(!IsDisableRead(), false);
		SetConnecting(false);
	}
	else
	{
		Set(false, false); // no more monitoring because connection failed
	}
	return r;
}


void Socket::OnAccept()
{
}


int Socket::Close()
{
	if (m_socket == INVALID_SOCKET) // this could happen
	{
		Handler().LogError(this, "Socket::Close", 0, "file descriptor invalid", LOG_LEVEL_WARNING);
		return 0;
	}
	int n;
	SetNonblocking(true);
	if (IsConnected())
	{
		if (shutdown(m_socket, SHUT_WR) == -1)
		{
			// failed...
			Handler().LogError(this, "shutdown", Errno, StrError(Errno), LOG_LEVEL_ERROR);
		}
	}
	//
	char tmp[100];
	if ((n = recv(m_socket,tmp,100,0)) == -1)
	{
//		Handler().LogError(this, "read() after shutdown", Errno, StrError(Errno), LOG_LEVEL_WARNING);
	}
	else
	{
		if (n)
		{
			Handler().LogError(this, "read() after shutdown", n, "bytes read", LOG_LEVEL_WARNING);
		}
	}
	if ((n = closesocket(m_socket)) == -1)
	{
		// failed...
		Handler().LogError(this, "close", Errno, StrError(Errno), LOG_LEVEL_ERROR);
	}
	Set(false, false, false); // remove from fd_set's
	Handler().AddList(m_socket, LIST_CALLONCONNECT, false);
	Handler().AddList(m_socket, LIST_DETACH, false);
	Handler().AddList(m_socket, LIST_CONNECTING, false);
	Handler().AddList(m_socket, LIST_RETRY, false);
	Handler().AddList(m_socket, LIST_CLOSE, false);
	m_socket = INVALID_SOCKET;
	return n;
}


SOCKET Socket::CreateSocket(int af,int type, const std::string& protocol)
{
	struct protoent *p = NULL;
	int optval;
	SOCKET s;

	m_socket_type = type;
	m_socket_protocol = protocol;
	if (protocol.size())
	{
		p = getprotobyname( protocol.c_str() );
		if (!p)
		{
			Handler().LogError(this, "getprotobyname", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			SetCloseAndDelete();
			return INVALID_SOCKET;
		}
	}
	int protno = p ? p -> p_proto : 0;

	s = socket(af, type, protno);
	if (s == INVALID_SOCKET)
	{
		Handler().LogError(this, "socket", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return INVALID_SOCKET;
	}
	OnOptions(af, type, protno, s);
#ifdef SO_NOSIGPIPE
	{
		optval = 1;
		if (setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (char *)&optval, sizeof(optval)) == -1)
		{
			Handler().LogError(this, "setsockopt(SOL_SOCKET, SO_NOSIGPIPE)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			SetCloseAndDelete();
			return INVALID_SOCKET;
		}
	}
#endif
	if (type == SOCK_STREAM)
	{
		optval = m_opt_reuse ? 1 : 0;
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) == -1)
		{
			Handler().LogError(this, "setsockopt(SOL_SOCKET, SO_REUSEADDR)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			SetCloseAndDelete();
			return INVALID_SOCKET;
		}

		optval = m_opt_keepalive ? 1 : 0;
		if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) == -1)
		{
			Handler().LogError(this, "setsockopt(SOL_SOCKET, SO_KEEPALIVE)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			closesocket(s);
			SetCloseAndDelete();
			return INVALID_SOCKET;
		}
	}
	return s;
}


void Socket::Attach(SOCKET s)
{
	m_socket = s;
}


SOCKET Socket::GetSocket()
{
	return m_socket;
}


void Socket::SetDeleteByHandler(bool x)
{
	m_bDel = x;
}


bool Socket::DeleteByHandler()
{
	return m_bDel;
}


void Socket::SetCloseAndDelete(bool x)
{
	if (x != m_bClose)
	{
		Handler().AddList(m_socket, LIST_CLOSE, x);
		m_bClose = x;
		if (x)
		{
			m_tClose = time(NULL);
		}
	}
}


bool Socket::CloseAndDelete()
{
	return m_bClose;
}


void Socket::SetConnecting(bool x)
{
	if (x != m_bConnecting)
	{
		Handler().AddList(m_socket, LIST_CONNECTING, x);
		m_bConnecting = x;
		if (x)
		{
			m_tConnect = time(NULL);
		}
	}
}


bool Socket::Connecting()
{
	return m_bConnecting;
}


void Socket::SetRemoteAddress(SocketAddress& ad) //struct sockaddr* sa, socklen_t l)
{
	if (m_remote_address)
	{
		delete m_remote_address;
	}
	m_remote_address = ad.GetCopy();
}


SocketAddress *Socket::GetRemoteSocketAddress()
{
	return m_remote_address;
}


ISocketHandler& Socket::Handler() const
{
	if (IsDetached())
		return *m_slave_handler;
	return m_handler;
}


ISocketHandler& Socket::MasterHandler() const
{
	return m_handler;
}


ipaddr_t Socket::GetRemoteIP4()
{
	ipaddr_t l = 0;
	if (m_ipv6)
	{
		Handler().LogError(this, "GetRemoteIP4", 0, "get ipv4 address for ipv6 socket", LOG_LEVEL_WARNING);
	}
	if (m_remote_address)
	{
		struct sockaddr *p = *m_remote_address;
		struct sockaddr_in *sa = (struct sockaddr_in *)p;
		memcpy(&l, &sa -> sin_addr, sizeof(struct in_addr));
	}
	return l;
}


#ifdef IPPROTO_IPV6
struct in6_addr Socket::GetRemoteIP6()
{
	if (!m_ipv6)
	{
		Handler().LogError(this, "GetRemoteIP6", 0, "get ipv6 address for ipv4 socket", LOG_LEVEL_WARNING);
	}
	struct sockaddr_in6 fail;
	if (m_remote_address)
	{
		struct sockaddr *p = *m_remote_address;
		memcpy(&fail, p, sizeof(struct sockaddr_in6));
	}
	else
	{
		memset(&fail, 0, sizeof(struct sockaddr_in6));
	}
	return fail.sin6_addr;
}
#endif


port_t Socket::GetRemotePort()
{
	if (!m_remote_address)
	{
		return 0;
	}
	return m_remote_address -> GetPort();
}


std::string Socket::GetRemoteAddress()
{
	if (!m_remote_address)
	{
		return "";
	}
	return m_remote_address -> Convert(false);
}


std::string Socket::GetRemoteHostname()
{
	if (!m_remote_address)
	{
		return "";
	}
	return m_remote_address -> Reverse();
}


bool Socket::SetNonblocking(bool bNb)
{
#ifdef _WIN32
	unsigned long l = bNb ? 1 : 0;
	int n = ioctlsocket(m_socket, FIONBIO, &l);
	if (n != 0)
	{
		Handler().LogError(this, "ioctlsocket(FIONBIO)", Errno, "");
		return false;
	}
	return true;
#else
	if (bNb)
	{
		if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
		{
			Handler().LogError(this, "fcntl(F_SETFL, O_NONBLOCK)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	else
	{
		if (fcntl(m_socket, F_SETFL, 0) == -1)
		{
			Handler().LogError(this, "fcntl(F_SETFL, 0)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	return true;
#endif
}


bool Socket::SetNonblocking(bool bNb, SOCKET s)
{
#ifdef _WIN32
	unsigned long l = bNb ? 1 : 0;
	int n = ioctlsocket(s, FIONBIO, &l);
	if (n != 0)
	{
		Handler().LogError(this, "ioctlsocket(FIONBIO)", Errno, "");
		return false;
	}
	return true;
#else
	if (bNb)
	{
		if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)
		{
			Handler().LogError(this, "fcntl(F_SETFL, O_NONBLOCK)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	else
	{
		if (fcntl(s, F_SETFL, 0) == -1)
		{
			Handler().LogError(this, "fcntl(F_SETFL, 0)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	return true;
#endif
}


void Socket::Set(bool bRead, bool bWrite, bool bException)
{
	Handler().Set(m_socket, bRead, bWrite, bException);
}


time_t Socket::GetConnectTime()
{
	return time(NULL) - m_tConnect;
}


bool Socket::Ready()
{
	if (m_socket != INVALID_SOCKET && !Connecting() && !CloseAndDelete())
		return true;
	return false;
}


bool Socket::Detach()
{
	if (!DeleteByHandler())
		return false;
	if (m_pThread)
		return false;
	if (m_detached)
		return false;
	SetDetach();
	return true;
}


void Socket::DetachSocket()
{
	SetDetached();
	m_pThread = new SocketThread(this);
	m_pThread -> SetRelease(true);
}


void Socket::OnLine(const std::string& )
{
}


void Socket::SetLineProtocol(bool x)
{
	m_line_protocol = x;
}


bool Socket::LineProtocol()
{
	return m_line_protocol;
}


/*
void Socket::ReadLine()
{
}
*/


void Socket::OnConnectFailed()
{
}


Socket::Socket(const Socket& s) : m_handler(s.Handler())
{
}


Socket *Socket::GetParent()
{
	return m_parent;
}


void Socket::SetParent(Socket *x)
{
	m_parent = x;
}


port_t Socket::GetPort()
{
	Handler().LogError(this, "GetPort", 0, "GetPort only implemented for ListenSocket", LOG_LEVEL_WARNING);
	return 0;
}


void Socket::CopyConnection(Socket *sock)
{
	Attach( sock -> GetSocket() );
	SetIpv6( sock -> IsIpv6() );
	SetSocketType( sock -> GetSocketType() );
	SetSocketProtocol( sock -> GetSocketProtocol() );

	SocketAddress *client = sock -> GetClientRemoteAddress();
	if (client)
	{
		SetClientRemoteAddress( *client );
	}

	SocketAddress *remote = GetRemoteSocketAddress();
	if (remote)
	{
		SetRemoteAddress(*remote);
	}
}


int Socket::Resolve(const std::string& host,port_t port)
{
	return Handler().Resolve(this, host, port);
}


int Socket::Resolve(ipaddr_t a)
{
	return Handler().Resolve(this, a);
}


int Socket::Resolve(const std::string& a)
{
	return Handler().Resolve(this, a);
}


void Socket::OnSocks4Connect()
{
	Handler().LogError(this, "OnSocks4Connect", 0, "Use with TcpSocket only");
}


void Socket::OnSocks4ConnectFailed()
{
	Handler().LogError(this, "OnSocks4ConnectFailed", 0, "Use with TcpSocket only");
}


bool Socket::OnSocks4Read()
{
	Handler().LogError(this, "OnSocks4Read", 0, "Use with TcpSocket only");
	return true;
}


void Socket::SetSocks4Host(const std::string& host)
{
	Utility::u2ip(host, m_socks4_host);
}


/*
void Socket::OnWriteComplete()
{
}
*/


void Socket::OnResolved(int,ipaddr_t,port_t)
{
}


void Socket::OnResolved(int,const std::string&)
{
}


void Socket::OnResolveFailed(int)
{
}


Socket *Socket::Create()
{
	return NULL;
}


void Socket::OnSSLConnect()
{
}


void Socket::OnSSLAccept()
{
}


bool Socket::OnConnectRetry()
{
	return true;
}


void Socket::OnReconnect()
{
}


bool Socket::SSLNegotiate()
{
	return false;
}


time_t Socket::Uptime()
{
	return time(NULL) - m_tCreate;
}


void Socket::OnDetached()
{
}


void Socket::SetDetach(bool x)
{
	Handler().AddList(m_socket, LIST_DETACH, x);
	m_detach = x;
}


bool Socket::IsDetach()
{
	return m_detach;
}


void Socket::SetDetached(bool x)
{
	m_detached = x;
}


const bool Socket::IsDetached() const
{
	return m_detached;
}


void Socket::SetIpv6(bool x)
{
	m_ipv6 = x;
}


bool Socket::IsIpv6()
{
	return m_ipv6;
}


void Socket::SetIsClient()
{
	m_bClient = true;
}


void Socket::SetSocketType(int x)
{
	m_socket_type = x;
}


int Socket::GetSocketType()
{
	return m_socket_type;
}


void Socket::SetSocketProtocol(const std::string& x)
{
	m_socket_protocol = x;
}


const std::string& Socket::GetSocketProtocol()
{
	return m_socket_protocol;
}


void Socket::SetRetain()
{
	if (m_bClient) m_bRetain = true;
}


bool Socket::Retain()
{
	return m_bRetain;
}


void Socket::SetLost()
{
	m_bLost = true;
}


bool Socket::Lost()
{
	return m_bLost;
}


void Socket::SetCallOnConnect(bool x)
{
	Handler().AddList(m_socket, LIST_CALLONCONNECT, x);
	m_call_on_connect = x;
}


bool Socket::CallOnConnect()
{
	return m_call_on_connect;
}


void Socket::SetReuse(bool x)
{
	m_opt_reuse = x;
}


void Socket::SetKeepalive(bool x)
{
	m_opt_keepalive = x;
}


bool Socket::Socks4()
{
	return m_bSocks4;
}


void Socket::SetSocks4(bool x)
{
	m_bSocks4 = x;
}


void Socket::SetSocks4Host(ipaddr_t a)
{
	m_socks4_host = a;
}


void Socket::SetSocks4Port(port_t p)
{
	m_socks4_port = p;
}


void Socket::SetSocks4Userid(const std::string& x)
{
	m_socks4_userid = x;
}


ipaddr_t Socket::GetSocks4Host()
{
	return m_socks4_host;
}


port_t Socket::GetSocks4Port()
{
	return m_socks4_port;
}


const std::string& Socket::GetSocks4Userid()
{
	return m_socks4_userid;
}


void Socket::SetConnectTimeout(int x)
{
	m_connect_timeout = x;
}


int Socket::GetConnectTimeout()
{
	return m_connect_timeout;
}


bool Socket::IsSSL()
{
	return m_b_enable_ssl;
}


void Socket::EnableSSL(bool x)
{
	m_b_enable_ssl = x;
}


bool Socket::IsSSLNegotiate()
{
	return m_b_ssl;
}


void Socket::SetSSLNegotiate(bool x)
{
	m_b_ssl = x;
}


bool Socket::IsSSLServer()
{
	return m_b_ssl_server;
}


void Socket::SetSSLServer(bool x)
{
	m_b_ssl_server = x;
}


void Socket::DisableRead(bool x)
{
	m_b_disable_read = x;
}


bool Socket::IsDisableRead()
{
	return m_b_disable_read;
}


void Socket::SetRetryClientConnect(bool x)
{
	Handler().AddList(m_socket, LIST_RETRY, x);
	m_b_retry_connect = x;
}


bool Socket::RetryClientConnect()
{
	return m_b_retry_connect;
}


void Socket::SendBuf(const char *,size_t,int)
{
}


void Socket::Send(const std::string&,int)
{
}


void Socket::SetConnected(bool x)
{
	m_connected = x;
}


bool Socket::IsConnected()
{
	return m_connected;
}


void Socket::SetFlushBeforeClose(bool x)
{
	m_flush_before_close = x;
}


bool Socket::GetFlushBeforeClose()
{
	return m_flush_before_close;
}


void Socket::OnSSLConnectFailed()
{
}


void Socket::OnSSLAcceptFailed()
{
}


int Socket::GetConnectionRetry()
{
	return m_connection_retry;
}


void Socket::SetConnectionRetry(int x)
{
	m_connection_retry = x;
}


int Socket::GetConnectionRetries()
{
	return m_retries;
}


void Socket::IncreaseConnectionRetries()
{
	m_retries++;
}


void Socket::ResetConnectionRetries()
{
	m_retries = 0;
}


void Socket::OnDisconnect()
{
}


void Socket::SetErasedByHandler(bool x)
{
	m_b_erased_by_handler = x;
}


bool Socket::ErasedByHandler()
{
	return m_b_erased_by_handler;
}


void Socket::SetSlaveHandler(ISocketHandler *p)
{
	m_slave_handler = p;
}


time_t Socket::TimeSinceClose()
{
	return time(NULL) - m_tClose;
}


void Socket::SetShutdown(int x)
{
	m_shutdown = x;
}


int Socket::GetShutdown()
{
	return m_shutdown;
}


void Socket::SetClientRemoteAddress(SocketAddress& ad)
{
	if (!ad.IsValid())
	{
		Handler().LogError(this, "SetClientRemoteAddress", 0, "remote address not valid", LOG_LEVEL_ERROR);
	}
	if (m_client_remote_address)
	{
		delete m_client_remote_address;
	}
	m_client_remote_address = ad.GetCopy();
}


SocketAddress *Socket::GetClientRemoteAddress()
{
	if (!m_client_remote_address)
	{
		Handler().LogError(this, "GetClientRemoteAddress", 0, "remote address not yet set", LOG_LEVEL_ERROR);
	}
	return m_client_remote_address;
}


uint64_t Socket::GetBytesSent(bool)
{
	return 0;
}


uint64_t Socket::GetBytesReceived(bool)
{
	return 0;
}


#ifdef SOCKETS_NAMESPACE
}
#endif

