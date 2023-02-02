/**
 **	File ......... Thread.h
 **	Published ....  2004-10-30
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
#ifndef _THREAD_H
#define _THREAD_H

#ifdef _WIN32
// to be
typedef void * threadfunc_t;
#else
#include <pthread.h>

typedef void * threadfunc_t;
#endif


class Thread
{
public:
	Thread(bool release = true);
	virtual ~Thread();

	static threadfunc_t StartThread(void *);

	virtual void Run() = 0;

	bool IsRunning();
	void SetRunning(bool x);
	bool IsReleased();
	void SetRelease(bool x);

private:
	Thread(const Thread& ) {}
	Thread& operator=(const Thread& ) { return *this; }
#ifdef _WIN32
	int m_thread;
#else
	pthread_t m_thread;
#endif
	bool m_running;
	bool m_release;
};


#endif // _THREAD_H
