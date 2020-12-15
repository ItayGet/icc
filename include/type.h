// A system for types

#pragma once

#include <stdbool.h>

typedef enum {
	#define TYPE(NAME, STR) type ## NAME,
	#include "type-definitions.h"
} TypeType;

struct Type;

// A member node is a part of a linked list that contains every member of a record type and their offset
typedef struct RecordMember {
	struct Type *type;

	// Alignment of member inside record
	int alignment;
	struct RecordMember *next;
} RecordMember;

typedef struct {
	RecordMember member;
} RecordType;

typedef struct {
	struct Type *type;
} PointerType;

typedef struct {
	int length;
	struct Type *type;
} ArrayType;

// TODO: Add function type

typedef struct {
	TypeType type;
	union {
		RecordType record;
		PointerType pointer;
		ArrayType array;
		// FunctionType function;
	};
} Type; 

bool areTypesEqual(Type *a, Type *b);
