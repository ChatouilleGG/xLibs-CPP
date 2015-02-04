#pragma once

#include "xString.h"
#include "xUtils.h"
#include "xArray.h"

#include <string>

class xMapPair;

class xMap
{

public:
	xMap();
	xMap(std::initializer_list<xMapPair*> list);
	xMap(const char*, ...);		// key1,type1,value1, ..., XMAP_DECL_END
	~xMap();

	void put(xMapPair*);
	void put(const char*, void*, xValType);
	template<typename T> inline void put(const char* key, T value) { put(new xMapPair(key, value)); }
	void put(std::initializer_list<xMapPair*> list);

	xMapPair* get(const char*) const;
	int geti(const char*) const;
	double getd(const char*) const;
	xString getstr(const char*) const;
	xArray* getarr(const char*) const;
	xMap* getmap(const char*) const;
	void* getp(const char*) const;

	void remove(const char*);
	void empty(void);

	void start(void);
	xMapPair* first(void);
	bool hasnext(void);
	xMapPair* next(void);


private:
	xMapPair** table;
	int size;
	int fill;
	int cur_index;
	xMapPair* cur_p;

	unsigned int _hash(unsigned char*) const;
	void _resize(int);

	static const int DEFAULT_SIZE;
	static const double ENLARGE_AT;
	static const double SHRINK_AT;
	static const double ENLARGE_MULT;
	static const double SHRINK_DIV;

};

// multi-types constructor
#define XMAP_DECL_END -1
#define xMAP(key1,...) xMap(key1, __VA_ARGS__, XMAP_DECL_END)


// xMapPair

class xMapPair
{
public:
	char* key;
	void* value;
	xValType type;
	xMapPair* next = NULL;

	inline xMapPair(const char* k, void* v, xValType t)
	{
		copy_key(k);
		value = v;
		type = t;
	}
	inline xMapPair(const char* k, int v)
	{
		copy_key(k);
		value = malloc(sizeof(int));
		if (value == NULL) throw "out of memory";
		*((int*)value) = v;
		type = VAL_INT;
	}
	inline xMapPair(const char* k, double v)
	{
		copy_key(k);
		value = malloc(sizeof(double));
		if (value == NULL) throw "out of memory";
		*((double*)value) = v;
		type = VAL_DOUBLE;
	}
	inline xMapPair(const char* k, const char* v)
	{
		copy_key(k);
		int s = strlen(v) + 1;
		value = malloc(s*sizeof(char));
		if (value == NULL) throw "out of memory";
		strcpy_s((char*)value, s, v);
		type = VAL_STRING;
	}
	inline xMapPair(const char* k, xArray* v)
	{
		copy_key(k);
		value = v;
		type = VAL_ARRAY;
	}
	inline xMapPair(const char* k, xMap* v)
	{
		copy_key(k);
		value = v;
		type = VAL_MAP;
	}
	inline xMapPair(const char* k, void* v)
	{
		copy_key(k);
		value = v;
		type = VAL_PTR;
	}

	inline xMapPair(const xMapPair& other)
	{
		copy_key(other.key);
		type = other.type;
		switch (type)
		{
		case VAL_INT:
			value = malloc(sizeof(int));
			if (value == NULL) throw "out of memory";
			*((int*)value) = *((int*)other.value);
			break;
		case VAL_DOUBLE:
			value = malloc(sizeof(double));
			if (value == NULL) throw "out of memory";
			*((double*)value) = *((double*)other.value);
			break;
		case VAL_STRING:
		{
			int s = strlen((char*)other.value) + 1;
			value = malloc(s*sizeof(char));
			if (value == NULL) throw "out of memory";
			strcpy_s((char*)value, s, (char*)other.value);
			break;
		}
		default:
			value = other.value;
		}
		next = other.next;
	}

	inline ~xMapPair(void)
	{
		free(key);
		if (type >= VAL_STRING)
			free(value);
	}

private:
	inline void copy_key(const char* k)
	{
		int s = strlen(k) + 1;
		key = (char*)malloc(s * sizeof(char));
		if (key == NULL) throw "out of memory";
		strcpy_s(key, s, k);
	}

};


