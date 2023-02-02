/**
 **	File ......... SSLSocket.cpp
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
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
#ifdef HAVE_OPENSSL
#include <assert.h>
#include "SSLSocket.h"
#include <openssl/rand.h>
#ifdef _WIN32
#define strcasecmp stricmp
#endif

#define DEB(x) 
#define D2(x) 


// statics
BIO *SSLSocket::bio_err = NULL;
std::string SSLSocket::m_password = "";


SSLSocket::SSLSocket(SocketHandler& h)
:TcpSocket(h)
,m_context(NULL)
,is_client(false)
,is_server(false)
,m_ssl(NULL)
,m_sbio(NULL)
{
D2(	printf("SSLSocket()\n");)
}


SSLSocket::~SSLSocket()
{
D2(	printf("~SSLSocket()\n");)
	if (m_ssl)
	{
DEB(		printf("SSL_free()\n");)
		SSL_free(m_ssl);
	}
	if (m_context)
	{
DEB(		printf("SSL_CTX_free()\n");)
		SSL_CTX_free(m_context);
	}
}


void SSLSocket::OnConnect()
{
D2(	printf("SSLSocket::OnConnect()\n");)
	SetNonblocking(true);
	if (!is_client)
	{
		if (m_context)
		{
DEB(			printf("SSL Context already initialized - closing socket\n");)
			SetCloseAndDelete(true);
			return;
		}
DEB(		printf("InitAsClient()\n");)
		InitAsClient();
		is_client = true;
	}
	if (m_context)
	{
		/* Connect the SSL socket */
		m_ssl = SSL_new(m_context);
		if (!m_ssl)
		{
DEB(			printf(" m_ssl is NULL\n");)
		}
		m_sbio = BIO_new_socket(GetSocket(), BIO_NOCLOSE);
		if (!m_sbio)
		{
DEB(			printf(" m_sbio is NULL\n");)
		}
		SSL_set_bio(m_ssl, m_sbio, m_sbio);
		SetSSLConnecting();
		if (SSLCheckConnect())
		{
			OnSSLInitDone();
		}
	}
	else
	{
		SetCloseAndDelete();
	}
}


void SSLSocket::InitAsClient()
{
	InitializeContext();
}


void SSLSocket::OnAccept()
{
D2(	printf("SSLSocket::OnAccept()\n");)
	SetNonblocking(true);
	if (!is_server)
	{
		if (m_context)
		{
DEB(			printf("SSL Context already initialized - closing socket\n");)
			SetCloseAndDelete(true);
			return;
		}
		InitAsServer();
		is_server = true;
	}
	if (m_context)
	{
		m_ssl = SSL_new(m_context);
		if (!m_ssl)
		{
DEB(			printf(" m_ssl is NULL\n");)
		}
		m_sbio = BIO_new_socket(GetSocket(), BIO_NOCLOSE);
		if (!m_sbio)
		{
DEB(			printf(" m_sbio is NULL\n");)
		}
		SSL_set_bio(m_ssl, m_sbio, m_sbio);
		SetSSLConnecting();
		if (SSLCheckConnect())
		{
			OnSSLInitDone();
		}
	}
}


void SSLSocket::InitAsServer()
{
	assert(!"not implemented.\n");
}


bool SSLSocket::SSLCheckConnect()
{
	if (is_client) // SSL_connect
	{
D2(		printf("SSLSocket::SSLCheckConnect() is_client\n");)
		int r = SSL_connect(m_ssl);
D2(		printf(" SSLCheckConnect is_client, SSL_connect returns %d\n",r);)
		if (r > 0)
		{
			SetSSLConnecting(false);
			CheckCertificateChain( "");//ServerHOST);
			SetNonblocking(false);
			return true;
		}
		else
		if (!r)
		{
			SetSSLConnecting(false);
			SetCloseAndDelete();
		}
		else
		{
			r = SSL_get_error(m_ssl, r);
			if (r != SSL_ERROR_WANT_READ && r != SSL_ERROR_WANT_WRITE)
			{
D2(				printf("SSL_connect() failed - closing socket, return code: %d\n",r);)
				SetSSLConnecting(false);
				SetCloseAndDelete(true);
			}
		}
	}
	else
	if (is_server)
	{
D2(		printf("SSLSocket::SSLCheckConnect() is_server\n");)
		int r = SSL_accept(m_ssl);
D2(		printf(" SSLCheckConnect is_server, SSL_accept returns %d\n",r);)
		if (r > 0)
		{
			SetSSLConnecting(false);
			CheckCertificateChain( "");//ClientHOST);
			SetNonblocking(false);
			return true;
		}
		else
		if (!r)
		{
			SetSSLConnecting(false);
			SetCloseAndDelete();
		}
		else
		{
			r = SSL_get_error(m_ssl, r);
			if (r != SSL_ERROR_WANT_READ && r != SSL_ERROR_WANT_WRITE)
			{
D2(				printf("SSL_accept() failed - closing socket, return code: %d\n",r);)
				SetSSLConnecting(false);
				SetCloseAndDelete(true);
			}
		}
	}
	else
	{
D2(		printf("SSLSocket::SSLCheckConnect() is_NOTHING!!!!!!\n");)
		SetSSLConnecting(false);
		SetCloseAndDelete();
	}
	return false;
}


