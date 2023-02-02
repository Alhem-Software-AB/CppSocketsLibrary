/**
 **	File ......... SSLSocket.h
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
#ifndef _SSLSOCKET_H
#define _SSLSOCKET_H
#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>
#include "SocketHandler.h"
#include "TcpSocket.h"

#ifdef _WIN32
#define RANDOM "systray.exe"
#else
#define RANDOM "/dev/urandom"
#endif


class SSLSocket : public TcpSocket
{
public:
	SSLSocket(SocketHandler&);
	~SSLSocket();

	virtual void InitAsClient();
	virtual void InitAsServer();

	void OnConnect(); // init as client
	void OnAccept(); // init as server

	bool SSLCheckConnect();

	void OnRead();
	void OnWrite();

	int Close();

protected:
	void InitializeContext(SSL_METHOD * = NULL);
	void InitializeContext(const std::string& keyfile,const std::string& password,SSL_METHOD * = NULL);

private:
	bool CheckCertificateChain(const std::string& );
static	int verify_cb(int ok, X509_STORE_CTX *store);
static	int password_cb(char *buf,int num,int rwflag,void *userdata);

	//
	SSL_CTX *m_context;
	bool is_client;
	bool is_server;
	SSL *m_ssl;
	BIO *m_sbio;
static	BIO *bio_err;
static	std::string m_password;
};


#endif // HAVE_OPENSSL
#endif // _SSLSOCKET_H
