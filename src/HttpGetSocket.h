/** \file HttpGetSocket.h
 **	\date  2004-02-13
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004,2005  Anders Hedstrom

This library is made available under the terms of the GNU GPL.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about 
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

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
#ifndef _HTTPGETSOCKET_H
#define _HTTPGETSOCKET_H

#include "HTTPSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** Get http page. 
	\ingroup http */
class HttpGetSocket : public HTTPSocket
{
public:
	HttpGetSocket(SocketHandler&);
	HttpGetSocket(SocketHandler&,const std::string& url,const std::string& = "");
	HttpGetSocket(SocketHandler&,const std::string& host,port_t port,const std::string& url,const std::string&);
	~HttpGetSocket();

	void OnConnect();
	/** New callback method fires when all data is received. */
	virtual void OnContent();
	void OnDelete();

	void OnFirst();
	void OnHeader(const std::string& key,const std::string& value);
	void OnHeaderComplete();
	void OnData(const char *,size_t);

	bool Complete() { return m_bComplete; }

	size_t GetContentLength() { return m_content_length; }
	size_t GetPos() { return m_content_ptr; }

	/** Returns received http headers. */
	const std::string& GetContent() { return m_content; }

	void SetFilename(const std::string& );
	void Url(const std::string& url,std::string& host,port_t&);

	/** Set external data buffer. */
	void SetDataPtr(unsigned char *p,size_t l);
	/** Get ptr to content buffer. 
		Use together with GetContentPtr to get entire document. */
	const unsigned char *GetDataPtr();
	/** Current number of bytes read from http request body. */
	size_t GetContentPtr() { return m_content_ptr; }

protected:
	HttpGetSocket(const HttpGetSocket& s) : HTTPSocket(s) {}
private:
	HttpGetSocket& operator=(const HttpGetSocket& ) { return *this; }
	std::string m_protocol;
	std::string m_host;
	port_t m_port;
	std::string m_url;
	std::string m_to_file;
	//
	FILE *m_fil;
	bool m_bComplete;
	//
	size_t m_content_length;
	std::string m_content_type;
	size_t m_content_ptr;
	std::string m_content; ///< Received http headers
	unsigned char *m_data; ///< Content buffer
	bool m_data_set; ///< Content buffer set from outside this object
	size_t m_data_max; ///< Content buffer maximum size
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _HTTPGETSOCKET_H
