/** \file sockets_stdptr.h
 **	\date  2023-02-02
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2023  Alhem Software AB

This library is made available under the terms of the GNU GPL, with
the additional exemption that compiling, linking, and/or using OpenSSL 
is allowed.

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
#ifndef _SOCKETS_STDPTR_H
#define _SOCKETS_STDPTR_H

#include "sockets_cpp11.h"

#ifdef HAVE_CPP11
#define USING_AUTOPTR_AS std::unique_ptr
#else
#define USING_AUTOPTR_AS std::auto_ptr
#endif

#endif // _SOCKETS_STDPTR_H
