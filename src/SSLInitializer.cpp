/**
 **	\file SSLInitializer.cpp
 **	\date  2007-04-30
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2007-2008  Anders Hedstrom

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
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <io.h>
#endif
#include "SSLInitializer.h"
#ifdef HAVE_OPENSSL
#include <map>
#include "Utility.h"
#include <openssl/rand.h>
#include "Lock.h"

#ifdef _DEBUG
#define DEB(x) x
#else
#define DEB(x)
#endif


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


std::map<int, Mutex *> SSLInitializer::m_mmap;
Mutex SSLInitializer::m_mmap_mutex;


SSLInitializer::SSLInitializer()
{
DEB(	fprintf(stderr, "SSLInitializer()\n");)

	bio_err = NULL;
	m_rand_size = 1024;

	/* An error write context */
	bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

	/* Global system initialization*/
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	CRYPTO_set_locking_callback( SSL_locking_function );
	CRYPTO_set_id_callback( SSL_id_function );

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
DEB(		fprintf(stderr, "SSLInitializer: no random file generated\n");)
	}

	/* Load randomness */
	if (!m_rand_file.size() || !RAND_load_file(m_rand_file.c_str(), m_rand_size))
	{
DEB(		fprintf(stderr, "SSLInitializer: PRNG not initialized\n");)
	}

}


SSLInitializer::~SSLInitializer()
{
DEB(	fprintf(stderr, "~SSLInitializer()\n");)
	DeleteRandFile();
	// %! delete mutexes
}


void SSLInitializer::DeleteRandFile()
{
	if (m_rand_file.size())
	{
		unlink(m_rand_file.c_str());
	}
}


void SSLInitializer::SSL_locking_function(int mode, int n, const char *file, int line)
{
	Mutex *mutex = NULL;
	{
		Lock lock(m_mmap_mutex);
		if (m_mmap.find(n) == m_mmap.end())
		{
			m_mmap[n] = new Mutex;
		}
		mutex = m_mmap[n];
	}
	if (mode & CRYPTO_LOCK)
	{
		mutex -> Lock();
	}
	else
	{
		mutex -> Unlock();
	}
}


unsigned long SSLInitializer::SSL_id_function()
{
	return Utility::ThreadID();
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // HAVE_OPENSSL

