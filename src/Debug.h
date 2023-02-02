#ifndef _SOCKETS_Debug_H
#define _SOCKETS_Debug_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class Debug
{
public:
	Debug(const std::string& x) : m_id(0), m_text(x) {
		fprintf(stderr, "%s\n", x.c_str());
	}
	Debug(int id, const std::string& x) : m_id(id), m_text(x) {
		fprintf(stderr, "%d> %s\n", m_id, x.c_str());
	}
	~Debug() {
		if (m_id)
			fprintf(stderr, "%d> /%s\n", m_id, m_text.c_str());
		else
			fprintf(stderr, "/%s\n", m_text.c_str());
		fflush(stderr);
	}
private:
	int m_id;
	std::string m_text;
};


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_Debug_H
