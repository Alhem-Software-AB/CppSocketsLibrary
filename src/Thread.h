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
#ifdef _WIN32
  int m_thread;
#else
	pthread_t m_thread;
#endif
	bool m_running;
	bool m_release;
};


#endif // _THREAD_H
