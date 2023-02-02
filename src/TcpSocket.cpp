/** \file TcpSocket.cpp
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
#include "ISocketHandler.h"
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <stdarg.h>
#ifdef HAVE_OPENSSL
#include <openssl/rand.h>
#endif

#include "TcpSocket.h"
#include "PoolSocket.h"
#include "Utility.h"
#include "Ipv4Address.h"
#include "Ipv6Address.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x) 
#endif


// statics
BIO *TcpSocket::bio_err = NULL;
bool TcpSocket::m_b_rand_file_generated = false;
std::string TcpSocket::m_rand_file;
long TcpSocket::m_rand_size = 1024;
TcpSocket::SSLInitializer TcpSocket::m_ssl_init;


// thanks, q
#ifdef _WIN32
#pragma warning(disable:4355)
#endif
TcpSocket::TcpSocket(ISocketHandler& h) : Socket(h)
,ibuf(*this, TCP_BUFSIZE_READ)
,obuf(*this, 32768)
,m_line("")
#ifdef SOCKETS_DYNAMIC_TEMP
,m_buf(new char[TCP_BUFSIZE_READ + 1])
#endif
,m_socks4_state(0)
,m_resolver_id(0)
,m_ssl_ctx(NULL)
,m_ssl(NULL)
,m_sbio(NULL)
,m_b_reconnect(false)
,m_b_is_reconnect(false)
,m_b_input_buffer_disabled(false)
,m_bytes_sent(0)
,m_bytes_received(0)
{
#ifdef HAVE_OPENSSL
	if (!m_b_rand_file_generated)
	{
		m_b_rand_file_generated = true;
		char *randfile = getenv("RANDFILE");
		char *home = getenv("HOME");
		if (!randfile && !home)
		{
			char *homepath = getenv("HOMEPATH");
			if (homepath)
			{
				Utility::SetEnv("HOME", homepath);
			}
		}
		char path[512];
		*path = 0;
		RAND_file_name(path, 512);
		if (*path)
		{
			m_rand_file = path;
			m_rand_size = 1024;
			RAND_write_file(path);
		}
		else
		{
			Handler().LogError(this, "TcpSocket constructor", 0, "No random file generated", LOG_LEVEL_ERROR);
		}
	}
#endif // HAVE_OPENSSL
}
#ifdef _WIN32
#pragma warning(default:4355)
#endif


#ifdef _WIN32
#pragma warning(disable:4355)
#endif
TcpSocket::TcpSocket(ISocketHandler& h,size_t isize,size_t osize) : Socket(h)
,ibuf(*this, isize)
,obuf(*this, osize)
,m_line("")
#ifdef SOCKETS_DYNAMIC_TEMP
,m_buf(new char[TCP_BUFSIZE_READ + 1])
#endif
,m_socks4_state(0)
,m_resolver_id(0)
,m_ssl_ctx(NULL)
,m_ssl(NULL)
,m_sbio(NULL)
,m_b_reconnect(false)
,m_b_is_reconnect(false)
,m_b_input_buffer_disabled(false)
,m_bytes_sent(0)
,m_bytes_received(0)
{
#ifdef HAVE_OPENSSL
	if (!m_b_rand_file_generated)
	{
		m_b_rand_file_generated = true;
		char *randfile = getenv("RANDFILE");
		char *home = getenv("HOME");
		if (!randfile && !home)
		{
			char *homepath = getenv("HOMEPATH");
			if (homepath)
			{
				Utility::SetEnv("HOME", homepath);
			}
		}
		char path[512];
		*path = 0;
		RAND_file_name(path, 512);
		if (*path)
		{
			m_rand_file = path;
			RAND_write_file(path);
		}
		else
		{
			Handler().LogError(this, "TcpSocket constructor", 0, "No random file generated", LOG_LEVEL_ERROR);
		}
	}
#endif // HAVE_OPENSSL
}
#ifdef _WIN32
#pragma warning(default:4355)
#endif


TcpSocket::~TcpSocket()
{
	if (m_mes.size())
	{
		Handler().LogError(this, "TcpSocket destructor", 0, "Output buffer not empty", LOG_LEVEL_WARNING);
	}
	while (m_mes.size())
	{
		ucharp_v::iterator it = m_mes.begin();
		MES *p = *it;
		delete p;
		m_mes.erase(it);
	}
#ifdef SOCKETS_DYNAMIC_TEMP
	delete[] m_buf;
#endif
#ifdef HAVE_OPENSSL
	if (m_ssl)
	{
		SSL_free(m_ssl);
	}
	if (m_ssl_ctx)
	{
		SSL_CTX_free(m_ssl_ctx);
	}
#endif
}


bool TcpSocket::Open(ipaddr_t ip,port_t port,bool skip_socks)
{
	Ipv4Address ad(ip, port);
	return Open(ad, skip_socks);
}


#ifdef IPPROTO_IPV6
bool TcpSocket::Open(in6_addr ip,port_t port,bool skip_socks)
{
	Ipv6Address ad(ip, port);
	return Open(ad, skip_socks);
}
#endif


bool TcpSocket::Open(SocketAddress& ad,bool skip_socks)
{
	if (!ad.IsValid())
	{
		Handler().LogError(this, "Open", 0, "Invalid SocketAddress", LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return false;
	}
	if (Handler().GetCount() >= FD_SETSIZE)
	{
		Handler().LogError(this, "Open", 0, "no space left in fd_set", LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return false;
	}
	SetConnecting(false);
	SetSocks4(false);
	// check for pooling
	if (Handler().PoolEnabled())
	{
		PoolSocket *pools = Handler().FindConnection(SOCK_STREAM, "tcp", ad);
		if (pools)
		{
			CopyConnection( pools );
			delete pools;

			SetIsClient();
			SetCallOnConnect(); // ISocketHandler must call OnConnect
			Handler().LogError(this, "SetCallOnConnect", 0, "Found pooled connection", LOG_LEVEL_INFO);
			return true;
		}
	}
	// if not, create new connection
	SOCKET s = CreateSocket(ad.GetFamily(), SOCK_STREAM, "tcp");
	if (s == INVALID_SOCKET)
	{
		return false;
	}
	// socket must be nonblocking for async connect
	if (!SetNonblocking(true, s))
	{
		SetCloseAndDelete();
		closesocket(s);
		return false;
	}
	SetIsClient(); // client because we connect
	SetClientRemoteAddress(ad);
	int n = 0;
	if (!skip_socks && GetSocks4Host() && GetSocks4Port())
	{
		Ipv4Address sa(GetSocks4Host(), GetSocks4Port());
		{
			std::string sockshost;
			Utility::l2ip(GetSocks4Host(), sockshost);
			Handler().LogError(this, "Open", 0, "Connecting to socks4 server @ " + sockshost + ":" +
				Utility::l2string(GetSocks4Port()), LOG_LEVEL_INFO);
		}
		SetSocks4();
		n = connect(s, sa, sa);
		SetRemoteAddress(sa);
	}
	else
	{
		n = connect(s, ad, ad);
		SetRemoteAddress(ad);
	}
	if (n == -1)
	{
		// check error code that means a connect is in progress
#ifdef _WIN32
		if (Errno == WSAEWOULDBLOCK)
#else
		if (Errno == EINPROGRESS)
#endif
		{
//			Handler().LogError(this, "connect: connection pending", Errno, StrError(Errno), LOG_LEVEL_INFO);
			Attach(s);
			SetConnecting( true ); // this flag will control fd_set's
		}
		else
		if (Socks4() && Handler().Socks4TryDirect() ) // retry
		{
			closesocket(s);
			return Open(ad, true);
		}
		else
		if (Reconnect())
		{
			Handler().LogError(this, "connect: failed, reconnect pending", Errno, StrError(Errno), LOG_LEVEL_INFO);
			Attach(s);
			SetConnecting( true ); // this flag will control fd_set's
		}
		else
		{
			Handler().LogError(this, "connect: failed", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			SetCloseAndDelete();
			closesocket(s);
			return false;
		}
	}
	else
	{
//		Handler().LogError(this, "connect", 0, "connection established", LOG_LEVEL_INFO);
		Attach(s);
		SetCallOnConnect(); // ISocketHandler must call OnConnect
//		Handler().LogError(this, "SetCallOnConnect", n, "connect() returns != -1", LOG_LEVEL_INFO);
	}

	// 'true' means connected or connecting(not yet connected)
	// 'false' means something failed
	return true; //!Connecting();
}


bool TcpSocket::Open(const std::string &host,port_t port)
{
#ifdef IPPROTO_IPV6
	if (IsIpv6())
	{
		in6_addr a;
		/// \todo enable ipv6 async resolver
		if (!Utility::u2ip(host, a))
		{
			SetCloseAndDelete();
			return false;
		}
		Ipv6Address ad(a, port);
		return Open(ad);
	}
#endif
	if (!Handler().ResolverEnabled() || Utility::isipv4(host) )
	{
		ipaddr_t l;
		if (!Utility::u2ip(host,l))
		{
			SetCloseAndDelete();
			return false;
		}
		Ipv4Address ad(l, port);
		return Open(ad);
	}
	// resolve using async resolver thread
	m_resolver_id = Resolve(host, port);
	return true;
}


void TcpSocket::OnResolved(int id,ipaddr_t a,port_t port)
{
	if (id == m_resolver_id)
	{
		if (a && port)
		{
			Ipv4Address ad(a, port);
			if (Open(ad))
			{
				if (!Handler().Valid(this))
				{
					Handler().Add(this);
				}
			}
		}
		else
		{
			Handler().LogError(this, "OnResolved", 0, "Resolver failed", LOG_LEVEL_FATAL);
			SetCloseAndDelete();
		}
	}
	else
	{
		Handler().LogError(this, "OnResolved", id, "Resolver returned wrong job id", LOG_LEVEL_FATAL);
		SetCloseAndDelete();
	}
}


void TcpSocket::OnRead()
{
	int n = 0;
#ifdef SOCKETS_DYNAMIC_TEMP
	char *buf = m_buf;
#else
	char buf[TCP_BUFSIZE_READ];
#endif
	if (IsSSL())
	{
#ifdef HAVE_OPENSSL
		if (!Ready())
			return;
		n = SSL_read(m_ssl, buf, TCP_BUFSIZE_READ);
		if (n == -1)
		{
			n = SSL_get_error(m_ssl, n);
			switch (n)
			{
			case SSL_ERROR_NONE:
				break;
			case SSL_ERROR_ZERO_RETURN:
DEB(				printf("SSL_read() returns zero - closing socket\n");)
				SetCloseAndDelete(true);
				SetFlushBeforeClose(false);
				SetLost();
				break;
			default:
DEB(				printf("SSL read problem, errcode = %d\n",n);)
				SetCloseAndDelete(true);
				SetFlushBeforeClose(false);
				SetLost();
			}
			return;
		}
		else
		if (!n)
		{
			Handler().LogError(this, "SSL_read", 0, "read returns 0", LOG_LEVEL_FATAL);
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
			SetLost();
			SetConnected(false); // avoid shutdown in Close()
			return;
		}
		else
		{
			m_bytes_received += n;
			OnRawData(buf,n);
			if (!m_b_input_buffer_disabled && !ibuf.Write(buf,n))
			{
				// overflow
				Handler().LogError(this, "OnRead(ssl)", 0, "ibuf overflow", LOG_LEVEL_WARNING);
			}
		}
#endif // HAVE_OPENSSL
	}
	else
	{
		n = recv(GetSocket(), buf, TCP_BUFSIZE_READ, MSG_NOSIGNAL);
		if (n == -1)
		{
			Handler().LogError(this, "read", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
			SetLost();
			return;
		}
		else
		if (!n)
		{
//			Handler().LogError(this, "read", 0, "read returns 0", LOG_LEVEL_FATAL);
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
			SetLost();
			SetConnected(false); // avoid shutdown in Close()
			return;
		}
		else
		{
			m_bytes_received += n;
			OnRawData(buf,n);
			if (!m_b_input_buffer_disabled && !ibuf.Write(buf,n))
			{
				// overflow
				Handler().LogError(this, "OnRead", 0, "ibuf overflow", LOG_LEVEL_WARNING);
			}
		}
	}
	// unbuffered
	if (LineProtocol())
	{
		size_t x = 0;

		buf[n] = 0;
		for (int i = 0; i < n; i++)
		{
			while (buf[i] == 13 || buf[i] == 10)
			{
				char c = buf[i];
				buf[i] = 0;
				if (buf[x])
				{
					m_line += (buf + x);
				}
				OnLine( m_line );
				i++;
				if (i < n && (buf[i] == 13 || buf[i] == 10) && buf[i] != c)
				{
					i++;
				}
				x = i;
				m_line = "";
			}
		}
		if (buf[x])
		{
			m_line += (buf + x);
		}
	}
	if (m_b_input_buffer_disabled)
	{
		return;
	}
	// further processing: socks4 and line protocol
	if (Socks4())
	{
		bool need_more = false;
		while (GetInputLength() && !need_more && !CloseAndDelete())
		{
			need_more = OnSocks4Read();
		}
	}
/*
	else
	if (LineProtocol())
	{
		size_t x = 0;
		size_t n = ibuf.GetLength();
#ifdef SOCKETS_DYNAMIC_TEMP
		char *tmp = m_buf;
#else
		char tmp[TCP_BUFSIZE_READ + 1];
#endif
		n = (n >= TCP_BUFSIZE_READ) ? TCP_BUFSIZE_READ : n;
		ibuf.Read(tmp,n);
		tmp[n] = 0;

		for (size_t i = 0; i < n; i++)
		{
			while (tmp[i] == 13 || tmp[i] == 10)
			{
				char c = tmp[i];
				tmp[i] = 0;
				if (tmp[x])
				{
					m_line += (tmp + x);
				}
				OnLine( m_line );
				i++;
				if (i < n && (tmp[i] == 13 || tmp[i] == 10) && tmp[i] != c)
				{
					i++;
				}
				x = i;
				m_line = "";
			}
		}
		if (tmp[x])
		{
			m_line += (tmp + x);
		}
	}
*/
}


