#include <errno.h>
#include "SocketHandler.h"
#include "Stdin.h"


Stdin::Stdin(SocketHandler& h) : Socket(h)
{
	Attach( STDIN_FILENO );
	Set(true, false, false);
}


void Stdin::OnRead()
{
	char buf[1000];
	int n = read(GetSocket(), buf, 1000); //recv(0, buf, 1000, 0);
	if (n == -1)
	{
		Handler().LogError(this, "OnRead", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
		return;
	}
	OnData(buf, n);
}


