#include <stdio.h>
#include <string.h>
#include "socket_include.h"
/*
CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD -D_VERSION='"$(VERSION)"' 
# manual autoconf ....

# uncomment if your operating system is linux, mac os x, or solaris
#CFLAGS +=	-DLINUX
#CFLAGS +=	-DMACOSX 
#CFLAGS +=	-DSOLARIS

# uncomment if openssl is installed
CFLAGS += 	-DHAVE_OPENSSL


*/

int main(int argc,char *argv[])
{
	if (argc > 1 && !strcmp(argv[1], "-info"))
	{
#ifdef HAVE_OPENSSL
		printf("SSL support\n");
#endif
#ifdef IPPROTO_IPV6
		printf("IPv6 support\n");
#endif
#ifdef USE_SCTP
#ifdef IPPROTO_SCTP
		printf("SCTP support\n");
#	ifdef HAVE_SCTP
		printf("  HAVE_SCTP: yes\n");
#	else
		printf("  HAVE_SCTP: no\n");
#	endif
#	ifdef HAVE_KERNEL_SCTP
		printf("  HAVE_KERNEL_SCTP: yes\n");
#	else
		printf("  HAVE_KERNEL_SCTP: no\n");
#	endif
#	ifdef HAVE_SCTP_PRSCTP
		printf("  HAVE_SCTP_PRSCTP: yes\n");
#	else
		printf("  HAVE_SCTP_PRSCTP: no\n");
#	endif
#	ifdef HAVE_SCTP_ADDIP
		printf("  HAVE_SCTP_ADDIP: yes\n");
#	else
		printf("  HAVE_SCTP_ADDIP: no\n");
#	endif
#	ifdef HAVE_SCTP_CANSET_PRIMARY
		printf("  HAVE_SCTP_CANSETPRIMARY: yes\n");
#	else
		printf("  HAVE_SCTP_CANSETPRIMARY: no\n");
#	endif
#	ifdef HAVE_SCTP_SAT_NETWORK_CAPABILITY
		printf("  HAVE_SCTP_SAT_NETWORK_CAPABILITY: yes\n");
#	else
		printf("  HAVE_SCTP_SAT_NETWORK_CAPABILITY: no\n");
#	endif
#	ifdef HAVE_SCTP_MULTIBUF
		printf("  HAVE_SCTP_MULTIBUF: yes\n");
#	else
		printf("  HAVE_SCTP_MULTIBUF: no\n");
#	endif
#	ifdef HAVE_SCTP_NOCONNECT
		printf("  HAVE_SCTP_NOCONNECT: yes\n");
#	else
		printf("  HAVE_SCTP_NOCONNECT: no\n");
#	endif
#	ifdef HAVE_SCTP_EXT_RCVINFO
		printf("  HAVE_SCTP_EXT_RCVINFO: yes\n");
#	else
		printf("  HAVE_SCTP_EXT_RCVINFO: no\n");
#	endif
#else
		printf("No SCTP support\n");
#endif
#endif
		return 0;
	}
	printf(" -D_VERSION='\"%s\"'", _VERSION);

#ifdef LINUX
	printf(" -DLINUX");
#endif
#ifdef MACOSX
	printf(" -DMACOSX");
#endif
#ifdef SOLARIS
	printf(" -DSOLARIS");
#endif
#ifdef SOLARIS8
	printf(" -DSOLARIS8");
#endif
#ifdef HAVE_OPENSSL
	printf(" -DHAVE_OPENSSL");
#endif
#ifdef SOCKETS_NAMESPACE
	printf(" -DSOCKETS_NAMESPACE=%s", SOCKETS_NAMESPACE_STR);
#endif
#ifdef _DEBUG
	printf(" -D_DEBUG");
#endif
#ifdef USE_SCTP
	printf(" -DUSE_SCTP");
#endif

}


