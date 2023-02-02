/** \file SocketHandler.cpp
 **	\date  2004-02-13
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004,2005  Anders Hedstrom

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
#include <stdio.h>
#ifdef _WIN32
#pragma warning(disable:4786)
#include <stdlib.h>
#else
#include <errno.h>
#endif

#include "SocketHandler.h"
#include "UdpSocket.h"
#include "PoolSocket.h"
#include "ResolvSocket.h"
#include "ResolvServer.h"
#include "TcpSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x)
#endif


SocketHandler::SocketHandler(StdLog *p)
:m_stdlog(p)
,m_maxsock(0)
,m_host("")
,m_ip(0)
#ifdef _WIN32
,m_preverror(-1)
#endif
,m_slave(false)
,m_local_resolved(false)
,m_socks4_host(0)
,m_socks4_port(0)
,m_bTryDirect(false)
,m_resolv_id(0)
,m_resolver(NULL)
,m_b_enable_pool(false)
{
	FD_ZERO(&m_rfds);
	FD_ZERO(&m_wfds);
	FD_ZERO(&m_efds);
}


SocketHandler::~SocketHandler()
{
	if (m_resolver)
	{
		m_resolver -> Quit();
	}
//	if (!m_slave)
	{
		while (m_sockets.size())
		{
			socket_m::iterator it = m_sockets.begin();
			Socket *p = (*it).second;
			if (p)
			{
				p -> Close();
//				p -> OnDelete(); // hey, I turn this back on. what's the worst that could happen??!!
				// MinionSocket breaks, calling MinderHandler methods in OnDelete -
				// MinderHandler is already gone when that happens...
				m_sockets.erase(it);
				if (p -> DeleteByHandler())
				{
					p -> SetErasedByHandler();
					delete p;
				}
			}
			else
			{
				m_sockets.erase(it);
			}
		}
	}
	if (m_resolver)
	{
		delete m_resolver;
	}
}


void SocketHandler::ResolveLocal()
{
	char h[256];

	// get local hostname and translate into ip-address
	*h = 0;
	gethostname(h,255);
	{
		Socket zl(*this);
		if (zl.u2ip(h, m_ip))
		{
			zl.l2ip(m_ip, m_addr);
		}
	}
#ifdef IPPROTO_IPV6
	memset(&m_local_ip6, 0, sizeof(m_local_ip6));
	{
		Socket zl(*this);
		zl.SetIpv6();
		if (zl.u2ip(h, m_local_ip6))
		{
			zl.l2ip(m_local_ip6, m_local_addr6);
		}
	}
#endif
	m_host = h;
	m_local_resolved = true;
}


void SocketHandler::Add(Socket *p)
{
	if (p -> GetSocket() == INVALID_SOCKET)
	{
		LogError(p, "Add", -1, "Invalid socket", LOG_LEVEL_WARNING);
		if (p -> CloseAndDelete())
		{
			m_delete.push_back(p);
		}
		return;
	}
	m_add[p -> GetSocket()] = p;
}


void SocketHandler::Get(SOCKET s,bool& r,bool& w,bool& e)
{
	if (s >= 0)
	{
		r = FD_ISSET(s, &m_rfds) ? true : false;
		w = FD_ISSET(s, &m_wfds) ? true : false;
		e = FD_ISSET(s, &m_efds) ? true : false;
	}
}


void SocketHandler::Set(SOCKET s,bool bRead,bool bWrite,bool bException)
{
	if (s >= 0)
	{
		if (bRead)
		{
			if (!FD_ISSET(s, &m_rfds))
			{
				FD_SET(s, &m_rfds);
			}
		}
		else
		{
			FD_CLR(s, &m_rfds);
		}
		if (bWrite)
		{
			if (!FD_ISSET(s, &m_wfds))
			{
				FD_SET(s, &m_wfds);
			}
		}
		else
		{
			FD_CLR(s, &m_wfds);
		}
		if (bException)
		{
			if (!FD_ISSET(s, &m_efds))
			{
				FD_SET(s, &m_efds);
			}
		}
		else
		{
			FD_CLR(s, &m_efds);
		}
	}
}


int SocketHandler::Select(long sec,long usec)
{
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	return Select(&tv);
}


int SocketHandler::Select()
{
	return Select(NULL);
}


int SocketHandler::Select(struct timeval *tsel)
{
	while (m_add.size())
	{
		if (m_sockets.size() >= FD_SETSIZE)
		{
			LogError(NULL, "Select", (int)m_sockets.size(), "FD_SETSIZE reached", LOG_LEVEL_WARNING);
			break;
		}
		socket_m::iterator it = m_add.begin();
		SOCKET s = (*it).first;
		Socket *p = (*it).second;
		// call Open before Add'ing a socket...
		if (p -> Connecting())
		{
			Set(s,false,true);
		}
		else
		{
			if (p -> IsDisableRead())
				Set(s, false, false);
			else
				Set(s,true,false);
		}
		m_maxsock = (s > m_maxsock) ? s : m_maxsock;
		m_fds.push_back(s);
		m_sockets[s] = p;
//printf("Add: %d\n", s);
		m_add.erase(it);
	}
#ifdef MACOSX
	fd_set rfds;
	fd_set wfds;
	fd_set efds;
	FD_COPY(&m_rfds, &rfds);
	FD_COPY(&m_wfds, &wfds);
	FD_COPY(&m_efds, &efds);
#else
	fd_set rfds = m_rfds;
	fd_set wfds = m_wfds;
	fd_set efds = m_efds;
#endif
	int n = select( (int)(m_maxsock + 1),&rfds,&wfds,&efds,tsel);
	if (n == -1)
	{
		LogError(NULL, "select", Errno, StrError(Errno));
#ifdef _WIN32
DEB(
		int errcode = Errno;
		if (errcode != m_preverror)
		{
			printf("  select() errcode = %d\n",errcode);
			m_preverror = errcode;
			for (size_t i = 0; i <= m_maxsock; i++)
			{
				if (FD_ISSET(i, &m_rfds))
					printf("%4d: Read\n",i);
				if (FD_ISSET(i, &m_wfds))
					printf("%4d: Write\n",i);
				if (FD_ISSET(i, &m_efds))
					printf("%4d: Exception\n",i);
			}
		}
		exit(-1); // %! remove....
) // DEB
#endif
	}
	else
	if (n > 0)
	{
		for (socket_v::iterator it2 = m_fds.begin(); it2 != m_fds.end() && n; it2++)
		{
			SOCKET i = *it2;
			if (FD_ISSET(i, &rfds))
			{
				Socket *p = m_sockets[i];
				if (!p)
				{
					n--;
					continue;
				}
				// new SSL negotiate method
				if (p -> IsSSLNegotiate())
				{
					p -> SSLNegotiate();
				}
				else
				{
					// LockWrite (save total output buffer size)
					// Sockets with write lock won't call OnWrite in SendBuf
					// That will happen in UnlockWrite, if necessary
					p -> OnRead();
					if (p -> Socks4())
					{
						bool need_more = false;
						TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
						while (tcp && tcp -> GetInputLength() && !need_more && !p -> CloseAndDelete())
						{
							need_more = p -> OnSocks4Read();
						}
					}
					else
					{
						if (p -> LineProtocol())
						{
							p -> ReadLine();
						}
//							p -> Touch();
					}
					// UnlockWrite (call OnWrite if saved size == 0 && total output buffer size > 0)
				}
				n--;
			}
			if (FD_ISSET(i, &wfds))
			{
				Socket *p = m_sockets[i];
				if (!p)
				{
					n--;
					continue;
				}
				// new SSL negotiate method
				if (p -> IsSSLNegotiate())
				{
					p -> SSLNegotiate();
				}
				else
				{
					if (p -> Connecting())
					{
						if (p -> CheckConnect())
						{
							p -> SetCallOnConnect();
						}
						else
						{
							TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
							// failed
							if (p -> Socks4())
							{
								p -> OnSocks4ConnectFailed();
							}
							else
							if (tcp && (tcp -> GetConnectionRetry() == -1 ||
								(tcp -> GetConnectionRetry() &&
								 tcp -> GetConnectionRetries() < tcp -> GetConnectionRetry() )))
							{
								// even though the connection failed at once, only retry after
								// the connection timeout.
								// should we even try to connect again, when CheckConnect returns
								// false it's because of a connection error - not a timeout...
							}
							else
							{
//									LogError(p, "connect failed", Errno, StrError(Errno), LOG_LEVEL_FATAL);
								p -> SetCloseAndDelete( true );
								p -> OnConnectFailed();
							}
						}
//								p -> Touch();
					}
					else
					{
						p -> OnWrite();
//								p -> Touch();
					}
				}
				n--;
			}
			if (FD_ISSET(i, &efds))
			{
				Socket *p = m_sockets[i];
				if (!p)
				{
					n--;
					continue;
				}
				p -> OnException();
				n--;
			}
		}
	} // if (n > 0)

/*
	for (socket_m::iterator it3 = m_sockets.begin(); it3 != m_sockets.end(); it3++)
	{
		Socket *p = (*it3).second;
		if (p)
		{
			if (p -> CallOnConnect() && p -> Ready() )
			{
				if (p -> IsSSL()) // SSL Enabled socket
					p -> OnSSLConnect();
				else
				if (p -> Socks4())
					p -> OnSocks4Connect();
				else
				{
					TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
					if (tcp)
					{
						p -> SetConnected();
						if (tcp -> GetOutputLength())
						{
							p -> OnWrite();
						}
					}
					if (tcp && tcp -> IsReconnect())
						p -> OnReconnect();
					else
					{
						LogError(p, "Calling OnConnect", 0, "Because CallOnConnect", LOG_LEVEL_INFO);
						p -> OnConnect();
					}
				}
				p -> SetCallOnConnect( false );
			}
			if (!m_slave && p -> IsDetach())
			{
				Set(p -> GetSocket(), false, false, false);
				p -> DetachSocket();
				m_fds_erase.push_back(p -> GetSocket());
//				m_sockets.erase(it3);
//				repeat = true;
//				break;
				continue;
			}
//			if (p && p -> Timeout() && p -> Inactive() > p -> Timeout())
//			{
//				p -> SetCloseAndDelete();
//			}
			if (p -> Connecting() && p -> GetConnectTime() >= p -> GetConnectTimeout() )
			{
				TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
				LogError(p, "connect", -1, "connect timeout", LOG_LEVEL_FATAL);
				if (p -> Socks4())
				{
					p -> OnSocks4ConnectFailed();
					// retry direct connection
				}
				else
				if (tcp && (tcp -> GetConnectionRetry() == -1 ||
					(tcp -> GetConnectionRetry() &&
					 tcp -> GetConnectionRetries() < tcp -> GetConnectionRetry() )))
				{
					tcp -> IncreaseConnectionRetries();
					if (p -> OnConnectRetry())
					{
						p -> SetRetryClientConnect();
					}
					else
					{
						p -> SetCloseAndDelete( true );
						p -> OnConnectFailed();
					}
				}
				else
				{
					p -> SetCloseAndDelete(true);
					p -> OnConnectFailed();
				}
			}
			if (p -> RetryClientConnect())
			{
				TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
				SOCKET nn = (*it3).first;
				p -> SetRetryClientConnect(false);
				p -> Close();
#ifdef IPPROTO_IPV6
				if (p -> IsIpv6())
				{
					tcp -> Open(p -> GetClientRemoteAddr6(), p -> GetClientRemotePort());
				}
				else
#endif
				{
					tcp -> Open(p -> GetClientRemoteAddr(), p -> GetClientRemotePort());
				}
//					m_sockets.erase(it3); // remove old SOCKET/Socket* pair
				Add(p);
//					repeat = true;
//					break;
				m_fds_erase.push_back(nn);
				continue;
			}
			if (p -> CloseAndDelete() )
			{
				TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
				if (tcp && tcp -> GetOutputLength() && 
					tcp -> GetFlushBeforeClose() && 
					!tcp -> IsSSL() &&
					tcp -> CheckSendTimeoutCount() < 100 // magic number of cycles
					) // wait until all data sent
				{
					LogError(p, "Closing", (int)tcp -> GetOutputLength(), "Sending all data before closing", LOG_LEVEL_INFO);
				}
				else
				if (tcp && p -> IsConnected() && tcp -> Reconnect())
				{
					SOCKET nn = (*it3).first;
					p -> SetCloseAndDelete(false);
					tcp -> SetIsReconnect();
					p -> SetConnected(false);
					p -> Close(); // dispose of old file descriptor (Open creates a new)
					p -> OnDisconnect();
#ifdef IPPROTO_IPV6
					if (p -> IsIpv6())
					{
						tcp -> Open(p -> GetClientRemoteAddr6(), p -> GetClientRemotePort());
					}
					else
#endif
					{
						tcp -> Open(p -> GetClientRemoteAddr(), p -> GetClientRemotePort());
					}
					tcp -> ResetConnectionRetries();
//						m_sockets.erase(it3); // remove old SOCKET/Socket* pair
					Add(p);
//						repeat = true;
//						break;
					m_fds_erase.push_back(nn);
					continue;
				}
				else
				{
					SOCKET nn = (*it3).first;
					if (tcp && tcp -> GetOutputLength())
					{
						LogError(p, "Closing", (int)tcp -> GetOutputLength(), "Closing socket while data still left to send", LOG_LEVEL_WARNING);
					}
					if (p -> Retain() && !p -> Lost())
					{
						PoolSocket *p2 = new PoolSocket(*this, p);
						p2 -> SetDeleteByHandler();
						Add(p2);
					}
					else
					{
						Set(p -> GetSocket(),false,false,false);
						p -> Close();
					}
					p -> OnDelete();
//						m_sockets.erase(it3);
					if (p -> DeleteByHandler())
					{
						p -> SetErasedByHandler();
						delete p;
					}
//						repeat = true;
//						break;
					m_fds_erase.push_back(nn);
					continue;
				}
			}
		} // if (p)
	}
*/

	// check CallOnConnect
	if (m_fds_callonconnect.size())
	{
		socket_v tmp = m_fds_callonconnect;
		for (socket_v::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
//printf("Check fd callonconnect: %d\n", *it);
			Socket *p = m_sockets[*it];
			if (p)
			{
				if (p -> CallOnConnect() && p -> Ready() )
				{
					if (p -> IsSSL()) // SSL Enabled socket
						p -> OnSSLConnect();
					else
					if (p -> Socks4())
						p -> OnSocks4Connect();
					else
					{
						TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
						if (tcp)
						{
							p -> SetConnected();
							if (tcp -> GetOutputLength())
							{
								p -> OnWrite();
							}
						}
						if (tcp && tcp -> IsReconnect())
							p -> OnReconnect();
						else
						{
							LogError(p, "Calling OnConnect", 0, "Because CallOnConnect", LOG_LEVEL_INFO);
							p -> OnConnect();
						}
					}
					p -> SetCallOnConnect( false );
				}
			}
		}
	}
	// if (!m_slave) check Detach
	if (!m_slave && m_fds_detach.size())
	{
		socket_v tmp = m_fds_detach;
		for (socket_v::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
//printf("Check fd detach: %d\n", *it);
			Socket *p = m_sockets[*it];
			if (p)
			{
				if (p -> IsDetach())
				{
					Set(p -> GetSocket(), false, false, false);
					p -> DetachSocket();
					m_fds_erase.push_back(p -> GetSocket());
					//
					p -> SetDetach(false); // added
				}
			}
		}
	}
	// check Connecting
	if (m_fds_connecting.size())
	{
		socket_v tmp = m_fds_connecting;
		for (socket_v::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
//printf("Check fd connecting: %d\n", *it);
			Socket *p = m_sockets[*it];
			if (p)
			{
				if (p -> Connecting() && p -> GetConnectTime() >= p -> GetConnectTimeout() )
				{
					TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
					LogError(p, "connect", -1, "connect timeout", LOG_LEVEL_FATAL);
					if (p -> Socks4())
					{
						p -> OnSocks4ConnectFailed();
						// retry direct connection
					}
					else
					if (tcp && (tcp -> GetConnectionRetry() == -1 ||
						(tcp -> GetConnectionRetry() &&
						 tcp -> GetConnectionRetries() < tcp -> GetConnectionRetry() )))
					{
						tcp -> IncreaseConnectionRetries();
						if (p -> OnConnectRetry())
						{
							p -> SetRetryClientConnect();
						}
						else
						{
							p -> SetCloseAndDelete( true );
							p -> OnConnectFailed();
						}
					}
					else
					{
						p -> SetCloseAndDelete(true);
						p -> OnConnectFailed();
					}
					//
					p -> SetConnecting(false); // added
				}
			}
		}
	}
	// check retry client connect
	if (m_fds_retry.size())
	{
		socket_v tmp = m_fds_retry;
		for (socket_v::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
//printf("Check fd retry client connect: %d\n", *it);
			Socket *p = m_sockets[*it];
			if (p)
			{
				if (p -> RetryClientConnect())
				{
					TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
					SOCKET nn = *it; //(*it3).first;
					p -> SetRetryClientConnect(false);
					p -> Close(); // removes from m_fds_retry
#ifdef IPPROTO_IPV6
					if (p -> IsIpv6())
					{
						tcp -> Open(p -> GetClientRemoteAddr6(), p -> GetClientRemotePort());
					}
					else
#endif
					{
						tcp -> Open(p -> GetClientRemoteAddr(), p -> GetClientRemotePort());
					}
					Add(p);
					m_fds_erase.push_back(nn);
				}
			}
		}
	}
	// check close and delete
	if (m_fds_close.size())
	{
		socket_v tmp = m_fds_close;
		for (socket_v::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
//printf("Check fd close: %d\n", *it);
			Socket *p = m_sockets[*it];
			if (p)
			{
				if (p -> CloseAndDelete() )
				{
					TcpSocket *tcp = dynamic_cast<TcpSocket *>(p);
					if (tcp && tcp -> GetOutputLength() && 
						tcp -> GetFlushBeforeClose() && 
						!tcp -> IsSSL() &&
						tcp -> CheckSendTimeoutCount() < 100 // magic number of cycles
						) // wait until all data sent
					{
						LogError(p, "Closing", (int)tcp -> GetOutputLength(), "Sending all data before closing", LOG_LEVEL_INFO);
					}
					else
					if (tcp && p -> IsConnected() && tcp -> Reconnect())
					{
						SOCKET nn = *it; //(*it3).first;
						p -> SetCloseAndDelete(false);
						tcp -> SetIsReconnect();
						p -> SetConnected(false);
						p -> Close(); // dispose of old file descriptor (Open creates a new)
						p -> OnDisconnect();
#ifdef IPPROTO_IPV6
						if (p -> IsIpv6())
						{
							tcp -> Open(p -> GetClientRemoteAddr6(), p -> GetClientRemotePort());
						}
						else
#endif
						{
							tcp -> Open(p -> GetClientRemoteAddr(), p -> GetClientRemotePort());
						}
						tcp -> ResetConnectionRetries();
						Add(p);
						m_fds_erase.push_back(nn);
					}
					else
					{
						SOCKET nn = *it; //(*it3).first;
						if (tcp && tcp -> GetOutputLength())
						{
							LogError(p, "Closing", (int)tcp -> GetOutputLength(), "Closing socket while data still left to send", LOG_LEVEL_WARNING);
						}
						if (p -> Retain() && !p -> Lost())
						{
							PoolSocket *p2 = new PoolSocket(*this, p);
							p2 -> SetDeleteByHandler();
							Add(p2);
							//
							p -> SetCloseAndDelete(false); // added - remove from m_fds_close
						}
						else
						{
							Set(p -> GetSocket(),false,false,false);
							p -> Close();
						}
						p -> OnDelete();
						if (p -> DeleteByHandler())
						{
							p -> SetErasedByHandler();
							delete p;
						}
						m_fds_erase.push_back(nn);
					}
				}
			}
		}
	}

	// check erased sockets
	bool repeat = false;
	while (m_fds_erase.size())
	{
		socket_v::iterator it = m_fds_erase.begin();
		SOCKET nn = *it;
		{
			for (socket_v::iterator it = m_fds.begin(); it != m_fds.end(); it++)
			{
				if (*it == nn)
				{
					m_fds.erase(it);
					break;
				}
			}
		}
		{
			for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
			{
				if ((*it).first == nn)
				{
					m_sockets.erase(it);
					break;
				}
			}
		}
		m_fds_erase.erase(it);
		repeat = true;
	}
	if (repeat)
	{
		m_maxsock = 0;
		for (socket_v::iterator it = m_fds.begin(); it != m_fds.end(); it++)
		{
			SOCKET s = *it;
			m_maxsock = s > m_maxsock ? s : m_maxsock;
		}
	}
	// remove Add's that fizzed
	while (m_delete.size())
	{
		std::list<Socket *>::iterator it = m_delete.begin();
		Socket *p = *it;
		p -> OnDelete();
		m_delete.erase(it);
		if (p -> DeleteByHandler())
		{
			p -> SetErasedByHandler();
			delete p;
		}
	}
	return n;
}