void TcpSocket::OnWrite()
{
	if (Connecting())
	{
		if (CheckConnect())
		{
			SetCallOnConnect();
			return;
		}
		// failed
		if (Socks4())
		{
			OnSocks4ConnectFailed();
			return;
		}
		if (GetConnectionRetry() == -1 ||
			(GetConnectionRetry() && GetConnectionRetries() < GetConnectionRetry()) )
		{
			// even though the connection failed at once, only retry after
			// the connection timeout.
			// should we even try to connect again, when CheckConnect returns
			// false it's because of a connection error - not a timeout...
			return;
		}
		SetConnecting(false);
		SetCloseAndDelete( true );
		/// \todo state reason why connect failed
		OnConnectFailed();
		return;
	}
	if (IsSSL())
	{
#ifdef HAVE_OPENSSL
		int n = SSL_write(m_ssl,obuf.GetStart(),(int)obuf.GetLength());
		if (n == -1)
		{
			// check code
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
DEB(			perror("SSL_write() error");)
			SetLost();
		}
		else
		if (!n)
		{
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
DEB(			printf("SSL_write() returns 0\n");)
			SetLost();
		}
		else
		{
			m_bytes_sent += n;
			obuf.Remove(n);
			// move data from m_mes to immediate output buffer
			while (obuf.Space() && m_mes.size())
			{
				ucharp_v::iterator it = m_mes.begin();
				MES *p = *it;
				if (obuf.Space() > p -> left())
				{
					obuf.Write(p -> curbuf(),p -> left());
					delete p;
					m_mes.erase(it);
				}
				else
				{
					size_t sz = obuf.Space();
					obuf.Write(p -> curbuf(),sz);
					p -> ptr += sz;
				}
			}
		}
		//
		{
			bool br;
			bool bw;
			bool bx;
			Handler().Get(GetSocket(), br, bw, bx);
			if (obuf.GetLength() || m_mes.size())
				Set(br, true);
			else
				Set(br, false);
		}
		return;
#endif // HAVE_OPENSSL
	}
	int n = send(GetSocket(),obuf.GetStart(),(int)obuf.GetLength(),MSG_NOSIGNAL);
/*
When writing onto a connection-oriented socket that has been shut down (by the  local
or the remote end) SIGPIPE is sent to the writing process and EPIPE is returned.  The
signal is not sent when the write call specified the MSG_NOSIGNAL flag.
*/
	if (n == -1)
	{
	// normal error codes:
	// WSAEWOULDBLOCK
	//       EAGAIN or EWOULDBLOCK
#ifdef _WIN32
		if (Errno != WSAEWOULDBLOCK)
#else
		if (Errno != EWOULDBLOCK)
#endif
		{	
			Handler().LogError(this, "write", Errno, StrError(Errno), LOG_LEVEL_FATAL);
			SetCloseAndDelete(true);
			SetFlushBeforeClose(false);
			SetLost();
		}
	}
	else
	if (!n)
	{
//		SetCloseAndDelete(true);
	}
	else
	{
		m_bytes_sent += n;
		obuf.Remove(n);
		// move data from m_mes to immediate output buffer
		while (obuf.Space() && m_mes.size())
		{
			ucharp_v::iterator it = m_mes.begin();
			MES *p = *it;
			if (obuf.Space() > p -> left())
			{
				obuf.Write(p -> curbuf(),p -> left());
				delete p;
				m_mes.erase(it);
			}
			else
			{
				size_t sz = obuf.Space();
				obuf.Write(p -> curbuf(),sz);
				p -> ptr += sz;
			}
		}
	}
	// set write flag if data still left to write
	{
		bool br;
		bool bw;
		bool bx;
		Handler().Get(GetSocket(), br, bw, bx);
		if (obuf.GetLength() || m_mes.size())
			Set(br, true);
		else
			Set(br, false);
	}
}


