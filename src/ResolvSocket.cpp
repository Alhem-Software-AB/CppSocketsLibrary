/**
 **	File ......... ResolvSocket.cpp
 **	Published ....  2005-03-24
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
#include <stdio.h>

#include "Parse.h"
#include "ResolvSocket.h"




ResolvSocket::ResolvSocket(SocketHandler& h,Socket *parent)
:TcpSocket(h)
,m_bServer(false)
,m_parent(parent)
{
	SetLineProtocol();
}


ResolvSocket::~ResolvSocket()
{
}


void ResolvSocket::OnLine(const std::string& line)
{
	Parse pa(line, ":");
	if (m_bServer)
	{
		m_query = pa.getword();
		m_data = pa.getrest();
		if (!Detach()) // detach failed?
		{
			SetCloseAndDelete();
		}
		return;
	}
	std::string key = pa.getword();
	std::string value = pa.getrest();

	if (key == "A" && m_parent)
	{
		ipaddr_t l;
		u2ip(value, l); // ip2ipaddr_t
		m_parent -> Resolved(m_resolv_id, l, m_resolv_port);
		m_parent = NULL; // always use first ip in case there are several
	}
}


void ResolvSocket::OnDetached()
{
	if (m_query == "gethostbyname")
	{
		struct hostent *h = gethostbyname(m_data.c_str());
		if (h)
		{
			char slask[1000];
			sprintf(slask, "Name: %s\n", h -> h_name);
			Send( slask );
			size_t i = 0;
			while (h -> h_aliases[i])
			{
				sprintf(slask, "Alias: %s\n", h -> h_aliases[i]);
				Send( slask );
				i++;
			}
			sprintf(slask, "AddrType: %d\n", h -> h_addrtype);
			Send( slask );
			sprintf(slask, "Length: %d\n", h -> h_length);
			Send( slask );
			i = 0;
			while (h -> h_addr_list[i])
			{
				// let's assume 4 byte IPv4 addresses
				char ip[40];
				*ip = 0;
				for (int j = 0; j < 4; j++)
				{
					if (*ip)
						strcat(ip,".");
					sprintf(ip + strlen(ip),"%u",(unsigned char)h -> h_addr_list[i][j]);
				}
				sprintf(slask, "A: %s\n", ip);
				Send( slask );
				i++;
			}
		}
		else
		{
			Send("Failed\n");
		}
		Send( "\n" );
	}
	else
	if (m_query == "gethostbyaddr")
	{
		Send("Not Implemented\n\n");
	}
	else
	{
		std::string msg = "Unknown query type: " + m_query;
		Handler().LogError(this, "OnDetached", 0, msg);
		Send("Unknown\n\n");
	}
	SetCloseAndDelete();
}


/*
       The hostent structure is defined in <netdb.h> as follows:

              struct hostent {
                      char    *h_name;        // official name of host 
                      char    **h_aliases;    // alias list 
                      int     h_addrtype;     // host address type 
                      int     h_length;       // length of address 
                      char    **h_addr_list;  // list of addresses 
              }
              #define h_addr  h_addr_list[0]  // for backward compatibility 

       The members of the hostent structure are:

       h_name The official name of the host.

       h_aliases
              A zero-terminated array of alternative names for the host.

       h_addrtype
              The type of address; always AF_INET at present.

       h_length
              The length of the address in bytes.

       h_addr_list
              A zero-terminated array of network addresses for the host in network byte order.
*/


void ResolvSocket::OnConnect()
{
	std::string msg = "gethostbyname " + m_resolv_host + "\n";
	Send( msg );
}