const std::string& SocketHandler::GetLocalHostname()
{
	if (!m_local_resolved)
		LogError(NULL, "GetLocalHostname", 0, "local address not resolved");
	return m_host;
}


ipaddr_t SocketHandler::GetLocalIP()
{
	if (!m_local_resolved)
		LogError(NULL, "GetLocalHostname", 0, "local address not resolved");
	return m_ip;
}


const std::string& SocketHandler::GetLocalAddress()
{
	if (!m_local_resolved)
		LogError(NULL, "GetLocalHostname", 0, "local address not resolved");
	return m_addr;
}


bool SocketHandler::Valid(Socket *p0)
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		Socket *p = (*it).second;
		if (p0 == p)
			return true;
	}
	return false;
}


void SocketHandler::RegStdLog(StdLog *x)
{
	m_stdlog = x;
}


bool SocketHandler::OkToAccept(Socket *)
{
	return true;
}


size_t SocketHandler::GetCount()
{
	return m_sockets.size() + m_add.size() + m_delete.size();
}


void SocketHandler::SetSlave(bool x)
{
	m_slave = x;
}


void SocketHandler::LogError(Socket *p,const std::string& user_text,int err,const std::string& sys_err,loglevel_t t)
{
	if (m_stdlog)
	{
		m_stdlog -> error(this, p, user_text, err, sys_err, t);
	}
}