void TcpSocket::Send(const std::string &str,int i)
{
	SendBuf(str.c_str(),str.size(),i);
}


void TcpSocket::SendBuf(const char *buf,size_t len,int)
{
	size_t n = obuf.GetLength();
	if (!Ready() && !Connecting())
	{
		Handler().LogError(this, "SendBuf", -1, "Attempt to write to a non-ready socket" ); // warning
		if (GetSocket() == INVALID_SOCKET)
			Handler().LogError(this, "SendBuf", 0, " * GetSocket() == INVALID_SOCKET", LOG_LEVEL_INFO);
		if (Connecting())
			Handler().LogError(this, "SendBuf", 0, " * Connecting()", LOG_LEVEL_INFO);
		if (CloseAndDelete())
			Handler().LogError(this, "SendBuf", 0, " * CloseAndDelete()", LOG_LEVEL_INFO);
		return;
	}
	if (!IsConnected())
	{
		Handler().LogError(this, "SendBuf", -1, "Attempt to write to a non-connected socket, will be sent on connect" ); // warning
	}
	//
	size_t ptr = 0;
	if (!m_mes.size() && obuf.Space())
	{
		if (len <= obuf.Space()) // entire block of data fits
		{
			ptr = len;
			obuf.Write(buf, len);
		}
		else // a part of the block fits
		{
			ptr = obuf.Space();
			obuf.Write(buf, ptr);
		}
	}
	/// \todo check good value for max blocksize
#define MAX_BLOCKSIZE 1024000
	while (ptr < len) // data still left to buffer
	{
		size_t sz = len - ptr; // size of data block that has not been buffered
		if (sz > MAX_BLOCKSIZE)
			sz = MAX_BLOCKSIZE;
		m_mes.push_back(new MES(buf + ptr, sz));
		ptr += sz;
	}
	// kick a real write
	if (!n && IsConnected())
	{
		OnWrite();
	}
}


