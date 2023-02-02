/**
 **	File ......... NullCrypt.cpp
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
#include "Base64.h"

#include "NullCrypt.h"




NullCrypt::NullCrypt()
:ICrypt()
{
}


std::string NullCrypt::encrypt(unsigned char *,const std::string& input)
{
	Base64 b;
	std::string tmp;
	b.encode(input, tmp, false);
	return tmp;
}


bool NullCrypt::decrypt(unsigned char *,const std::string& input,std::string& output)
{
	Base64 b;
	b.decode(input, output);
	return true;
}


