#pragma once

#include "xString.h"

#include <stdlib.h>
#include <initializer_list>

#include "xUtils.h"

typedef struct {
	xValType type;
	void* value;
} xArrayElem;

typedef int(*xArrayElemComp)(const xArrayElem**, const xArrayElem**);

class xArray
{

public:
	xArray();
	template<typename T> inline xArray(std::initializer_list<T> list)
	{
		max_size = xArray::DEFAULT_SIZE;
		while ((unsigned int)max_size < list.size())
			max_size = (int)floor((double)max_size * xArray::ENLARGE_MULT);
		cur_size = 0;
		table = (xArrayElem**)calloc(max_size, sizeof(xArrayElem*));
		if (table == NULL) throw "out of memory";
		for (auto elem : list) add(elem);
	}
	xArray(xValType, ...);		// type1,value1, type2,value2, ..., XARRAY_DECL_END
	~xArray();

	int size(void);

	void set(int, int);
	void set(int, double);
	void set(int, const char*);
	void set(int, xArray*);
	template<typename T> inline void set(int index, T val) { _set(index, val, VAL_PTR); }
	void setp(int, void*);

	xArrayElem* get(int) const;
	int geti(int) const;
	double getd(int) const;
	xString getstr(int) const;
	xArray* getarr(int) const;
	void* getp(int index) const;

	void insert(int);
	template<typename T> inline void insert(T val, int index) { insert(index); set(index, val); }

	template<typename T> inline void add(T val) { set(cur_size, val); }
	template<typename T, typename... Args> inline void add(T val, Args... args)
	{
		set(cur_size, val);
		add(args...);
	}
	template<typename T> inline void add(std::initializer_list<T> list)
	{
		for (auto elem : list) add(elem);
	}

	void remove(int);
	void empty(void);

	int sort(xArrayElemComp);
	int sorti(void);
	int sortd(void);
	int sortstr(void);
	int sortstr_i(void);


private:
	xArrayElem** table;
	int cur_size;
	int max_size;

	void _set(int, void*, xValType);
	void _resize(int);
	int _grouptypes(unsigned int);

	static const int DEFAULT_SIZE;
	static const double ENLARGE_MULT;
	static const double ARR_SHRINK_AT;
	static const double SHRINK_DIV;

};

int _xArray_comp_int(const xArrayElem**, const xArrayElem**);
int _xArray_comp_double(const xArrayElem**, const xArrayElem**);
int _xArray_comp_string(const xArrayElem**, const xArrayElem**);
int _xArray_comp_string_i(const xArrayElem**, const xArrayElem**);

// multi-types constructor
#define XARRAY_DECL_END -1
#define xARR(type1,...) xArray(type1, __VA_ARGS__, XARRAY_DECL_END)
