#ifndef _POOLSOCKET_H
#define _POOLSOCKET_H

#include "Socket.h"


class SocketHandler;

class PoolSocket : public Socket
{
public:
	PoolSocket(SocketHandler& h,Socket *src);
	~PoolSocket();

	void OnRead();

private:
	PoolSocket(const PoolSocket& s) : Socket(s) {} // copy constructor
	PoolSocket& operator=(const PoolSocket& ) { return *this; } // assignment operator
};




#endif // _POOLSOCKET_H
