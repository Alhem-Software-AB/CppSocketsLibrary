#include <stdio.h>
#include <sys/select.h>
#include <string.h>
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


