// Type system

#pragma once

#include <stdbool.h>

// Whether a type is basic or which structural type it is
typedef enum {
	#define STRUCT_TYPE(NAME, STR) type ## NAME,
	#include "type.def"
} TypeType;

// All basic types
typedef enum {
	#define BASIC_TYPE(NAME, STR) basic ## NAME,
	#include "type.def"
} BasicType;

// The rank of a basic type
// As defined in C99 6.3.1.1p1
typedef int BasicRank;

extern BasicRank basicRankValuesArr[];

#define GET_BASIC_RANK(TYPE) (basicRankValuesArr[TYPE])

typedef enum {
	basicTypeError, // Void mapped as an error
	basicTypeSigned,
	basicTypeUnsigned,
	basicTypeFloat,
} BasicTypeType;

// A BasicType could be used as an index to get if the enum is either a signed, unsigned or float type,
// represented by BasicTypeType
extern BasicTypeType basicTypeTypeValuesArr[];

#define GET_BASIC_TYPE_TYPE(TYPE) (basicTypeTypeValuesArr[TYPE])

#define IS_SIGNED(TYPE) (GET_BASIC_TYPE_TYPE(TYPE) == basicTypeSigned)
#define IS_UNSIGNED(TYPE) (GET_BASIC_TYPE_TYPE(TYPE) == basicTypeUnsigned)
#define IS_INTEGER(TYPE) (IS_SIGNED(TYPE) || IS_UNSIGNED(TYPE))
#define IS_FLOAT(TYPE) (GET_BASIC_TYPE_TYPE(TYPE) == basicTypeFloat)


// Forward declare Type
struct Type;

// A member node is a part of a linked list that contains every member of a record type and their offset
typedef struct RecordMember {
	struct Type *type;

	// Alignment of member inside record
	int alignment;
	struct RecordMember *next;
} RecordMember;

typedef struct {
	RecordMember *member;
} RecordType;

typedef struct {
	struct Type *type;
} PointerType;

typedef struct {
	int length;
	struct Type *type;
} ArrayType;

// TODO: Add function type

typedef struct Type {
	TypeType type;

	union {
		BasicType basic;
		RecordType record;
		PointerType pointer;
		ArrayType array;
		// FunctionType function;
	};

	// Refernce counting
	unsigned int references;

	// TODO: Qualifiers
} Type; 

void makeType(Type *t);

void increaseReferencesType(Type *t);

void cleanType(Type *t);

bool areTypesEqual(Type *a, Type *b);

BasicType getUnignedBasicType(BasicType signedType);
