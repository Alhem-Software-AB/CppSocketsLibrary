/**
 **	File ......... ResolvServer.h
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
#ifndef _RESOLVSERVER_H
#define _RESOLVSERVER_H

#include <Thread.h>


class ResolvServer : public Thread
{
public:
	ResolvServer(port_t);
	~ResolvServer();

	void Run();
	void Quit();

private:
	ResolvServer(const ResolvServer& ) {} // copy constructor
	ResolvServer& operator=(const ResolvServer& ) { return *this; } // assignment operator

	bool m_quit;
	port_t m_port;
};




#endif // _RESOLVSERVER_H
