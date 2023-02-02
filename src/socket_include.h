#ifndef _SOCKET_INCLUDE_H
#define _SOCKET_INCLUDE_H

#ifdef SOLARIS
//typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

#elif defined MACOSX
typedef unsigned long ipaddr_t;
//typedef unsigned short port_t;
#define s6_addr16 __u6_addr.__u6_addr16
#define MSG_NOSIGNAL 0 // oops - thanks Derek
#define SOL_IP IPPROTO_IP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP

#else // LINUX / _WIN32
typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

#endif


#ifdef _USING_IPV6
#define CreateSocket CreateSocket6
#define GetRemoteIP GetRemoteIP6
#define Bind Bind6
#define Open Open6
#define SendTo SendTo6
#define SendToBuf SendToBuf6
#else
#define CreateSocket CreateSocket4
#define GetRemoteIP GetRemoteIP4
#define Bind Bind4
#define Open Open4
#define SendTo SendTo4
#define SendToBuf SendToBuf4
#endif


// ---> note!
// (modified) rip from httpsync.c

/**********************************************************/
/*   WinSock and BSD-style sockets portability            */
/* Macros and conditionals for portable sockets code
 * 1. Write code using readsocket, writesocket, closesocket,
 *    INVALID_SOCKET, SOCKET_ERROR, SOCKET, and INADDR_NONE
 * 2. Always use SocketStartup() and SocketCleanup()
 * 3. Conditional includes and definitions for those macros
 *    allow operation under Windows and BSD-style sockets.
 */

#ifdef WIN32 /* Windows systems */

/*
0  	swShutdownRead  	Disable reception of data.
1 	swShutdownWrite 	Disable transmissioin of data.
2 	swShutdownReadWrite
swShutdownBoth 	Disable both reception and transmission of data.
*/

#define SHUT_RDWR 2
typedef int socklen_t;
#include <winsock.h>
#define readsocket(a,b,c) recv(a,b,c,0)
#define writesocket(a,b,c) send(a,b,c,0)

/* closesocket() does not need a macro.
 * INVALID_SOCKET, SOCKET_ERROR, SOCKET, and INADDR_NONE
 * are already defined in winsock.h
 */

#define SocketStartup() \
    if (WSAStartup(0x101,&libmibWSAdata)) exit(-1)
#define SocketCleanup() WSACleanup()

#else /* Unix-style systems */

#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#define readsocket read
//#define writesocket write
#define readsocket(a,b,c) recv(a,b,c,MSG_NOSIGNAL)
#define writesocket(a,b,c) send(a,b,c,MSG_NOSIGNAL)
#define closesocket close
#define SocketStartup()
#define SocketCleanup()
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
/* define INADDR_NONE if not already */
#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

#endif

// end of (modified) rip from httpsync.c


#endif // _SOCKET_INCLUDE_H
