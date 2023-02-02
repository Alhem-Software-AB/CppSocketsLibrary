// HttpdSocket.h
/*
Copyright (C) 2001-2004,2005  Anders Hedstrom (grymse@alhem.net)

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
#ifndef _HTTPDSOCKET_H
#define _HTTPDSOCKET_H

#include "HTTPSocket.h"
#include "SocketHandler.h"


class HttpdCookies;
class HttpdForm;
class IFile;

class HttpdSocket : public HTTPSocket
{
public:
	HttpdSocket(SocketHandler& );
	~HttpdSocket();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);

	virtual void Exec() = 0;

	const std::string& GetHttpDate() { return m_http_date; }
	HttpdCookies *GetCookies() { return m_cookies; }
	HttpdForm *GetForm() { return m_form; }

protected:
	void Send64(const std::string& str64, const std::string& type);
	std::string datetime2httpdate(const std::string& dt);
	std::string GetDate();
	// headers
	std::string m_http_cookie;
	std::string m_content_type;
	std::string m_content_length_str;
	std::string m_if_modified_since;

private:
static	int m_request_count;
static	std::string m_start;
	size_t m_content_length;
	IFile *m_file;
	size_t m_received;
	int m_request_id;
	std::string m_http_date;
	HttpdCookies *m_cookies;
	HttpdForm *m_form;
};


#endif // _HTTPDSOCKET_H