#ifdef IPPROTO_IPV6
const struct in6_addr& SocketHandler::GetLocalIP6()
{
	if (!m_local_resolved)
		LogError(NULL, "GetLocalHostname", 0, "local address not resolved");
	return m_local_ip6;
}


const std::string& SocketHandler::GetLocalAddress6()
{
	if (!m_local_resolved)
		LogError(NULL, "GetLocalHostname", 0, "local address not resolved");
	return m_local_addr6;
}
#endif


PoolSocket *SocketHandler::FindConnection(int type,const std::string& protocol,ipaddr_t a,port_t port)
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end() && m_sockets.size(); it++)
	{
		PoolSocket *pools = dynamic_cast<PoolSocket *>((*it).second);
		if (pools)
		{
			if (pools -> GetSocketType() == type &&
			    pools -> GetSocketProtocol() == protocol &&
			    pools -> GetClientRemoteAddr() == a &&
			    pools -> GetClientRemotePort() == port)
			{
				m_sockets.erase(it);
				pools -> SetRetain(); // avoid Close in Socket destructor
				return pools; // Caller is responsible that this socket is deleted
			}
		}
	}
	return NULL;
}


#ifdef IPPROTO_IPV6
PoolSocket *SocketHandler::FindConnection(int type,const std::string& protocol,in6_addr a,port_t port)
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end() && m_sockets.size(); it++)
	{
		PoolSocket *pools = dynamic_cast<PoolSocket *>((*it).second);
		if (pools)
		{
			if (pools -> GetSocketType() == type &&
			    pools -> GetSocketProtocol() == protocol &&
			    !in6_addr_compare(pools -> GetClientRemoteAddr6(), a) &&
			    pools -> GetClientRemotePort() == port)
			{
				m_sockets.erase(it);
				pools -> SetRetain(); // avoid Close in Socket destructor
				return pools; // Caller is responsible that this socket is deleted
			}
		}
	}
	return NULL;
}
#endif