void TcpSocket::OnLine(const std::string& )
{
}


/*
void TcpSocket::ReadLine()
{
	if (ibuf.GetLength())
	{
		size_t x = 0;
		size_t n = ibuf.GetLength();
#ifdef SOCKETS_DYNAMIC_TEMP
		char *tmp = m_buf;
#else
		char tmp[TCP_BUFSIZE_READ + 1];
#endif
		n = (n >= TCP_BUFSIZE_READ) ? TCP_BUFSIZE_READ : n;
		ibuf.Read(tmp,n);
		tmp[n] = 0;

		for (size_t i = 0; i < n; i++)
		{
			while (tmp[i] == 13 || tmp[i] == 10)
			{
				char c = tmp[i];
				tmp[i] = 0;
				if (tmp[x])
				{
					m_line += (tmp + x);
				}
				OnLine( m_line );
				i++;
				if (i < n && (tmp[i] == 13 || tmp[i] == 10) && tmp[i] != c)
				{
					i++;
				}
				x = i;
				m_line = "";
			}
		}
		if (tmp[x])
		{
			m_line += (tmp + x);
		}
	}
}
*/


#ifdef _WIN32
#pragma warning(disable:4355)
#endif
TcpSocket::TcpSocket(const TcpSocket& s)
:Socket(s)
,ibuf(*this,0)
,obuf(*this,0)
{
}
#ifdef _WIN32
#pragma warning(default:4355)
#endif


