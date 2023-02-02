// HttpdCookies.h
/*
Copyright (C) 2003-2005  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _COOKIES_H
#define _COOKIES_H

#include <list>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


//! Store the cookies name/value pairs.



//! Retrieve and manage cookies during a cgi call.
class HTTPSocket;

/** HTTP Cookie parse/container class. 
\sa HttpdSocket
\sa HttpdForm 
\ingroup webserver */
class HttpdCookies 
{
	/** Name/value pair store struct. 
		\ingroup webserver */
	struct COOKIE
	{
		COOKIE(const std::string& n,const std::string& v) : name(n),value(v) {}
		std::string name;
		std::string value;
	};
	typedef std::list<COOKIE *> cookie_v;
public:
	HttpdCookies();
	HttpdCookies(const std::string& query_string);
	~HttpdCookies();

//	int getvalue(const std::string& ,char *,size_t); // (name, buffer, length)
	bool getvalue(const std::string&,std::string&);
	void replacevalue(const std::string& ,const std::string& );
	void replacevalue(const std::string& ,long);
	void replacevalue(const std::string& ,int);
	size_t getlength(const std::string& );
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,const std::string& v);
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,long v);
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,int v);
	const std::string& expiredatetime();

	cookie_v& GetHttpdCookies() { return m_cookies; }

private:
	cookie_v m_cookies;
	std::string m_date;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _COOKIES_H
