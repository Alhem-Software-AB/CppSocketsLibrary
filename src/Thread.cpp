#include <stdio.h>
#ifdef _WIN32
#include "socket_include.h"
#else
#include <unistd.h>
#endif

#include "Thread.h"


Thread::Thread(bool release)
:m_thread(0)
,m_running(true)
,m_release(false)
{
#ifdef _WIN32
#else
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (pthread_create(&m_thread,&attr,StartThread,this) == -1)
	{
		perror("Thread: create failed");
		SetRunning(false);
	}
//	pthread_attr_destroy(&attr);
#endif
	m_release = release;
}


Thread::~Thread()
{
//	while (m_running || m_thread)
	if (m_running)
	{
		SetRunning(false);
		SetRelease(true);

#ifdef _WIN32
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    select(0,NULL,NULL,NULL,&tv);
#else
    sleep(1);
#endif
	}
}


threadfunc_t Thread::StartThread(void *zz)
{
	Thread *pclThread = (Thread *)zz;

	while (pclThread -> m_running && !pclThread -> m_release)
	{
#ifdef _WIN32
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    select(0,NULL,NULL,NULL,&tv);
#else
    sleep(1);
#endif
	}
	if (pclThread -> m_running)
	{
		pclThread -> Run();
	}
	pclThread -> SetRunning(false); // if return
	return zz;
}


bool Thread::IsRunning() 
{
 	return m_running; 
}


void Thread::SetRunning(bool x) 
{
 	m_running = x; 
}


bool Thread::IsReleased() 
{
 	return m_release; 
}


void Thread::SetRelease(bool x) 
{
 	m_release = x; 
}


