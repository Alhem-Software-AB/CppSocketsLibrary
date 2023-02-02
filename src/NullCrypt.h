/**
 **	File ......... NullCrypt.h
 **	Published ....  2004-02-13
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004,2005  Anders Hedstrom

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
#ifndef _NULLCRYPT_H
#define _NULLCRYPT_H

#include "ICrypt.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class NullCrypt : public ICrypt
{
public:
	NullCrypt();

	std::string encrypt(unsigned char *,const std::string& );
	bool decrypt(unsigned char *,const std::string& ,std::string& );

};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _NULLCRYPT_H