void TcpSocket::OnSocks4Connect()
{
	char request[1000];
	memset(request, 0, sizeof(request));
	request[0] = 4; // socks v4
	request[1] = 1; // command code: CONNECT
	{
		SocketAddress *ad = GetClientRemoteAddress();
		if (ad)
		{
			struct sockaddr *p0 = (struct sockaddr *)*ad;
			struct sockaddr_in *p = (struct sockaddr_in *)p0;
			if (p -> sin_family == AF_INET)
			{
				memcpy(request + 2, &p -> sin_port, 2); // nwbo is ok here
				memcpy(request + 4, &p -> sin_addr, sizeof(struct in_addr));
			}
			else
			{
				/// \todo warn
			}
		}
		else
		{
			/// \todo warn
		}
	}
	strcpy(request + 8, GetSocks4Userid().c_str());
	size_t length = GetSocks4Userid().size() + 8 + 1;
	SendBuf(request, length);
	m_socks4_state = 0;
}


void TcpSocket::OnSocks4ConnectFailed()
{
	Handler().LogError(this,"OnSocks4ConnectFailed",0,"connection to socks4 server failed, trying direct connection",LOG_LEVEL_WARNING);
	if (!Handler().Socks4TryDirect())
	{
		SetConnecting(false);
		SetCloseAndDelete();
		OnConnectFailed(); // just in case
	}
	else
	{
		SetRetryClientConnect();
	}
}


