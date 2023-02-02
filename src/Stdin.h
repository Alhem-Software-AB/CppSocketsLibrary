#ifndef _STDIN_H
#define _STDIN_H

#include "Socket.h"


class Stdin : public Socket
{
public:
	Stdin(SocketHandler&);

	void OnRead();
	virtual void OnData(const char *,size_t) = 0;

};


#endif // _STDIN_H
