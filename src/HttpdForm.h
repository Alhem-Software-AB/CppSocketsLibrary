/**
 **	HttpdForm.h - read stdin, parse cgi input
 **
 **	Written: 1999-Feb-10 grymse@alhem.net
 **/

/*
Copyright (C) 1999-2005  Anders Hedstrom

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

#ifndef _FORM_H
#define _FORM_H

#include <string>
#include <list>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/**
 * Store the name/value pairs from a GET/POST operation.
 * "name" does not have to be unique.
*/

struct CGI
{
	CGI(const std::string& n,const std::string& v) : name(n),value(v) {}
	CGI(const std::string& n,const std::string& v,const std::string& p) : name(n),value(v),path(p) {}
	std::string name;
	std::string value;
	std::string path;
};
typedef std::list<CGI *> cgi_v;


//! Process and store user input from a GET/POST operation.
class IFile;

class HttpdForm
{
public:
	/**
	 * Default constructor (used in POST operations).
	 * Input is read from stdin. Number of characters to read
	 * can be found in the environment variable CONTENT_LENGTH.
	*/
	HttpdForm(IFile *);
	/**
	 * Another constructor (used in GET operations).
	 * Input is read from the environment variable QUERY_STRING.
	 * @param query_string The httpd server provided QUERY_STRING
	 * @param length Query string length.
	*/
	HttpdForm(const std::string& query_string,size_t length);
	~HttpdForm();

	void EnableRaw(bool);

	void strcpyval(char *,const char *,size_t);

	/* get names */
	bool getfirst(char *,size_t);
	bool getnext(char *,size_t);

	/* get names and values */
	bool getfirst(char *,size_t,char *,size_t);
	bool getnext(char *,size_t,char *,size_t);

	/* get value */
	int getvalue(const std::string& ,char *,size_t);
	std::string getvalue(const std::string& );
	size_t getlength(const std::string& );
	cgi_v& getbase() { return m_cgi; }

	const std::string& GetBoundary() { return m_strBoundary; }

private:
	HttpdForm(const HttpdForm& ) {}
	HttpdForm& operator=(const HttpdForm& ) { return *this; }
	cgi_v m_cgi;
	cgi_v::iterator m_current;
	std::string m_strBoundary;
	bool raw;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _FORM_H
