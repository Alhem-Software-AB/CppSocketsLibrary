/*
 **	File ......... Mutex.cpp
 **	Published ....  2004-10-30
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
#include <stdio.h>
#include "Mutex.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


Mutex::Mutex()
{
#ifdef _WIN32
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
}


Mutex::~Mutex()
{
#ifdef _WIN32
	::CloseHandle(m_mutex);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}


void Mutex::Lock()
{
#ifdef _WIN32
	DWORD d = WaitForSingleObject(m_mutex, INFINITE);
	// %! check 'd' for result
#else
	pthread_mutex_lock(&m_mutex);
#endif
}


void Mutex::Unlock()
{
#ifdef _WIN32
	::ReleaseMutex(m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}


#ifdef SOCKETS_NAMESPACE
}
#endif