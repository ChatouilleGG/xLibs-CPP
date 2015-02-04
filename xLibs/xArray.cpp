#include "stdafx.h"
#include "xArray.h"

#include <stdlib.h>
#include <string>
#include <stdarg.h>

//================================================================
// Constructors
//================================================================

const int xArray::DEFAULT_SIZE = 10;
const double xArray::ENLARGE_MULT = 2.0;
const double xArray::ARR_SHRINK_AT = 0.25;
const double xArray::SHRINK_DIV = 2.0;

xArray::xArray()
{
	max_size = xArray::DEFAULT_SIZE;
	cur_size = 0;
	table = (xArrayElem**)calloc(max_size, sizeof(xArrayElem*));
	if (table == NULL) throw "out of memory";
}

xArray::xArray(xValType type1, ...)
{
	max_size = xArray::DEFAULT_SIZE;
	cur_size = 0;
	table = (xArrayElem**)calloc(max_size, sizeof(xArrayElem*));
	if (table == NULL) throw "out of memory";

	int curtype = type1;
	va_list args;
	va_start(args, type1);
	while (curtype >= 0)
	{
		switch (curtype)
		{
		case VAL_PTR:		add(va_arg(args, void*));			break;
		case VAL_ARRAY:		add(va_arg(args, xArray*));			break;
		case VAL_STRING:	add(va_arg(args, const char*));		break;
		case VAL_INT:		add(va_arg(args, int));				break;
		case VAL_DOUBLE:	add(va_arg(args, double));			break;
		}
		curtype = va_arg(args, int);
	}
	va_end(args);
}


//================================================================
// Operators
//================================================================




//================================================================
// Public Functions
//================================================================

//==================== size ====================

int xArray::size(void)
{
	return cur_size;
}

//==================== set ====================

void xArray::set(int index, int val)
{
	int* value = (int*)malloc(sizeof(int));
	if (value == NULL) throw "out of memory";
	*value = val;
	try {
		_set(index, value, VAL_INT);
	}
	catch (const char* e) {
		free(value);
		throw e;
	}
}

void xArray::set(int index, double val)
{
	double* value = (double*)malloc(sizeof(double));
	if (value == NULL) throw "out of memory";
	*value = val;
	try {
		_set(index, value, VAL_DOUBLE);
	}
	catch (const char* e) {
		free(value);
		throw e;
	}
}

void xArray::set(int index, const char* val)
{
	int len = strlen(val) + 1;
	char* value = (char*)malloc(len * sizeof(int));
	if (value == NULL) throw "out of memory";
	strcpy_s(value, len, val);
	try {
		_set(index, value, VAL_STRING);
	} catch (const char* e) {
		free(value);
		throw e;
	}
}

void xArray::set(int index, xArray* val)
{
	_set(index, val, VAL_ARRAY);
}

void xArray::setp(int index, void* val)
{
	_set(index, val, VAL_PTR);
}

//==================== get ====================

xArrayElem* xArray::get(int index) const
{
	if (index >= 0 && index < cur_size)
		return table[index];
	return NULL;
}

int xArray::geti(int index) const
{
	xArrayElem* v = get(index);
	return (v != NULL && v->type == VAL_INT) ? *((int*)(v->value)) : 0;
}

double xArray::getd(int index) const
{
	xArrayElem* v = get(index);
	return (v != NULL && v->type == VAL_DOUBLE) ? *((double*)(v->value)) : 0.0;
}

xString xArray::getstr(int index) const
{
	xArrayElem* v = get(index);
	return (v != NULL && v->type == VAL_STRING) ? xString((char*)(v->value)) : "";
}

xArray* xArray::getarr(int index) const
{
	xArrayElem* v = get(index);
	return (v != NULL && (v->type == VAL_ARRAY || v->type == VAL_PTR)) ? (xArray*)(v->value) : NULL;
}

void* xArray::getp(int index) const
{
	xArrayElem* v = get(index);
	return (v != NULL && (v->type & xPointerTypes) != 0) ? v->value : NULL;
}

//==================== insert ====================

void xArray::insert(int index)
{
	if (index < 0) throw "negative index";
	if (index < cur_size)
	{
		if (cur_size == max_size)
			_resize((int)floor((double)max_size * xArray::ENLARGE_MULT));

		memmove_s(table + index + 1, (max_size + 1 - index)*sizeof(xArrayElem*), table + index, (cur_size - index)*sizeof(xArrayElem*));
		table[index] = NULL;
		cur_size++;
	}
}

//==================== remove ====================

void xArray::remove(int index)
{
	if (index < 0) throw "negative index";
	if (index >= cur_size) return;

	if (table[index] != NULL)
	{
		if ((table[index]->type & xDuplicatedTypes) != 0)
			free(table[index]->value);
		free(table[index]);
		table[index] = NULL;
	}
	if (index < cur_size - 1)
	{
		int s = (cur_size - (index + 1));
		memmove_s(table + index, (s+1)*sizeof(xArrayElem*), table + index + 1, s*sizeof(xArrayElem*));
		table[cur_size - 1] = NULL;
	}
	cur_size--;
	if ((double)cur_size / (double)max_size <= xArray::ARR_SHRINK_AT)
		_resize((int)floor((double)max_size / (double)xArray::SHRINK_DIV));
}

