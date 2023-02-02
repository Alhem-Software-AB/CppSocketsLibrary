//#include <stdio.h>

#include "Semaphore.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


Semaphore::Semaphore()
{
	sem_init(&m_sem, 0, 0);
}


Semaphore::Semaphore(unsigned int start_val)
{
	sem_init(&m_sem, 0, start_val);
}


Semaphore::~Semaphore()
{
	sem_destroy(&m_sem);
}


int Semaphore::Post()
{
	return sem_post(&m_sem);
}


int Semaphore::Wait()
{
	return sem_wait(&m_sem);
}


int Semaphore::TryWait()
{
	return sem_trywait(&m_sem);
}


int Semaphore::GetValue(int& i)
{
	return sem_getvalue(&m_sem, &i);
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
