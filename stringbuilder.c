#include "stringbuilder.h"

#include <stdlib.h>

void makeStringBuilder(StringBuilder *sb, int capacity) {
	sb->buffer = malloc(capacity * sizeof(char));
	sb->capacity = capacity;
	sb->length = 0;
}

void appendStringBuilderChar(StringBuilder *sb, char c) {
	// Handle reallocaction
	if(sb->capacity == sb->length) {
		sb->capacity += CAP_INC_SB;
		sb->buffer = realloc(sb->buffer, sb->capacity);
	}

	sb->buffer[sb->length++] = c;
}

void stringifyStringBuilderBuffer(StringBuilder *sb) {
	sb->capacity = sb->length + 1;

	// Insert null termination character at the end of the buffer
	sb->buffer[sb->length] = 0;

	sb->buffer = realloc(sb->buffer, sb->capacity);
}
