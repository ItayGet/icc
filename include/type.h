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

typedef enum {
	basicTypeError, // Void mapped as an error
	basicTypeSigned,
	basicTypeUnsigned,
	basicTypeFloat,
} BasicTypeType;

// A BasicType could be used as an index to get if the enum is either a signed, unsigned or float type,
// represented by BasicTypeType
extern BasicTypeType typeValuesArr[];

#define IS_SIGNED(TYPE) typeValuesArr[TYPE] == basicTypeSigned
#define IS_UNSIGNED(TYPE) typeValuesArr[TYPE] == basicTypeUnsigned
#define IS_FLOAt(TYPE) typeValuesArr[TYPE] == basicTypeFloat

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
	int references;

	// TODO: Qualifiers
} Type; 

void makeType(Type *t);

void increaseReferencesType(Type *t);

void cleanType(Type *t);

// As defined in C99 6.3.1.1
int getBasicTypeRank(BasicType basic);

BasicType getUnignedBasicType(BasicType signedType);

// As defined in C99 6.3.1.8
BasicType doUsualArithConversion(BasicType a, BasicType b);

// Helper function of doUsualArithConversion
BasicType doUsualArithInt(BasicType signedType, BasicType unsignedType, int sRank, int uRank);
