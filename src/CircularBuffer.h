/**
 **	File ......... CircularBuffer.h
 **	Published ....  2004-02-13
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
#ifndef _CIRCULARBUFFER_H
#define _CIRCULARBUFFER_H




class CircularBuffer
{
public:
	CircularBuffer(size_t size);
	~CircularBuffer();

	bool Write(const char *,size_t l);
	bool Read(char *dest,size_t l);
	bool Remove(size_t l);

	size_t GetLength() { return m_q; }
	char *GetStart() { return buf + m_b; }
	size_t GetL() { return (m_b + m_q > m_max) ? m_max - m_b : m_q; }
	size_t Space() { return m_max - m_q; }

	unsigned long GetCount() { return m_count; }

private:
	char *buf;
	size_t m_max;
	size_t m_q;
	size_t m_b;
	size_t m_t;
	unsigned long m_count;
};




#endif // _CIRCULARBUFFER_H
