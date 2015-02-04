#pragma once

typedef enum {
	VAL_PTR =		0x0000001,	// unknown pointer		NOT duplicated
	VAL_MAP =		0x0000010,	// pointer to xMap		NOT duplicated
	VAL_ARRAY =		0x0000100,	// pointer to xArray	NOT duplicated
	VAL_STRING =	0x0001000,	// pointer to char*		duplicated
	VAL_INT =		0x0010000,	// int					duplicated
	VAL_DOUBLE =	0x0100000,	// double				duplicated
} xValType;

const unsigned int xDuplicatedTypes = (VAL_STRING | VAL_INT | VAL_DOUBLE);
const unsigned int xPointerTypes = (VAL_STRING | VAL_MAP | VAL_ARRAY | VAL_PTR);

#define CALL_MEMBERFUNC(object,memberfunc) ((object).*(memberfunc))
#define CALL_MEMBERFUNC_P(objptr,memberfunc) ((objptr)->*(memberfunc))
