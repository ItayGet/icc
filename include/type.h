// A system for types

#pragma once

#include <stdbool.h>

typedef enum {
	#define TYPE(NAME, STR) type ## NAME,
	#include "type.def"
} TypeType;

#define IS_BASIC_TYPE(A) ((A) > typeSeparator)

#define MAX_BASIC_TYPE(A, B) ((A) > (B) ? (A) : (B))

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
		RecordType record;
		PointerType pointer;
		ArrayType array;
		// FunctionType function;
	};

	// Refernce counting
	int references;
} Type; 

void makeType(Type *t);

void increaseReferences(Type *t);

void cleanType(Type *t);

bool areTypesEqual(Type *a, Type *b);
