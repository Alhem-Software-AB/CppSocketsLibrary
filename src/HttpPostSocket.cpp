/**
 **	File ......... HttpPostSocket.cpp
 **	Published ....  2004-10-30
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004  Anders Hedstrom

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
//#include <stdio.h>
#ifdef _WIN32
#else
#include <errno.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "SocketHandler.h"
#include "Parse.h"
#include "Utility.h"

#include "HttpPostSocket.h"



HttpPostSocket::HttpPostSocket(SocketHandler& h,const std::string& url)
:HTTPSocket(h)
{
	std::string host;
	{
		Parse pa(url,"/");
		pa.getword(); // 'http:'
		host = pa.getword();
		m_url = "/" + pa.getrest();
	}
	{
		Parse pa(host,":");
		m_host = pa.getword();
		m_port = pa.getvalue();
		if (!m_port)
		{
			m_port = 80;
		}
	}
	std::string m_boundary = "----";
	for (int i = 0; i < 12; i++)
	{
		char c = 0;
		while (!isalnum(c))
		{
			c = rand() % 96 + 32;
		}
		m_boundary += c;
	}
}


HttpPostSocket::~HttpPostSocket()
{
}


void HttpPostSocket::AddField(const std::string& name,const std::string& value)
{
	std::list<std::string> vec;
	vec.push_back(value);
	m_fields[name] = vec;
}


void HttpPostSocket::AddMultilineField(const std::string& name,std::list<std::string>& values)
{
	m_fields[name] = values;
}


void HttpPostSocket::AddFile(const std::string& name,const std::string& filename,const std::string& type)
{
	struct stat st;
	if (!stat(filename.c_str(), &st))
	{
		m_files[name] = filename;
		m_content_length[filename] = st.st_size;
		m_content_type[filename] = type;
	}
	else
	{
		Handler().LogError(this, "AddFile", errno, strerror(errno), LOG_LEVEL_FATAL);
		SetCloseAndDelete();
	}
}


void HttpPostSocket::Open()
{
	// why do I have to specify TcpSocket:: to get to the Open() method??
	TcpSocket::Open(m_host, m_port);
}


void HttpPostSocket::OnConnect()
{
	long length = 0; // calculate content_length of our post body
	std::string tmp;

	// fields
	{
	for (std::map<std::string,std::list<std::string> >::iterator it = m_fields.begin(); it != m_fields.end(); it++)
	{
		std::string name = (*it).first;
		std::list<std::string>& ref = (*it).second;
		tmp = "--" + m_boundary + "\r\n"
			"content-disposition: form-data; name=\"" + name + "\"\r\n"
			"\r\n";
		for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
		{
			std::string value = *it;
			tmp += value + "\r\n";
		}
		length += tmp.size();
	}
	}

	// files
	{
	for (std::map<std::string,std::string>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		std::string name = (*it).first;
		std::string filename = (*it).second;
		long content_length = m_content_length[filename];
		std::string content_type = m_content_type[filename];
		tmp = "--" + m_boundary + "\r\n"
			"content-disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"\r\n"
			"content-type: " + content_type + "\r\n"
			"\r\n";
		length += tmp.size();
		length += content_length;
		length += 2; // crlf after file
	}
	}

	// end
	tmp = "--" + m_boundary + "--\r\n";
	length += tmp.size();

	// build header, send body
	tmp =
		"POST /check HTTP/1.1\r\n"
		"Host: " + m_host + "\r\n"
		"User-agent: C++ Sockets library\r\n"
		"Accept: text/html, text/plain, */*;q=0.01\r\n"
		"Connection: close\r\n"
		"Content-type: multipart/form-data; boundary=" + m_boundary + "\r\n"
		"Content-length: " + Utility::l2string(length) + "\r\n"
		"\r\n";
	Send(tmp);

	// send fields
	{
	for (std::map<std::string,std::list<std::string> >::iterator it = m_fields.begin(); it != m_fields.end(); it++)
	{
		std::string name = (*it).first;
		std::list<std::string>& ref = (*it).second;
		tmp = "--" + m_boundary + "\r\n"
			"content-disposition: form-data; name=\"" + name + "\"\r\n"
			"\r\n";
		for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
		{
			std::string value = *it;
			tmp += value + "\r\n";
		}
		Send( tmp );
	}
	}

	// send files
	{
	for (std::map<std::string,std::string>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		std::string name = (*it).first;
		std::string filename = (*it).second;
		std::string content_type = m_content_type[filename];
		tmp = "--" + m_boundary + "\r\n"
			"content-disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"\r\n"
			"content-type: " + content_type + "\r\n"
			"\r\n";
		Send( tmp );
		{
			FILE *fil = fopen(filename.c_str(),"rb");
			if (fil)
			{
				char slask[2000];
				int n;
				while ((n = fread(slask, 1, 2000, fil)) > 0)
				{
					SendBuf(slask, n);
				}
				fclose(fil);
			}
		}
		Send("\r\n");
	}
	}

	// end of send
	Send("--" + m_boundary + "--\r\n");
}


void HttpPostSocket::OnFirst()
{
	int status = atoi(GetStatus().c_str());
	printf("Response status %d: %s\n", status, GetStatusText().c_str());
}


void HttpPostSocket::OnHeader(const std::string& ,const std::string& )
{
}


void HttpPostSocket::OnHeaderComplete()
{
	SetCloseAndDelete();
}


void HttpPostSocket::OnData(const char *,size_t)
{
}


