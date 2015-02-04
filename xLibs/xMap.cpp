#include "stdafx.h"
#include "xMap.h"

#include <stdarg.h>


//================================================================
// Constructors
//================================================================

const int xMap::DEFAULT_SIZE = 10;
const double xMap::ENLARGE_AT = 0.75;
const double xMap::SHRINK_AT = 0.25;
const double xMap::ENLARGE_MULT = 2.0;
const double xMap::SHRINK_DIV = 2.0;

xMap::xMap()
{
	size = xMap::DEFAULT_SIZE;
	table = (xMapPair**)calloc(size, sizeof(xMapPair*));
	if (table == NULL) throw "out of memory";
	fill = 0;
}

xMap::xMap(std::initializer_list<xMapPair*> list)
{
	size = xMap::DEFAULT_SIZE;
	table = (xMapPair**)calloc(size, sizeof(xMapPair*));
	if (table == NULL) throw "out of memory";
	fill = 0;
	for (xMapPair* elem : list) put(elem);
}

xMap::xMap(const char* key1, ...)
{
	size = xMap::DEFAULT_SIZE;
	table = (xMapPair**)calloc(size, sizeof(xMapPair*));
	if (table == NULL) throw "out of memory";
	fill = 0;

	const char* curkey = key1;
	va_list args;
	va_start(args, key1);
	while (curkey != NULL)
	{
		xValType curtype = va_arg(args, xValType);
		switch (curtype)
		{
		case VAL_PTR:		put(curkey, va_arg(args, void*), VAL_PTR);	break;
		case VAL_MAP:		put(curkey, va_arg(args, xMap*));			break;
		case VAL_ARRAY:		put(curkey, va_arg(args, xArray*));			break;
		case VAL_STRING:	put(curkey, va_arg(args, char*));			break;
		case VAL_INT:		put(curkey, va_arg(args, int));				break;
		case VAL_DOUBLE:	put(curkey, va_arg(args, double));			break;
		}
		curkey = va_arg(args, char*);
	}
	va_end(args);
}


//================================================================
// Operators
//================================================================




//================================================================
// Public Functions
//================================================================

//==================== put ====================

void xMap::put(xMapPair* pair)
{
	unsigned int index = _hash((unsigned char*)pair->key) % size;
	if (table[index] == NULL)
	{
		pair->next = NULL;
		table[index] = pair;
		fill++;
		if ((double)fill / (double)size >= xMap::ENLARGE_AT)
			_resize((int)floor((double)size * xMap::ENLARGE_MULT));
	}
	else
	{
		remove(pair->key);
		pair->next = table[index];
		table[index] = pair;
	}
}

void xMap::put(const char* key, void* value, xValType type)
{
	put(new xMapPair(key, value, type));
}

void xMap::put(std::initializer_list<xMapPair*> list)
{
	for (xMapPair* elem : list) put(elem);
}

//==================== get ====================

xMapPair* xMap::get(const char* key) const
{
	if (key != NULL && strlen(key) > 0)
	{
		unsigned int index = _hash((unsigned char*)key) % size;
		xMapPair* pair = table[index];
		while (pair != NULL)
		{
			if (strcmp(pair->key, key) == 0)
				return pair;
			pair = pair->next;
		}
	}
	return NULL;
}

int xMap::geti(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && p->type == VAL_INT) ? *((int*)(p->value)) : 0;
}

double xMap::getd(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && p->type == VAL_DOUBLE) ? *((double*)(p->value)) : 0.0;
}

xString xMap::getstr(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && p->type == VAL_STRING) ? xString((char*)(p->value)) : xString("");
}

xArray* xMap::getarr(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && (p->type == VAL_ARRAY || p->type == VAL_PTR)) ? (xArray*)(p->value) : NULL;
}

xMap* xMap::getmap(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && (p->type == VAL_MAP || p->type == VAL_PTR)) ? (xMap*)(p->value) : NULL;
}

void* xMap::getp(const char* key) const
{
	xMapPair* p = get(key);
	return (p != NULL && (p->type & xPointerTypes) != 0) ? p->value : NULL;
}

//==================== remove ====================

void xMap::remove(const char* key)
{
	if (key != NULL && strlen(key) > 0)
	{
		unsigned int index = _hash((unsigned char*)key) % size;
		if (table[index] == NULL) return;

		xMapPair* pair = table[index];
		if (strcmp(pair->key, key) == 0)
		{
			free(pair->key);
			if ((pair->type & xDuplicatedTypes) != 0)
				free(pair->value);
			table[index] = pair->next;
			free(pair);

			if (table[index] == NULL)
			{
				fill--;
				if ((double)fill / (double)size <= xMap::SHRINK_AT)
					_resize((int)floor((double)size / xMap::SHRINK_DIV));
			}
		}
		else
		{
			while (pair->next != NULL)
			{
				if (strcmp(pair->next->key, key) == 0)
				{
					xMapPair* nxt = pair->next;
					free(nxt->key);
					if ((nxt->type & xDuplicatedTypes) != 0)
						free(nxt->value);
					pair->next = nxt->next;
					free(nxt);
					return;
				}
				pair = pair->next;
			}
		}
	}
}

void xMap::empty(void)
{
	for (int i = 0; i < size; i++)
	{
		while (table[i] != NULL)
		{
			xMapPair* p = table[i];
			free(p->key);
			if ((p->type & xDuplicatedTypes) != 0)
				free(p->value);
			table[i] = p->next;
			free(p);
		}
	}
	fill = 0;
	_resize(xMap::DEFAULT_SIZE);
}

//==================== iterate ====================

void xMap::start(void)
{
	for (int i = 0; i < size; i++)
	{
		if (table[i] != NULL)
		{
			cur_index = i;
			cur_p = table[i];
			return;
		}
	}
	cur_p = NULL;
}

bool xMap::hasnext(void)
{
	return (cur_p != NULL);
}

xMapPair* xMap::next(void)
{
	xMapPair* p = cur_p;
	if (p != NULL)
	{
		if (p->next != NULL)
			cur_p = p->next;
		else
		{
			cur_p = NULL;
			for (int i = cur_index + 1; i < size; i++)
			{
				if (table[i] != NULL)
				{
					cur_index = i;
					cur_p = table[i];
					break;
				}
			}
		}
	}
	return p;
}

xMapPair* xMap::first(void)
{
	start();
	return next();
}


//================================================================
// Private Functions
//================================================================

unsigned int xMap::_hash(unsigned char* str) const
{
	unsigned int hash = 5381;
	if (str != NULL)
	{
		unsigned int i = 0;
		while (str[i] != '\0')
		{
			hash = hash + (hash << 5) + (char)str[i];
			i++;
		}
	}
	return hash;
}

void xMap::_resize(int new_size)
{
	if (new_size < xMap::DEFAULT_SIZE) new_size = xMap::DEFAULT_SIZE;
	if (new_size == size) return;

	int old_size = size;
	xMapPair** old_table = table;

	size = new_size;
	table = (xMapPair**)calloc(size, sizeof(xMapPair*));
	fill = 0;

	for (int i = 0; i < old_size; i++)
	{
		xMapPair* pair = old_table[i];
		while (pair != NULL)
		{
			xMapPair* nxt = pair->next;
			put(pair);
			pair = nxt;
		}
	}

	free(old_table);
}


//================================================================
// Destructors
//================================================================

xMap::~xMap()
{
	empty();
	free(table);
}

