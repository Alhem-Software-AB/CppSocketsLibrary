/**
 **	File ......... File.h
 **	Published ....  2005-04-25
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
#ifndef _FILE_H
#define _FILE_H

#include "IFile.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class File : public IFile
{
public:
	File();
	~File();

	bool fopen(const std::string&, const std::string&);
	void fclose();

	size_t fread(char *, size_t, size_t);
	size_t fwrite(const char *, size_t, size_t);

	char *fgets(char *, int);
	void fprintf(char *format, ...);

	off_t size();
	bool eof();

private:
	File(const File& ) {} // copy constructor
	File& operator=(const File& ) { return *this; } // assignment operator

	std::string m_path;
	std::string m_mode;
	FILE *m_fil;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _FILE_H
