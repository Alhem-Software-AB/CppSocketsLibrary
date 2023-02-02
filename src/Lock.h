/*
 **	File ......... Lock.h
 **	Published ....  2005-08-22
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2005  Anders Hedstrom

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
#ifndef _LOCK_H
#define _LOCK_H

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class Mutex;

/** Mutex encapsulation class. 
	\ingroup threading */
class Lock
{
public:
	Lock(Mutex&);
	~Lock();

private:
	Mutex& m_mutex;
};




#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _LOCK_H