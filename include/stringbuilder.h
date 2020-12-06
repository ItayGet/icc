// A string builder that can append characters effieciently
// Please note that before shrinking, string will not be stored with a null character

#pragma once

#include <stddef.h>

// The increase in the size of a StringBuffer every time it gets reallocated
#define CAP_INC_SB 16

typedef struct {
	char* buffer;

	// The actual size of the allocated buffer
	int capacity;

	// Length of the string itself
	int length;
} StringBuilder;

void makeStringBuilder(StringBuilder *sb, int capacity);

void appendStringBuilderChar(StringBuilder *sb, char c);

// Shrink the capacity of the buffer to be equal to the length of the string
// Additionally, this function adds a null termination character at the end of the buffer
void stringifyStringBuilderBuffer(StringBuilder *sb);
