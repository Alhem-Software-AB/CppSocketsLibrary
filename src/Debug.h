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
		for (int i = 0; i < m_level; i++)
			fprintf(stderr, "  ");
		fprintf(stderr, "%s\n", x.c_str());
		m_level++;
	}
	Debug(int id, const std::string& x) : m_id(id), m_text(x) {
		for (int i = 0; i < m_level; i++)
			fprintf(stderr, "  ");
		fprintf(stderr, "%d> %s\n", m_id, x.c_str());
		m_level++;
	}
	~Debug() {
		if (m_level)
			m_level--;
		for (int i = 0; i < m_level; i++)
			fprintf(stderr, "  ");
		if (m_id)
			fprintf(stderr, "%d> /%s\n", m_id, m_text.c_str());
		else
			fprintf(stderr, "/%s\n", m_text.c_str());
		fflush(stderr);
	}
private:
	int m_id;
	std::string m_text;
static	int m_level;
};


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_Debug_H