void SocketHandler::SetSocks4Host(ipaddr_t a)
{
	m_socks4_host = a;
}


void SocketHandler::SetSocks4Host(const std::string& host)
{
	Socket s(*this);
	s.u2ip(host, m_socks4_host);
}


void SocketHandler::SetSocks4Port(port_t port)
{
	m_socks4_port = port;
}


void SocketHandler::SetSocks4Userid(const std::string& id)
{
	m_socks4_userid = id;
}


int SocketHandler::Resolve(Socket *p,const std::string& host,port_t port)
{
	// check cache
	ResolvSocket *resolv = new ResolvSocket(*this, p);
	resolv -> SetId(++m_resolv_id);
	resolv -> SetHost(host);
	resolv -> SetPort(port);
	resolv -> SetDeleteByHandler();
	ipaddr_t local;
	resolv -> u2ip("127.0.0.1", local);
	if (!resolv -> Open(local, m_resolver_port))
	{
		LogError(resolv, "Resolve", -1, "Can't connect to local resolve server", LOG_LEVEL_FATAL);
	}
	Add(resolv);
	return m_resolv_id;
}


void SocketHandler::EnableResolver(port_t port)
{
	if (!m_resolver)
	{
		m_resolver_port = port;
		m_resolver = new ResolvServer(port);
	}
}


