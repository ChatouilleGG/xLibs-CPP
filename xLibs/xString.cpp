#include "stdafx.h"
#include "xString.h"

#include <string>
#include <stdarg.h>


//================================================================
// Constructors
//================================================================

const short xString::STR_MIN_MEMSIZE = 6;
const double xString::STR_MEM_FILL = 1.5;
const double xString::STR_SHRINK_AT = 0.16;

xString::xString(void)
{
	len = 0;
	memsize = xString::STR_MIN_MEMSIZE;
	chars = (char*)malloc(memsize * sizeof(char));
	if (chars == NULL) throw "out of memory";
	chars[0] = '\0';
}

xString::xString(const char* format, ...)
{
	if (format == NULL) format = "";

	va_list args;
	va_start(args, format);

	len = _vscprintf(format, args);
	memsize = (int)fmax(floor((double)(len+1) * xString::STR_MEM_FILL), xString::STR_MIN_MEMSIZE);
	chars = (char*)malloc(memsize * sizeof(char));
	if (chars == NULL) throw "out of memory";

	vsnprintf_s(chars, memsize, _TRUNCATE, format, args);

	va_end(args);
}

xString::xString(const xString& other)
{
	len = other.len;
	memsize = other.memsize;
	chars = (char*)malloc(memsize * sizeof(char));
	if (chars == NULL) throw "out of memory";
	strcpy_s(chars, memsize*sizeof(char), other.chars);
}


//================================================================
// Operators
//================================================================

xString::operator char*() const
{
	return chars;
}

xString& xString::operator =(char* str)
{
	if (chars == str) return *this;
	free(chars);
	len = strlen(str);
	memsize = (int)fmax(floor((double)(len + 1) * xString::STR_MEM_FILL), xString::STR_MIN_MEMSIZE);
	chars = (char*)malloc(memsize * sizeof(char));
	if (chars == NULL) throw "out of memory";
	strcpy_s(chars, memsize*sizeof(char), str);
	return *this;
}

const xString xString::operator +(const char* str) const
{
	return xString("%s%s", chars, str);
}

const xString xString::operator +(const int i) const
{
	 return xString("%s%i", chars, i);
}

const xString xString::operator +(const double d) const
{
	 return xString("%s%f", chars, d);
}

const xString xString::operator +(const bool b) const
{
	return xString("%s%s", chars, b ? "true" : "false");
}


//================================================================
// Public Functions
//================================================================

//==================== length ====================

int xString::length(void)
{
	return len;
}

//==================== equality ====================

bool xString::eq(const xString& other) const
{
	return (strcmp(chars, other.chars) == 0);
}
bool xString::ieq(const xString& other) const
{
	return (_stricmp(chars, other.chars) == 0);
}

//==================== substring ====================

#ifndef fclamp
	#define fclamp(x,min,max) fmin(fmax(x,min),max)
#endif

xString xString::sub(int begin, int end) const
{
	begin = (int)fclamp(begin, 0, len);
	end = (int)fclamp(end, -1, len - 1);

	if (end >= begin)
		return xString((end - begin) + 1, chars + begin);

	return xString("");
}

xString xString::left(int num) const
{
	return sub(0, num-1);
}

xString xString::tail(int offset) const
{
	return sub(offset, len-1);
}

xString xString::right(int num) const
{
	return sub(len-num, len-1);
}

//==================== find ====================

int xString::findc(char* c) const
{
	if (chars != NULL)
	{
		int span = strcspn(chars, c);
		if (span < len)
			return span;
	}
	return -1;
}

int xString::find(char* str) const
{
	if (str != NULL)
	{
		char* p = strstr(chars, str);
		if (p != NULL)
			return (int)((long)p - (long)chars);
	}
	return -1;
}


//================================================================
// Private Functions
//================================================================

xString::xString(int length, char* start)
{
	len = length;
	memsize = (int)fmax(floor((double)(len + 1) * xString::STR_MEM_FILL), xString::STR_MIN_MEMSIZE);
	chars = (char*)malloc(memsize * sizeof(char));
	if (chars == NULL) throw "out of memory";
	chars[0] = '\0';
	if (length > 0)
		strncat_s(chars, memsize*sizeof(char), start, length*sizeof(char));
}

/*
void xString::resize(int newsize)
{
	char* newmem;

	if (len >= newsize)
		newsize = (int)floor((double)(len + 1) * xString::STR_MEM_FILL);

	newmem = (char*)malloc(newsize * sizeof(char));
	if (newmem == NULL) throw "out of memory";

	newmem[0] = '\0';
	strcat_s(newmem, newsize, chars);
	free(chars);
	chars = newmem;
}
*/

//================================================================
// Destructors
//================================================================

xString::~xString()
{
	free(chars);
}

