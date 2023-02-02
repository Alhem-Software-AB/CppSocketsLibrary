#ifndef _SOCKET_INCLUDE_H
#define _SOCKET_INCLUDE_H

#ifdef SOLARIS 
// ----------------------------------------
// Solaris
typedef unsigned short port_t;

#elif defined MACOSX 
// ----------------------------------------
// Mac OS X
typedef unsigned long ipaddr_t;
#define s6_addr16 __u6_addr.__u6_addr16
#define MSG_NOSIGNAL 0 // oops - thanks Derek
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP

#elif defined _WIN32 
// ----------------------------------------
// Win32
#pragma comment(lib, "wsock32.lib")
typedef unsigned long ipaddr_t;
typedef unsigned short port_t;
typedef int socklen_t;
#define MSG_NOSIGNAL 0
#define SHUT_RDWR 2
#include <winsock.h>

#define Errno WSAGetLastError()
const char *StrError(int x);

// class WSAInitializer is a part of the Socket class (on win32)
// as a static instance - so whenever an application uses a Socket,
// winsock is initialized
class WSAInitializer // Winsock Initializer
{
public:
	WSAInitializer() {
		if (WSAStartup(0x101,&m_wsadata)) 
		{
			exit(-1);
		}
	}
	~WSAInitializer() {
		WSACleanup();
	}
private:
	WSADATA m_wsadata;
};

#else 
// ----------------------------------------
// LINUX 
typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

#endif

// ----------------------------------------
// Generic
#ifndef SOL_IP
#define SOL_IP IPPROTO_IP
#endif

#ifndef _WIN32 
// ----------------------------------------
// common unix includes / defines
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define Errno errno
#define StrError strerror

// WIN32 adapt
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif // INADDR_NONE

#endif // _WIN32


#endif // _SOCKET_INCLUDE_H