bool TcpSocket::OnSocks4Read()
{
	switch (m_socks4_state)
	{
	case 0:
		ibuf.Read(&m_socks4_vn, 1);
		m_socks4_state = 1;
		break;
	case 1:
		ibuf.Read(&m_socks4_cd, 1);
		m_socks4_state = 2;
		break;
	case 2:
		if (GetInputLength() > 1)
		{
			ibuf.Read( (char *)&m_socks4_dstport, 2);
			m_socks4_state = 3;
		}
		else
		{
			return true;
		}
		break;
	case 3:
		if (GetInputLength() > 3)
		{
			ibuf.Read( (char *)&m_socks4_dstip, 4);
			SetSocks4(false);
			
			switch (m_socks4_cd)
			{
			case 90:
				OnConnect();
				Handler().LogError(this, "OnSocks4Read", 0, "Connection established", LOG_LEVEL_INFO);
				break;
			case 91:
			case 92:
			case 93:
				Handler().LogError(this,"OnSocks4Read",m_socks4_cd,"socks4 server reports connect failed",LOG_LEVEL_FATAL);
				SetConnecting(false);
				SetCloseAndDelete();
				OnConnectFailed();
				break;
			default:
				Handler().LogError(this,"OnSocks4Read",m_socks4_cd,"socks4 server unrecognized response",LOG_LEVEL_FATAL);
				SetCloseAndDelete();
				break;
			}
		}
		else
		{
			return true;
		}
		break;
	}
	return false;
}


void TcpSocket::Sendf(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	char slask[5000]; // vsprintf / vsnprintf temporary
#ifdef _WIN32
	vsprintf(slask, format, ap);
#else
	vsnprintf(slask, 5000, format, ap);
#endif
	va_end(ap);
	Send( slask );
}


void TcpSocket::OnSSLConnect()
{
#ifdef HAVE_OPENSSL
	SetNonblocking(true);
	{
		if (m_ssl_ctx)
		{
DEB(			printf("SSL Context already initialized - closing socket\n");)
			SetCloseAndDelete(true);
			return;
		}
		InitSSLClient();
	}
	if (m_ssl_ctx)
	{
		/* Connect the SSL socket */
		m_ssl = SSL_new(m_ssl_ctx);
		if (!m_ssl)
		{
DEB(			printf(" m_ssl is NULL\n");)
		}
		m_sbio = BIO_new_socket((int)GetSocket(), BIO_NOCLOSE);
		if (!m_sbio)
		{
DEB(			printf(" m_sbio is NULL\n");)
		}
		SSL_set_bio(m_ssl, m_sbio, m_sbio);
		if (!SSLNegotiate())
			SetSSLNegotiate();
	}
	else
	{
		SetCloseAndDelete();
	}
#endif
}


void TcpSocket::OnSSLAccept()
{
#ifdef HAVE_OPENSSL
	SetNonblocking(true);
	{
		if (m_ssl_ctx)
		{
DEB(			printf("SSL Context already initialized - closing socket\n");)
			SetCloseAndDelete(true);
			return;
		}
		InitSSLServer();
		SetSSLServer();
	}
	if (m_ssl_ctx)
	{
		m_ssl = SSL_new(m_ssl_ctx);
		if (!m_ssl)
		{
DEB(			printf(" m_ssl is NULL\n");)
		}
		m_sbio = BIO_new_socket((int)GetSocket(), BIO_NOCLOSE);
		if (!m_sbio)
		{
DEB(			printf(" m_sbio is NULL\n");)
		}
		SSL_set_bio(m_ssl, m_sbio, m_sbio);
		if (!SSLNegotiate())
			SetSSLNegotiate();
	}
#endif
}


