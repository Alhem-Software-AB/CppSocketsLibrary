#include <stdio.h>

#ifdef _WIN32
const char *StrError(int x) {
static	char tmp[100];
	sprintf(tmp, "Winsock error code: %d", x);
	return tmp;
}
#endif
