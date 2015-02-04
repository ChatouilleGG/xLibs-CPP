#pragma once

class xString
{

public:
	xString(void);
	xString(const char* format, ...);
	xString(const xString&);
	~xString();

	int length(void);

	bool eq(const xString&) const;
	bool ieq(const xString&) const;

	xString sub(int, int) const;
	xString left(int) const;
	xString tail(int) const;
	xString right(int) const;

	int findc(char*) const;
	int find(char*) const;

	operator char*() const;
	xString& operator =(char*);
	const xString operator +(const char*) const;
	const xString operator +(const int) const;
	const xString operator +(const double) const;
	const xString operator +(const bool) const;

private:
	char* chars;
	int len;
	int memsize;

	xString::xString(int, char*);

	// void resize(int);

	static const short STR_MIN_MEMSIZE;
	static const double STR_MEM_FILL;
	static const double STR_SHRINK_AT;

};

