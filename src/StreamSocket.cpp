/**
 **	\file StreamSocket.cpp
 **	\date  2008-12-20
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2015-2023  Alhem Software AB
Copyright (C) 2008-2011  Anders Hedstrom

This library is made available under the terms of the GNU GPL, with
the additional exemption that compiling, linking, and/or using OpenSSL 
is allowed.

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
#include "StreamSocket.h"
#include "ISocketHandler.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


StreamSocket::StreamSocket(ISocketHandler& h) : Socket(h)
,m_bConnecting(false)
,m_connect_timeout(5)
,m_flush_before_close(true)
,m_connection_retry(0)
,m_retries(0)
,m_line_protocol(false)
,m_shutdown(0)
{
}


StreamSocket::~StreamSocket()
{
}


void StreamSocket::SetConnecting(bool x)
{
	if (x != m_bConnecting)
	{
		m_bConnecting = x;
		if (x)
		{
			SetTimeout( GetConnectTimeout() );
		}
		else
		{
			SetTimeout( 0 );
		}
	}
}


bool StreamSocket::Connecting()
{
	return m_bConnecting;
}


bool StreamSocket::Ready()
{
	if (GetSocket() != INVALID_SOCKET && !Connecting() && !CloseAndDelete())
		return true;
	return false;
}


void StreamSocket::SetConnectTimeout(int x)
{
	m_connect_timeout = x;
}


int StreamSocket::GetConnectTimeout()
{
	return m_connect_timeout;
}


void StreamSocket::SetFlushBeforeClose(bool x)
{
	m_flush_before_close = x;
}


bool StreamSocket::GetFlushBeforeClose()
{
	return m_flush_before_close;
}


int StreamSocket::GetConnectionRetry()
{
	return m_connection_retry;
}


void StreamSocket::SetConnectionRetry(int x)
{
	m_connection_retry = x;
}


int StreamSocket::GetConnectionRetries()
{
	return m_retries;
}


void StreamSocket::IncreaseConnectionRetries()
{
	m_retries++;
}


void StreamSocket::ResetConnectionRetries()
{
	m_retries = 0;
}


void StreamSocket::SetLineProtocol(bool x)
{
	m_line_protocol = x;
}


bool StreamSocket::LineProtocol()
{
	return m_line_protocol;
}


void StreamSocket::SetShutdown(int x)
{
	m_shutdown = x;
}


int StreamSocket::GetShutdown()
{
	return m_shutdown;
}




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