#ifdef IPPROTO_IPV6
int SocketHandler::in6_addr_compare(in6_addr a,in6_addr b)
{
	for (size_t i = 0; i < 16; i++)
	{
		if (a.s6_addr[i] < b.s6_addr[i])
			return -1;
		if (a.s6_addr[i] > b.s6_addr[i])
			return 1;
	}
	return 0;
}
#endif


bool SocketHandler::ResolverReady()
{
	return m_resolver ? m_resolver -> Ready() : false;
}


void SocketHandler::EnablePool(bool x)
{
	m_b_enable_pool = x;
}


void SocketHandler::SetSocks4TryDirect(bool x)
{
	m_bTryDirect = x;
}


ipaddr_t SocketHandler::GetSocks4Host()
{
	return m_socks4_host;
}


port_t SocketHandler::GetSocks4Port()
{
	return m_socks4_port;
}


const std::string& SocketHandler::GetSocks4Userid()
{
	return m_socks4_userid;
}


bool SocketHandler::Socks4TryDirect()
{
	return m_bTryDirect;
}


bool SocketHandler::ResolverEnabled() 
{ 
	return m_resolver ? true : false; 
}


port_t SocketHandler::GetResolverPort() 
{ 
	return m_resolver_port; 
}


bool SocketHandler::PoolEnabled() 
{ 
	return m_b_enable_pool; 
}