bool TcpSocket::SSLNegotiate()
{
#ifdef HAVE_OPENSSL
	if (!IsSSLServer()) // client
	{
		int r = SSL_connect(m_ssl);
		if (r > 0)
		{
			SetSSLNegotiate(false);
			/// \todo: resurrect certificate check... client
//			CheckCertificateChain( "");//ServerHOST);
			SetNonblocking(false);
			//
			{
				SetConnected();
				if (GetOutputLength())
				{
					OnWrite();
				}
			}
			if (IsReconnect())
				OnReconnect();
			else
			{
//				Handler().LogError(this, "Calling OnConnect", 0, "SSLNegotiate", LOG_LEVEL_INFO);
				OnConnect();
			}
//			OnConnect();
			Handler().LogError(this, "SSLNegotiate", 0, "Connection established", LOG_LEVEL_INFO);
			return true;
		}
		else
		if (!r)
		{
			SetSSLNegotiate(false);
			SetCloseAndDelete();
			OnSSLConnectFailed();
		}
		else
		{
			r = SSL_get_error(m_ssl, r);
			if (r != SSL_ERROR_WANT_READ && r != SSL_ERROR_WANT_WRITE)
			{
DEB(				printf("SSL_connect() failed - closing socket, return code: %d\n",r);)
				SetSSLNegotiate(false);
				SetCloseAndDelete(true);
				OnSSLConnectFailed();
			}
		}
	}
	else // server
	{
		int r = SSL_accept(m_ssl);
		if (r > 0)
		{
			SetSSLNegotiate(false);
			/// \todo: resurrect certificate check... server
//			CheckCertificateChain( "");//ClientHOST);
			SetNonblocking(false);
			//
			{
				SetConnected();
				if (GetOutputLength())
				{
					OnWrite();
				}
			}
			OnAccept();
			return true;
		}
		else
		if (!r)
		{
			SetSSLNegotiate(false);
			SetCloseAndDelete();
			OnSSLAcceptFailed();
		}
		else
		{
			r = SSL_get_error(m_ssl, r);
			if (r != SSL_ERROR_WANT_READ && r != SSL_ERROR_WANT_WRITE)
			{
DEB(				printf("SSL_accept() failed - closing socket, return code: %d\n",r);)
				SetSSLNegotiate(false);
				SetCloseAndDelete(true);
				OnSSLAcceptFailed();
			}
		}
	}
#endif // HAVE_OPENSSL
	return false;
}


void TcpSocket::InitSSLClient()
{
#ifdef HAVE_OPENSSL
//	InitializeContext();
	InitializeContext(SSLv23_method());
#endif
}


void TcpSocket::InitSSLServer()
{
	Handler().LogError(this, "InitSSLServer", 0, "You MUST implement your own InitSSLServer method", LOG_LEVEL_FATAL);
	SetCloseAndDelete();
}


#ifdef HAVE_OPENSSL
void TcpSocket::InitializeContext(SSL_METHOD *meth_in)
{
	SSL_METHOD *meth;

	if (!bio_err)
	{
		/* An error write context */
		bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

		/* Global system initialization*/
		SSL_library_init();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();
	}

	/* Create our context*/
	meth = meth_in ? meth_in : SSLv3_method();
	m_ssl_ctx = SSL_CTX_new(meth);

	/* Load the CAs we trust*/
/*
	if (!(SSL_CTX_load_verify_locations(m_ssl_ctx, CA_LIST, 0)))
	{
DEB(		printf("Couldn't read CA list\n");)
	}
	SSL_CTX_set_verify_depth(m_ssl_ctx, 1);
	SSL_CTX_set_verify(m_ssl_ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_cb);
*/

	/* Load randomness */
	if (!m_rand_file.size() || !RAND_load_file(m_rand_file.c_str(), m_rand_size))
	{
		Handler().LogError(this, "TcpSocket InitializeContext", 0, "Couldn't load randomness", LOG_LEVEL_ERROR);
	}
		
}


void TcpSocket::InitializeContext(const std::string& keyfile,const std::string& password,SSL_METHOD *meth_in)
{
	SSL_METHOD *meth;

	if (!bio_err)
	{
		/* An error write context */
		bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

		/* Global system initialization*/
		SSL_library_init();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();
	}

	/* Create our context*/
	meth = meth_in ? meth_in : SSLv3_method();
	m_ssl_ctx = SSL_CTX_new(meth);

	/* Load our keys and certificates*/
	if (!(SSL_CTX_use_certificate_file(m_ssl_ctx, keyfile.c_str(), SSL_FILETYPE_PEM)))
	{
DEB(		printf("Couldn't read certificate file\n");)
	}

	m_password = password;
	SSL_CTX_set_default_passwd_cb(m_ssl_ctx, password_cb);
	SSL_CTX_set_default_passwd_cb_userdata(m_ssl_ctx, this);
	if (!(SSL_CTX_use_PrivateKey_file(m_ssl_ctx, keyfile.c_str(), SSL_FILETYPE_PEM)))
	{
DEB(		printf("Couldn't read key file\n");)
	}

	/* Load the CAs we trust*/
/*
	if (!(SSL_CTX_load_verify_locations(m_ssl_ctx, CA_LIST, 0)))
	{
DEB(		printf("Couldn't read CA list\n");)
	}
	SSL_CTX_set_verify_depth(m_ssl_ctx, 1);
	SSL_CTX_set_verify(m_ssl_ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_cb);
*/

	/* Load randomness */
	if (!m_rand_file.size() || !RAND_load_file(m_rand_file.c_str(), m_rand_size))
	{
		Handler().LogError(this, "TcpSocket InitializeContext", 0, "Couldn't load randomness", LOG_LEVEL_ERROR);
	}
		
}


