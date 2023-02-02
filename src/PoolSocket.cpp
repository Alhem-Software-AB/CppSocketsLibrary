//#include <stdio.h>
#include "SocketHandler.h"
#include "PoolSocket.h"




PoolSocket::PoolSocket(SocketHandler& h,Socket *src)
:Socket(h)
{
	CopyConnection( src );

	SetIsClient();
}


PoolSocket::~PoolSocket()
{
}


void PoolSocket::OnRead() 
{
	Handler().LogError(this, "OnRead", 0, "data on hibernating socket", LOG_LEVEL_FATAL);
	SetCloseAndDelete();
}