void xArray::empty(void)
{
	for (int i = 0; i < cur_size; i++)
	{
		if (table[i] != NULL)
		{
			if ((table[i]->type & xDuplicatedTypes) != 0)
				free(table[i]->value);
			free(table[i]);
			table[i] = NULL;
		}
	}
	cur_size = 0;
	_resize(xArray::DEFAULT_SIZE);
}

//==================== sort ====================

int xArray::sort(int(*func)(const xArrayElem**, const xArrayElem**))
{
	if (func == NULL) return 0;
	int num = _grouptypes((unsigned int)INT_MAX);	// all types: get rid of NULL's
	if (num > 0)
		qsort(table, num, sizeof(xArrayElem*), (int(*)(const void*, const void*))func);
	return num;
}

int xArray::sorti(void)
{
	int num = _grouptypes(VAL_INT);
	if ( num > 0 )
		qsort(table, num, sizeof(xArrayElem*), (int(*)(const void*, const void*))_xArray_comp_int);
	return num;
}

int xArray::sortd(void)
{
	int num = _grouptypes(VAL_DOUBLE);
	if ( num > 0 )
		qsort(table, num, sizeof(xArrayElem*), (int(*)(const void*, const void*))_xArray_comp_double);
	return num;
}

int xArray::sortstr(void)
{
	int num = _grouptypes(VAL_STRING);
	if ( num > 0 )
		qsort(table, num, sizeof(xArrayElem*), (int(*)(const void*, const void*))_xArray_comp_string);
	return num;
}

int xArray::sortstr_i(void)
{
	int num = _grouptypes(VAL_STRING);
	if ( num > 0 )
		qsort(table, num, sizeof(xArrayElem*), (int(*)(const void*, const void*))_xArray_comp_string_i);
	return num;
}


//================================================================
// Private Functions
//================================================================

void xArray::_set(int index, void* value, xValType type)
{
	if (index < 0) throw "negative index";
	if (index >= max_size)
	{
		int new_size = (int)floor((double)max_size * xArray::ENLARGE_MULT);
		while (index >= new_size)
			new_size = (int)floor((double)new_size * xArray::ENLARGE_MULT);
		_resize(new_size);
	}

	if (table[index] == NULL)
	{
		table[index] = (xArrayElem*)malloc(sizeof(xArrayElem));
		if (table[index] == NULL) throw "out of memory";
	}
	else if ((table[index]->type & xDuplicatedTypes) != 0)
		free(table[index]->value);

	table[index]->type = type;
	table[index]->value = value;

	if (index >= cur_size)
		cur_size = index + 1;
}

void xArray::_resize(int new_size)
{
	if (new_size < xArray::DEFAULT_SIZE) new_size = xArray::DEFAULT_SIZE;
	if (max_size == new_size) return;

	xArrayElem** new_table = (xArrayElem**)calloc(new_size, sizeof(xArrayElem*));
	if (new_table == NULL) throw "out of memory";

	memcpy_s(new_table, new_size*sizeof(xArrayElem*), table, cur_size*sizeof(xArrayElem*));
	free(table);
	table = new_table;
	max_size = new_size;
}

int xArray::_grouptypes(unsigned int types)
{
	// group elems of the right type at the beginning of the array
	int j = 0;
	int num = 0;
	for (int i = 0; i < cur_size; i++)
	{
		if (table[i] == NULL || (table[i]->type & types) == 0)
		{
			if (i + 1 > j)
				j = i + 1;
			for (; j < cur_size; j++)
			{
				if (table[j] != NULL && (table[j]->type & types) != 0)
				{
					xArrayElem* v = table[i];
					table[i] = table[j];
					table[j] = v;
					num = i + 1;
					break;
				}
			}
			if (num < i + 1)	// no more items
				break;
		}
		else
			num = i + 1;
	}
	return num;
}

//note: not inside class (because of the implicit argument)
int _xArray_comp_int(const xArrayElem** a, const xArrayElem** b)
{
	return (*(int*)((*a)->value) - *(int*)((*b)->value));
}

int _xArray_comp_double(const xArrayElem** a, const xArrayElem** b)
{
	char tmp[16];
	sprintf_s(tmp, 16 * sizeof(char), "%f", (*(double*)((*a)->value) - *(double*)((*b)->value)));
	bool neg = (tmp[0] == '-');
	for (unsigned int i = (neg ? 1 : 0); i < strlen(tmp); i++)
	{
		if (tmp[i] != '0' && tmp[i] != '.')
			return (neg ? -1 : 1);
	}
	return 0;
}

int _xArray_comp_string(const xArrayElem** a, const xArrayElem** b)
{
	return strcmp((char*)((*a)->value), (char*)((*b)->value));
}

int _xArray_comp_string_i(const xArrayElem** a, const xArrayElem** b)
{
	return _stricmp((char*)((*a)->value), (char*)((*b)->value));
}


//================================================================
// Destructors
//================================================================

xArray::~xArray()
{
	empty();
	free(table);
}