void SSLSocket::OnRead()
{
D2(	printf("SSLSocket::OnRead()\n");)
	if (!Ready())
		return;
	char buf[TCP_BUFSIZE_READ];
	int n = SSL_read(m_ssl, buf, TCP_BUFSIZE_READ);
	if (n == -1)
	{
		n = SSL_get_error(m_ssl, n);
		switch (n)
		{
		case SSL_ERROR_NONE:
			if (!ibuf.Write(buf, n))
			{
				// overflow
			}
			break;
		case SSL_ERROR_ZERO_RETURN:
DEB(			printf("SSL_read() returns zero - closing socket\n");)
			SetCloseAndDelete(true);
			break;
		default:
			{
DEB(				printf("SSL read problem, errcode = %d\n",n);)
			}
		}
		SetCloseAndDelete(true); // %!
DEB(		perror("read() error");)
	}
	else
	if (!n)
	{
		SetCloseAndDelete(true);
DEB(		printf("read() returns 0\n");)
	}
	else
	{
DEB(		printf("SSLSocket OnRead read %d bytes\n",n);)
//		for (size_t i = 0; i < n; i++)
//			printf("%c",buf[i]);
		if (!ibuf.Write(buf,n))
		{
			// overflow
DEB(			printf(" *** overflow ibuf Write\n");)
		}
	}
}


void SSLSocket::OnWrite()
{
/*
	if (!Handler().Valid(this))
		return;
	if (!Ready())
		return;
*/
D2(	printf("SSLSocket::OnWrite()\n");)
	int n = SSL_write(m_ssl,obuf.GetStart(),obuf.GetL());
DEB(	printf("OnWrite: %d bytes sent\n",n);)
	if (n == -1)
	{
		SetCloseAndDelete(true);
DEB(		perror("write() error");)
	}
	else
	if (!n)
	{
		SetCloseAndDelete(true);
DEB(		printf("write() returns 0\n");)
	}
	else
	{
DEB(		printf(" %d bytes written\n",n);)
		obuf.Remove(n);
	}
	{
		bool br;
		bool bw;
		bool bx;
		Handler().Get(GetSocket(), br, bw, bx);
		if (obuf.GetLength())
			Set(br, true);
		else
			Set(br, false);
	}
}


void SSLSocket::InitializeContext(SSL_METHOD *meth_in)
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
	m_context = SSL_CTX_new(meth);

	/* Load the CAs we trust*/
/*
	if (!(SSL_CTX_load_verify_locations(m_context, CA_LIST, 0)))
	{
DEB(		printf("Couldn't read CA list\n");)
	}
	SSL_CTX_set_verify_depth(m_context, 1);
	SSL_CTX_set_verify(m_context, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_cb);
*/

	/* Load randomness */
	if (!(RAND_load_file(RANDOM, 1024*1024)))
	{
DEB(		printf("Couldn't load randomness\n");)
	}
		 
}


void SSLSocket::InitializeContext(const std::string& keyfile,const std::string& password,SSL_METHOD *meth_in)
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
	m_context = SSL_CTX_new(meth);

	/* Load our keys and certificates*/
	if (!(SSL_CTX_use_certificate_file(m_context, keyfile.c_str(), SSL_FILETYPE_PEM)))
	{
DEB(		printf("Couldn't read certificate file\n");)
	}

	m_password = password;
	SSL_CTX_set_default_passwd_cb(m_context, password_cb);
	if (!(SSL_CTX_use_PrivateKey_file(m_context, keyfile.c_str(), SSL_FILETYPE_PEM)))
	{
DEB(		printf("Couldn't read key file\n");)
	}

	/* Load the CAs we trust*/
/*
	if (!(SSL_CTX_load_verify_locations(m_context, CA_LIST, 0)))
	{
DEB(		printf("Couldn't read CA list\n");)
	}
	SSL_CTX_set_verify_depth(m_context, 1);
	SSL_CTX_set_verify(m_context, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_cb);
*/

	/* Load randomness */
	if (!(RAND_load_file(RANDOM, 1024*1024)))
	{
DEB(		printf("Couldn't load randomness\n");)
	}
		 
}


// static
int SSLSocket::verify_cb(int ok, X509_STORE_CTX *store)
{
	char data[256];

	if (!ok)
	{
		X509 *cert = X509_STORE_CTX_get_current_cert(store);
		int	depth = X509_STORE_CTX_get_error_depth(store);
		int	err = X509_STORE_CTX_get_error(store);

		fprintf(stderr, "-Error with certificate at depth: %i\n", depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
		fprintf(stderr, "	issuer	 = %s\n", data);
		X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
		fprintf(stderr, "	subject	= %s\n", data);
		fprintf(stderr, "	err %i:%s\n", err, X509_verify_cert_error_string(err));
	}
	return ok;
}


// static
int SSLSocket::password_cb(char *buf,int num,int rwflag,void *userdata)
{
	if((size_t)num<m_password.size()+1)
		return(0);

	strcpy(buf,m_password.c_str());
	return(m_password.size());
}


bool SSLSocket::CheckCertificateChain(const std::string& host)
{
	X509 *peer;
	char peer_CN[256];

	if (SSL_get_verify_result(m_ssl) != X509_V_OK)
	{
DEB(		printf("Certificate doesn't verify\n");)
		return false;
	}

	/*Check the cert chain. The chain length
		is automatically checked by OpenSSL when we
		set the verify depth in the ctx */

	/*Check the common name*/
	peer = SSL_get_peer_certificate(m_ssl);
	if (!peer)
	{
		return false;
	}
	X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peer_CN, 256);
	if (host.size() && strcasecmp(peer_CN,host.c_str()))
	{
DEB(		printf("Common name doesn't match host name\n");)
		return false;
	}
	return true;
}


int SSLSocket::Close()
{
D2(	printf("SSLSocket::Close()\n");)
	if (m_ssl)
		SSL_shutdown(m_ssl);
	return Socket::Close();
}


#endif // HAVE_OPENSSL
