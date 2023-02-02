/**
 **	File ......... CTcpSocket.cpp
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
#include "CTcpSocket.h"




CTcpSocket::CTcpSocket(SocketHandler& h)
:TcpSocket(h)
,m_crypt(NULL)
{
}


CTcpSocket::~CTcpSocket()
{
	if (m_crypt)
		delete m_crypt;
}


void CTcpSocket::Init()
{
	m_crypt = AllocateCrypt();
}


std::string CTcpSocket::encrypt(unsigned char *ik,const std::string& msg)
{
	return m_crypt ? m_crypt -> encrypt(ik, msg) : "";
}


bool CTcpSocket::decrypt(unsigned char *ik,const std::string& msg,std::string& output)
{
	return m_crypt ? m_crypt -> decrypt(ik, msg, output) : false;
}


