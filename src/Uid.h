/**
 **	File ......... Uid.h
 **	Published ....  2004-04-06
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
#ifndef _UID_H
#define _UID_H

// donerat till Tekton 2003-11-12 / AH

#include <string>


class Uid
{
public:
	Uid();
	Uid(const std::string& );
	Uid(unsigned char *);
	~Uid();

	std::string GetUid();
	const unsigned char *GetBuf() { return m_bufuid; }

private:
	Uid(const Uid& ) {}
	Uid& operator=(const Uid& ) { return *this; }
	unsigned char m_bufuid[16];
//	std::string m_uid;
};




#endif // _UID_H
