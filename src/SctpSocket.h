/**
 **	\file SctpSocket.h
 **	\date  2006-09-04
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2006  Anders Hedstrom

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
#ifndef _SOCKETS_NAMESPACE_SCTPSOCKET_H
#define _SOCKETS_NAMESPACE_SCTPSOCKET_H

#include <map>
#include "Socket.h"
#ifdef USE_SCTP
#include <netinet/sctp.h>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#define SCTP_BUFSIZE_READ 16400


class SctpSocket : public Socket
{
public:
	/** SctpSocket constructor.
		\param h Owner
		\param type SCTP_STREAM or SCTP_SEQPACKET */
	SctpSocket(SocketHandler& h,int type);
	~SctpSocket();

	int CreateSa(struct sockaddr_in&,const std::string&,port_t);
#ifdef IPPROTO_IPV6
	int CreateSa(struct sockaddr_in6&,const std::string&,port_t);
#endif

	/** bind() */
	int Bind(const std::string&,port_t);
	/** sctp_bindx() */
	int AddAddress(const std::string&,port_t);
	/** sctp_bindx() */
	int RemoveAddress(const std::string&,port_t);

	/** connect() */
	int Open(const std::string&,port_t);
	/** sctp_connectx() */
	int AddConnection(const std::string&,port_t);

	/** Get peer addresses of an association. */
	int getpaddrs(sctp_assoc_t id,std::list<std::string>&);
	/** Get all bound addresses of an association. */
	int getladdrs(sctp_assoc_t id,std::list<std::string>&);

	/** sctp_peeloff */
	int PeelOff(sctp_assoc_t id);

	/** recvmsg callback */
	virtual void OnReceiveMessage(const char *buf,size_t sz,struct sockaddr *sa,socklen_t sa_len,struct sctp_sndrcvinfo *sinfo,int msg_flags) = 0;

protected:
	void OnRead();

private:
	int m_type; ///< SCTP_STREAM or SCTP_SEQPACKET
	char *m_buf; ///< Temporary receive buffer
};


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE
#endif

#endif // USE_SCTP
#endif // _SOCKETS_NAMESPACE_SCTPSOCKET_H