// static
int TcpSocket::password_cb(char *buf,int num,int rwflag,void *userdata)
{
	Socket *p0 = static_cast<Socket *>(userdata);
	TcpSocket *p = dynamic_cast<TcpSocket *>(p0);
	std::string pw = p ? p -> GetPassword() : "";
	if ( (size_t)num < pw.size() + 1)
	{
		return 0;
	}
	strcpy(buf,pw.c_str());
	return (int)pw.size();
}
#else
void TcpSocket::InitializeContext(SSL_METHOD *) {}
void TcpSocket::InitializeContext(const std::string& ,const std::string& ,SSL_METHOD *) {}
int TcpSocket::password_cb(char *,int ,int ,void *) { return 0; }
#endif // HAVE_OPENSSL


int TcpSocket::Close()
{
#ifdef HAVE_OPENSSL
	if (IsSSL() && m_ssl)
		SSL_shutdown(m_ssl);
	if (m_ssl)
	{
		SSL_free(m_ssl);
		m_ssl = NULL;
	}
	if (m_ssl_ctx)
	{
		SSL_CTX_free(m_ssl_ctx);
		m_ssl_ctx = NULL;
	}
#endif
	return Socket::Close();
}


SSL_CTX *TcpSocket::GetSslContext()
{
	if (!m_ssl_ctx)
		Handler().LogError(this, "GetSslContext", 0, "SSL Context is NULL; check InitSSLServer/InitSSLClient", LOG_LEVEL_WARNING);
	return m_ssl_ctx;
}

SSL *TcpSocket::GetSsl()
{
	if (!m_ssl)
		Handler().LogError(this, "GetSsl", 0, "SSL is NULL; check InitSSLServer/InitSSLClient", LOG_LEVEL_WARNING);
	return m_ssl;
}


void TcpSocket::SetReconnect(bool x)
{
	m_b_reconnect = x;
}


void TcpSocket::OnRawData(const char *buf_in,size_t len)
{
}


size_t TcpSocket::GetInputLength()
{
	return ibuf.GetLength();
}


size_t TcpSocket::GetOutputLength()
{
	return obuf.GetLength();
}


uint64_t TcpSocket::GetBytesReceived(bool clear)
{
	uint64_t z = m_bytes_received;
	if (clear)
		m_bytes_received = 0;
	return z;
}


uint64_t TcpSocket::GetBytesSent(bool clear)
{
	uint64_t z = m_bytes_sent;
	if (clear)
		m_bytes_sent = 0;
	return z;
}


bool TcpSocket::Reconnect()
{
	return m_b_reconnect;
}


void TcpSocket::SetIsReconnect(bool x)
{
	m_b_is_reconnect = x;
}


bool TcpSocket::IsReconnect()
{
	return m_b_is_reconnect;
}


const std::string& TcpSocket::GetPassword()
{
	return m_password;
}


void TcpSocket::SetRandFile(const std::string& file,long size)
{
	m_rand_file = file;
	m_rand_size = size;
	FILE *fil = fopen(file.c_str(), "wb");
	if (fil)
	{
		for (long i = 0; i < size; i++)
		{
#ifdef _WIN32
			long rnd = rand();
#else
			long rnd = random();
#endif
			fwrite(&rnd, 1, 1, fil);
		}
		fclose(fil);
	}
	else
	{
		Handler().LogError(this, "TcpSocket SetRandFile", 0, "Couldn't write to random file", LOG_LEVEL_ERROR);
	}
}


void TcpSocket::DeleteRandFile()
{
	if (m_rand_file.size())
	{
		unlink(m_rand_file.c_str());
	}
}


void TcpSocket::DisableInputBuffer(bool x)
{
	m_b_input_buffer_disabled = x;
}


void TcpSocket::OnOptions(int family,int type,int protocol,SOCKET s)
{
DEB(printf("Socket::OnOptions()\n");)
/*
	Handler().LogError(this, "OnOptions", family, "Address Family", LOG_LEVEL_INFO);
	Handler().LogError(this, "OnOptions", type, "Type", LOG_LEVEL_INFO);
	Handler().LogError(this, "OnOptions", protocol, "Protocol", LOG_LEVEL_INFO);
*/
	SetReuse(true);
	SetKeepalive(true);
}


void TcpSocket::SetLineProtocol(bool x)
{
	Socket::SetLineProtocol(x);
	DisableInputBuffer(x);
}


#ifdef SOCKETS_NAMESPACE
}
#endif

