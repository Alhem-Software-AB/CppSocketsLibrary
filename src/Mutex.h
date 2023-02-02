/*
 **	File ......... Mutex.h
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
#ifndef _MUTEX_H
#define _MUTEX_H

#ifndef _WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** Mutex container class, used by Lock. 
	\ingroup threading */
class Mutex
{
	friend class Lock;
public:
	Mutex();
	~Mutex();

private:
	void Lock();
	void Unlock();
#ifdef _WIN32
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
};


#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _MUTEX_H