void SocketHandler::Remove(Socket *p)
{
	if (p -> ErasedByHandler())
		return;
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		if ((*it).second == p)
		{
			LogError(p, "Remove", -1, "Socket destructor called while still in use", LOG_LEVEL_WARNING);
			m_sockets.erase(it);
			return;
		}
	}
	for (socket_m::iterator it2 = m_add.begin(); it2 != m_add.end(); it2++)
	{
		if ((*it2).second == p)
		{
			LogError(p, "Remove", -2, "Socket destructor called while still in use", LOG_LEVEL_WARNING);
			m_add.erase(it2);
			return;
		}
	}
	for (std::list<Socket *>::iterator it3 = m_delete.begin(); it3 != m_delete.end(); it3++)
	{
		if (*it3 == p)
		{
			LogError(p, "Remove", -3, "Socket destructor called while still in use", LOG_LEVEL_WARNING);
			m_delete.erase(it3);
			return;
		}
	}
}


socket_v& SocketHandler::GetFdsCallOnConnect()
{
	return m_fds_callonconnect;
}


socket_v& SocketHandler::GetFdsDetach()
{
	return m_fds_detach;
}


socket_v& SocketHandler::GetFdsConnecting()
{
	return m_fds_connecting;
}


socket_v& SocketHandler::GetFdsRetry()
{
	return m_fds_retry;
}


socket_v& SocketHandler::GetFdsClose()
{
	return m_fds_close;
}


#ifdef SOCKETS_NAMESPACE
}
#endif

