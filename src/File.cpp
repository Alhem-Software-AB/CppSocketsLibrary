/**
 **	File ......... File.cpp
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
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "File.h"


File::File()
:m_fil(NULL)
{
}


File::~File()
{
}


bool File::fopen(const std::string& path, const std::string& mode)
{
	m_path = path;
	m_mode = mode;
	m_fil = ::fopen(path.c_str(), mode.c_str());
	return m_fil ? true : false;
}


void File::fclose()
{
	if (m_fil)
		::fclose(m_fil);
}



size_t File::fread(char *ptr, size_t size, size_t nmemb)
{
	return m_fil ? ::fread(ptr, size, nmemb, m_fil) : 0;
}


size_t File::fwrite(const char *ptr, size_t size, size_t nmemb)
{
	return m_fil ? ::fwrite(ptr, size, nmemb, m_fil) : 0;
}



char *File::fgets(char *s, int size)
{
	return m_fil ? ::fgets(s, size, m_fil) : NULL;
}


void File::fprintf(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(m_fil, format, ap);
	va_end(ap);
}


off_t File::size()
{
	struct stat st;
	if (stat(m_path.c_str(), &st) == -1)
	{
		return 0;
	}
	return st.st_size;
}


bool File::eof()
{
	if (m_fil)
	{
		if (feof(m_fil))
			return true;
	}
	return false;
}


