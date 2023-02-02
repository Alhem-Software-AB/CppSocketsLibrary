#ifndef _SOCKETS_SEMAPHORE_H
#define _SOCKETS_SEMAPHORE_H

#include <pthread.h>
#include <semaphore.h>


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** pthread semaphore wrapper.
	\ingroup threading */
class Semaphore
{
public:
	Semaphore();
	Semaphore(unsigned int start_val);
	~Semaphore();

	int Post();
	int Wait();
	int TryWait();
	int GetValue(int&);

private:
	Semaphore(const Semaphore& ) {} // copy constructor
	Semaphore& operator=(const Semaphore& ) { return *this; } // assignment operator
	sem_t m_sem;
};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // _SOCKETS_SEMAPHORE_H
