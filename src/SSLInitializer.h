/**
 **	\file SSLInitializer.h
 **	\date  2007-04-30
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2007  Anders Hedstrom

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
#ifndef __SOCKETS_SSLInitializer_H
#define __SOCKETS_SSLInitializer_H
#include "sockets-config.h"
#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>
#include <string>


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class SSLInitializer
{
public:
	/**
		init openssl
		bio_err
		create random file
	*/
	SSLInitializer();

	/**
		remove random file
	*/
	~SSLInitializer();

static	void DeleteRandFile();

	/** SSL; mutex locking function callback. */
static	void SSL_locking_function(int mode, int n, const char *file, int line);

	/** Return thread id. */
static	unsigned long SSL_id_function();

static	BIO *bio_err;

private:
static	std::string m_rand_file;
static	long m_rand_size;

};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // HAVE_OPENSSL
#endif // __SOCKETS_SSLInitializer_H
